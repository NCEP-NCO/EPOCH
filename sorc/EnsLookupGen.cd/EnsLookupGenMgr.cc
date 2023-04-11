/**
 * @file EnsLookupGenMgr.cc
 */

#include "EnsLookupGenMgr.hh"
#include "Info.hh"
#include "GriddedThresh.hh"
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/ConvWxTime.hh>
#include <dsdata/DsEnsembleLeadTrigger.hh>
#include <toolsa/TaThreadSimple.hh>
#include <toolsa/LogStream.hh>

using std::vector;
using std::pair;
using std::string;

//----------------------------------------------------------------------
static bool _setValueVec(const vector<const Grid *> &grids, int k, 
			 vector<double> &values, bool debug, int ensIndex)
{
  bool status = true;
  if (debug)
  {
    printf("Retrieving values for ensemble member %d ", ensIndex);
  }

  for (size_t n=0; n<grids.size(); ++n)
  {
    double v;
    if (!grids[n]->getValue(k, v))
    {
      if (debug)
      {
	printf(" %s = missing ", grids[n]->getName().c_str());
      }
      status = false;
    }
    else
    {
      values[n] = v;
      if (debug)
      {
	printf(" %s = %lf", grids[n]->getName().c_str(), v);
      }
    }
  }
  if (debug)
  {
    printf("\n");
  }
  if (debug && status == false)
  {
    printf(" Ignore this point, need data for all fields\n");
  }
  return status;
}

//----------------------------------------------------------------------
TaThread *EnsLookupGenMgr::EnsLookupThreads::clone(int index)
{
  TaThreadSimple *t = new TaThreadSimple(index);
  t->setThreadContext(this);
  t->setThreadMethod(EnsLookupGenMgr::compute);
  return dynamic_cast<TaThread *>(t);
}

//----------------------------------------------------------------------
EnsLookupGenMgr::EnsLookupGenMgr(const ParmsEnsLookupGenIO &params,
				 void cleanExit(int)):
  _params(params),  _trigger(NULL),  _genTime(0),  _multiThreshInfo(params)
{
  time_t t = time(0);
  LOG(DEBUG) << "Restarted at " << ConvWxTime::stime(t);

  // to make things thread safe
  InterfaceIO::disallowStoringVerticalLevels();

  // set up triggering one of two ways
  if (params._main.isRealtime())
  {
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
    _trigger = new DsEnsembleLeadTrigger(params._main.pArchiveT0,
                                         params._main.pArchiveT1,
                                         params._modelUrls,
					 params._leadSeconds);
  }
  _thread.init(params._numThreads, false);
}

//----------------------------------------------------------------------
EnsLookupGenMgr::~EnsLookupGenMgr()
{
  _thread.waitForThreads();
  if (_trigger != NULL)
  {
    delete _trigger;
    _trigger = NULL;
  }

  time_t t = time(0);
  LOG(DEBUG) << "Terminated at ", ConvWxTime::stime(t);
}

//----------------------------------------------------------------------
bool EnsLookupGenMgr::run(void)
{
  // Register with process mapper 
  InterfaceLL::doRegister("Running EnsLookupGen algorithm");

  time_t genTime;
  int leadTime;
  vector<string> url;
  bool complete;
  int index = 0;

  // Process data while trigger returns valid generation and lead time pair
  while (_trigger->nextTrigger(genTime, leadTime, url, complete))
  {
    // Process data for  generation time lead time pair
    _process(genTime, leadTime, url.size(), complete, index);
  }
  return true;
}

//----------------------------------------------------------------------
void EnsLookupGenMgr::_process(const time_t &genTime, int leadTime,
			       size_t num, bool complete, int &index)
{
  string cpt = "Incomplete";
  if (complete) cpt = "Complete";
  LOG(DEBUG) << "Triggered " << ConvWxTime::stime(genTime) << "+" 
	     << leadTime << " for " << num << " URls, " << cpt;

  if (_genTime != genTime)
  {
    _thread.waitForThreads();
    LOG(DEBUG) << "New gen time, look for new thresholds";
    _genTime = genTime;
    _multiThreshInfo.newGenTime(genTime);
    index = 0;
  }
  else
  {
    index++;
  }

  Info *info = new Info(genTime, leadTime, this);
  _thread.thread(index, info);
}

//----------------------------------------------------------------------
void EnsLookupGenMgr::compute(void *ti)
{
  Info *algInfo = static_cast<Info *>(ti);
  EnsLookupGenMgr *alg = algInfo->_alg;

  Grid ensembleCount = Grid("counts", "none", alg->_params._projExtended.pNx,
			    alg->_params._projExtended.pNy,-99.99);
  ensembleCount.setAllToValue(0.0);

  GriddedThresh gthresh(alg->_params);
  gthresh.newLeadTime(algInfo->_genTime, algInfo->_lt, alg->_multiThreshInfo);

  InterfaceLL::doRegister("Processing forecast data");

  // Loop through ensemble members and load data, check data thresholds
  // at each point, increment output grid elements satisfying 
  // threshold criteria as appropriate
  for (size_t i=0; i <alg->_params._modelInput.size(); i++)
  {
    alg->_processEnsembleMember(i, algInfo->_genTime, algInfo->_lt,
				ensembleCount, gthresh);
  }

  // normalize the results using counts
  alg->_normalize(ensembleCount, gthresh);

  // Output data
  alg->_outputData(algInfo->_genTime, algInfo->_lt, gthresh);
  delete algInfo;
}

