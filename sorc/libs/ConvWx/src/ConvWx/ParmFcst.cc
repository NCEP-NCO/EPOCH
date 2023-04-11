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
 * @file ParmFcst.cc
 */

//----------------------------------------------------------------
#include <string>
#include <vector>
#include <algorithm>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ConvWxTime.hh>
using std::string;
using std::vector;
using std::pair;

//----------------------------------------------------------------
ParmFcst::ParmFcst(void) :
  pOk(false),
  pName("not_set"),
  pDescription("not_set"),
  pIsRequired(false),
  pUrl("not_set"),
  pField(),
  pUnits(),
  pLtHour0(0),
  pLtHour1(0),
  pLtDeltaHours(0),
  pRemap(false),
  pGenFrequencySeconds(15*convWx::SECS_PER_MINUTE)
{
}

//----------------------------------------------------------------
ParmFcst::ParmFcst(const string &name, const string &descr,
		   const bool isRequired) :
  pOk(true),
  pName(name),
  pDescription(descr),
  pIsRequired(isRequired),
  pUrl("not_set"),
  pField(),
  pUnits(),
  pLtHour0(0),
  pLtHour1(0),
  pLtDeltaHours(0),
  pRemap(false),
  pGenFrequencySeconds(15*convWx::SECS_PER_MINUTE)
{
}

//----------------------------------------------------------------
ParmFcst::ParmFcst(const string &name, const string &descr,
		   const bool isRequired, const string &url,
		   const string &field, const string &units,
		   const double hour0, const double hour1,
		   const double deltaHours, const bool remap,
		   const int genDeltaMinutes) :
  pOk(true),
  pName(name),
  pDescription(descr),
  pIsRequired(isRequired),
  pUrl(url),
  pField(field),
  pUnits(units),
  pLtHour0(hour0),
  pLtHour1(hour1),
  pLtDeltaHours(deltaHours),
  pRemap(remap),
  pGenFrequencySeconds(genDeltaMinutes*convWx::SECS_PER_MIN)
{
  pOk = pIsPeriodic();
  if (!pOk)
  {
    exit(1);
  }
}

#ifdef NOTDEF
//----------------------------------------------------------------
ParmFcst::ParmFcst(const string &name, const string &descr,
		   const bool isRequired, const string &url,
		   const string &field, const string &units,
		   const double hour0, const double hour1,
		   const double deltaHours, const bool remap) :
  pOk(true),
  pName(name),
  pDescription(descr),
  pIsRequired(isRequired),
  pUrl(url),
  pField(field),
  pUnits(units),
  pLtHour0(hour0),
  pLtHour1(hour1),
  pLtDeltaHours(deltaHours),
  pRemap(remap),
  pGen15(false),
  pGenUnknown(true)
{
}
#endif

//----------------------------------------------------------------
ParmFcst::~ParmFcst()
{
}

//----------------------------------------------------------------
void ParmFcst::setDetails(const string &url, const string &field,
			  const string &units, const double hour0,
			  const double hour1, const double deltaHours,
			  const bool remap,  const int genDeltaMinutes)
{
  pUrl = url;
  pField = field;
  pUnits = units;
  pLtHour0 = hour0;
  pLtHour1 = hour1;
  pLtDeltaHours = deltaHours;
  pRemap = remap;
  pGenFrequencySeconds = genDeltaMinutes*convWx::SECS_PER_MIN;
  pOk = pIsPeriodic();
  if (!pOk)
  {
    exit(1);
  }
}

#ifdef NOTDEF
//----------------------------------------------------------------
void ParmFcst::setDetails(const string &url, const string &field,
			  const string &units, const double hour0,
			  const double hour1, const double deltaHours,
			  const bool remap)
{
  pUrl = url;
  pField = field;
  pUnits = units;
  pLtHour0 = hour0;
  pLtHour1 = hour1;
  pLtDeltaHours = deltaHours;
  pRemap = remap;
  pGen15 = false;
  pGenUnknown = true;
  pOk = true;
}
#endif

//----------------------------------------------------------------
bool ParmFcst::isLastLt(const int ltSeconds) const
{
  return static_cast<int>(pLtHour1*convWx::DOUBLE_SECS_PER_HOUR) == ltSeconds;
}

//----------------------------------------------------------------
int ParmFcst::nlead(const int lt0, const int lt1) const
{
  double h;
  int num=0;
  for (h=pLtHour0; h<=pLtHour1; h += pLtDeltaHours)
  {
    int lti = static_cast<int>(h*convWx::DOUBLE_SECS_PER_HOUR);
    if (lti >= lt0 && lti <= lt1)
    {
      ++num;
    }
  }
  return num;
}

