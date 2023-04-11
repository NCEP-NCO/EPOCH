/**
 * @file PrecipAccumCalcMgr.cc
 * @brief Source for PrecipAccumCalcMgr class
 */
#include "PrecipAccumCalcMgr.hh"
#include "ParmsPrecipAccumCalc.hh"
#include "PrecipAccumCalc.hh"
#include "CalcInstanceInfo.hh"

#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/Grid.hh>

#include <toolsa/LogStream.hh>
#include <toolsa/pmu.h>

#include <unistd.h>
#include <cstdlib>

using std::vector;
using std::string;

PrecipAccumCalcMgr::PrecipAccumCalcMgr(const ParmsPrecipAccumCalcIO &params,
				       void cleanExit(int)):
  pParams(params),
  pInputDataState(params),
  pTrigger(NULL)
{ 
  time_t t = time(0);

  LOG(DEBUG) << "Restarted at " << ConvWxTime::stime(t);

  vector<string> urls;

  for (size_t i=0; i<pParams.modelInput.size(); ++i)
  {
    LOG(DEBUG) << pParams.modelInput[i].pUrl;
    urls.push_back(pParams.modelInput[i].pUrl);
  }

  if (params.main.isRealtime())
  {
    LOG(DEBUG) << "IS REALTIME";
    PMU_auto_init("PrecipAccumCalc",
		  (const char*)pParams.main.pInstance.c_str(),
		  static_cast<int>(pParams.main.pRegistrationSeconds));
    
    pTrigger = new DsEnsembleAnyTrigger(urls, params.leadSeconds);
  
    if (params.main.pDebugTrigger)
    {
      pTrigger->setDebug();
    }
  }
  else
  {
    pTrigger = new DsEnsembleAnyTrigger(params.main.pArchiveT0,
					params.main.pArchiveT1,
					urls, params.leadSeconds);
  }
  
  pThread.init(pParams.numThreads, false, static_cast<void *>(this),
               PrecipAccumCalcMgr::process);
}

PrecipAccumCalcMgr::~PrecipAccumCalcMgr()
{
  if (pTrigger != NULL)
  {
    delete pTrigger;
    pTrigger = NULL;
  }
  
  LOG(DEBUG) << "Waiting for threads to finish";

  pThread.waitForThreads();

  LOG(DEBUG) << "Cleanup of threads";

  pThread.cleanup();

  time_t t = time(0);

  PMU_auto_unregister();
  LOG(DEBUG) << "Terminated at " <<  ConvWxTime::stime(t);
}


PrecipAccumCalcMgr::Status_t PrecipAccumCalcMgr::run(void)
{
 
  //
  // Forecast generation time, 
  //
  time_t genTime;
  
  //
  // Forecast lead time in seconds
  //
  int leadTime;

  bool hasData;

  string url;

  while (pTrigger->nextTime(genTime, leadTime, url, hasData))
  {
    PMU_force_register("triggered");
    if(hasData)
    {
      LOG(DEBUG) << "-----Trigger-----" << ConvWxTime::stime(genTime)
		 << "+" << leadTime << ", " << url << "------";
    }
    else
    {
      LOG(DEBUG_VERBOSE) << "-----Timeout-----" << url << "------";
      continue;
    }

    InterfaceLL::doRegister("Processing forecast data");

    pUpdateState(genTime, leadTime, url);
    vector<int> leadTimesReady = pInputDataState.leadTimesReadyAtUrl(url);
    for (size_t i=0; i<leadTimesReady.size(); ++i)
    {
      // update the state outside the threading
      pInputDataState.setLeadTimeDone(url, leadTimesReady[i]);
	
      CalcInstanceInfo *calcInfo = new CalcInstanceInfo(genTime,
							leadTimesReady[i],
							url,
							pParams,
							pThread);
    
      //
      // thread index = (ensembleNum -1)* (number of leads) + leadNum
      //
      int ensembleNum = atoi(url.substr(url.length()-2, 2).c_str());
      int threadIndex = ( atoi( url.substr( url.length() -2,2).c_str()) - 1 ) * (int) pParams.leadSeconds.size()/2  + leadTimesReady[i]/21600;   
      
      LOG(DEBUG) << "Starting a thread now ensemble=" << ensembleNum
		 << ", index=" << threadIndex;
      pThread.thread( threadIndex, static_cast<void *>(calcInfo));
    }
  }
 
  return PRECIP_ACCUM_CALC_SUCCESS;
}

void PrecipAccumCalcMgr::process(void *calcInfo)
{
  PMU_force_register("process");
 
  CalcInstanceInfo *calcInstanceInfo =  static_cast<CalcInstanceInfo *> (calcInfo);
 
  LOG(DEBUG) << "Thread: Processing "
	     << ConvWxTime::stime(calcInstanceInfo->genTime) << "+"
	     <<  calcInstanceInfo->leadTime << " at url "
	     << calcInstanceInfo->url;

 
  //
  // Object to hold multiple gridded data fields corresponding
  // to the same generation and lead time
  //
  PrecipAccumCalc precipAccumCalc(calcInstanceInfo->genTime, 
				  calcInstanceInfo->leadTime, 
				  calcInstanceInfo->url, 
				  calcInstanceInfo->params);
 
  precipAccumCalc.process();
  
  //
  // Note that this is not thread safe if not on a Linux box with current implementation of Mdv write
  // 20170127
  //
  // Trying locking for write..
  calcInstanceInfo->pThread.lock();
  precipAccumCalc.write();
  calcInstanceInfo->pThread.unlock();
  
  LOG(DEBUG) << "Thread: Finish "
	     << ConvWxTime::stime(calcInstanceInfo->genTime) << "+"
	     <<  calcInstanceInfo->leadTime << " at url "
	     << calcInstanceInfo->url;
  delete calcInstanceInfo;
} 

void PrecipAccumCalcMgr::pUpdateState(const time_t &genTime, int leadTime,
				      const std::string &url)
{
  if (pInputDataState.newGenTime(genTime))
  {
    pInputDataState = InputDataState(genTime, pParams);
  }
  pInputDataState.addTrigger(leadTime, url);
}
