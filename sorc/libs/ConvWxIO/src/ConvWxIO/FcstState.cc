// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file FcstState.cc
 */
#include <vector>
#include <cstring>
#include <ConvWxIO/FcstState.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::pair;
using std::vector;
using std::string;

/**
 * Maximum printed lead times per line
 */
static const int MAX_PRINTS_PER_LINE=6;

//----------------------------------------------------------------
FcstState::FcstState() :
  pGenTime(convWx::BAD_TIME),
  pLeadStatus(),
  pParm()
{
}

//----------------------------------------------------------------
FcstState::FcstState(const time_t &gt, const ParmFcst &parms) :
  pGenTime(gt),
  pLeadStatus(),
  pParm(parms)
{
  // fill in initial state of false for all lead times from p.
  pLeadStatus.reserve(parms.nlead());
  for (int i=0; i<parms.nlead(); ++i)
  {
    int lt = parms.ithLeadSeconds(i);
    pair<int,bool> p(lt, false);
    pLeadStatus.push_back(p);
  }

  // now set state to an accurate value using server
  pUpdate();
}

//----------------------------------------------------------------
FcstState::FcstState(const time_t &gt, const ParmFcst &parms,
		     const vector<pair<int,bool> > &status) :
  pGenTime(gt),
  pLeadStatus(status),
  pParm(parms)
{
}

//----------------------------------------------------------------
FcstState::~FcstState()
{
}

//----------------------------------------------------------------
string FcstState::sprint(void) const
{
  return pPrint();
}

//----------------------------------------------------------------
void FcstState::print(void) const
{
  pPrint(LogStream::PRINT);
}

//----------------------------------------------------------------
void FcstState::print(LogStream::Log_t severity) const
{
  pPrint(severity);
}


//----------------------------------------------------------------
int FcstState::numUnavailableFcsts(void) const
{
  int nmissing = 0;
  for (int i=0; i<static_cast<int>(pLeadStatus.size()); ++i)
  {
    if (pLeadStatus[i].second == false)
    {
      ++nmissing;
    }
  }
  return nmissing;
}

//----------------------------------------------------------------
int FcstState::numUnavailableLatencyAdjustedFcsts(const time_t &outputGt,
						  const ParmFcst &parm) const
{
  int nmissing = 0;

  // set latency
  int latency = outputGt - pGenTime;

  // loop through input parameter forecasts
  for (int i=0; i<parm.nlead(); ++i)
  {

    // add the latency
    int lt = parm.ithLeadSeconds(i) + latency;

    //look for this lead in internal state
    int k = pMatch(lt);
    if (k != convWx::BAD_INDEX)
    {
      // use that to determine status and inc count if missing
      if (pLeadStatus[k].second == false)
      {
	++nmissing;
      }
    }
    else
    {
      // off the end or not in the set.. must! be missing
      ++nmissing;
    }
  }
  return nmissing;
}

