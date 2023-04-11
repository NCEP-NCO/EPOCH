/**
 * @file State.hh
 * @brief status of each forecast gen time that has triggered
 * @class State
 * @brief status of each forecast gen time that has triggered
 *
 * There can be many gen times that have not yet gotten all verifying
 * obs data to match forecasts.  They remain in state until they get
 * too old, or until everything is there and can be processed in full.
 */

#ifndef State_HH
#define State_HH

#include "ForecastState.hh"
#include "ParmsThreshFromObarPbar.hh"
#include <vector>
#include <ctime>

class State
{
public:
  /**
   * Constructor 
   * @param[in] parms Stored locally
   */
  State(const ParmsThreshFromObarPbar &parms);

  /**
   * Destructor
   */
  inline ~State(void) {}

  /**
   * Add a new gen time, nothing is available yet, updates oldest and
   * newest gen times in state
   *
   * @param[in] genTime  
   */
  void add(const time_t &genTime, const std::vector<int> &leadSeconds);

  /**
   * Modify state to indicate a particular gen/lead has triggered
   *
   * Also query the obar database and update ALL forecasts in state
   * (for all gen times) as more obs become available. Do this repeatedly
   * because it's fast and lightweight to do so.
   * 
   * @param[in] genTime  
   * @param[in] leadSeconds
   */
  void addTrigger(const time_t &genTime, int leadSeconds);

  /**
   * Prune out a particular gen time by removing all lead times
   * from the state for that gen time that never triggered
   * @param[in] genTime
   */
  void prune(const time_t &genTime);

  /**
   * For all gen times in state, if either too old compared to
   * current gen time, or completely set (all lead times triggered,
   * and all obs data available for each such lead time).
   *
   * @param[in] currentGen  
   */
  void removeIfCompleted(const time_t &currentGen);
  
  /**
   * @return reference to i'th forecast's state
   * @param[in] i
   */
  inline ForecastState & operator[](std::size_t i) {return _fcst[i];}

  /**
   * @return reference to i'th forecast's state
   * @param[in] i
   */
  inline const ForecastState & operator[](std::size_t i) const
  {
    return _fcst[i];
  }

  /**
   * @return number of forecasts in state
   */
  inline std::size_t size(void) const {return _fcst.size();}
  
  /**
   * Debug print
   */
  void print(void) const;

protected:
private:

  ParmsThreshFromObarPbar _parms;           /**< params */
  time_t _oldestGen;                   /**< Oldest gen in state */
  time_t _newestGen;                   /**< Newest gen in state */
  int _leadSecondsMin;
  int _leadSecondsMax;
  std::vector<ForecastState> _fcst;    /**< state for each forecast */
};

#endif
