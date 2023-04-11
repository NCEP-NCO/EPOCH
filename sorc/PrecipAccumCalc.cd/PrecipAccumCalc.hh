/**
 * @file PrecipAccumCalcMgr.hh
 * @brief PrecipAccumCalc calculates 3 hr precip from alternating  
 * lead times containing 3 and 6 hr accumulations
 * @class PrecipAccumCalc
 * @brief PrecipAccumCalc  calculates 3 hr precip from alternating  
 * lead times containing 3 and 6 hr accumulations
 */
#ifndef PRECIPACCUMCALC_HH
#define PRECIPACCUMCALC_HH


#include <utility>
#include <dsdata/DsEnsembleAnyTrigger.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include "ParmsPrecipAccumCalcIO.hh"

class PrecipAccumCalc {

public:

  /**
   *  Return PrecipAccumCalc success/failure indicators
   */
  enum Status_t {CALC_SUCCESS, CALC_FAILURE};
  
  /**
   * Constructor
   * @param[in]   
   * @param[in] 
   * @param[in] 
   */
  PrecipAccumCalc(const time_t &genTime, const int &leadTime, const std::string &url,
		  const ParmsPrecipAccumCalcIO &params);

  /**
   *  Destructor
   */
  ~PrecipAccumCalc(void);

  /**
   * Performs differences of 6hr and 3hr accumulations 
   * Private members determine url for input, output, genTime, leadTime, and field names
   */ 
  void process(void);

  /**
   * Write data to output url.
   * Currently a public function for isolating the write method 
   * to insure thread safety
   */ 
  void write(void);


protected:
   
private:

  

  time_t pGenTime;
  int  pLeadTime;
  std::string pUrl;
  ParmsPrecipAccumCalcIO pParams;
  std::string pEnsembleMem;
  std::string outputDirTail;
  MultiFcstGrid pMultiInGrid3hrAccumPrevLead;
  MultiFcstGrid pMultiInGrid6hrAccum;
  MultiFcstGrid pMultiOutGrid3hrAccumPrevLead;
  MultiFcstGrid pMultiOutGrid3hrAccum;

  /**
   * Load input data for specified by generation, lead time, and url members
   * @return status value
   */
  Status_t pLoadInputData();

  /**
   * Output one or more fields of average Fractions Skill Score data. 
   * @param[in] gtOut  Generation time assigned to output data
   * @param[in] ltOut  Lead time assigned to output data
   * @param[in] mOutGrid  MultiFcstGrid object containing precip accum and other
   *                      requested fields
   */
  void pWriteOutput(const time_t gtOut, const int ltOut,
		    const MultiFcstGrid &mOutGrid, 
		    const std::string &outputDirTail ); 
};

#endif
