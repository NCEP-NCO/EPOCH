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
 * @file ConvWxTime.cc
 * @brief 
 */

#include <ConvWxIO/ILogMsg.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxTime.hh>
#include <cstdio>
#include <unistd.h>
using std::string;


//------------------------------------------------------------------
/**
 * Static function to calculate calendar day from julian date, 
 * as Taken from Computer Language- Dec 1990, pg 58
 *
 * @param[in] jdate Julian date
 * @param[out] day  Calendar day
 * @param[out] month  Calendar month
 * @param[out] year  Calendar year
 */
static void sCalendarDate(long jdate, int &day, int &month, int &year);

//------------------------------------------------------------------
/**
 * Calc the Julian calendar Day Number
 * As Taken from Computer Language- Dec 1990, pg 58
 */
static long sJulianDate(int day, int month, int year);

//------------------------------------------------------------------
static bool sTokenizeString( const std::string &strWhen,
			     int &yyyy, int &mm, int &dd,
			     int &hh,  int &min, int &ss );

//------------------------------------------------------------------
string ConvWxTime::stime(const time_t &ot)
{
  DateTime_t mtime;
  mtime.unixTime = ot;
  pConvertFromUtime(mtime);
  char str[convWx::ARRAY_LEN_VERY_SHORT];
  sprintf(str, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
	  mtime.year, mtime.month, mtime.day,
	  mtime.hour, mtime.min, mtime.sec);
  string ret = str;
  return ret;
}

//------------------------------------------------------------------
string ConvWxTime::xtime(const time_t &ot)
{
  DateTime_t mtime;
  mtime.unixTime = ot;
  pConvertFromUtime(mtime);
  char str[convWx::ARRAY_LEN_VERY_SHORT];
  sprintf(str, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
	  mtime.year, mtime.month, mtime.day,
	  mtime.hour, mtime.min, mtime.sec);
  string ret = str;
  return ret;
}  

//------------------------------------------------------------------
string ConvWxTime::shortStime(const time_t &ut, const bool showSeconds)
{
  DateTime_t dtime;
  dtime.unixTime = ut;
  pConvertFromUtime(dtime);
  static const string mname[13] = {"Jan", "Feb", "Mar", "Apr", "May",
				   "Jun", "Jul", "Aug", "Sep", "Oct",
				   "Nov", "Dec", "???"};
  char buf[convWx::ARRAY_LEN_SHORT];
  int m = dtime.month - 1;
  if (m < 0)
  {
    m = 12;
  }
  if (m > 11)
  {
    m = 12;
  }
  if (showSeconds)
  {
    sprintf(buf, "%02d %s,%02d:%02d:%02d", dtime.day, mname[m].c_str(), 
	    dtime.hour, dtime.min, dtime.sec);
  }
  else
  {
    sprintf(buf, "%02d %s,%02d:%02d", dtime.day, mname[m].c_str(),
	    dtime.hour, dtime.min);
  }
  string ret = buf;
  return ret;
}

//------------------------------------------------------------------
string ConvWxTime::veryShortStime(const time_t &ut)
{
  DateTime_t dtime;
  dtime.unixTime = ut;
  pConvertFromUtime(dtime);

  char buf[convWx::ARRAY_LEN_VERY_SHORT];
  sprintf(buf, "%02d:%02d:%02d", dtime.hour, dtime.min, dtime.sec);
  string ret = buf;
  return ret;
}

//------------------------------------------------------------------
time_t ConvWxTime::utime(const string &stime)
{
  int y, m, d, h, min, sec;

  if (pParseDateTime(stime, y, m, d, h, min, sec))
  {
    DateTime_t when;
    when.year  = y;
    when.month = m;
    when.day   = d;
    when.hour  = h;
    when.min   = min;
    when.sec   = sec;
    pConvertToUtime(when);
    return when.unixTime;
  }
  else
  {
    ILOGF(ERROR, "Parsing time value %s", stime.c_str());
    return 0;
  }
}

//------------------------------------------------------------------
void ConvWxTime::expandTime(const time_t t, int &y, int &m, int &d, int &h,
			    int &min, int &sec)
{
  DateTime_t dt;
  dt.unixTime = t;
  pConvertFromUtime(dt);
  y = dt.year;
  m = dt.month;
  d = dt.day;
  h = dt.hour;
  min = dt.min;
  sec = dt.sec;
}

//------------------------------------------------------------------
int ConvWxTime::getYear(const time_t ut)
{
  DateTime_t dt;
  dt.unixTime = ut;
  pConvertFromUtime(dt);
  return dt.year;
}

//------------------------------------------------------------------
int ConvWxTime::getMonth(const time_t ut)
{
  DateTime_t dt;
  dt.unixTime = ut;
  pConvertFromUtime(dt);
  return dt.month;
}

//------------------------------------------------------------------
int ConvWxTime::getDay(const time_t ut)
{
  DateTime_t dt;
  dt.unixTime = ut;
  pConvertFromUtime(dt);
  return dt.day;
}

