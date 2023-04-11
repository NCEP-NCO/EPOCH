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
class ThreshFromObarPbarMgr;
class ParmsThreshFromObarPbar;

//------------------------------------------------------------------
class Info 
{
public:

  /**
   * constructor
   * @param[in] t  Gen time
   * @param[in] lt  Lead seconds
   * @param[in] alg  Pointer to Mgr
   */
  Info(const time_t &genTime, ForecastState::LeadStatus_t state,
       const ParmsThreshFromObarPbar &parms, ThreshFromObarPbarMgr *alg);

  /**
   *  destructor
   */
  virtual ~Info(void);

  time_t _genTime;   /**< Gen or obs time */
  int _leadTime;     /**< Lead seconds (when forecast, ignored if not) */
  ForecastState::LeadStatus_t _state;
  LeadtimeThreadData _ltData;
  ThreshFromObarPbarMgr *_alg; /**< Pointer to context */

protected:
private:  
  
};

# endif
