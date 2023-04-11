/**
 * @file ThreshFromObarPbarMgr.cc
 */

#include "ThreshFromObarPbarMgr.hh"
#include <toolsa/TaThreadSimple.hh>
#include <toolsa/DateTime.hh>
#include <ConvWx/InterfaceLL.hh>
#include <algorithm>
#include "Info.hh"
#include <Epoch/SpdbObsHandler.hh>

using std::vector;
using std::string;

//----------------------------------------------------------------------
TaThread *ThreshFromObarPbarMgr::ThreshFromObarPbarThreads::clone(int index)
{
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadContext(this);
  t->setThreadMethod(ThreshFromObarPbarMgr::compute);
  return dynamic_cast<TaThread *>(t);
}

//----------------------------------------------------------------------
ThreshFromObarPbarMgr::
ThreshFromObarPbarMgr(const ParmsThreshFromObarPbarIO &params, void cleanExit(int)):
  _params(params),
  _strigger(NULL),
  _latestGen(0),
  _state(_params),
  _genTime(0),
  _pbarSpdb(params._pbarSpdb),
  _threshSpdb1(params._thresholdsSpdb1),
  _threshSpdb2(params._thresholdsSpdb2)
{
  time_t t = time(0);
  LOG(DEBUG) << "Restarted at " << DateTime::strn(t);

  if (params._main.isRealtime())
  {
    _strigger = new DsUrlTrigger(params._pbarSpdb,
				 DsUrlTrigger::OBS, false);
  }
  else
  {
    _strigger = new DsUrlTrigger(params._main.pArchiveT0,
				 params._main.pArchiveT1,
				 params._pbarSpdb,
				 DsUrlTrigger::OBS, false, true);
  }
  _thread.init(params._numThreads, false);
  LOG(DEBUG) << "end of constructor";
}

//----------------------------------------------------------------------
ThreshFromObarPbarMgr::~ThreshFromObarPbarMgr()
{
  _thread.waitForThreads();
  if (_strigger != NULL)
  {
    delete _strigger;
    _strigger = NULL;
  }
  time_t t = time(0);
  LOG(DEBUG) << "Terminated at ", DateTime::strn(t);
}

//----------------------------------------------------------------------
bool ThreshFromObarPbarMgr::run(void)
{
  // Register with process mapper 
  InterfaceLL::doRegister("Running ThreshFromObarPbar algorithm");

  time_t genTime;
 
  if (_params._main.isRealtime())
  {
    // upon startup fill gaps in processing by looking back
    _fillGaps(time(0));
  }
  
  // Process data while trigger returns valid generation and lead time pair
  LOG(DEBUG) << "Triggering";
  
  bool first = true;
  while (_strigger->nextTime(genTime))
  {
    {
      _processTrigger(genTime, first);
      first = false;
    }
  }
  LOG(DEBUG) << "No more triggering";
  return true;
}

