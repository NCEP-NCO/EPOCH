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
 * @file InterfaceIO.hh
 * @brief  Static functions that give Low Level interface to infrastructure
 * software that is not part of ConvWx for Input and output
 * @class InterfaceIO
 * @brief  Static functions that give Low Level interface to infrastructure
 * software that is not part of ConvWx for Input and output
 *
 * @note These are the functions that are implemented differently for different
 *       implementations
 */
# ifndef    INTERFACE_IO_HH
# define    INTERFACE_IO_HH

#include <string>
#include <vector>

class ParmProjection;
class ParmFcst;
class Grid;
class FcstGrid;
class MultiFcstGrid;
class MultiGrid;
class MetaData;
class ConvWxThreadMgr;

//----------------------------------------------------------------
class InterfaceIO
{
public:

  /**
   * Set up exit call for various 'program ending or in fatal trouble' signals
   *
   * @param[in] cleanup  Function that is called when the signals happen.
   */
  static void setCleanExit(void cleanup(int));

  /**
   * Set up logging flags
   * @param[in] debug  Set to true to activate regular debugging
   * @param[in] debugVerbose  Set to true to activate more detailed debugging
   * @param[in] debugTriggering  Set to true to activate debugging regarding
   *                             data triggering
   * @param[in] showRealtime  Set to true to show the real time in each logged
   *                          message
   * @param[in] showClassAndMethod  Set to true to show information about where
   *                                in the source code logging messages were
   *                                called
   */
  static void setLogging(const bool debug, const bool debugVerbose,
			 const bool debugTriggering, const bool showRealtime, 
			 const bool showClassAndMethod);

  /**
   * Starts up a process by informing any monitoring software of its
   * existence and expected registration frequency.
   * 
   * This call is intended to allow a process to interact with other
   * software that monitors its health and is kept aware of its existence
   * through calls to the InterfaceIO::doRegister() method.
   *
   * @param[in] process  Process name
   * @param[in] instance  Process instance 
   * @param[in] maxSeconds  Expected max interval (seconds) between calls to the
   *                        InterfaceIO::doRegister method
   */
  static void startup(const std::string &process, const std::string &instance,
		      const int maxSeconds=60);

  /**
   * Inform any monitoring software of process activity, with a message
   *
   * @note An app should call the InterfaceIO::startup() method prior to
   *       calling this.
   *
   * @param[in] msg  The message sent to the monitoring software.
   */
  static void doRegister(const std::string &msg);

  /**
   * Inform any monitoring software of process termination
   */
  static void finish(void);

  /**
   * Wait for new data and return with data time of new data when it becomes
   * available.
   * This is a realtime triggering mechanism for observation data.
   *
   * @param[in] url  Where the data is located
   * @param[in] subsampleMinutes  Minutes within each hour to subsample,
   *                              empty to trigger all times.
   * @param[in] debug  True for debugging output
   * @param[out] triggerTime  The returned data time value
   *
   * @return True if successful and triggerTime has a meaningful value.
   *         false if there is a problem such that triggering will never happen.
   * 
   * @note This method can hang forever in some cases (no new data arriving)
   *
   */
  static bool triggerObs(const std::string &url,
			 const std::vector<int> &subsampleMinutes,
			 const bool debug, time_t &triggerTime);

  /**
   * Wait for new data only as long as configured to wait through call to
   * setNoWaitTriggering(int maxWaitSeconds), which must be called before
   * this method is called.
   *
   * This is a realtime triggering mechanism for observation data.
   *
   * @param[in] url  Where the data is located
   * @param[in] subsampleMinutes  Minutes within each hour to subsample,
   *                              empty to trigger all times.
   * @param[in] debug  True for debugging output
   * @param[out] triggerTime  The returned data time value
   * @param[out] timedOut  Set to true if return status is false due to
   *                       a timeout, false otherwise
   *
   * @return True if successful and triggerTime has a meaningful value.
   *         false if there is a timeout or there is no more triggering.
   */
  static bool triggerObsNoWait(const std::string &url,
			       const std::vector<int> &subsampleMinutes,
			       const bool debug, time_t &triggerTime,
			       bool &timedOut);

  /**
   * Return with data time of next data.
   * This is an archive mode triggering mechanism for observation data.
   *
   * @param[in] url  Where the data is located
   * @param[in] subsampleMinutes  Minutes within each hour to subsample,
   *                              empty to trigger all times.
   * @param[in] debug  True for debugging output
   * @param[in] t0  Lower limit of archive times
   * @param[in] t1  Upper limit of archive times
   * @param[out] triggerTime  The returned data time value
   *
   * @return True if successful and triggerTime has a meaningful value.
   *         false if no more data in the specified range.
   *
   * @note The first call to this method returns the smallest time not smaller
   *       than t0. Successive calls to this method return the archived
   *       observation data times, in ascending order. 
   */
  static bool triggerObs(const std::string &url,
			 const std::vector<int> &subsampleMinutes,
			 const bool debug, const time_t &t0,
			 const time_t &t1, time_t &triggerTime);