//----------------------------------------------------------------
int ParmFcst::leadSecondsIndex(const int lt) const
{
  double h;
  int i;
  for (i=0, h=pLtHour0; h<=pLtHour1; h += pLtDeltaHours, ++i)
  {
    int lti = static_cast<int>(h*convWx::DOUBLE_SECS_PER_HOUR);
    if (lti == lt)
    {
      return i;
    }
  }
  return convWx::BAD_INDEX;
}

//----------------------------------------------------------------
vector<double> ParmFcst::getLtHours(void) const
{
  vector<double> ret;
  for (double h=pLtHour0; h<=pLtHour1; h += pLtDeltaHours)
  {
    ret.push_back(h);
  }
  return ret;
}

//----------------------------------------------------------------
std::vector<int> ParmFcst::getLtSeconds(void) const
{
  vector<int> ret;
  for (double h=pLtHour0; h<=pLtHour1; h += pLtDeltaHours)
  {
    ret.push_back(static_cast<int>(h*convWx::DOUBLE_SECS_PER_HOUR));
  }
  return ret;
}

//----------------------------------------------------------------
vector<pair<time_t,int> > ParmFcst::fcstsValidAt(const time_t &vt) const
{
  vector<pair<time_t,int> > ret;

  int lt0 = static_cast<int>(pLtHour0*convWx::DOUBLE_SECS_PER_HOUR);
  int max = static_cast<int>(pLtHour1*convWx::DOUBLE_SECS_PER_HOUR);
  int dlt = static_cast<int>(pLtDeltaHours*convWx::DOUBLE_SECS_PER_HOUR);
  time_t t0 = vt - max;
  t0 = smallestGenGe(t0);

  // put all the appropriate generation times onto the list in ascending order
  // (those with a lead time that is in range)
  vector<time_t> gtList;
  while (t0 <= vt)
  {
    int lt = vt - t0;
    // is this lead time a valid lead time for this object?
    if (lt >= lt0 && lt <= max)
    {
      double dk = static_cast<double>(lt-lt0)/static_cast<double>(dlt);
      int k = static_cast<int>(dk);
      if (k*dlt + lt0 == lt)
      {
	gtList.push_back(t0);
      }
      else
      {
	ILOGF(WARNING, "lead %d not on list", lt);
      }
    }
    t0 += pGenFrequencySeconds;
  }

  // pull back out in reverse order for return
  vector<time_t>::reverse_iterator ig;
  for (ig=gtList.rbegin(); ig!=gtList.rend(); ++ig)
  {
    int lt = vt - *ig;
    pair<time_t, int> p(*ig, lt);
    ret.push_back(p);
  }
  return ret;
}


//----------------------------------------------------------------
vector<time_t> ParmFcst::genPossible(const time_t t0,
				     const int latencyHours[2],
				     const time_t oldest) const
{
  vector<time_t> ret;

  // this is the absolute largest generation time we could have.
  time_t t1 = t0 - static_cast<int>(latencyHours[0]*
				    convWx::DOUBLE_SECS_PER_HOUR);

  // adjust to allowed generation times
  t1 = largestGenLe(t1);

  // this is absolute smallest generation time we could have
  time_t tmin = t0 - static_cast<int>(latencyHours[1]*
				      convWx::DOUBLE_SECS_PER_HOUR);

  // adjust to allowed generation times
  tmin = smallestGenGe(tmin);

  // adjust using oldest
  if (tmin < oldest)
  {
    tmin = oldest;
  }

  // now build up all generation times in this ranges
  time_t ti;

  for (ti=tmin; ti<=t1; ti += pGenFrequencySeconds)
  {
    ret.push_back(ti);
  }

  ILOG(DEBUG_VERBOSE, "Range of possible gen times:");
  for (int i=0; i<static_cast<int>(ret.size()); ++i)
  {
    ILOG(DEBUG_VERBOSE,  ConvWxTime::stime(ret[i]).c_str());
  }
  return ret;
}

//----------------------------------------------------------------
time_t ParmFcst::smallestGenGe(time_t t) const
{
  // pull  out year/month/day of the time
  int y, m, d, h, min, sec;
  ConvWxTime::expandTime(t, y, m, d, h, min, sec);

  // t0 = earliest gen time in this day  ASSUMED TO BE 0,0,0
  time_t t0 = ConvWxTime::setTime(y, m, d, 0, 0, 0);
  while (t0 < t)
  {
    t0 += pGenFrequencySeconds;
  }
  // here t0 is the smallest gen time >= t
  return t0;
}

//----------------------------------------------------------------
time_t ParmFcst::largestGenLe(time_t t) const
{
  int y, m, d, h, min, sec;

  // pull out year/month/day of the time
  ConvWxTime::expandTime(t, y, m, d, h, min, sec);

  // t0 = earliest lead time in this day ASSUMED TO BE 0,0,0
  time_t t0 = ConvWxTime::setTime(y, m, d, 0, 0, 0);
  // t1 = gen time one beyond t0
  time_t t1 = t0 + pGenFrequencySeconds;
  while (t1 <= t)
  {
    t0 = t1;
    t1 += pGenFrequencySeconds;
  }
  // here t1 = 1st gen time > t, so t0 = last gen time <= t
  return t0;
}

