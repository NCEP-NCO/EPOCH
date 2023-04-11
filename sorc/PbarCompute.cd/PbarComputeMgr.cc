/**
 * @file PbarComputeMgr.cc
 */

#include "PbarComputeMgr.hh"
#include "Info.hh"
#include "LeadtimeThreadData.hh"
#include "PbarVector.hh"
#include <Epoch/SpdbPbarHandler2.hh>
#include <Epoch/TileRange.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/InterfaceLL.hh>
#include <dsdata/DsEnsembleLeadTrigger.hh>
#include <toolsa/LogStream.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/TaThreadSimple.hh>
#include <algorithm>

using std::vector;
using std::string;

//----------------------------------------------------------------------
static bool _gotAllValues(int gridIndex,
			  const Grid &thresholdedGrid1,
			  const Grid &thresholdedGrid2,
			  LeadtimeThreadData &ltData,
			  double &thresholdedValue1,
			  double &thresholdedValue2,
			  bool &got2)

{
  LOG(DEBUG_VERBOSE) << "Checking index=" << gridIndex;
  if (!thresholdedGrid1.getValue(gridIndex, thresholdedValue1))
  {
    return false;
  }
  if (!ltData.setAdditionalValues(gridIndex, 1))
  {
    return false;
  }
  got2 = thresholdedGrid2.getValue(gridIndex, thresholdedValue2);
  if (got2)
  {
    got2 = ltData.setAdditionalValues(gridIndex, 2);
  }
  return true;
}

//----------------------------------------------------------------------
TaThread *PbarComputeMgr::PbarComputeThreads::clone(int index)
{
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadContext(this);
  t->setThreadMethod(PbarComputeMgr::compute);
  return dynamic_cast<TaThread *>(t);
}

//----------------------------------------------------------------------
PbarComputeMgr::
PbarComputeMgr(const ParmsPbarComputeIO &params, void cleanExit(int)):
  _params(params),
  _trigger(NULL),
  _latestGen(0),
  _state(),
  _pbarSpdb(_params._pbarSpdb)
{
  time_t t = time(0);
  LOG(DEBUG) << "Restarted at " << DateTime::strn(t);

  // to make things thread safe
  InterfaceIO::disallowStoringVerticalLevels();

  // set up triggering one of four ways
  if (params._main.isRealtime())
  {
    LOG(DEBUG) << "Setting up triggering ";
    _trigger = new DsEnsembleLeadTrigger(params._modelUrls,
					 params._leadSeconds);
    if (params._main.pDebugTrigger)
    {
      _trigger->setDebug();
    }
    _trigger->setMaximumAgeSeconds(params._maxAgeSeconds);
    _trigger->setTriggerMaxWaitSeconds(params._triggerFeedbackSeconds);
    _trigger->setMaxSecondsBeforeTimeout(params._urlTimeoutSeconds);
    _trigger->setMaxSecondsBeforeDisable(params._urlDisableSeconds);
  }
  else
  {
    LOG(DEBUG) << "Setting up archive triggering";
    _trigger = new DsEnsembleLeadTrigger(params._main.pArchiveT0,
					 params._main.pArchiveT1,
					 params._modelUrls,
					 params._leadSeconds);
  }
  _thread.init(params._numThreads, false);
  LOG(DEBUG) << "end of constructor";
}

//----------------------------------------------------------------------
PbarComputeMgr::~PbarComputeMgr()
{
  _thread.waitForThreads();
  if (_trigger != NULL)
  {
    delete _trigger;
    _trigger = NULL;
  }
  time_t t = time(0);
  LOG(DEBUG) << "Terminated at ", DateTime::strn(t);
}

//----------------------------------------------------------------------
bool PbarComputeMgr::run(void)
{
  // Register with process mapper 
  InterfaceLL::doRegister("Running PbarCompute algorithm");

  time_t genTime;
  int leadTime;
  vector<string> url;
  bool complete;
 
  // Process data while trigger returns valid generation and lead time pair
  LOG(DEBUG) << "Triggering";
  
  while (_trigger->nextTrigger(genTime, leadTime, url, complete))
  {
    // Process data for  generation time lead time pair
    _process(genTime, leadTime, url.size(), complete);
  }
  LOG(DEBUG) << "No more triggering";
  return true;
}

