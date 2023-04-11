/**
 * @file CalcInstanceInfo.hh
 * @brief CalcInstanceInfo contains parameters used to instantiate an instance of
          PrecipAccumCalc
 * @class CalcInstanceInfo
 * @brief CalcInstanceInfo contains parameters used to instantiate an instance of
 *        PrecipAccumCalc
 */

#ifndef CALC_INSTANCE_INFO_HH
#define CALC_INSTANCE_INFO_HH

#include <utility>
#include <dsdata/DsEnsembleAnyTrigger.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include "ParmsPrecipAccumCalcIO.hh"

class CalcInstanceInfo {

public:

  CalcInstanceInfo( const time_t &gTime, const int &lTime, const std::string &urlStr,
		    const ParmsPrecipAccumCalcIO &parameters,
		    ConvWxThreadMgr &thread):
    genTime(gTime),
    leadTime(lTime),
    url(urlStr),
    params(parameters),
    pThread(thread) {}
  
  ~CalcInstanceInfo() {}

  time_t genTime;
  time_t leadTime;
  std::string  url;
  ParmsPrecipAccumCalcIO params;
  ConvWxThreadMgr &pThread;  // from mgr 
  
protected:
private:

};

#endif