//----------------------------------------------------------------------
void EnsLookupGenMgr::_processEnsembleMember(int ensIndex,
					     const time_t &genTime,
					     int leadTime,
					     Grid &ensembleCount,
					     GriddedThresh &gthresh)
{
  MultiFcstGrid mInGrid;
  vector<const Grid *> grids;
  if (!_loadAllInputFieldsForEnsemble(ensIndex, genTime, leadTime,
				      mInGrid, grids))
  {
    LOG(WARNING) << "Did not get all inputs";
    return;
  }

  vector<double> values;
  values.resize(static_cast<int>(grids.size()), 0.0);

  int k=0;
  for (int y = 0; y <_params._projExtended.pNy; ++y)
  {
    for (int x=0; x<_params._projExtended.pNx; ++x)
    {
      if (_setValueVec(grids, k, values, _params.isDebugPoint(x,y), ensIndex))
      {
	// all values were present, if not all present, don't increment anything
	ensembleCount.incrementValueAtPoint(k, 1.0);
	gthresh.updateCount(values, k, x, y);
      }
      k++;
    }
  }
}

//----------------------------------------------------------------------
bool
EnsLookupGenMgr::
_loadAllInputFieldsForEnsemble(int i, const time_t &genTime, int leadTime, 
			       MultiFcstGrid &mInGrid,
			       std::vector<const Grid *> &grids) const
{
  LOG(DEBUG) << "Loading data for gen " << ConvWxTime::stime(genTime) 
	     << " lead " << leadTime << " at url "
	     << _params._modelInput[i].pUrl;
  if (!_loadInputData(i, genTime, leadTime, mInGrid))
  {
    return false;
  }

  LOG(DEBUG_VERBOSE) << "Data loaded";
  for (size_t i=0; i<_params._fieldNames.size(); ++i)
  {
    const Grid *g = mInGrid.constGridPtr(_params._fieldNames[i]);
    if (g == NULL)
    {
      return false;
    }
    grids.push_back(g);
  }
  return true;
}

//----------------------------------------------------------------------
bool
EnsLookupGenMgr::_loadInputData(int ensembleMember,
				const time_t &genTime, int leadTime,
				MultiFcstGrid &mInGrid) const

{
  InterfaceLL::doRegister("Loading data");

  // NOTE: read the larger domain
  if (!InterfaceIO::loadMultiFcst(genTime, leadTime, _params._projExtended,
                                  _params._modelInput[ensembleMember].pUrl,
                                  _params._fieldNames,
                                  _params._modelInput[ensembleMember].pRemap,
                                  mInGrid))
  {
    LOG(ERROR) << "Failure to load fcst data for gen "
	       << ConvWxTime::stime(genTime) << " lead " << leadTime 
	       << " at url " << _params._modelInput[ensembleMember].pUrl;
    return false;
  }
  return true;
}

//----------------------------------------------------------------------
void EnsLookupGenMgr::_outputData(const time_t &genTime, int leadTime,
				  GriddedThresh &gthresh)
{
  InterfaceLL::doRegister("Writing data");

  LOG(DEBUG) << "Writing data to " << _params._modelOut.pUrl << " for gen time "
	     << ConvWxTime::stime(genTime) << " lead time " << leadTime;
  
  MultiGrid mOutGrid;
  string xml;
  _multiThreshInfo.prepareOutput(//mOutGrid,
				 xml);
  gthresh.prepareOutput(mOutGrid);
  
  MetaData md;
  md.xmlAddFreeform(xml);

  ParmFcstIO parm(_params._modelOut);
  mOutGrid.setEncoding(_params._encodingType);
  
  _thread.lockForIO();
  parm.write(genTime, leadTime, _params._projExtended, mOutGrid, md);
  _thread.unlockAfterIO();
}

//----------------------------------------------------------------------
void EnsLookupGenMgr::_normalize(const Grid &ensembleCount,
				 GriddedThresh &gthresh) const
{
  for (int k = 0; k < ensembleCount.getNdata(); ++k)
  { 
    int x = ensembleCount.xAtIndex(k);
    int y = ensembleCount.yAtIndex(k);
    bool debug = _params.isDebugPoint(x,y);
    double v=0.0;
    bool hasCount = ensembleCount.getValue(k, v);
    if (hasCount && v <= 0)
    {
      if (debug) printf("The denom N=%lf\n", v);
      hasCount = false;
    }
    if (!hasCount)
    {
      if (debug) printf("Setting output to missing, no count\n");
      gthresh.setEnsembleSumMissing(k);
    }
    else
    {
      gthresh.normalizeEnsembleSum(k, v, debug);
    } 
  }
}