//----------------------------------------------------------------------
void PbarComputeMgr::_process(const time_t &genTime, int leadTime,
			      size_t num, bool complete)
{
  string cpt = "Incomplete";
  if(complete) cpt = "Complete";
  
  LOG(DEBUG) << "Triggered " << DateTime::strn(genTime) << "+" 
	     << leadTime << " for " << num << " URls, " << cpt;

  // if it is a new gen time start a new state
  if (_latestGen != genTime)
  {
    _state = ForecastState(genTime, _params._leadSeconds);
    _latestGen = genTime;
  }
   _state.addTrigger(leadTime);
  if (_params._debugState)
  {
    LOG(DEBUG) << "Before checking state for complete";
    _state.print();
  }
  if (_state.isComplete())
  {
    LOG(DEBUG) << "Process now: " << DateTime::strn(_latestGen)
		 << " has all forecasts";
    _processGenTime(_latestGen);
    _state = ForecastState();
    if (_params._debugState)
    {
      LOG(DEBUG) << "After processing what we could";
      _state.print();
    }
  }
}

//----------------------------------------------------------------------
void PbarComputeMgr::_processGenTime(const time_t &genTime)
{
  _genTime = genTime;

  SpdbPbarMetadata2 pmeta(genTime, _params._ltHours, _params._tileInfo,
			  _params._inputThresholdedField1,
			  _params._inputThresholdedField2,
			  _params._thresh1, _params._thresh2,
			  _params._hasFixedField1,
			  _params._inputFixedField1,
			  _params._fixedFieldThresh1,
			  _params._hasFixedField2,
			  _params._inputFixedField2,
			  _params._fixedFieldThresh2);
  
  _pbarSpdb = SpdbPbarHandler2(_params._pbarSpdb, pmeta);  

  _modified = false;
  for (size_t i=0; i<_state.size(); ++i)
  {
    // here is where we go to thrading and set up that object
    Info *info = new Info(genTime, _state[i], _params, this);
    _thread.thread(i+1, info);
  }
  _thread.waitForThreads();
  if (_modified)
  {
    _pbarSpdb.write();
  }
}    

//----------------------------------------------------------------------
void PbarComputeMgr::compute(void *ti)
{
  Info *algInfo = static_cast<Info *>(ti);
  PbarComputeMgr *alg = algInfo->_alg;

  if (alg->_processGenLead(algInfo->_genTime, algInfo->_state, algInfo->_ltData))
  {
    alg->_thread.lockForIO();
    alg->_modified = true;
    alg->_thread.unlockAfterIO();
    alg->_processTiles(algInfo->_genTime, algInfo->_state, algInfo->_ltData);
  }
  delete algInfo;
}

//----------------------------------------------------------------------
bool PbarComputeMgr::_processGenLead(const time_t &genTime,
				     const ForecastState::LeadStatus_t s,
				     LeadtimeThreadData &ltData)
{
  int leadTime = s.leadSeconds;
  LOG(DEBUG) << "----- Processing ---- " << DateTime::strn(genTime) << " + " << leadTime;
  FcstGrid egrid;
  if (!_loadExampleInputData(genTime, leadTime, egrid))
  {
    return false;
  }
  ltData.setupCountSums(egrid);

  // now loop through the ensembles
  for (size_t i=0; i<_params._modelInput.size(); ++i)
  {
    _processEnsembleMember(genTime, leadTime, i, ltData);
  }
  
  // hopeless if grid1 was not go
  if (!ltData.agrid1WasModified())
  {
    LOG(ERROR) << "No data at all, nothing possible";
    return false;
  }
  LOG(DEBUG_VERBOSE) << "Normalizing results";
  ltData.normalizeCountSums();
  return true;
}