//----------------------------------------------------------------
bool FcstState::allFcstsAreAvailable(void) const
{
  for (int i=0; i<static_cast<int>(pLeadStatus.size()); ++i)
  {
    if (!pLeadStatus[i].second)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------
bool FcstState::allLatencyAdjustedFcstsAreAvailable(const time_t &outputGt,
						    const ParmFcst &parm) const
{
  // check how many are unavailable in latency adjusted outputs.
  if (numUnavailableLatencyAdjustedFcsts(outputGt, parm) == 0)
  {
    // also check internal availability, to give a debug informational message
    if (numUnavailableFcsts() > 0)
    {
      ILOGF(DEBUG, "the case I want %d missing",
	    numUnavailableFcsts());
    }
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
bool FcstState::noFcstsAreAvailable(void) const
{
  int nlead = pParm.nlead();
  int nmissing = numUnavailableFcsts();
  return nmissing >= nlead;
}

//----------------------------------------------------------------
bool FcstState::noLatencyAdjustedFcstsAreAvailable(const time_t &outputGt,
						   const ParmFcst &parm) const
{
  int nlead = parm.nlead();
  int nmissing = numUnavailableLatencyAdjustedFcsts(outputGt, parm);
  return nmissing >= nlead;
}

//----------------------------------------------------------------
bool FcstState::getLatencyAdjustedGrid(const time_t &outputGt,
				       const ParmFcst &parm,
				       const ParmProjection &proj,
				       const int ltIndex,
				       double &latencyHours,
				       FcstGrid &outputGrid) const
{
  // figure out latency and set return value
  int latency = outputGt - pGenTime;
  latencyHours = static_cast<double>(latency)/convWx::DOUBLE_SECS_PER_HOUR;

  // get lead seconds from input parm object
  int l = parm.ithLeadSeconds(ltIndex);

  // add the latency to get internal lead time
  int lt = l + latency;

  ILOGF(DEBUG_VERBOSE, "Loading index=%d", ltIndex);
  ILOG(DEBUG_VERBOSE, "getting available");
  ILOGF(DEBUG_VERBOSE, "lead (input) = %d", lt);
  ILOGF(DEBUG_VERBOSE, "lead (output)=%d", l);

  bool available;

  // see if the internal lead exists or not.
  int k = pMatch(lt);
  if (k != convWx::BAD_INDEX)
  {
    // set availability based on internal state
    available = pLeadStatus[k].second;
    ILOGF(DEBUG_VERBOSE, "index=%d, status=%d", k, available);
  }
  else
  {
    // set unavailable since not in the internal state
    ILOGF(DEBUG_VERBOSE, "no index for lead %d", lt);
    available = false;
  }

  // fill in a grid (if available) or make an all missing one (if not)
  return pFillGrid(available, lt, proj, LogStream::DEBUG_VERBOSE, outputGrid);
}

//----------------------------------------------------------------
bool FcstState::retrieveAllFcsts(const ParmProjection &proj,
				 vector<FcstGrid> &outGrids) const
{
  bool stat = false;

  // prepare to fill in the return vector
  outGrids.clear();

  ILOG(DEBUG, "getting available");

  // for each internal lead time
  for (int i=0; i<pParm.nlead(); ++i)
  {
    int lt = pParm.ithLeadSeconds(i);
    ILOGF(DEBUG, "getting available %d", lt);

    InterfaceLL::doRegister("FcstState::loadingForecast");

    // get its availability
    int k = pMatch(lt);
    bool available;
    if (k != convWx::BAD_INDEX)
    {
      available = pLeadStatus[k].second;
    }
    else
    {
      available = false;
    }

    // fill in a grid (if available) or make an all missing one (if not)
    FcstGrid grid;
    if (pFillGrid(available, lt, proj, LogStream::DEBUG_VERBOSE, grid))
    {
      stat = true;
    }
    outGrids.push_back(grid);
  }

  // true if at least one forecast was loaded.
  return stat;
}

//----------------------------------------------------------------
bool FcstState::updateAvailability(void)
{
  // is anything in need of checking for availability?
  if (!allFcstsAreAvailable())
  {
    // yes..
    return pUpdate();
  }
  return false;
}

//----------------------------------------------------------------
bool FcstState::setFcstUnavailable(const int ltSeconds)
{
  // find match index to input lead seconds
  int k = pMatch(ltSeconds);
  if (k != convWx::BAD_INDEX)
  {
    // set that one to false
    pLeadStatus[k].second = false;
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
bool FcstState::setIthFcstUnavailable(const int i)
{
  if (i < 0 || i >= static_cast<int>(pLeadStatus.size()))
  {
    return false;
  }
  pLeadStatus[i].second = false;
  return true;
}

//----------------------------------------------------------------
bool FcstState::fcstIsAvailable(const int ltSeconds) const
{
  // find match index to lead seconds
  int k = pMatch(ltSeconds);
  if (k == convWx::BAD_INDEX)
  {
    // no match means not available for sure
    return false;
  }
  else
  {
    // pull status
    return pLeadStatus[k].second;
  }
}

//----------------------------------------------------------------
bool FcstState::ithFcstIsAvailable(const int i) const
{
  if (i < 0 || i >= static_cast<int>(pLeadStatus.size()))
  {
    // ith out of range means not available
    return false;
  }
  else
  {
    // pull status
    return pLeadStatus[i].second;
  }
}

//----------------------------------------------------------------
bool FcstState::getIthFcstStatus(const int i, double &leadHour,
				 bool &available) const
{
  if (i >= 0 && i < static_cast<int>(pLeadStatus.size()))
  {
    leadHour =
      static_cast<double>(pLeadStatus[i].first)/convWx::DOUBLE_SECS_PER_HOUR;
    available = pLeadStatus[i].second;
    return true;
  }
  else
  {
    return false;
  }
}


//----------------------------------------------------------------
void FcstState::getLogInfo(string &gen, string &missing) const
{
  // create the wanted return generation time 'hh:mm'
  int y, m, d, h, min, sec;
  ConvWxTime::expandTime(pGenTime, y, m, d, h, min, sec);
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "%02d:%02d", h, min);
  gen = buf;

  // create a list of floating point lead time hours for unavailable forecasts
  missing = "";
  for (int i=0; i<static_cast<int>(pLeadStatus.size()); ++i)
  {
    if (!pLeadStatus[i].second)
    {
      double d =
	static_cast<double>(pLeadStatus[i].first)/convWx::DOUBLE_SECS_PER_HOUR;
      sprintf(buf, "%5.2lf ", d);
      missing += buf;
    }      
  }
  if (missing.empty())
  {
    missing = "None";
  }
}

//----------------------------------------------------------------
void FcstState::getLogInfo(const time_t &outputGt, const ParmFcst &parm,
			   string &gt, string &latency,
			   string &missing, string &outGt,
			   string &outMissing) const

{
  char buf[convWx::ARRAY_LEN_SHORT];

  // create return gt and outGt strings "hh:mm"
  int y, m, d, h, min, sec;
  ConvWxTime::expandTime(pGenTime, y, m, d, h, min, sec);
  sprintf(buf, "%02d:%02d", h, min);
  gt = buf;

  ConvWxTime::expandTime(outputGt, y, m, d, h,min,sec);
  sprintf(buf, "%02d:%02d", h, min);
  outGt = buf;

  // create return latency hours string
  int latencySec = outputGt - pGenTime;
  double latencyHours = 
    static_cast<double>(latencySec)/convWx::DOUBLE_SECS_PER_HOUR;
  sprintf(buf, "%.2lf", latencyHours);
  latency = buf;

  // create the list of missing lead hours
  missing = "";
  for (int i=0; i<static_cast<int>(pLeadStatus.size()); ++i)
  {
    if (!pLeadStatus[i].second)
    {
      double d =
	static_cast<double>(pLeadStatus[i].first)/convWx::DOUBLE_SECS_PER_HOUR;
      sprintf(buf, "%5.2lf ", d);
      missing += buf;
    }
  }
  if (missing.empty())
  {
    missing = "None";
  }

  // create the list of missing output lead hours
  outMissing = "";
  for (int i=0; i<parm.nlead(); ++i)
  {
    // figure out the internal index to check
    double lth = parm.ithLeadHours(i);
    int lt = parm.ithLeadSeconds(i) + latencySec;
    int k = pMatch(lt);
    if (k != convWx::BAD_INDEX)
    {
      if (!pLeadStatus[k].second)
      {
	sprintf(buf, "%5.2lf ", lth);
	outMissing += buf;
      }
    }
    else
    {
      sprintf(buf, "%5.2lf ", lth);
      outMissing += buf;
    }
  }
  if (outMissing.empty())
  {
    outMissing = "None";
  }
}

//----------------------------------------------------------------
bool FcstState::pUpdate(void)
{
  bool ret = false;
  InterfaceLL::doRegister("FcstState::Update");

  ILOG(DEBUG_VERBOSE, "before get available lead");

  // get the available lead times for this generation time (ask server)
  vector<int> l = pParm.getAvailableLeadTimes(pGenTime);

  ILOG(DEBUG_VERBOSE, "after get available lead");

  // for each such lead time
  for (int i=0; i<static_cast<int>(l.size()); ++i)
  {
    // get index from state, if <0, we are not interested in the lead time
    int k = pMatch(l[i]);
    if (k != convWx::BAD_INDEX)
    {
      if (pLeadStatus[k].second == false)
      {
	// change occurs here (false to true)
	ret = true;
	ILOGF(DEBUG, "%s lt=%d newly available",
	      ConvWxTime::stime(pGenTime).c_str(), pLeadStatus[k].first);
	pLeadStatus[k].second = true;
      }
    }
  }
  // note this method assumes nothing ever goes from available
  // to not available in the time frames of real time change.
  // might not be true for L.L.
  // might want to beef up this method therefore.
  ILOG(DEBUG_VERBOSE, "At end");
  return ret;
}

//----------------------------------------------------------------
int FcstState::pMatch(const int ltSec) const
{
  for (int k=0; k<static_cast<int>(pLeadStatus.size()); ++k)
  {
    if (ltSec == pLeadStatus[k].first)
    {
      return k;
    }
  }
  return convWx::BAD_INDEX;
}

//----------------------------------------------------------------
string FcstState::pPrint(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "----Gen:%s------\n", ConvWxTime::stime(pGenTime).c_str());
  int count = 0;
  for (int i=0; i<static_cast<int>(pLeadStatus.size()); ++i)
  {
    string a;
    if (pLeadStatus[i].second)
    {
      a = "[  :";
    }
    else
    {
      a = "[NO:";
    }
    sprintf(buf+strlen(buf), "%s%5.2lf]", a.c_str(),
	    static_cast<double>(pLeadStatus[i].first)/
	    convWx::DOUBLE_SECS_PER_HOUR);
    if (++count > MAX_PRINTS_PER_LINE)
    {
      sprintf(buf+strlen(buf), "\n");
      count = 0;
    }
    else
    {
      sprintf(buf+strlen(buf), " ");
    }
  }
  if (count != 0)
  {
    sprintf(buf + strlen(buf), "\n");
  }
  string ret = buf;
  return ret;
}

//----------------------------------------------------------------
void FcstState::pPrint(LogStream::Log_t severity) const
{
  ILOGFV(severity, "----Gen:%s------", ConvWxTime::stime(pGenTime).c_str());
  int count = 0;
  for (int i=0; i<static_cast<int>(pLeadStatus.size()); ++i)
  {
    string a;
    if (pLeadStatus[i].second)
    {
      a = "[  :";
    }
    else
    {
      a = "[NO:";
    }
    ILOG_ACCUMULATEF("%s%5.2lf]",a.c_str(),
		     static_cast<double>(pLeadStatus[i].first)/
		     convWx::DOUBLE_SECS_PER_HOUR);
    if (++count > MAX_PRINTS_PER_LINE)
    {
      ILOG_ACCUM(severity);
      count = 0;
    }
    else
    {
      // hack to get it to work
      int x;
      ILOG_ACCUMULATEF(" ", x);
    }
  }
  if (count != 0)
  {
    ILOG_ACCUM(severity);
  }
}

//----------------------------------------------------------------
bool FcstState::pFillGrid(const bool available, const int lt,
			  const ParmProjection &proj,
			  const LogStream::Log_t severity,
			  FcstGrid &outGrid) const
{
  if (available)
  {
    // try load the forecast
    ILOGV(severity, "loading now");
    if (pParm.loadFcstData(pGenTime, lt, proj, outGrid))
    {
      // got it, we are done
      ILOGFV(severity, "Loaded input=%s+%d",
	    ConvWxTime::stime(pGenTime).c_str(), lt);
      return true;
    }
    else
    {
      // what happened?
      ILOGF(ERROR, "Couldn't load %s+%d when available",
	    ConvWxTime::stime(pGenTime).c_str(), lt);
    }
  }

  // create an 'all missing' object to return
  outGrid = FcstGrid(pGenTime, lt, pParm.pField, pParm.pUnits, proj.pNx,
		     proj.pNy, convWx::FCST_MISSING);
  ILOGFV(severity, "set all missing..no availability for %s+%d",
	ConvWxTime::stime(pGenTime).c_str(), lt);
  return false;
}