//----------------------------------------------------------------------
void ThreshFromObarPbarMgr::compute(void *ti)
{
  Info *algInfo = static_cast<Info *>(ti);
  ThreshFromObarPbarMgr *alg = algInfo->_alg;

  // pull the oBar value out of spdb for the valid time
  SpdbObsHandler obs1(alg->_params._obarSpdb1);
  SpdbObsHandler obs2(alg->_params._obarSpdb2);

  time_t vt = algInfo->_genTime + algInfo->_leadTime;

  bool has1=true;
  bool has2=true;
  if (!obs1.read(vt))
  {
    LOG(DEBUG_VERBOSE) << "No precip oBar from database yet at " << DateTime::strn(vt);
    has1=false;
  }
  if (!obs2.read(vt))
  {
    LOG(DEBUG_VERBOSE) << "No CTH oBar from database yet at " << DateTime::strn(vt);
    has2=false;
  }
  // make sure thresholds match
  if (has1)
  {
    if (obs1.getNumThresh() != (int)alg->_params._obarThreshTargetBias1.size())
    {
      LOG(ERROR) << "Inconsistent database/param settings for precip obar";
      LOG(ERROR) << "The cmorph obar database has " << obs1.getNumThresh() << " obar thresholds";
      LOG(ERROR) << "The parameter settings are configured for " << alg->_params._obarThreshTargetBias1.size()
		 <<  " obar thresholds";
      for (size_t i=0; i<alg->_params._obarThreshTargetBias1.size(); ++i)
      {
	LOG(ERROR) << "  param thresh1[" << i << "]:" << alg->_params._obarThreshTargetBias1[i].first;
	LOG(ERROR) << "  param bias1[" << i << "]:" << alg->_params._obarThreshTargetBias1[i].second;
      }
      delete algInfo;
      return;
    }
  }
  if (has2)
  {
    if (obs2.getNumThresh() != (int)alg->_params._obarThreshTargetBias2.size())
    {
      LOG(ERROR) << "Inconsistent database/param settings for CTH obar";
      LOG(ERROR) << "The CTH obar database has " << obs2.getNumThresh() << " obar thresholds";
      LOG(ERROR) << "The parameter settings are configured for " << alg->_params._obarThreshTargetBias2.size()
		 <<  " obar thresholds";
      for (size_t i=0; i<alg->_params._obarThreshTargetBias2.size(); ++i)
      {
	LOG(ERROR) << "  param thresh2[" << i << "]:" << alg->_params._obarThreshTargetBias2[i].first;
	LOG(ERROR) << "  param bias2[" << i << "]:" << alg->_params._obarThreshTargetBias2[i].second;
      }
      delete algInfo;
      return;
    }
  }
  if (has1)
  {
    for (int i=0; i<obs1.getNumThresh(); ++i)
    {
      if (obs1.getIthThresh(i) != alg->_params._obarThreshTargetBias1[i].first)
      {
	LOG(ERROR) << "Inconsistant precip obar thresh values, " << i << "th threshold";
	LOG(ERROR) << "  param    thresh1[" << i << "]:" << alg->_params._obarThreshTargetBias1[i].first;
	LOG(ERROR) << "  database thresh1[" << i << "]:" << obs1.getIthThresh(i);
	delete algInfo;
	return;
      }
    }
  }
  if (has2)
  {
    for (int i=0; i<obs2.getNumThresh(); ++i)
    {
      if (obs2.getIthThresh(i) != alg->_params._obarThreshTargetBias2[i].first)
      {
	LOG(ERROR) << "Inconsistant CTH obar thresh values, " << i << "th threshold";
	LOG(ERROR) << "  param    thresh2[" << i << "]:" << alg->_params._obarThreshTargetBias2[i].first;
	LOG(ERROR) << "  database thresh2[" << i << "]:" << obs2.getIthThresh(i);
	delete algInfo;
	return;
      }
    }
  }
  
  alg->_thread.lockForIO();
  alg->_modified = true;
  alg->_thread.unlockAfterIO();

  vector<int> pbarIndexAtTile;
  if (has1)
  {
    //  for each threshold, field 1
    for (int i=0; i<obs1.getNumThresh(); ++i)
    {
      // process each tile to get an optimum threshold for that tile
      alg->_processTilesAtObsThresh1(algInfo->_genTime, algInfo->_state,
				     algInfo->_ltData, obs1, i, pbarIndexAtTile);
      // store that info to SPDB
      algInfo->_ltData.updateSpdbForAllTiles(alg->_threshSpdb1, i, 1, alg->_thread);

      if (has2)
      {
	// for each threshold, field 2
	for (int j=0; j<obs2.getNumThresh(); ++j)
	{
	  // process each tile using results of field 1 above to get an optimum threshold for that tile
	  alg->_processTilesAtObsThresh2(algInfo->_genTime, algInfo->_state,
					 pbarIndexAtTile, algInfo->_ltData, obs2, j);
	  // store that info into SPDB
	  algInfo->_ltData.updateSpdbForAllTiles(alg->_threshSpdb2, j, 2, alg->_thread);
	}
      }
    }
  }      
  delete algInfo;
}

//----------------------------------------------------------------------
void ThreshFromObarPbarMgr::_processTrigger(const time_t &genTime, bool first)
{
  LOG(DEBUG) << "Triggered " << DateTime::strn(genTime);
  if (first && !_params._main.isRealtime())
  {
    // look back into database to see if there are gaps compared to
    // previous forecast gen times
    _fillGaps(genTime);
  }    

  // read in the SPDB now
  if (!_pbarSpdb.read(genTime))
  {
    LOG(ERROR) << "Expect pbar data at gen time but none";
    return;
  }
  // pull in all the lead times
  vector<int> ltSeconds = _pbarSpdb.getLeadSeconds();
  
  // if it is a new gen time add to state
  if (_latestGen != genTime)
  {
    _state.add(genTime, ltSeconds);
    if (_latestGen > 0)
    {
      _state.prune(_latestGen);
    }
    _latestGen = genTime;
  }
  for (size_t i=0; i<ltSeconds.size(); ++i)
  {
    _state.addTrigger(genTime, ltSeconds[i]);
  }

  if (_params._debugState)
  {
    LOG(DEBUG) << "Before checking state for complete";
    _state.print();
  }
  for (size_t i=0; i<_state.size(); ++i)
  {
    if (_state[i].isComplete(genTime, _params._maxIncompleteSeconds))
    {
      time_t gen = _state[i].getGenTime();
      LOG(DEBUG) << "Process now: " << DateTime::strn(gen)
		 << " has all valid obs";
      _processGenTime(_state[i]);
    }
    else if (_state[i].hasLargestLead())
    {
      time_t gen = _state[i].getGenTime();
      LOG(DEBUG) << "Process now with some missing lead times obar" << DateTime::strn(gen);
      _processGenTime(_state[i]);
    }
  }
  if (_params._debugState)
  {
    LOG(DEBUG) << "After processing what we could";
    _state.print();
  }
  _state.removeIfCompleted(genTime);
}

