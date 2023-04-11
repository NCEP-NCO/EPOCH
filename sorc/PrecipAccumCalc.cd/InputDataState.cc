/**
 * @file InputDataState.cc
 */

#include "ParmsPrecipAccumCalc.hh"
#include "InputDataState.hh"
#include <cstdio>

InputDataState::InputDataState(const ParmsPrecipAccumCalc &parameters)
{
  pGenTime = 0;
  for (size_t i=0; i<parameters.modelUrls.size(); ++i)
  {
    pMemberDataState[parameters.modelUrls[i]] =
      InputEnsembleMemberDataState(parameters.modelUrls[i], parameters);
  }
}  

InputDataState::InputDataState(const time_t &gTime,
			       const ParmsPrecipAccumCalc &parameters)
{
  pGenTime = gTime;
  for (size_t i=0; i<parameters.modelUrls.size(); ++i)
  {
    pMemberDataState[parameters.modelUrls[i]] =
      InputEnsembleMemberDataState(parameters.modelUrls[i], parameters);
  }
}  

InputDataState::~InputDataState()
{
}
  
void InputDataState::addTrigger(int leadTime, const std::string &url)
{
  pMemberDataState[url].addTrigger(leadTime);
}

std::vector<int> InputDataState::leadTimesReadyAtUrl(const std::string url)
{
  return pMemberDataState[url].leadTimesReady();
}

void InputDataState::setLeadTimeDone(const std::string &url, int leadTime)
{
  pMemberDataState[url].setLeadTimeDone(leadTime);
}

InputEnsembleMemberDataState::
InputEnsembleMemberDataState(void)
{
  pUrl = "unknown";
}

InputEnsembleMemberDataState::
InputEnsembleMemberDataState(const std::string url,
			     const ParmsPrecipAccumCalc &parameters)
{
  pUrl = url;
  for (size_t i=0; i < parameters.leadSeconds.size(); ++i)
  {
    pForecastDataState[parameters.leadSeconds[i]] =
      InputForecastDataState(parameters.leadSeconds[i]);
  }
}

InputEnsembleMemberDataState::~InputEnsembleMemberDataState()
{
}

void InputEnsembleMemberDataState::addTrigger(int leadTime)
{
  pForecastDataState[leadTime].addTrigger();
}

std::vector<int> InputEnsembleMemberDataState::leadTimesReady(void)
{
  std::vector<int> ret;
  std::map<int, InputForecastDataState>::const_iterator i;
  for (i=pForecastDataState.begin(); i!=pForecastDataState.end(); ++i)
  {
    // hardwired leads here?  Maybe change
    if ( i->first % 21600 == 0)
    {
      int oldLt = i->first - 10800;
      if (i->second.isReadyButNotProcessed() &&
	  pForecastDataState[oldLt].isReady())
      {
	ret.push_back(i->first);
      }
    }
  }
  return ret;
}

void InputEnsembleMemberDataState::setLeadTimeDone(int leadTime)
{
  pForecastDataState[leadTime].setLeadTimeDone();
}


InputForecastDataState::InputForecastDataState(void) :
  pLeadSeconds(-1),
  pTriggered(false),
  pProcessed(false)
{
}

InputForecastDataState::InputForecastDataState(const int leadSeconds) :
  pLeadSeconds(leadSeconds),
  pTriggered(false),
  pProcessed(false)
  
{
}

InputForecastDataState::~InputForecastDataState()
{
}

void InputForecastDataState::addTrigger(void)
{
  if (pTriggered)
  {
    printf("ERROR double trigger of %d\n", pLeadSeconds);
  }
  pTriggered = true;
  pProcessed = false;
}

void InputForecastDataState::setLeadTimeDone(void)
{
  if (pProcessed)
  {
    printf("ERROR double set processed of %d\n", pLeadSeconds);
  }
  pProcessed = true;
}
