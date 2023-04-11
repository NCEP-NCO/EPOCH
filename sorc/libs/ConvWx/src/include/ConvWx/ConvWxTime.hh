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
 * @file ConvWxTime.hh
 * @brief  UNIX Time handling methods, all static.
 * @class ConvWxTime
 * @brief  UNIX Time handling methods, all static.
 */

#ifndef CONVWX_TIME_HH
#define CONVWX_TIME_HH

#include <string>

class ConvWxTime
{

public:

  /**
   * Return string representation of a UNIX time that is human friendly.
   *
   * @param[in] ut  A UNIX time.
   *
   * @return  The string
   *
   * @note  This implementation returns "yyyy/mm/dd hh:mm:ss"
   */
  static std::string stime(const time_t &ut);


  /**
   * Return string representation of a UNIX time that is human friendly.
   *
   * @param[in] ut  A UNIX time.
   *
   * @return  The string
   *
   * @note  This implementation returns "yyyy-mm-ddThh:mm:ss"
   */
  static std::string xtime(const time_t &ut);

  /**
   * Return a short abbreviation of UNIX time that is human friendly.
   *
   * @param[in] ut  A UNIX time.
   * @param[in] showSeconds  True to show seconds (hh:mm:ss), false to
   *                         show hh:mm without seconds
   *
   * @return  The string
   *
   * @note this implementation returns "dd month, hh:mm"
   *       where month is 3 letter abbreviation Jan, Feb, Mar,..
   *       or "dd month, hh:mm:ss"
   */
  static std::string shortStime(const time_t &ut, const bool showSeconds=false);

  /**
   * Return a very short abbreviation of UNIX time that is human friendly.
   *
   * @param[in] ut  A UNIX time.
   *
   * @return  The string
   *
   * @note this implementation returns "hh:mm:ss"
   */
  static std::string veryShortStime(const time_t &ut);

  /**
   * Return The UNIX representation of an InterfaceLL::stime() string.
   *
   * @param[in] st  String for the time, as from InterfaceLL::stime()
   *
   * @return The UNIX time associated with st.
   */
  static time_t utime(const std::string &st);

  /**
   * Return individual values for a UNIX time
   *
   * @param[in] ut  UNIX Time
   * @param[out] year  Year
   * @param[out] month  Month
   * @param[out] day  Day
   * @param[out] hour  Hour
   * @param[out] min  Minute
   * @param[out] sec  Second
   */
  static void expandTime(const time_t ut, int &year, int &month, int &day,
			 int &hour, int &min, int &sec);

  /**
   * @return year value for input UNIX time
   * @param[in] ut  UNIX time
   */
  static int getYear(const time_t ut);

  /**
   * @return month value for input UNIX time
   * @param[in] ut  UNIX time
   */
  static int getMonth(const time_t ut);

  /**
   * @return day value for input UNIX time
   * @param[in] ut  UNIX time
   */
  static int getDay(const time_t ut);

  /**
   * @return hour value for input UNIX time
   * @param[in] ut  UNIX time
   */
  static int getHour(const time_t ut);

  /**
   * @return minute value for input UNIX time
   * @param[in] ut  UNIX time
   */
  static int getMinute(const time_t ut);

  /**
   * Convert individual time values into a UNIX time
   * @param[in] year  Year 
   * @param[in] month  Month 
   * @param[in] day  Day 
   * @param[in] hour  Hour 
   * @param[in] min  Minute 
   * @param[in] sec  Second 
   *
   * @return UNIX time for the inputs.
   */
  static time_t setTime(const int year, const int month, const int day,
			const int hour, const int min, const int sec);

  /**
   * Return number of hours into the day for a UNIX time.
   *
   * @param[in] ut  UNIX time.
   *
   * @return  Hours into the day for this time.
   *
   * For example a time with hh:mm:ss = 08:15:36 would return
   * 8 + 15/60 + 36/3600 = 8.26 hours into the day
   */
  static double getTimeFraction(const time_t &ut);

  /**
   * Return day of the year
   *
   * @param[in] ut  UNIX time.
   *
   * @return  day within the year: 1, 2, ...
   */
  static int getDayOfYear(const time_t &ut);

  /**
   * @return the earliest time in the input day, i.e.
   * year,month,day,0,0,0
   *
   * @param[in] ut  Time 
   */
  static time_t earliestTimeInDay(const time_t &ut);
  
  /**
   * sleep using input params until real time is in the next
   * day at a specified hour.  Either uses system clock or a
   * URL with observation data that should trigger and provide time values
   *
   * @param[in] wantedHour  Hour of day at which to return from sleep
   * @param[in] dataTrigger  True if time values come from a triggering URL,
   *                         false if time values come from time(0)
   * @param[in] obsDataUrl  If dataTrigger=true, this is the URL listened to
   *                        for new data. Not used if dataTrigger=false
   * @param[in] lastRun     The time that must change by at least one day
   *                        before returning
   *
   * @return with time value when done sleeping
   */
  static time_t sleepUntilHourInNextDay(int wantedHour,
					bool dataTrigger,
					const std::string &obsDataUrl,
					const time_t &lastRun);

  /**
   * retrieve the real time, or a data time
   * Either uses system clock or a
   * URL with observation data that should trigger and provide time values
   *
   * @param[in] dataTrigger  True if time values come from a triggering URL,
   *                         false if time values come from time(0)
   * @param[in] obsDataUrl  If dataTrigger=true, this is the URL listened to
   *                        for new data. Not used if dataTrigger=false
   *
   * @return with time value
   */
  static time_t realTime(bool dataTrigger, const std::string &obsDataUrl);

protected:
private:

  /**
   * @struct Date_time_t
   * @brief This struct holds time in two formats, individual 6 values, and
   *        a unix time.
   *
   * This is used to convert back and forth between 6 values and a UNIX time
   */
  typedef struct
  {
    int year;  /**< Year, 1rst of 6 values */
    int month; /**< Month, 2nd of 6 values */
    int day;   /**< Day, 3rd of 6 values */
    int hour;  /**< Hour, 4th of 6 values */
    int min;   /**< Minute, 5th of 6 values */
    int sec;   /**< Second, 6th of 6 values */
    time_t unixTime;  /**< UNIX time equivalent */
  } DateTime_t;

  /**
   * Set the 6 other fields in the dateTimeT structure based on the value
   * of the unixTime field in that structure
   *
   * @param[in,out] dateTime  The struct to fill in
   */
  static void pConvertFromUtime(DateTime_t &dateTime);

  /**
   * Parse a string to extract 6 values for time, which are returned
   * @param[in] strWhen The string to parse
   * @param[out] year  Parsed year
   * @param[out] month  Parsed month
   * @param[out] day  Parsed day
   * @param[out] hour  Parsed hour
   * @param[out] min  Parsed minute
   * @param[out] sec  Parsed second
   * 
   * @return true for success
   */
  static bool pParseDateTime(const std::string &strWhen,
			     int &year, int &month, int &day,
			     int &hour, int &min, int &sec);

  /**
   * Sets the unixTtime field in the DateTime_t structure, using the other
   * 6 fields 
   * 
   * @param[in,out] dateTime  Struct to modify
   */
  static void pConvertToUtime(DateTime_t &dateTime);

};

#endif