  /**
   * Wait for new data and return with generation time of new data
   * when it becomes available.
   * This is a realtime triggering mechanism for forecast data.
   * All lead times that are configured for (full or subset) must be
   * available before the method will return.
   *
   * @param[in] url  Where the data is located
   * @param[in] isLeadTimeSubsample  True to wait for a subsample of the
   *                                 lead times, false to wait for ALL
   *                                 configured lead times
   * @param[in] leadTime0  Smallest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTime1  Largest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTimeDelta Increment between lead times (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] genTimeSubsampleMinutes  Minutes within each hour for
   *                                     subsampling of generation times, empty to
   *                                     trigger all generation times.
   * @param[in] debug  True for debugging output
   * @param[out] triggerGenTime    The returned generation time value
   *
   * @return True if successful and triggerGenTime has a meaningful value.
   *         false if there is a problem such that triggering will never happen.
   *
   * @note This method can hang forever in some cases (no new data arriving)
   */
  static bool triggerFcst(const std::string &url,
			  const bool isLeadTimeSubsample, const int leadTime0,
			  const int leadTime1, const int leadTimeDelta,
			  const std::vector<int> &genTimeSubsampleMinutes,
			  const bool debug, time_t &triggerGenTime);

  /**
   * Return with next data time if available.
   * This is archive mode triggering mechanism for forecast data.
   * All lead times that are configured for (full or subset) must be available
   * for a successful return. If any are missing, the method returns with
   * a false status
   *
   * @param[in] url  Where the data is located
   * @param[in] isLeadTimeSubsample  True to wait for a subsample of the
   *                                 lead times, false to wait for ALL
   *                                 configured lead times
   * @param[in] leadTime0  Smallest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTime1  Largest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTimeDelta Increment between lead times (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] genTimeSubsampleMinutes  Minutes within each hour for
   *                                     subsampling of generation times,
   *                                     empty to
   *                                     trigger all generation times.
   * @param[in] debug  True for debugging output
   * @param[in] t0  Lower limit of archive times
   * @param[in] t1  Upper limit of archive times
   * @param[out] triggerGenTime     The returned generation time value
   *
   * @return True if successful and triggerGenTime has a meaningful value.
   *         false if there is a problem or no more data is in range
   *
   * @note The first call to this method returns the smallest time not smaller
   *       than t0. Successive calls to this method return the archived
   *       observation data times, in ascending order. 
   */
  static bool triggerFcst(const std::string &url,
			  const bool isLeadTimeSubsample, const int leadTime0,
			  const int leadTime1, const int leadTimeDelta,
			  const std::vector<int> &genTimeSubsampleMinutes,
			  const bool debug, const time_t &t0, const time_t &t1,
			  time_t &triggerGenTime);

  /**
   * Wait for new data and return with generation time and lead time
   * of new data when it becomes available.
   * This is a realtime triggering mechanism for forecast data.
   *
   * It will return as soon as there is new forecast data that has not yet been
   * returned through a call to this method, which will happen for each 
   * forecast (lead time). It returns the forecasts in the order they arrive.
   *
   * @param[in] url  Where the data is located
   * @param[in] isLeadTimeSubsample  True to wait for a subsample of the
   *                                 lead times, false to wait for ALL
   *                                 configured lead times
   * @param[in] leadTime0  Smallest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTime1  Largest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTimeDelta Increment between lead times (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] genTimeSubsampleMinutes  Minutes within each hour for
   *                                     subsampling of generation times, empty to
   *                                     trigger all generation times.
   * @param[in] debug  True for debugging output
   * @param[out] triggerGenTime     The returned generation time value
   * @param[out] triggerLeadTime    The returned lead time (seconds)
   *
   * @return True if successful and triggerGenTime and triggerLeadTime have
   *         meaningful values.
   *         false if there is a problem such that triggering will never happen.
   *
   * @note This method can hang forever in some cases (no new data arriving)
   */
  static bool triggerFcstNext(const std::string &url,
			      const bool isLeadTimeSubsample,
			      const int leadTime0, const int leadTime1,
			      const int leadTimeDelta,
			      const std::vector<int> &genTimeSubsampleMinutes,
			      const bool debug, time_t &triggerGenTime,
			      int &triggerLeadTime);

  /**
   * Wait for new data only as long as configured to wait through call to
   * setNoWaitTriggering(int maxWaitSeconds), which must be called before
   * this method is called.
   *
   * This is a realtime triggering mechanism for forecast data, and returns
   * with generation and lead times when they become available.
   * It willl return true if there is new forecast data that has not yet been
   * returned through a call to this method, which will happen for each 
   * forecast (lead time). It returns the forecasts in the order they arrive.
   *
   * @param[in] url  Where the data is located
   * @param[in] isLeadTimeSubsample  True to wait for a subsample of the
   *                                 lead times, false to wait for ALL
   *                                 configured lead times
   * @param[in] leadTime0  Smallest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTime1  Largest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTimeDelta Increment between lead times (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] genTimeSubsampleMinutes  Minutes within each hour for
   *                                     subsampling of generation times, empty to
   *                                     trigger all generation times.
   * @param[in] debug  True for debugging output
   * @param[out] triggerGenTime     The returned generation time value
   * @param[out] triggerLeadTime    The returned lead time (seconds)
   * @param[out] timedOut  Set to true if return status is false due to
   *                       a timeout, false otherwise
   *
   * @return True if successful and triggerTime has a meaningful value.
   *         false if there is a timeout or there is no more triggering.
   *
   */
  static bool
  triggerFcstNextNoWait(const std::string &url,
			const bool isLeadTimeSubsample,
			const int leadTime0, const int leadTime1,
			const int leadTimeDelta,
			const std::vector<int> &genTimeSubsampleMinutes,
			const bool debug, time_t &triggerGenTime,
			int &triggerLeadTime, bool &timedOut);