//------------------------------------------------------------------
int ConvWxTime::getHour(const time_t ut)
{
  DateTime_t dt;
  dt.unixTime = ut;
  pConvertFromUtime(dt);
  return dt.hour;
}  

//------------------------------------------------------------------
int ConvWxTime::getMinute(const time_t ut)
{
  DateTime_t dt;
  dt.unixTime = ut;
  pConvertFromUtime(dt);
  return dt.min;
}  

//------------------------------------------------------------------
time_t ConvWxTime::setTime(const int y, const int m, const int d,
			   const int h, const int min, const int sec)
{
  DateTime_t dt;
  dt.year = y;
  dt.month = m;
  dt.day = d;
  dt.hour = h;
  dt.min = min;
  dt.sec = sec;
  pConvertToUtime(dt);
  return dt.unixTime;
}

//------------------------------------------------------------------
double ConvWxTime::getTimeFraction(const time_t &myTime)
{
  DateTime_t dt;
  dt.unixTime = myTime;
  pConvertFromUtime(dt);
  return static_cast<double>(dt.hour) + 
    (static_cast<double>(dt.min) / convWx::DOUBLE_SECS_PER_MIN) +
    (static_cast<double>(dt.sec) / convWx::DOUBLE_SECS_PER_HOUR);
}

//------------------------------------------------------------------
int ConvWxTime::getDayOfYear(const time_t &myTime)
{
  DateTime_t dt;
  dt.unixTime = myTime;
  pConvertFromUtime(dt);

  DateTime_t dtStart;
  dtStart.unixTime = myTime;
  pConvertFromUtime(dtStart);
  
  dtStart.month = 1;
  dtStart.day = 1;
  dtStart.hour = 0;
  dtStart.min = 0;
  dtStart.sec = 0;
  pConvertToUtime(dtStart);

  int delta = myTime - dtStart.unixTime;
  int doy = 1 + delta/convWx::SECS_PER_DAY;
  return doy;
}

//------------------------------------------------------------------
time_t ConvWxTime::earliestTimeInDay(const time_t &ut)
{
  int year, month, day, hour, minute, second;
  expandTime(ut, year, month, day, hour, minute, second);
  time_t ret = setTime(year, month, day, 0, 0, 0);
  return ret;
}


//------------------------------------------------------------------
time_t ConvWxTime::sleepUntilHourInNextDay(int wantedHour,
					   bool dataTrigger,
					   const std::string &obsDataUrl,
					   const time_t &lastRun)
{
  //
  // Cycle between sleep and checking in with the process mapper until 
  // we reach the proper run hour of the day and processing for the current day
  // has not occurred.
  //

  //
  // Time struct pointer
  //
  time_t currentT;

  time_t lastRunYearMonthDay = earliestTimeInDay(lastRun);
  int lastRunDay = getDay(lastRun);
  time_t currentYearMonthDay = 0;
  int hour = 0;
  do
  {
    InterfaceLL::doRegister("Sleeping");
    sleep(convWx::SECS_PER_MINUTE);

    //
    // current time
    //
    currentT = realTime(dataTrigger, obsDataUrl);
    currentYearMonthDay = earliestTimeInDay(currentT);
    hour = getHour(currentT);
    int min = getMinute(currentT);

    //
    // Log debug message at regular intervals while process is inactive
    // 
    if (min%10 == 0)
    {
      int day = getDay(currentT);
      ILOGF(DEBUG, "Previous run_day %d, current day %d current hour"
	    " %d (run hour is %d)", lastRunDay, day, hour, wantedHour);
    }
  }
  while( wantedHour != hour || lastRunYearMonthDay >= currentYearMonthDay);
  return currentT;
}

//------------------------------------------------------------------
time_t ConvWxTime::realTime(bool dataTrigger,
			     const std::string &obsDataUrl)
{
  if (dataTrigger)
  {
    std::vector<int> gtSubsample;
    time_t t=convWx::BAD_TIME;
    if (InterfaceIO::triggerObs(obsDataUrl, gtSubsample, false, t))
    {
      return t;
    }
    else
    {
      ILOGF(WARNING, "No triggering of %s, use clock time",
	    obsDataUrl.c_str());
      return time(0);
    }
  }
  else
  {
    return time(0);
  }
}

//------------------------------------------------------------------
void ConvWxTime::pConvertFromUtime(DateTime_t &dateTime)
{

  time_t unixTime;
  long day;
  int timeOfDay;
  
  unixTime = dateTime.unixTime;

  timeOfDay = (unixTime % convWx::SECS_PER_DAY);
  day = (unixTime / convWx::SECS_PER_DAY);
  if (unixTime < 0 && timeOfDay != 0)
  {
    day--;
    timeOfDay += convWx::SECS_PER_DAY;
  }
  
  sCalendarDate((convWx::JULIAN_JAN_1_1970+day), dateTime.day, dateTime.month,
		dateTime.year);
  
  dateTime.hour = timeOfDay/convWx::SECS_PER_HOUR;
  dateTime.min = (timeOfDay/convWx::SECS_PER_MINUTE) -
    (dateTime.hour*convWx::SECS_PER_MINUTE);
  dateTime.sec = timeOfDay % convWx::SECS_PER_MINUTE;
}

