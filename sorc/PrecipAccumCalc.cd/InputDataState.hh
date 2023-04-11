/**
 * @file InputDataState.hh
 * @brief InputDataState contains information about each URL/lead time for a 
 *        gentime.
 * @class InputDataState
 * @brief InputDataState contains information about each URL/lead time for a 
 *        gentime.
 */

#ifndef INPUT_DATA_STATE_HH
#define INPUT_DATA_STATE_HH

#include <map>
#include <vector>
#include <string>
class ParmsPrecipAccumCalc;
class InputEnsembleMemberDataState;
class InputForecastDataState;

class InputDataState
{

public:

  InputDataState(const ParmsPrecipAccumCalc &parameters);
  
  InputDataState(const time_t &gTime,
		 const ParmsPrecipAccumCalc &parameters);
  
  ~InputDataState();
  inline bool newGenTime(const time_t &genTime) const
  {
    return pGenTime != genTime;
  }

  void addTrigger(int leadTime, const std::string &url);
  
  std::vector<int> leadTimesReadyAtUrl(const std::string url);
  void setLeadTimeDone(const std::string &url, int leadTime);
  
protected:
private:

  time_t pGenTime;
  // map from URL name 
  std::map<std::string, InputEnsembleMemberDataState> pMemberDataState;
};


class InputEnsembleMemberDataState
{

public:

  InputEnsembleMemberDataState(void);
  InputEnsembleMemberDataState(const std::string url,
			       const ParmsPrecipAccumCalc &parameters);
  
  ~InputEnsembleMemberDataState();

  void addTrigger(int leadTime);

  std::vector<int> leadTimesReady(void);
  void setLeadTimeDone(int leadTime);

protected:
private:

  std::string pUrl;

  // map from lead seconds
  std::map<int, InputForecastDataState> pForecastDataState;
};

class InputForecastDataState
{
public:

  InputForecastDataState(void);
  InputForecastDataState(const int leadSeconds);
  ~InputForecastDataState();

  void addTrigger();

  inline bool isReady(void) const {return pTriggered;}
  inline bool isReadyButNotProcessed(void) const
  {return pTriggered && !pProcessed;}
  void setLeadTimeDone(void);

protected:
private:
  int pLeadSeconds;
  bool pTriggered;
  bool pProcessed;

};



#endif