//----------------------------------------------------------------------
void ThreshFromObarPbarMgr::_processGenTime(const ForecastState &state)
{
  time_t gt = state.getGenTime();
  _genTime = gt;

  if (!_pbarSpdb.read(_genTime))
  {
    LOG(DEBUG) << "No pbar at gen time " << DateTime::strn(_genTime);
    return;
  }
  _queryThreshSpdbAtGenTime(gt);
  _modified = false;
  for (size_t i=0; i<state.size(); ++i)
  {
    // here is where we go to threading and set up that object, based on lead time
    Info *info = new Info(gt, state[i], _params, this);
    _thread.thread(i+1, info);
  }    
  _thread.waitForThreads();
  if (_modified)
  {
    _threshSpdb1.write();
    _threshSpdb2.write();
  }
}

//----------------------------------------------------------------------
void ThreshFromObarPbarMgr::_queryThreshSpdbAtGenTime(const time_t &gt)
{
  bool isFixed1, isFixed2;
  double fixedThresh1, fixedThresh2;
  string fixedField1, fixedField2, field1, field2;
  
  _pbarSpdb.getFixed1(isFixed1, fixedField1, fixedThresh1);
  _pbarSpdb.getFixed2(isFixed2, fixedField2, fixedThresh2);
  field1 = _pbarSpdb.getField1();
  field2 = _pbarSpdb.getField2();

  vector<int> lts = _pbarSpdb.getLeadSeconds();
  vector<double> lth;
  for (size_t i=0; i<lts.size(); ++i)
  {
    lth.push_back((double)lts[i]/3600.0);
  }

  // read the existing spdb for this gen time, or start a new one.
  if (!_threshSpdb1.read(gt))
  {
    LOG(DEBUG) << "Creating new Thresholds Database object 1 for "
	       << DateTime::strn(gt);
    SpdbGenBasedMetadata md;
    if (isFixed1)
    {
      md = SpdbGenBasedMetadata(gt, lth, field1,
				_params._thresholdedFieldColdstartThresh1,
				fixedField1, fixedThresh1,
				_params._tileInfo, _params._obarThresh1);
    }
    else
    {
      md = SpdbGenBasedMetadata(gt, lth, field1,
				_params._thresholdedFieldColdstartThresh1,
				_params._tileInfo, _params._obarThresh1);
    }
    _threshSpdb1 = SpdbGenBasedThreshHandler(_params._thresholdsSpdb1, md);
  }
  else
  {
    LOG(DEBUG) << "Use existing Thresholds Database object 1 for "
	       << DateTime::strn(gt);
  }

  // read the existing spdb for this gen time, or start a new one.
  if (!_threshSpdb2.read(gt))
  {
    LOG(DEBUG) << "Creating new Thresholds Database object 2 for "
	       << DateTime::strn(gt);
    SpdbGenBasedMetadata md;
    if (isFixed2)
    {
      md = SpdbGenBasedMetadata(gt, lth, field2,
				_params._thresholdedFieldColdstartThresh2,
				fixedField2, fixedThresh2,
				_params._tileInfo, _params._obarThresh2);
    }
    else
    {
      md = SpdbGenBasedMetadata(gt, lth, field2,
				_params._thresholdedFieldColdstartThresh2,
				_params._tileInfo, _params._obarThresh2);
    }
    _threshSpdb2 = SpdbGenBasedThreshHandler(_params._thresholdsSpdb2, md);
  }
  else
  {
    LOG(DEBUG) << "Use existing Thresholds Database object 2 for "
	       << DateTime::strn(gt);
  }
}

