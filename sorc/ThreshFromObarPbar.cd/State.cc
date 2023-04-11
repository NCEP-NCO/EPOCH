/**
 * @file State.cc
 */
#include "State.hh"
#include <Epoch/SpdbObsHandler.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

//--------------------------------------------------------------------
State::State(const ParmsThreshFromObarPbar &parms) : _parms(parms),
						     _oldestGen(0),
						     _newestGen(0),
						     _leadSecondsMin(0),
						     _leadSecondsMax(0)
{
}

//-------------------------------------------------------------------
void State::add(const time_t &genTime, const std::vector<int> &leadSeconds)
{
  LOG(DEBUG) << "Adding " << DateTime::strn(genTime) << " to state";
  if (_oldestGen == 0)
  {
    _oldestGen = _newestGen = genTime;
    // here we'd be purging some older stuff
  }
  else
  {
    // assume ascending
    _newestGen = genTime;
  }
  _fcst.push_back(ForecastState(genTime, leadSeconds));

  // assume all inputs have same lead seconds
  _leadSecondsMin = leadSeconds[0];
  _leadSecondsMax = *(leadSeconds.rbegin());

  LOG(DEBUG_VERBOSE) << "State";
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    LOG(DEBUG_VERBOSE) << _fcst[i].sprintSummary();
  }
}

//-------------------------------------------------------------------
void State::addTrigger(const time_t &genTime, int leadSeconds)
{
  // get ALL valid time that are in range
  SpdbObsHandler obs1(_parms._obarSpdb1);
  SpdbObsHandler obs2(_parms._obarSpdb2);

  vector<time_t> vtimes1 =
    obs1.timesInRange(_oldestGen + _leadSecondsMin - 1,
		     _newestGen + _leadSecondsMax + 1);

  vector<time_t> vtimes2 =
    obs2.timesInRange(_oldestGen + _leadSecondsMin - 1,
		     _newestGen + _leadSecondsMax + 1);

  // update all the forecast states using these
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    _fcst[i].addTrigger(genTime, leadSeconds);
    _fcst[i].update(vtimes1, vtimes2);
  }
}

//-------------------------------------------------------------------
void State::prune(const time_t &genTime)
{
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (_fcst[i].prune(genTime))
    {
      return;
    }
  }
}

//-------------------------------------------------------------------
void State::removeIfCompleted(const time_t &currentGen)
{
  std::vector<ForecastState>::iterator i;
  for (i=_fcst.begin(); i!=_fcst.end(); )
  {
    if (i->isComplete(currentGen, _parms._maxIncompleteSeconds))
    {
      LOG(DEBUG) << "Removing completed " << DateTime::strn(i->getGenTime());
      i = _fcst.erase(i);
    }
    else if (i->hasLargestLead())
    {
      LOG(DEBUG) << "Removing completed (with missing obars) " << DateTime::strn(i->getGenTime());
      i = _fcst.erase(i);
    }
    else
    {
      i++;
    }
  }
}

//-------------------------------------------------------------------
void State::print(void) const
{
  LOG(DEBUG) << "Oldest gen:" << DateTime::strn(_oldestGen);
  LOG(DEBUG) << "Newest gen:" << DateTime::strn(_newestGen);
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    _fcst[i].print();
  }
}
  
