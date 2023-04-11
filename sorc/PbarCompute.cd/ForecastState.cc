/**
 * @file ForecastState.cc
 */

#include "ForecastState.hh"
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <string>
using std::string;

//--------------------------------------------------------------------
ForecastState::ForecastState()
{
  _genTime = 0;
}

//--------------------------------------------------------------------
ForecastState::ForecastState(const time_t &genTime,
			     const std::vector<int> &leadSeconds) :
  _genTime(genTime)
{
  for (size_t i=0; i<leadSeconds.size(); ++i)
  {
    LeadStatus_t l;
    l.leadSeconds = leadSeconds[i];
    l.fcstTriggered = false;
    _leadStatus.push_back(l);
  }
}

//--------------------------------------------------------------------
ForecastState::~ForecastState(void)
{
}

//--------------------------------------------------------------------
bool ForecastState::addTrigger(int leadSeconds)
{
  for (size_t il=0; il<_leadStatus.size(); ++il)
  {
    if (_leadStatus[il].leadSeconds == leadSeconds)
    {
      _leadStatus[il].fcstTriggered = true;
      return true;
    }
  }
  LOG(ERROR) << "No lead time " << leadSeconds << " in state";
  return false;
}

//--------------------------------------------------------------------
void ForecastState::print(void) const
{
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    string s2;
    if (_leadStatus[i].fcstTriggered)
    {
      s2 = "Fcst     Available";
    }
    else
    {
      s2 = "Fcst Not Available";
    }
    printf("%s gt:%slt:%d\n", s2.c_str(), DateTime::strn(_genTime).c_str(),
	   _leadStatus[i].leadSeconds);
  }
}

//--------------------------------------------------------------------
std::string ForecastState::sprintSummary(void) const
{
  int numTrigger=0;
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    if (_leadStatus[i].fcstTriggered)
    {
      ++numTrigger;
    }
  }
  char buf[1000];
  sprintf(buf, "%s  numLead:%d", DateTime::strn(_genTime).c_str(),
	  numTrigger);
  string ret = buf;
  return ret;
}
  
//--------------------------------------------------------------------
bool ForecastState::isComplete(void) const
{
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    if (!_leadStatus[i].fcstTriggered)
    {
      return false;
    }
  }
  return true;
}
  