  /**
   * Return a sequence of generation time and lead times for data.
   * This is the archive mode triggering mechanism for forecast data.
   * It returns the forecasts in ascending lead time order at each generation
   * time, and ascending generation time order.
   *
   * @param[in] url  Where the data is located
   * @param[in] isLeadTimeSubsample  True to wait for a subsample of the
   *                                 lead times, false to wait for ALL
   *                                 configured lead times
   * @param[in] leadTime0  Smallest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTime1  Largest forecast lead time (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] leadTimeDelta Increment between lead times (seconds) when
   *                       isLeadTimeSubsample=true
   * @param[in] genTimeSubsampleMinutes  Minutes within each hour for
   *                                     subsampling of generation times, empty to
   *                                     trigger all generation times.
   * @param[in] t0  Lower limit of archive times
   * @param[in] t1  Upper limit of archive times
   * @param[in] debug  True for debugging output
   * @param[out] triggerGenTime     The returned generation time value
   * @param[out] triggerLeadTime    The returned lead time (seconds)
   *
   * @return True if successful and triggerGenTime and triggerLeadTime have
   *         meaningful values.
   *         false if there are no more times with data in range.
   *
   * @note The first call to this method returns the first wanted lead time
   *       for the smallest generation time not smaller than t0. Successive calls 
   *       to this method return the archived generation/lead times, in ascending
   *       order. 
   */
  static bool
  triggerFcstNextArchive(const std::string &url, 
			 const bool isLeadTimeSubsample,
			 const int leadTime0, const int leadTime1,
			 const int leadTimeDelta,
			 const std::vector<int> &genTimeSubsampleMinutes,
			 const time_t &t0, const time_t &t1,
			 const bool debug, time_t &triggerGenTime,
			 int &triggerLeadTime);


  /**
   * Change the maximum age (seconds) of data for it to cause a trigger in
   * real time
   *
   * @param[in] maxValidAgeSeconds   The new value to use in triggering
   */
  static void triggerSetMaxValidAge(const int maxValidAgeSeconds);


  /**
   * Rewind when in archive mode triggering. (calling one of the trigger
   * methods that is for archive data).  After this method is called, the
   * returned times will 'rewind' so that the next triggering call will
   * return the smallest time in range.
   *
   * @return true for success
   */
  static bool triggerArchiveRewind(void);

  /**
   * Return with the path of the next data file in an archive interval.
   * This is a archive mode triggering mechanism for data files
   *
   * @param[in] inputDir  Local directory where the data is located
   * @param[in] debug  True for debugging output
   * @param[in] t0  Lower limit of archive times
   * @param[in] t1  Upper limit of archive times
   * @param[out] filepath  The returned file path
   *
   * @return true if the next file has been returned in filepath, false
   *         if no more files are in the time range
   */
  static bool triggerFile(const std::string &inputDir,
			  const bool debug, const time_t &t0, const time_t &t1,
			  std::string &filepath);

 

  /**
   * Return with the paths of the next data files in an archive interval.
   * This is a archive mode triggering mechanism for data files
   *
   * @param[in] inputDir1  Local directory where the data is located
   * @param[out] filepath1  The returned file path
   * @param[in] inputDir2  Local directory where the data is located
   * @param[out] filepath2  The returned file path
   * @param[in] debug  True for debugging output
   * @param[in] t0  Lower limit of archive times
   * @param[in] t1  Upper limit of archive times
   * @param[in] isGenLeadForecastFormat  Flag to indicate the file naming convention
   *                                      
   *
   * @return true if the next files have been returned in filepath1 and filepath2, false
   *         if no more files are in the time range. 
   */
  static bool triggerFiles(const std::string &inputDir1,std::string &filepath1,
                           const std::string &inputDir2,std::string &filepath2,
                           const bool debug, const time_t &t0,
                           const time_t &t1, bool isGenLeadForecastFormat);
 

 /**
   * Return with the paths of the next data files in an archive interval.
   * This is a real-time mode triggering mechanism for data files
   *
   * @param[in] inputDir1  Local directory where the data is located
   * @param[out] filepath1  The returned file path
   * @param[in] inputDir2  Local directory where the data is located
   * @param[out] filepath2  The returned file path
   * @param[in] lookback  The number of seconds previous to current time which
   *                      defines the beginning of the time interval in 
   *                      which to look for data
   * @param[in] debug  True for debugging output
   * @param[in] isGenLeadForecastFormat  Flag to indicate the file naming convention
   *
   *
   * @return true if the next files have been returned in filepath1 and filepath2, false
   *         if no files exist in the time range. In realtime the most recent files
   *         in the range are returned.
   */
  static bool triggerFilesRealtime(const std::string &inputDir1,std::string &filepath1,
                                   const std::string &inputDir2,std::string &filepath2,
                                   const int lookback, const bool debug, bool isGenLeadForecastFormat);