//----------------------------------------------------------------------
void
ThreshFromObarPbarMgr::_processTilesAtObsThresh1(const time_t &genTime,
						const ForecastState::LeadStatus_t s,
						LeadtimeThreadData &ltData,
						SpdbObsHandler &obs1,
						int obsIndex1,
						 vector<int> &pbarIndexAtTile)
{
  // set the initial thresholds using older data or coldstart, field 1
  ltData.setInitialThresholds(1, obsIndex1);

  // pull in all the candidate thresholds
  std::vector<double> thresholds = _pbarSpdb.getThresh(1);

  // do the mothertile first
  int motherIndex = TileInfo::motherTileIndex();
  int pbarIndex0 = _setupAndRunAlg(ltData, motherIndex, true, obs1,
				  obsIndex1, thresholds);
  ltData.handleMotherResults(motherIndex, pbarIndex0);

  // now do all the other tiles
  for (int tileIndex=0; tileIndex<_params._tileInfo.numTiles(); ++tileIndex)
  {
    if (tileIndex != motherIndex)
    {
      int pbarIndex = _setupAndRunAlg(ltData, tileIndex, false, obs1,
				  obsIndex1, thresholds);
      pbarIndexAtTile.push_back(pbarIndex);
    }
    else
    {
      pbarIndexAtTile.push_back(pbarIndex0);
    }
  }
}

//----------------------------------------------------------------------
void
ThreshFromObarPbarMgr::_processTilesAtObsThresh2(const time_t &genTime,
						 const ForecastState::LeadStatus_t s,
						 const vector<int> &pbarIndexAtTile,
						 LeadtimeThreadData &ltData,
						 SpdbObsHandler &obs2,
						 int obsIndex2)
{
  // set the initial thresholds using older data or coldstart
  ltData.setInitialThresholds(2, obsIndex2);

  // pull in all the candidate thresholds
  std::vector<double> thresholds = _pbarSpdb.getThresh(2);

  // do the mothertile first
  int motherIndex = TileInfo::motherTileIndex();
  int pbarIndex = _setupAndRunAlg2(ltData, motherIndex, true, obs2,
				   obsIndex2, thresholds,
				   pbarIndexAtTile[motherIndex]);
  ltData.handleMotherResults(motherIndex, pbarIndex);

  // now do all the other tiles
  for (int tileIndex=0; tileIndex<_params._tileInfo.numTiles(); ++tileIndex)
  {
    pbarIndex = _setupAndRunAlg2(ltData, tileIndex, false, obs2,
				 obsIndex2, thresholds,
				 pbarIndexAtTile[tileIndex]);
  }
}

//----------------------------------------------------------------
int ThreshFromObarPbarMgr::_setupAndRunAlg(LeadtimeThreadData &ltData,
					    int tileIndex, 
					    bool isMotherTile, 
					    SpdbObsHandler &obs,
					    int obsThreshIndex,
					   std::vector<double> thresholds)

{
  int belowTile;
  int threshIndex;
  if (ltData.usedTileBelow(tileIndex, isMotherTile, thresholds, belowTile,
			   threshIndex))
  {
    return threshIndex;
  }

  int lt = ltData.getLeadSeconds();
						
  // get all the pbars for this tile/lead time
  vector<double> pbarForTile = _pbarSpdb.getPbar1(lt, tileIndex);

  // get the obar value for this obs index and tile index
  double oBar;
  bool gotObar = _getObar(obs, obsThreshIndex, tileIndex, oBar);

  // check for ability to compute anything (lack of data means no)
  if (!gotObar)
  {
    return ltData.setToMotherOrColdstart(tileIndex, isMotherTile);
  }
  
  if (oBar == 0.0)
  {
    // use max threshold (or min threshold that gives 0), unless all thresolds
    // give 0, in which case use default or mothertile threshold
    return ltData.setupAndRunObsZero(tileIndex, pbarForTile, thresholds, isMotherTile, obsThreshIndex);
  }
  else
  {
    // try previous best, and if it gives a result of 0, only go down.
    // and if all are zero use that minimum,
    // if it gives a nonzero result go both up and down.
    return ltData.setupAndRunObsNonZero(oBar, tileIndex, pbarForTile, thresholds, isMotherTile, obsThreshIndex);
  }
}

//----------------------------------------------------------------
int ThreshFromObarPbarMgr::_setupAndRunAlg2(LeadtimeThreadData &ltData,
					    int tileIndex, 
					    bool isMotherTile, 
					    SpdbObsHandler &obs,
					    int obsThreshIndex,
					    std::vector<double> thresholds,
					    int index1)

