/**
 * @file Info.hh
 * @brief Information passed in and out of the algorithm
 * @class Info
 * @brief Information passed in and out of the algorithm
 */

# ifndef   INFO_HH
# define   INFO_HH

#include "ForecastState.hh"
#include "LeadtimeThreadData.hh"
#include <string>

class PbarComputeMgr;
class ParmsPbarCompute;
//------------------------------------------------------------------
class Info 
{
public:

  /**
   * constructor
   * @param[in] genTime
   * @param[in] state  Status, including lead time
   * @param[in] parms
   * @param[in] alg  Pointer to Mgr
   */
  Info(const time_t &genTime, ForecastState::LeadStatus_t state,
       const ParmsPbarCompute &parms, PbarComputeMgr *alg);

  /**
   *  destructor
   */
  virtual ~Info(void);

  time_t _genTime;   /**< Gen time */
  int _leadTime;     /**< Lead seconds */
  ForecastState::LeadStatus_t _state;  /**< Status of this forecast */
  LeadtimeThreadData _ltData;          /**< Lead time threading goes here as an object 
					*   to prevent thread problems */
  PbarComputeMgr *_alg; /**< Pointer to context */

protected:
private:  
  
};

# endif