  /**
   * Clear the internal state when the InterfaceIO::triggerFile() method is
   * no longer to be called.
   */
  static void clearFileTrigger(void);

  /**
   * Parse a standard ConvWx command line, return the range of times if
   * that is present in the arguments
   *
   * If in the arguments the format is "-interval yyyymmddhhmmss yyyymmddhhmmss"
   * the two times will be parsed for time values
   *
   * @param[in] argc  Number of arguments
   * @param[in] argv  The Arguments
   * @param[out] t0  Earliest time specified in the command line
   * @param[out] t1  Latest time specified in the command line
   *
   * @return True if a time range is specified in the arguments
   *
   */
  static bool getArchiveCmdargRange(int argc, char **argv, time_t &t0,
				    time_t &t1);


  /**
   * Determine time of newest observation data that is available in a range.
   *
   * @param[in] t0  Earliest allowed time
   * @param[in] t1  Latest allowed time
   * @param[in] url  Location of data
   * @param[out] newestTime   Time of newest available observation data in
   *                          [t0,t1]
   *
   * @return true for success
   *
   * @note queries a server specified by the URL
   */
  static bool newestObsTime(const time_t &t0, const time_t &t1,
			    const std::string &url, time_t &newestTime);

  /**
   * Determine if observation data at a time is available or not within a
   * tolerance.
   *
   * @param[in] targetTime  Time to try and match
   * @param[in] toleranceSeconds  Allowed difference between targetTime and
   *                              actual time
   * @param[in] url  Location of data
   * @param[out] actualTime  The time that was found
   *
   * @return true if data meeting requirements exists
   *
   * @note queries a server specified by the URL
   */
  static bool obsExists(const time_t &targetTime, const int toleranceSeconds,
			const std::string &url, time_t &actualTime);


  /**
   * Determine if forecast data at a generation/lead time is available or not.
   *
   * @param[in] gt  Forecast generation time 
   * @param[in] lt  Forecast lead seconds
   * @param[in] url  Location of data
   *
   * @return true if data meeting requirements exists
   *
   * @note queries a server specified by the URL
   */
  static bool fcstExists(const time_t &gt, const int lt, 
			 const std::string &url);

  /**
   * Determine if a particular forecast is available or not within a tolerance.
   *
   * @param[in] targetGenTime  Forecast generation time to try and match
   * @param[in] lt  Forecast lead seconds
   * @param[in] toleranceSeconds  Allowed difference between targetGenTime and
   *                              actual time
   * @param[in] url  Location of data
   * @param[out] actualGenTime  The generation time that was found
   *
   * @return true if data meeting requirements exists
   *
   * @note queries a server specified by the URL
   */
  static bool fcstExists(const time_t &targetGenTime, const int lt,
			 const int toleranceSeconds,
			 const std::string &url, time_t &actualGenTime);

  /**
   * Return all forecast valid times that are available for a particular 
   * forecast generation time
   *
   * @param[in] url  Location of data
   * @param[in] gt  The fixed forecast generation time
   *
   * @return The valid times that are available for this generation time
   *
   * @note valid time = generation time + lead time
   * @note queries a server specified by the URL
   */
  static std::vector<time_t> getAvailableValidTimes(const std::string &url,
						    const time_t &gt);

  /**
   * Return all forecast lead time (seconds) that are available and
   * are for a particular generation time, restricted to a configured set of 
   * possible lead times.
   *
   * @param[in] gt  The fixed forecast generation time
   * @param[in] url  Location of data
   * @param[in] leadHour0  Minimum hour lead time wanted
   * @param[in] leadHour1  Maximum hour lead time wanted
   * @param[in] leadDeltaHours  Increment between lead times wanted
   *
   * @return The lead times (seconds) that meet the requirements
   *
   * @note Any lead times that are not in the input specification yet are
   *       available are not returned.
   *
   * @note queries a server specified by the URL
   *
   */
  static std::vector<int> getAvailableLeadTimes(const time_t &gt, 
						const std::string &url,
						const double leadHour0,
						const double leadHour1,
						const double leadDeltaHours);
  /**
   * Return those forecast generation times in a range for which
   * forecast data is currently available.
   *
   * @param[in] url  Specification of the location of forecast data
   * @param[in] genTime0  Earliest generation time in the allowed range
   * @param[in] genTime1  Latest generation time in the allowed range.
   *
   * @return A vector of generation times between genTime0 and genTime1 inclusive for
   *         which data is available
   *
   * @note the range of generation times searched is genTime0 <= genTime <= genTime1
   *
   * @note queries a server specified by the URL
   */
  static std::vector<time_t>
  availableFcstGenTimesInARange(const std::string &url, const time_t &genTime0,
				const time_t &genTime1);

