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
/**
 * @file ConvWxConstants.hh
 * @brief Commonly used constants 
 *
 * Constants are all put into this file
 */

#ifndef CONVWX_CONSTANTS_HH
#define CONVWX_CONSTANTS_HH

#include <ctime>
#include <string>

namespace convWx
{
/**
 * hours in a day
 */
const int HOURS_PER_DAY = 24;

/**
 * seconds per day
 */
const int SECS_PER_DAY = 86400;

/**
 * seconds per hour 
 */
const int SECS_PER_HOUR = 3600;

/**
 * seconds per minute 
 */
const int SECS_PER_MINUTE = 60;


/**
 * seconds per hour  (double)
 */
const double DOUBLE_SECS_PER_HOUR = 3600.0;

/**
 * Common number of seconds per forecast lead time (15 minutes)
 */
const int FCST_RESOLUTION_SECS = 900;

/**
 * Resolution of model generation times 
 */
const double MODEL_RESOLUTION_HOURS = 1.0;

/**
 * constant for minutes per hour 
 */
const int MINS_PER_HOUR = 60;

/**
 * constant for minutes per hour 
 */
const double DOUBLE_MINS_PER_HOUR = 60.0;

/**
 * constant for seconds per minute
 */
const int SECS_PER_MIN = 60;

/**
 * constant for seconds per minute (double)
 */
const double DOUBLE_SECS_PER_MIN = 60.0;

/**
 * constant for line length
 */
const int ARRAY_LEN_VERY_SHORT = 32;

/**
 * constant for line length
 */
const int ARRAY_LEN_SHORT = 256;

/**
  * constant for line length
 */
const int ARRAY_LEN_LONG = 512;

/**
  * constant for line length
 */
const int ARRAY_LEN_VERY_LONG = 1024;

/**
  * constant for small number
  */
const double EPSILON = .0000001;

/**
  * constant for slightly small number
  */
const double BIG_EPSILON = .001;

/**
  * alternate constant for small number
  */
const double TINY_DOUBLE = 1.e-10;

/**
  * constant for small number used for projection angles
  */
const double TINY_ANGLE = 1.e-4;

/**
  * constant for small distance
  */
const double TINY_DISTANCE = 1.e-2;

/**
  * constant for maximum lead hour in ConvWx
  */
const float MAX_LEAD_HOUR = 18;

/**
  * constant for maximum allowed model latency in ConvWx
  */
const float MAX_MODEL_LATENCY_HOURS = 3;

/**
  * Constant for an index into an array or vector that is out of range
  * (0 is the smallest index that is valid for arrays or vectors)
  */
const int BAD_INDEX = -1;

/**
  * Commonly used Forecast data missing value
  */
const double FCST_MISSING = -99.9;

/**
  * Commonly used Phase correction vector data missing value
  */
const double UV_MISSING = -99.9;

/**
  * a time value that is used as a bad value, UNIX time = 0
  */
const time_t BAD_TIME = 0;

/**
 * A bad latency hour value used for error returns or uninitialized state
 */
const double BAD_LATENCY_HOURS = 99.0;

/**
 * A bad data histogram bin total 
 */
const int BAD_HIST_BIN_TOTAL = -9999;

/**
 * A bad data histogram bin total
 */
const float BAD_HIST_BIN_STAT = -9999.0;


/**
 * Maximum value used in rounding and forcing values into a one byte range
 */
const int DIGITAL_MAX = 253;

/**
 * Meters per kilometer
 */
const double METERS_PER_KM = 1000.0;

/**
 * Number of hours after which incoming model data is late enough for a warning
 */
const double WARNING_LATENCY_HOURS = 4.0;

/**
 * Number of hours after which incoming model data is late enough for an error
 */
const double ERROR_LATENCY_HOURS = 6.0;

/**
 * mathematical PI
 */
const double CONVWX_PI = 3.14159265358979323846;

/**
 * math radians per degree (approximate)
 */
const double RADIANS_PER_DEGREE = 0.01745329251994372;

/**
 * Status of exit considered good
 */
const int GOOD_EXIT = 0;

/**
 * Status of exit considered bad
 */
const int BAD_EXIT = 1;

/**
 * Kilometers per degree longitude at the equator
 */
const double KILOMETERS_PER_DEGREE_AT_EQUATOR = 111.198487;

/**
 * Number of degrees per kilometer at the equator
 */
const double DEGREES_PER_KILOMETER_AT_EQUATOR = 0.008992928;

/**
 * Delimiter for file names, unix
 */
const std::string FILE_DELIMITER = "/";

/**
 * Julian value for January 1 1970
 */
const int JULIAN_JAN_1_1970 = 2440587;

}


#endif
 
