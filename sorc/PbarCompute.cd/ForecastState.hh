/**
 * @file ForecastState.hh
 * @brief status of one forecast gen time that has triggered
 * @class ForecastState
 * @brief status of one forecast gen time that has triggered
 */

#ifndef ForecastState_HH
#define ForecastState_HH

#include <vector>
#include <string>
#include <ctime>

class ForecastState
{
public:
  /**
   * The information about each lead time
   */
  typedef struct
  {
    int leadSeconds;    /**< The lead time seconds */
    bool fcstTriggered; /**< True if this lead time has triggered */
  } LeadStatus_t;

  /**
   * Empty constructor
   */
  ForecastState(void);
  
  /**
   * Constructor
   * @param[in] genTime  The gen time
   * @param[in] leadSeconds  All the expected lead seconds
   */
  ForecastState(const time_t &genTime, const std::vector<int> &leadSeconds);

  /**
   * Destructor
   */
  ~ForecastState(void);

  /**
   * Called when a particular gen/lead has triggered 
   * @param[in] leadSeconds
   */
  bool addTrigger(int leadSeconds);

  /**
   * Prune those lead times from the state that never triggered
   * @param[in] genTime
   */
  bool prune(const time_t &genTime);

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * @return string with summary information about the state
   */
  std::string sprintSummary(void) const;

  /**
   * @return the gen time for this state
   */
  inline time_t getGenTime(void) const {return _genTime;}

  /**
   * @return true if this object is 'complete'
   * 
   * If all forecasts have triggered, yes
   */
  bool isComplete(void) const;

  /**
   * @return reference to i'th status
   * @param[in] i
   */
  inline LeadStatus_t &operator[](size_t i) {return _leadStatus[i];}
  /**
   * @return reference to i'th status
   * @param[in] i
   */
  inline const LeadStatus_t &operator[](size_t i) const
  {
    return _leadStatus[i];
  }

  /**
   * return length of status vector
   */
  inline size_t size(void) const {return _leadStatus.size();}

protected:
private:

  time_t _genTime;                        /**< The gen time */
  std::vector<LeadStatus_t> _leadStatus;  /**< The status for each lead */
  
};

#endif