  /** 
   * Find the most recently generated forecast for data at input url
   * at given valid time.
   * 
   * @param[in] url  Specification of the location of forecast data
   * @param[in] gt0  Earliest generation time in the allowed range
   * @param[in] gt1  Latest generation time in the allowed range.
   *  @param[in] validTime  Valid time of forecast
   * @param[out] gen Generation time of newest forecast valid at validTime
   * @param[out] lead Lead time of newest forecast valid at validTime
   * @return Return true if forecast found and false otherwise
   */
  static bool newestFcstAtValidTime(const std::string &url,  
				    const time_t &gt0, 
				    const time_t &gt1,  
				    const time_t &validTime,   
				    time_t &gen, 
				    int &lead);


  /**
   * Return all lead times (seconds) for a particular forecast generation time for
   * which data is currently available.
   *
   * @param[in] url  Specification of the location of forecast data
   * @param[in] gt   The generation time to use.
   *
   * @return  A vector of lead time seconds for which forecasts are available
   *          at this generation time.
   *
   * @note Forecast valid time = generation time + lead time
   * @note queries a server specified by the URL
   */
  static std::vector<int>
  availableFcstLeadTimeSecondsAtGt(const std::string &url, const time_t &gt);

  /**
   * Return those forecast generation and lead times in a range for which
   * forecast data is currently available.
   *
   * @param[in] url  Location of forecast data
   * @param[in] genTime0  Earliest generation time 
   * @param[in] genTime1  Latest generation time
   *
   * @return A vector of pairs of generation times/lead times seconds between genTime0
   *         and genTime1 inclusive for which data is available
   *
   * @note The range of generation times searched is genTime0 <= gt <= genTime1
   * @note queries a server specified by the URL
   */
  static std::vector<std::pair<time_t, int>  > 
  availableFcstGenLeadTimesInARange(const std::string &url,
				    const time_t &genTime0,
				    const time_t &genTime1);

  /**
   * Return those observation times in a time range for which observations data
   * is currently available.
   *
   * @param[in] url  Location of observations data.
   * @param[in] t0  Earliest time
   * @param[in] t1  Latest time
   *
   * @return  Vector of times between t0 and t1, inclusive, for which data is
   *          available
   *
   * @note queries a server specified by the URL
   */
  static std::vector<time_t> availableObsTimesInARange(const std::string &url,
						       const time_t &t0,
						       const time_t &t1);

  /**
   * Return the observation time closest to a time within a tolerance that
   * is available
   *
   * @param[in] url  Location of observations data.
   * @param[in] targetTime  Time to get as close as possible to
   * @param[in] toleranceSeconds  Allowed seconds offset 
   * @param[out] actualTime  The Actual observations data time found if any
   *
   * @return True if such an observation time was found
   *
   * @note Observations data within [targetTime-toleranceSeconds,
   *       targetTime+toleranceSeconds] is allowed
   *
   * @note queries a server specified by the URL
   */
  static bool availableObsDataClosestToTime(const std::string &url,
					    const time_t &targetTime,
					    const int toleranceSeconds,
					    time_t &actualTime);

  /**
   * Find the best available gen time that most closely matches input time,
   * using time written
   * 
   * @param[in] t  Time to try and match
   * @param[in] maxSecondsBack  Maximum window backwards to look
   * @param[in] parm  Parameters for the data to check for
   * @param[out] bestT Returned time
   * @param[in] checkTimeWritten   True to reject data for which 'time written'
   *                               is newer than input time, false to ignore
   *                               time written 
   * @param[in] useIncomplete  True to allow return of the newest gen time,
   *                           when all gen times in the range are incomplete.
   *
   * @return true if found a time
   *
   * The method searches for the newest gen time in the range
   * [t-maxSecondsBack,t] for which all lead times in parm are available
   * and the time written for all forecasts is >= t (when checkTimeWritten=true)
   */
  static bool bestGenTime(const time_t &t, int maxSecondsBack,
			  const ParmFcst &parm, time_t &bestT,
			  bool checkTimeWritten=true,
			  bool useIncomplete=false);

  /**
   * Find the best available gen time that most closely matches input time,
   * with a latency of at least a certain amount.
   * 
   * @param[in] t  Time to try and match to 
   * @param[in] maxSecondsBack  Maximum window backwards to look
   * @param[in] minSecondsBack  Minimum latency
   * @param[in] parm  Parameters for the data to check for
   * @param[in] bestT Returned time
   *
   * @return true if found a time
   *
   * The method searches for the newest gen time in the range
   * [t-maxSecondsBack,t-minSecondsBack] for which all lead times in
   * parm are available
   */
  static bool bestGenTimeWithLatency(const time_t &t, int maxSecondsBack,
				     int minSecondsBack, const ParmFcst &parm,
				     time_t &bestT);

  /**
   * Change the internal state to restrict vertical level.
   * The vertical level has meaning when the incoming data is actually
   * a 3 dimensional grid, not a 2 dimensional grid.  This method assumes
   * such a 3rd dimension and allows choice of a particular 2 dimensional slice
   * when accessing data to put into a Grid.
   *
   * @param[in] index  The vertical level index 0,1,2,..  to choose.
   *
   * @note It is assumed there are enough vertical levels to have this work.
   *
   * @note The method InterfaceIO::unrestrictVerticalLevel() will return
   *       internal state back to normal
   */
  static void restrictVerticalLevel(const int index);

