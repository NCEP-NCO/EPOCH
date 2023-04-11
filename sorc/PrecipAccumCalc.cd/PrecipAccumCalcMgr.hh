/**
 * @file PrecipAccumCalcMgr.hh
 * @brief PrecipAccumCalcMgr calculates 3 hr precip from alternating  
 * lead times containing 3 and 6 hr accumulations
 * @class PrecipAccumCalcMgr
 * @brief PrecipAccumCalcMgr  calculates 3 hr precip from alternating  
 * lead times containing 3 and 6 hr accumulations
 */
#ifndef PRECIPACCUMCALCMGR_HH
#define PRECIPACCUMCALCMGR_HH


#include <utility>
#include <dsdata/DsEnsembleAnyTrigger.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include "ParmsPrecipAccumCalcIO.hh"
#include "InputDataState.hh"

class PrecipAccumCalcMgr {

public:

  /**
   *  Return PrecipAccumCalcMgr success/failure indicators
   */
  enum Status_t {PRECIP_ACCUM_CALC_SUCCESS, PRECIP_ACCUM_CALC_FAILURE};
  
  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] cleanExit  Cleanup function to call at exit
   */
  PrecipAccumCalcMgr(const ParmsPrecipAccumCalcIO &params, void cleanExit(int));

  /**
   *  Destructor
   */
  ~PrecipAccumCalcMgr(void);

  /**
   * Run the algorithm that performs differences of 6hr and 3hr accumulations 
   * as necessary
   * @return PRECIP_ACCUM_CALC_SUCCESS or PRECIP_ACCUM_CALC_FAILURE
   */ 
  Status_t run(void);

  static void process(void *calcInstanceInfo);

protected:
   
private:

  
  /**
   *  User defined parameters
   */
  ParmsPrecipAccumCalcIO pParams;
  
  /**
   * Input data state, because in real time data comes in out of order
   */
  InputDataState  pInputDataState;

  /**
   * Triggering mechanism that waits for a gen time
   */
  DsEnsembleAnyTrigger *pTrigger;

  /**
   * Load input data for 
   * specified generation and lead time.
   * @param[in] genTime  Generation time of data to load
   * @param[in] leadTime  Lead time of data to load
   * @param[out] mInGrid  A grid object containing multiple fields of data
   *                      corresponding to the same generation and lead time
   * @return status value
   */
  Status_t pLoadInputData(const time_t &genTime, const int &leadTime, 
			  const std::string &url, MultiFcstGrid &mInGrid6hr,
			  MultiFcstGrid &mInGrid3hr);

  void pUpdateState(const time_t &genTime, int leadTime,
		    const std::string &url);
 
  /**
   * Processing uses threads to calibrate subgrids of data
   */
  ConvWxThreadMgr pThread;

};

#endif