//------------------------------------------------------------------
void ConvWxTime::pConvertToUtime(DateTime_t &dateTime)
{
  long day, days;
  time_t uTime;

  day = sJulianDate(dateTime.day, dateTime.month, dateTime.year);
  days = day - convWx::JULIAN_JAN_1_1970;
  uTime = (days*convWx::SECS_PER_DAY) + (dateTime.hour*convWx::SECS_PER_HOUR) +
    (dateTime.min*convWx::SECS_PER_MINUTE) + dateTime.sec;
  dateTime.unixTime = uTime;
}

 
//------------------------------------------------------------------
bool ConvWxTime::pParseDateTime(const std::string &strWhen,
				int &year, int &month, int &day,
				int &hour, int &min, int &sec )
{
  //
  // Tokenize the dateTime string
  //
  if (!sTokenizeString( strWhen, year, month, day, hour, min, sec))
  {
    return false;
  }
     
  //
  // Now, do some validity checking
  //
  if ( year <= 0 )
  {
    return false;
  }
  if ( month <= 0 || month > 12 )
  {
    return false;
  }
  if ( day <= 0 || day > 31 )
  {
    return false;
  }
  if ( hour < 0 || hour > 23 )
  {
    return false;
  }
  if ( min < 0 || min > 59 )
  {
    return false;
  }
  if ( sec < 0 || sec > 59 )
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------
static void sCalendarDate(long jdate, int &day, int &month, int &year)
{

  long a, b, c, d, e, z, alpha;

  z = jdate +1;

  // Gregorian reform correction
  if (z < 2299161)
  { 
    a = z; 
  }
  else
  {
    alpha = static_cast<long> ((z - 1867216.25) / 36524.25);
    a = z + 1 + alpha - alpha / 4;
  }

  b = a + 1524;
  c = static_cast<long> ((b - 122.1) / 365.25);
  d = static_cast<long> (365.25 * c);
  e = static_cast<long> ((b - d) / 30.6001);
  day = static_cast<long>(b) - d - static_cast<long> (30.6001 * e);
  month = static_cast<long> ((e < 13.5)? e - 1 : e - 13);
  year = static_cast<long> ((month > 2.5)? (c - 4716) : c - 4715);
}

//------------------------------------------------------------------
static long sJulianDate(int day, int month, int year)
{

  long a, b;
  double yrCorr;

  // correct for negative year
  yrCorr = (year > 0? 0.0: 0.75);

  if(month <=2)
  {
    year--;
    month += 12;
  }

  b=0;

  // Cope with Gregorian Calendar reform
  if(year * 10000.0 + month * 100.0 + day >= 15821015.0)
  {
    a = year / 100;
    b = 2 - a + a / 4;
  }
	
  return
    static_cast<long>((365.25 * year - yrCorr) + 
		      static_cast<long> (30.6001 * (month +1)) +
		      day + 1720994L + b);
}

//------------------------------------------------------------------
static bool sTokenizeString( const std::string &strWhen,
			     int &yyyy, int &mm, int &dd,
			     int &hh,  int &min, int &ss )
{
  const char *sPtr = strWhen.c_str();

  //
  // Initialize the resulting values
  //
  yyyy = mm = dd = hh = min = ss = 0;

  //
  // Year
  //
  if ( sscanf( sPtr, "%4d", &yyyy ) != 1 )
  {
    return false;
  }

  //
  // Month
  //
  sPtr += 4;
  if ( !isdigit( sPtr[0] ))
  {
    sPtr++;
  }
  if ( sscanf( sPtr, "%2d", &mm ) != 1 )
  {
    return false;
  }

  //
  // Day
  //
  sPtr += 2;

  if ( !isdigit( sPtr[0] ))
  {
    sPtr++;
  }
  if ( sscanf( sPtr, "%2d", &dd ) != 1 )
  {
    return false;
  }

  //
  // Hour
  //
  sPtr += 2;
  if ( !isdigit( sPtr[0] ))
  {
    sPtr++;
  }
  if ( sscanf( sPtr, "%2d", &hh ) != 1 )
  {
    return false;
  }

  //
  // Minute
  //
  sPtr += 2;
  if ( !isdigit( sPtr[0] ))
  {
    sPtr++;
  }
  if ( sscanf( sPtr, "%2d", &min ) != 1 )
  {
    return false;
  }

  //
  // Second
  //
  sPtr += 2;
  if ( !isdigit( sPtr[0] ))
  {
    sPtr++;
  }
  if ( sscanf( sPtr, "%2d", &ss ) != 1 )
  {
    return false;
  }
  return true;
}