  /**
   * Change the internal state to restrict vertical level.
   * The vertical level has meaning when the incoming data is actually
   * a 3 dimensional grid, not a 2 dimensional grid.  This method assumes
   * such a 3rd dimension and allows choice of a subset or slab of the
   * vertical levels to be chosen.
   *
   * @param[in] index1  The lower vertical level index 0,1,2,...
   * @param[in] index2  The upper vertical level index 0,1,2,...
   *
   * @note It is assumed there are enough vertical levels to have this work.
   *
   * @note The method InterfaceIO::unrestrictVerticalLevel() will return
   *       internal state back to normal
   */
  static void restrictVerticalLevel(const int index1, const int index2);

  /**
   * Change the internal state so no restrictions are in place regarding
   * vertical levels.
   *
   * @note This is called to undo InterfaceIO::restrictVerticalLevel()
   */
  static void unrestrictVerticalLevel(void);

  /**
   * Change internal state so that vertical levels are not written into
   * the state.  This was put in to make certain calls thread safe.
   * An effect of calling this method is that the method getVerticaLevels()
   * will not return the vertical levels
   */
  static void disallowStoringVerticalLevels(void);

  /**
   * Retrieve the vertical level values after a read
   * The vertical level has meaning when the incoming data is actually
   * a 3 dimensional grid, not a 2 dimensional grid. 
   *
   * @return  vector of vertical level values found in the incoming data
   */
  static std::vector<double> getVerticalLevels(void);


  /**
   * Load observation data into a FcstGrid
   *
   * @param[in] t  Data time of the observation data to load.
   * @param[in] proj  Projection information to use (if remap=true)
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of the data.
   * @param[in] field  Name of the particular data at the URL that is wanted
   * @param[in] remap   True if proj should be used to remap the data,
   *                    false if proj is unused.
   * @param[out] outGrid  The returned FcstGrid
   *
   * @return True for success, false for failure.
   *
   * @note In the returned FcstGrid, the lead time is set to 0 since this is
   *       observation data where lead time is not relevant.
   *
   * @note queries a server specified by the URL
   */
  static bool loadObs(const time_t &t, const ParmProjection &proj,
		      const std::string &url, const std::string &field,
		      const bool remap, FcstGrid &outGrid,
		      bool suppresswarnings=false);


  /**
   * Load observation data into a FcstGrid
   *
   * @param[in] t  Data time of the observation data to load.
   * @param[in] proj  Projection information to use (if remap=true)
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of the data.
   * @param[in] field  Name of the particular data at the URL that is wanted
   * @param[in] remap   True if proj should be used to remap the data,
   *                    false if proj is unused.
   * @param[out] outGrid  The returned FcstGrid
   * @param[out] vLevels  The vertical level values of the grid
   *
   * @return True for success, false for failure.
   *
   * @note In the returned FcstGrid, the lead time is set to 0 since this is
   *       observation data where lead time is not relevant.
   *
   * @note queries a server specified by the URL
   */
  static bool loadObs(const time_t &t, const ParmProjection &proj,
                      const std::string &url, const std::string &field,
                      const bool remap, FcstGrid &outGrid,
		      std::vector <double> &vLevels);


  /**
   * Load observation data into a Grid
   *
   * @param[in] t  Data time of the observation data to load.
   * @param[in] proj  Projection information to use (if remap=true)
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of the data.
   * @param[in] field  Name of the particular data at the URL that is wanted
   * @param[in] remap   True if proj should be used to remap the data,
   *                    false if proj is unused.
   * @param[out] outGrid  The returned Grid
   *
   * @return True for success, false for failure.
   *
   * @note queries a server specified by the URL
   */
  static bool loadObs(const time_t &t, const ParmProjection &proj,
		      const std::string &url, const std::string &field,
		      const bool remap, Grid &outGrid,
		      bool suppresswarnings=false);

  /**
   * Load observation data into a Grid
   *
   * @param[in] t  Data time of the observation data to load.
   * @param[in] proj  Projection information to use (if remap=true)
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of the data.
   * @param[in] field  Name of the particular data at the URL that is wanted
   * @param[in] remap   True if proj should be used to remap the data,
   *                    false if proj is unused.
   * @param[out] outGrid  The returned Grid
   * @param[out] vLevels  The vertical level values of the grid
   *
   * @return True for success, false for failure.
   *
   * @note queries a server specified by the URL
   */
  static bool loadObs(const time_t &t, const ParmProjection &proj,
                      const std::string &url, const std::string &field,
                      const bool remap, Grid &outGrid,
		      std::vector<double> &vLevels);


  /**
   * Load multiple fields of observation data from a single source into a
   * MultiGrid
   *
   * @param[in] t  Data time of the observation data to load.
   * @param[in] proj  Projection information to use (if remap=true)
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of the data.
   * @param[in] field  Names of the particular data at the URL that are wanted
   *                   (any number).
   * @param[in] remap  True if proj should be used to remap the data
   *                   false if proj is unused.
   * @param[out] outGrids  The returned MultiGrid data, with one Grid per field
   *
   * @return True for success, false for failure.
   *
   * @note queries a server specified by the URL
   */
  static bool loadMultiObs(const time_t &t, const ParmProjection &proj,
			   const std::string &url,
			   const std::vector<std::string> &field,
			   const bool remap, MultiGrid &outGrids);