//----------------------------------------------------------------------
void PbarComputeMgr::_processEnsembleMember(const time_t &genTime,
					      int leadTime,
					      int ensembleIndex,
					      LeadtimeThreadData &ltData)
{
  MultiFcstGrid mInGrid;
  string url = _params._modelInput[ensembleIndex].pUrl;
  LOG(DEBUG_VERBOSE) << "Loading model data from " << url;
  if (!InterfaceIO::loadMultiFcst(genTime, leadTime, _params._proj, 
				  url, _params._inputFieldNames,
				  _params._modelInput[ensembleIndex].pRemap,
				  mInGrid))
  {
    LOG(WARNING) << "Failure to load fcst data at "
		  << DateTime::strn(genTime) << "+" << leadTime  << " from url " << url;
    return;
  }

  LOG(DEBUG_VERBOSE) << "Got model data at " << DateTime::strn(genTime)
		     << "+" << leadTime << " from url " << url;

  const Grid *thresholdedGrid1 = mInGrid.constGridPtr(_params._inputThresholdedField1);
  const Grid *thresholdedGrid2 = mInGrid.constGridPtr(_params._inputThresholdedField2);
  if (thresholdedGrid1 == NULL || thresholdedGrid2 == NULL)
  {
    LOG(ERROR) << "Could not set  pointers to ensemble data";
    return;
  }
  if (!ltData.setAdditionalGridPointers(mInGrid))
  {
    LOG(ERROR) << "Could not set additional input pointers to ensemble data";
    return;
  }

  for (int gridIndex=0; gridIndex<thresholdedGrid1->getNdata(); ++gridIndex)
  {
    double thresholdedValue1, thresholdedValue2;
    bool got2;
    
    if (_gotAllValues(gridIndex, *thresholdedGrid1, *thresholdedGrid2, ltData,
		      thresholdedValue1, thresholdedValue2, got2))
    {
      ltData.updateCountSums(gridIndex, thresholdedValue1, got2, thresholdedValue2);
    }
  }
}

//----------------------------------------------------------------------
bool
PbarComputeMgr::_loadExampleInputData(const time_t &genTime,
					int leadTime,
					FcstGrid &grid) const
{
  InterfaceLL::doRegister("Loading data");
  if (!InterfaceIO::loadFcst(genTime, leadTime, _params._proj,
			     _params._modelInput[0].pUrl,
			     _params._inputThresholdedField1,
			     _params._modelInput[0].pRemap,
			     grid))
  {
    LOG(ERROR) << "Failure to load fcst data for gen "
	       << DateTime::strn(genTime) << " lead " << leadTime 
	       << " at url " << _params._modelInput[0].pUrl;
    return false;
  }
  return true;
}

//----------------------------------------------------------------------
bool
PbarComputeMgr::_processTiles(const time_t &genTime,
			      const ForecastState::LeadStatus_t s,
			      LeadtimeThreadData &ltData)
{
  // do the mothertile first
  int motherIndex = TileInfo::motherTileIndex();
  _setupAndRunAlg(ltData, motherIndex);

  // now do all the other tiles
  for (int tileIndex=0; tileIndex<_params._tileInfo.numTiles(); ++tileIndex)
  {
    if (tileIndex != motherIndex)
    {
      _setupAndRunAlg(ltData, tileIndex);
    }
  }
  return true;
}

//----------------------------------------------------------------
void PbarComputeMgr::_setupAndRunAlg(LeadtimeThreadData &ltData,
				     int tileIndex)
{
  int belowTile;
  if (ltData.useTileBelow(tileIndex, belowTile))
  {
    _thread.lockForIO();
    _pbarSpdb.copyPbarForAllThresh(ltData.getLeadSeconds(), belowTile, tileIndex);
    _thread.unlockAfterIO();
    return;
  }
  
  PbarVector pbar = ltData.runAlg(tileIndex, 1);
  _thread.lockForIO();
  _pbarSpdb.setPbarForAllThresh(ltData.getLeadSeconds(), tileIndex, pbar.getPbar(), 1);
  _thread.unlockAfterIO();

  pbar = ltData.runAlg(tileIndex, 2);
  _thread.lockForIO();
  _pbarSpdb.setPbarForAllThresh(ltData.getLeadSeconds(), tileIndex, pbar.getPbar(), 2);
  _thread.unlockAfterIO();
}