//----------------------------------------------------------------
vector<int> ParmFcst::hourMinuteSeconds(void) const
{
  vector<int> ret;

  // take any old year/month/dayto build this up, starting at h,m,s=0,0,0
  int secondsIntoDay = 0;
  time_t t0 = ConvWxTime::setTime(2012, 07, 12, 0, 0, 0);
  time_t t1 = ConvWxTime::setTime(2012, 07, 13, 0, 0, 0);
  while (t0 < t1)
  {
    ret.push_back(secondsIntoDay);
    t0 += pGenFrequencySeconds;
    secondsIntoDay += pGenFrequencySeconds;
  }
  return ret;
}

//----------------------------------------------------------------
int ParmFcst::nearestLeadSeconds(int lt) const
{
  double minDiff = 0.0;
  int bestLt = 0;
  bool first = true;
  double dlt = static_cast<double>(lt);
  for (double h=pLtHour0; h<=pLtHour1; h += pLtDeltaHours)
  {
    double lts = h*convWx::DOUBLE_SECS_PER_HOUR;
    if (first)
    {
      minDiff = fabs(lts - dlt);
      bestLt = static_cast<int>(lts);
      first = false;
    }
    else
    {
      double x = fabs(lts - dlt);
      if (x < minDiff)
      {
	minDiff = x;
	bestLt = static_cast<int>(lts);
      }
    }
    if (minDiff == 0.0)
    {
      return bestLt;
    }
  }
  if (first)
  {
    ILOG(ERROR, "Empty lead time list");
    return -1;
  }
  else
  {
    return bestLt;
  }
}

//----------------------------------------------------------------
int
ParmFcst::nearestLeadSecondsValidTime(const time_t &validTime,
				      const time_t &generationTime) const
{
  double minDiff = 0.0;
  int bestLt = 0;
  bool first = true;
  for (double h=pLtHour0; h<=pLtHour1; h += pLtDeltaHours)
  {
    int lts = static_cast<int>(h*convWx::DOUBLE_SECS_PER_HOUR);
    time_t vt = generationTime +
      static_cast<int>(h*convWx::DOUBLE_SECS_PER_HOUR);
    double diff = fabs(static_cast<double>(validTime - vt));
    if (first)
    {
      minDiff = diff;
      bestLt = lts;
      first = false;
    }
    else
    {
      if (diff < minDiff)
      {
	minDiff = diff;
	bestLt = lts;
      }
    }
    if (minDiff == 0.0)
    {
      return bestLt;
    }
  }
  if (first)
  {
    ILOG(ERROR, "Empty lead time list");
    return -1;
  }
  else
  {
    return bestLt;
  }
}

//----------------------------------------------------------------
void ParmFcst::pAdjustHmsDiff(const time_t &gt, const double hms0,
			      double &bestDiff, bool &first) const
{
  double hms = ConvWxTime::getTimeFraction(gt);
  double diffi = hms0 - hms;
  if (diffi >= 0)
  {
    ILOGF(DEBUG_VERBOSE, "checking %s", ConvWxTime::stime(gt).c_str());
    if (first)
    {
      bestDiff = diffi;
      first = false;
      ILOGF(DEBUG_VERBOSE, "Initialize min delta to %lf", bestDiff);
    }
    else
    {
      if (diffi < bestDiff) 
      {
	bestDiff = diffi;
	ILOGF(DEBUG_VERBOSE, "Adjusting min delta to %lf", bestDiff);
      }
    }
  }
  else
  {
    ILOGF(DEBUG_VERBOSE, "skipping %s too new",
	  ConvWxTime::stime(gt).c_str());
  }
}

//----------------------------------------------------------------
bool ParmFcst::pIsPeriodic(void) const
{
  // take 24 hours and see if the gen delta is an exact multiple of that
  // Do this the dumb way to make sure it works, using an arbitrary day

  time_t t0 = ConvWxTime::setTime(2012, 07, 12, 0, 0, 0);
  time_t t1 = ConvWxTime::setTime(2012, 07, 13, 0, 0, 0);

  while (t0 < t1)
  {
    if (pGenFrequencySeconds == 0)
    {
      ILOG(ERROR, "Trying to check periodicity using gen frequency 0");
      return false;
    }
    t0 += pGenFrequencySeconds;
  }
  if (t0 != t1)
  {
    ILOGF(ERROR, "Non periodic gen time delta seconds %d, url=%s",
	  pGenFrequencySeconds, pUrl.c_str());
    return false;
  }
  else
  {
    return true;
  }
}