   /**
   * Load multiple fields of observation data from a single source into a
   * MultiGrid
   *
   * @param[in] t  Data time of the observation data to load.
   * @param[in] proj  Projection information to use (if remap=true)
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of the data.
   * @param[in] field  Names of the particular data at the URL that are wanted
   *                   (any number).
   * @param[in] remap  True if proj should be used to remap the data
   *                   false if proj is unused.
   * @param[out] outGrids  The returned MultiGrid data, with one Grid per field
   * @param[out] metadata The returned MetaData object associated with output grid data
   *
   * @return True for success, false for failure.
   *
   * @note queries a server specified by the URL
   */
  static bool loadMultiObsWMetaData(const time_t &t, const ParmProjection &proj,
                                    const std::string &url,
                                    const std::vector<std::string> &field,
                                    const bool remap, MultiGrid &outGrids,
                                    MetaData &metadata);


  /**
   * Load forecast data into a FcstGrid
   *
   * @param[in] gt  Generation time of forecast data to load.
   * @param[in] lt  Lead time seconds of forecast to load.
   * @param[in] proj  Projection information to use if remap=true,
   *                  if remap is false, this parameter is not used.
   * @param[in] url  Location of data.
   * @param[in] field  Name of the particular data at the URL that is wanted
   * @param[in] remap  True if proj should be used to remap the data
   *                   false if data is o.k. as it is stored.
   * @param[out] outGrid  The returned FcstGrid
   *
   * @return True for success false for failure
   *
   * @note queries a server specified by the URL
   */
  static bool loadFcst(const time_t &gt, const int lt, 
		       const ParmProjection &proj, const std::string &url,
		       const std::string &field, const bool remap,
		       FcstGrid &outGrid);

  /**
   * Load any forecast data for a particular generation time into a FcstGrid
   *
   * @param[in] gt  Generation time of forecast data to load.
   * @param[in] url  Location of data.
   * @param[out] outGrid  The returned FcstGrid
   *
   * @return true for success
   *
   * @note This method does not remap the data, and data for any lead time
   *       can be loaded into the return argument, and any field will do.
   *
   * @note queries a server specified by the URL
   */
  static bool loadAnyFcst(const time_t &gt, const std::string &url,
			  FcstGrid &outGrid);

  /**
   * Load multiple fields of forecast data from a single source into a
   * MultiFcstGrid
   *
   * @param[in] gt  Generation time of forecast data to load
   * @param[in] lt  Lead time seconds of forecast data to load
   * @param[in] proj  Projection information used if remap=true,
   *                  if remap=false, proj is not used.
   * @param[in] url  Location of data.
   * @param[in] field  Names of the particular data at the URL that are wanted
   *                   (any number).
   * @param[in] remap  True if proj should be used to remap the data
   *                   false if data is already correct as stored.
   * @param[out] outGrids  Returned MultiFcstGrid
   * @param[in] suppressErrorMessages  True to suppress errors when read fails
   *
   * @return True for success
   *
   * @note Designed to load multiple Grid from one input location.
   *
   * @note queries a server specified by the URL
   */
  static bool loadMultiFcst(const time_t gt, const int lt, 
			    const ParmProjection &proj, const std::string &url,
			    const std::vector<std::string> &field,
			    const bool remap, MultiFcstGrid &outGrids,
			    bool suppressErrorMessages = false);


  /**
   * Load multiple fields of forecast data from a single source into a
   * MultiFcstGrid, where the field names are unknown.  Exclude field names
   * that contain particular strings in their names
   *
   * @param[in] gt  Generation time of forecast data to load
   * @param[in] lt  Lead time seconds of forecast data to load
   * @param[in] proj  Projection information used if remap=true,
   *                  if remap=false, proj is not used.
   * @param[in] url  Location of data with hopefully one or more fields
   * @param[in] remap  True if proj should be used to remap the data
   *                   false if data is already correct as stored.
   * @param[in] excludes  Strings used to exclude particular fields.
   *                      If a field has one of these as a substring,
   *                      it is not included in the returned grids
   * @param[out] outGrids  Returned MultiFcstGrid
   *
   * @return True for success
   *
   * @note Designed to load multiple Grid from one input location.
   *
   * @note queries a server specified by the URL
   */
  static
  bool loadAllFcstExcludeContaining(const time_t gt, const int lt, 
				    const ParmProjection &proj,
				    const std::string &url,
				    const bool remap,
				    const std::vector<std::string> &excludes, 
				    MultiFcstGrid &outGrids);

  /**
   * Load multiple fields of forecast data from a single source into a
   * MultiFcstGrid, where the field names are unknown.
   *
   * @param[in] gt  Generation time of forecast data to load
   * @param[in] lt  Lead time seconds of forecast data to load
   * @param[in] proj  Projection information used if remap=true,
   *                  if remap=false, proj is not used.
   * @param[in] url  Location of data with hopefully one or more fields
   * @param[in] remap  True if proj should be used to remap the data
   *                   false if data is already correct as stored.
   * @param[out] outGrids  Returned MultiFcstGrid
   *
   * @return True for success
   *
   * @note Designed to load multiple Grid from one input location.
   *
   * @note queries a server specified by the URL
   */
  static bool loadAllFcst(const time_t gt, const int lt, 
			  const ParmProjection &proj, const std::string &url,
			  const bool remap, MultiFcstGrid &outGrids);

