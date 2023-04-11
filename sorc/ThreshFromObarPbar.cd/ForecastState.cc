/**
 * @file ForecastState.cc
 */

#include "ForecastState.hh"
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <string>
using std::string;

//--------------------------------------------------------------------
ForecastState::ForecastState(const time_t &genTime,
			     const std::vector<int> &leadSeconds) :
  _genTime(genTime)
{
  for (size_t i=0; i<leadSeconds.size(); ++i)
  {
    LeadStatus_t l;
    l.leadSeconds = leadSeconds[i];
    l.obs1Available = false;
    l.obs2Available = false;
    l.fcstTriggered = false;
    _leadStatus.push_back(l);
  }
}

//--------------------------------------------------------------------
ForecastState::~ForecastState(void)
{
}

//--------------------------------------------------------------------
bool ForecastState::addTrigger(const time_t &genTime, int leadSeconds)
{
  if (_genTime == genTime)
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
  }
  return false;
}

//--------------------------------------------------------------------
bool ForecastState::prune(const time_t &genTime)
{
  if (genTime == _genTime)
  {
    int npruned = 0;
    std::vector<LeadStatus_t>::iterator i;
    for (i =_leadStatus.begin(); i!=_leadStatus.end(); )
    {
      if (i->fcstTriggered)
      {
	i++;
      }
      else
      {
	i = _leadStatus.erase(i);
	++npruned;
      }
    }
    if (npruned > 0)
    {
      LOG(DEBUG) << "Pruned " << npruned << " Leads from "
		 << DateTime::strn(_genTime) << " never triggered";
    }
    return true;
  }
  return false;
}

//--------------------------------------------------------------------
void ForecastState::update(const std::vector<time_t> &availableValidTimes1,
			   const std::vector<time_t> &availableValidTimes2)
{
  int updated = 0;
  for (size_t il=0; il<_leadStatus.size(); ++il)
  {
    if (!_leadStatus[il].obs1Available)
    {
      time_t vt = _genTime + _leadStatus[il].leadSeconds;
      for (size_t ia=0; ia<availableValidTimes1.size(); ++ia)
      {
	if (vt == availableValidTimes1[ia])
	{
	  _leadStatus[il].obs1Available = true;
	  updated += 1;
	  break;
	}
      }
    }
    if (!_leadStatus[il].obs2Available)
    {
      time_t vt = _genTime + _leadStatus[il].leadSeconds;
      for (size_t ia=0; ia<availableValidTimes2.size(); ++ia)
      {
	if (vt == availableValidTimes2[ia])
	{
	  _leadStatus[il].obs2Available = true;
	  updated += 1;
	  break;
	}
      }
    }
  }
  if (updated > 0)
  {
    LOG(DEBUG_VERBOSE) << "Updated " << updated << " lead time status to true for"
	       << DateTime::strn(_genTime);
  }
}

//--------------------------------------------------------------------
void ForecastState::print(void) const
{
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    string s, s2, s3;
    if (_leadStatus[i].obs1Available)
    {
      s = "Obs1     Available";
    }
    else
    {
      s = "Obs1 Not Available";
    }
    if (_leadStatus[i].obs2Available)
    {
      s2 = "Obs2     Available";
    }
    else
    {
      s2 = "Obs2 Not Available";
    }
    if (_leadStatus[i].fcstTriggered)
    {
      s3 = "Fcst     Available";
    }
    else
    {
      s3 = "Fcst Not Available";
    }
    printf("%s %s %s gt:%slt:%d\n", s.c_str(), s2.c_str(), s3.c_str(),
	   DateTime::strn(_genTime).c_str(), _leadStatus[i].leadSeconds);
  }
}

//--------------------------------------------------------------------
std::string ForecastState::sprintSummary(void) const
{
  int numObs1=0, numObs2=0, numTrigger=0;
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    if (_leadStatus[i].obs1Available)
    {
      ++numObs1;
    }
    if (_leadStatus[i].obs2Available)
    {
      ++numObs2;
    }
    if (_leadStatus[i].fcstTriggered)
    {
      ++numTrigger;
    }
  }
  char buf[1000];
  sprintf(buf, "%s numObs1:%d  numObs2:%d numLead:%d", DateTime::strn(_genTime).c_str(),
	  numObs1, numObs2, numTrigger);
  string ret = buf;
  return ret;
}
  
//--------------------------------------------------------------------
bool ForecastState::isComplete(const time_t &genTime, int maxAgeSeconds) const
{
  bool complete = true;
  int numMissing = 0;
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    if (!_leadStatus[i].obs1Available || !_leadStatus[i].obs2Available ||
	!_leadStatus[i].fcstTriggered)
    {
      complete = false;
      numMissing++;
    }
  }
  if (complete)
  {
    return true;
  }

  if (_genTime < genTime - maxAgeSeconds)
  {
    LOG(DEBUG) << "state gen time " << DateTime::strn(_genTime)
	       << " too old compared to input " << DateTime::strn(genTime)
	       << " use as is, missing " << numMissing << " things";
    return true;
  }
  else
  {
    return false;
  }
}
  
//--------------------------------------------------------------------
bool ForecastState::hasLargestLead(void) const
{
  bool complete = true;
  int numMissing = 0;
  size_t biggestMissing = 0;
  size_t largestIndex = _leadStatus.size()-1; // assume in order
  for (size_t i=0; i<_leadStatus.size(); ++i)
  {
    if (!_leadStatus[i].obs1Available || !_leadStatus[i].obs2Available ||
	!_leadStatus[i].fcstTriggered)
    {
      complete = false;
      biggestMissing = i;
      numMissing++;
    }
  }
  if (complete)
  {
    return true;
  }
  else
  {
    if (biggestMissing < largestIndex)
    {
      LOG(DEBUG) << "Forecast at " << DateTime::strn(_genTime) << " has "
		 << numMissing << " missing obars, but largest is present";
      return true;
    }
    else
    {
      return false;
    }
  }
}