{
  int belowTile;
  int threshIndex;
  if (ltData.usedTileBelow(tileIndex, isMotherTile, thresholds, belowTile,
			   threshIndex))
  {
    return threshIndex;
  }

  if (index1 < 0)
  {
    // nothing from field1 to work with, so do same as if no obar to work with
    return ltData.setToMotherOrColdstart(tileIndex, isMotherTile);
  }

  int lt = ltData.getLeadSeconds();
						
  // get all the pbars for this tile/lead time
  vector<double> pbarForTile = _pbarSpdb.getPbar2(lt, tileIndex, index1);

  // get the obar value for this obs index and tile index
  double oBar;
  bool gotObar = _getObar(obs, obsThreshIndex, tileIndex, oBar);

  // check for ability to compute anything (lack of data means no)
  if (!gotObar)
  {
    return ltData.setToMotherOrColdstart(tileIndex, isMotherTile);
  }
  
  if (oBar == 0.0)
  {
    // use max threshold (or min threshold that gives 0), unless all thresolds
    // give 0, in which case use default or mothertile threshold
    return ltData.setupAndRunObsZero(tileIndex, pbarForTile, thresholds, isMotherTile, obsThreshIndex);
  }
  else
  {
    // try previous best, and if it gives a result of 0, only go down.
    // and if all are zero use that minimum,
    // if it gives a nonzero result go both up and down.
    return ltData.setupAndRunObsNonZero(oBar, tileIndex, pbarForTile, thresholds, isMotherTile, obsThreshIndex);
  }
}

//-----------------------------------------------------------------------
bool ThreshFromObarPbarMgr::_getObar(SpdbObsHandler &obs,
				int obarThreshIndex, int tileIndex,
				double &oBar)
{
  TileObarInfo obarInfo = obs.getObarInfo(obarThreshIndex, tileIndex);
  if (!obarInfo.ok())
  {
    oBar = 0;
    LOG(WARNING) << "NO OBAR";
    return false;
  }
  else
  {
    oBar = obarInfo.getObar();
    return true;
  }
}

//----------------------------------------------------------------------
void ThreshFromObarPbarMgr::_fillGaps(const time_t &genTime)
{
  LOG(DEBUG) << "Filling gaps up to " << _params._backfillDaysBack
	     << " days before " << DateTime::strn(genTime);
  time_t gt0 = genTime - _params._backfillDaysBack*24*3600;
  vector<time_t> ptimes = _pbarSpdb.timesInRange(gt0, genTime-1);
  if (ptimes.empty())
  {
    LOG(DEBUG) << "No gap filling, no triggering inputs back "
	       << _params._backfillDaysBack << " from "
	       << DateTime::strn(genTime);
    return;
  }

  // see what precip we need, ttime1 = what we already have processed
  vector<time_t> existing = _threshSpdb1.timesInRange(gt0, genTime-1);

  // now loop through and deal with whichever ones are not yet done
  for (size_t i=0; i<ptimes.size(); ++i)
  {
    time_t ti = ptimes[i];
    if (find(existing.begin(), existing.end(), ti) == existing.end())
    {
      // not yet processed
      if (!_pbarSpdb.read(ti))
      {
	LOG(ERROR) << "Expect pbar data at gen time but none";
	continue;
      }
      vector<int> ltSeconds = _pbarSpdb.getLeadSeconds();
      _state.add(ti, ltSeconds);
      for (size_t j=0; j<ltSeconds.size(); ++j)
      {
	_state.addTrigger(ti, ltSeconds[j]);
      }
      LOG(DEBUG) << "ADDED " << ltSeconds.size() << " lead times at gen "
		 << DateTime::strn(ti);
    }
  }

  if (_params._debugState)
  {
    LOG(DEBUG) << "Before processing state:";
    _state.print();
  }
  for (size_t i=0; i<_state.size(); ++i)
  {
    if (_state[i].isComplete(genTime, _params._maxIncompleteSeconds))
    {
      time_t gen = _state[i].getGenTime();
      LOG(DEBUG) << "Process now: " << DateTime::strn(gen)
		 << " has all valid obs";;
      _processGenTime(_state[i]);
    }
    else if (_state[i].hasLargestLead())
    {
      time_t gen = _state[i].getGenTime();
      LOG(DEBUG) << "Process now with some missing lead times obar" << DateTime::strn(gen);
      _processGenTime(_state[i]);
    }
  }
  _state.removeIfCompleted(genTime);

  if (_params._debugState)
  {
    LOG(DEBUG) << "After initial look for backfill";
    _state.print();
  }
}