  /**
   * Writes multiple observation Grid objects to one output location
   *
   * @param[in] t  Time to give the output
   * @param[in] url  Where to write the data
   * @param[in] proj  Projection information that the output should adhere to
   * @param[in] dataGrids  The data to write
   * @param[in] metadata  Metadata associated with the data to also write
   *
   * @note All output Grid objects share the same time, projection,
   *       and metadata.
   */
  static void write(const time_t &t, const std::string &url,
		    const ParmProjection &proj, const MultiGrid &dataGrids,
		    const MetaData &metadata);

  /**
   * Writes multiple forecast Grid objects to one output location
   *
   * @param[in] gt  Forecast generation time to give the outputs
   * @param[in] lt  Forecast lead time seconds to give the outputs
   * @param[in] url  Where to write the data
   * @param[in] proj  Projection information that the output should adhere to
   * @param[in] dataGrids  The data to write
   * @param[in] metadata  Metadata associated with the data to also write
   *
   * @note All output Grid objects share the same generation and lead time, projection,
   *       and metadata.
   */
  static void write(const time_t &gt, const int lt, const std::string &url,
		    const ParmProjection &proj, const MultiGrid &dataGrids,
		    const MetaData &metadata);

  /**
   * Writes multiple forecast Grid objects to one output location, with
   * thread locking.
   *
   * @param[in] gt  Forecast generation time to give the outputs
   * @param[in] lt  Forecast lead time seconds to give the outputs
   * @param[in] url  Where to write the data
   * @param[in] proj  Projection information that the output should adhere to
   * @param[in] dataGrids  The data to write
   * @param[in] metadata  Metadata associated with the data to also write
   * @param[in,out] threads  Object to lock during actual writing
   *
   * @note All output Grid objects share the same generation and lead time,
   *       projection, and metadata.
   */
  static void write(const time_t &gt, const int lt, const std::string &url,
		    const ParmProjection &proj, const MultiGrid &dataGrids,
		    const MetaData &metadata, ConvWxThreadMgr &threads);

  /**
   * Clear so there is no Obs data at the location at the time.
   * @param[in] t  Time at which to clear out data
   * @param[in] url  Location of data
   */
  static void clearData(const time_t &t, const std::string &url);

  /**
   * Clear so there is no forecast data at the location at the time.
   * @param[in] gt Generation time
   * @param[in] lt  Lead seconds
   * @param[in] url  Location of data
   */
  static void clearData(const time_t &gt, int lt, const std::string &url);

  /**
   * Append input multiple Grid objects to output location, by reading
   * in from the output location, adding the input data grids, and writing back
   * out, for observations data
   *
   * @param[in] t  Time of data read/write
   * @param[in] url  Location of data
   * @param[in] proj  Projection used to read/write
   * @param[in] dataGrids  New data grids to append
   */
  static void append(const time_t &t, const std::string &url,
		     const ParmProjection &proj, const MultiGrid &dataGrids);

  /**
   * Append input multiple Grid objects to output location, by reading
   * in from the output location, adding the input data grids, and writing back
   * out, for forecast data
   *
   * @param[in] gt  Generation time of data read/write
   * @param[in] lt  Lead seconds of data read/write
   * @param[in] url  Location of data
   * @param[in] proj  Projection used to read/write
   * @param[in] dataGrids  New data grids to append

   */
  static void append(const time_t &gt, int lt, const std::string &url,
		     const ParmProjection &proj, const MultiGrid &dataGrids);

  /**
   * Append input multiple Grid objects to output location, by reading
   * in from the output location, adding the input data grids, and writing back
   * out, for forecast data, in the case of metadata
   *
   * @param[in] gt  Generation time of data read/write
   * @param[in] lt  Lead seconds of data read/write
   * @param[in] url  Location of data
   * @param[in] proj  Projection used to read/write
   * @param[in] dataGrids  New data grids to append
   * @param[in] md   Metadata

   */
  static void append(const time_t &gt, int lt, const std::string &url,
		     const ParmProjection &proj, const MultiGrid &dataGrids,
		     const MetaData &md);

  /**
   * Write out latest data information using the conventions of the
   * implementation.  Writes to the path specified.
   * @param[in] path  Where to write information
   * @param[in] time  The latest data time
   */
  static void writeLatestDataInfo(const std::string &path,
				  const time_t &time);
  /**
   * Write out latest data information using the conventions of the
   * implementation.  Writes to the dir specified.
   * @param[in] dir  Where to write information
   * @param[in] relPath  Relative path of the data file
   * @param[in] time  The data time
   */
  static void writeLatestDataInfo(const std::string &dir,
				  const std::string &relPath,
				  const time_t &time);

  /**
   * Must be called BEFORE triggering
   * Sets state to that where triggering returns after maxWaitSeconds,
   * with or without new data, through calls to 
   * triggerObsNoWait() and triggerFcstNextNoWait()
   *
   * @param[in] maxWaitSeconds  Stored in local state
   */
  static void setNoWaitTriggering(int maxWaitSeconds);

protected:
private:  

};

# endif
