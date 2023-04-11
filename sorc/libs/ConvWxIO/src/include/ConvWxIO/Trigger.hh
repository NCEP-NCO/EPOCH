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
 * @file Trigger.hh
 * @brief Static functions for triggering actions
 * @class Trigger
 * @brief Static functions for triggering actions
 *
 * @note The ConvWx design includes 'triggering', which is data driven response
 * to a change in the data that is 'available for use', typically the change
 * means a newer data has become available (from upstream processing output).
 * Triggering specifies the source of data, and several other options, and
 * then when 'triggering' occurs, the trigger methods return the data time
 * information, which can include forecast generation/lead times, or
 * non-forecast data times.
 *
 * @note The design supports triggering only a subset of the incoming data,
 *       that is to say time values are returned only for a subset of the
 *       times that actually are new.
 *
 * @note Triggering can be set up on forecast data so that each
 *       generation/lead time pair is a new trigger. This is called 'sequence'
 *       triggering. The other option for incoming forecast data is that a
 *       trigger occurs only when all lead times (forecasts) that are
 *       configured for have become available.
 *
 * Class with no state, only static methods
 */
# ifndef    TRIGGER_HH
# define    TRIGGER_HH
#include <string>
#include <ctime>
class ParmMain;

//----------------------------------------------------------------
class Trigger
{
public:

  /**
   * @enum Trigger_t
   * @brief Triggering choices
   *
   * Several kinds of triggering inputs are supported. The input can
   * be observations (flat) data, forecast data (with many lead times), 
   * some kind of file data.  Triggering might not be known until runtime.
   */
  typedef enum
  {
    TRIGGER_OBS,         /**< Triggering is flat observation data */
    TRIGGER_FCST,        /**< Triggering is forecast (with many lead times) */
    TRIGGER_OBS_OR_FCST, /**< Triggering is a run time choice */
    TRIGGER_FILE,        /**< Trigger is a file */
    TRIGGER_NONE          /**< No triggering is needed */
  } Trigger_t;

  /**
   * Default triggering method, ConvWx apps. This waits for new data if there
   * is none.
   *
   * @param[in] parms  Parameters that specify the triggering
   * @param[out] time  Time that was triggered.
   *
   * @return True if a new time was triggered, false for no more triggering.
   *
   * @note This method waits for ALL lead times prior to triggering when
   *       the triggering type is Trigger::TRIGGER_FCST or
   *       Trigger::TRIGGER_OBS_OR_FCST
   */
  static bool trigger(const ParmMain &parms, time_t &time);

  /**
   * Rewind triggering to the beginning of the range, when in
   * ParmMain::ARCHIVE mode.
   *
   * @param[in] parms  Parameters that specify the triggering
   *
   * @return True if successful
   */
  static bool archiveRewind(const ParmMain &parms);

  /**
   * Triggering method for ConvWx apps to produce a full sequence of arriving
   * forecasts or observations data, one per call, in the order they are
   * created. This waits for new data if there is none.
   *
   * @param[in] parms  Parameters that specify the triggering
   * @param[out] gt  Generation Time that was triggered.
   * @param[out] lt  Lead time (seconds) that was triggered. (ignored when
   *                 Trigger::TRIGGER_OBS data)
   *
   * @return  True if triggering returns a new generation and/or lead time,
   *          false for no more
   */
  static bool sequenceTrigger(const ParmMain &parms, time_t &gt, int &lt);


   /**
   * Triggering method for ConvWx apps to produce a full sequence of arriving
   * forecasts or observations data, one per call, in the order they are
   * created. A wait will occur before trigger until mandatory latency is met.
   * This waits for new data if there is none.
   *
   * @param[in] parms  Parameters that specify the triggering
   * @param[out] gt  Generation Time that was triggered.
   * @param[out] lt  Lead time (seconds) that was triggered. (ignored when
   *                 Trigger::TRIGGER_OBS data)
   * @param[in] latency  Mandatory latency of model. A wait will occur before
   *                     trigger until latency is met
   *
   * @return  True if triggering returns a new generation and/or lead time,
   *          false for no more
   */
  static bool sequenceTriggerWLatency(const ParmMain &parms, time_t &gt,
				      int &lt, int latency);

   /**
   * Triggering method for ConvWx apps to produce a full sequence of arriving
   * forecasts or observations data, one per call, in the order they are
   * created. A wait will occur before trigger until mandatory latency is met.
   * This waits for new data if there is none.  In this method, the wait is
   * calculated based on another URL's most recent data times in comparision
   * to the triggering URL.
   *
   * @param[in] parms  Parameters that specify the triggering
   * @param[in] latency  Mandatory latency of model. A wait will occur before
   *                     trigger until latency is met
   * @param[in] dataTimeUrl the URL from which times are gotten for comparison
   * @param[out] gt  Generation Time that was triggered.
   * @param[out] lt  Lead time (seconds) that was triggered. (ignored when
   *                 Trigger::TRIGGER_OBS data)
   *
   * @return  True if triggering returns a new generation and/or lead time,
   *          false for no more
   */
  static bool sequenceTriggerWLatency(const ParmMain &parms, int latency,
				      const std::string &dataTimeUrl,
				      time_t &gt, int &lt);
   /**
   * Triggering method for ConvWx apps to produce a full sequence of arriving
   * forecasts or observations data, one per call, in the order they are
   * created.  This method returns after a configured number of seconds
   * if no new data is available.  The configured number of seconds is from
   * a call to InterfaceIO::setNoWaitTriggering(int maxWaitSeconds)
   *
   * @param[in] parms  Parameters that specify the triggering
   * @param[out] oldGt  Generation Time that was triggered most recently
   * @param[out] oldLt  Lead time (seconds) that was triggered most recently
   *                    (ignored when Trigger::TRIGGER_OBS data)
   * @param[out] gt  Generation Time that was triggered,
   *                 if return status is true.
   * @param[out] lt  Lead time (seconds) that was triggered. (ignored when
   *                 Trigger::TRIGGER_OBS data), if return status is true
   * @param[out] timedOut  True if no new data was triggered in the 
   *                       configured number of seconds
   *
   * @return  True if triggering returns a new generation and/or lead time,
   *          false for no new triggering.  If timedOut=true, then it is
   *          a temporary thing, if timeout=false, there will be no more data
   *
   */
  static bool sequenceTriggerNoWait(const ParmMain &parms, const time_t &oldGt,
				    int oldLt, time_t &gt, int &lt,
				    bool &timedOut);


  /**
   * Triggering method for ConvWx apps to produce a full sequence of arriving
   * observations data, one per call, in the order they are created.
   * This waits for new data if there is none.
   *
   * @param[in] parms  Parameters that specify the triggering
   * @param[out] time  Time that was triggered.
   *
   * @return  True if triggering returns a new t false for no more,
   *          or false if triggering off of a forecast data URL.
   */
  static bool sequenceTrigger(const ParmMain &parms, time_t &time);

   /**
    * Triggering method for ConvWx apps to return a collection of file paths
    * returned one at a time
    *
    * @param[in] parms  Parameters that specify the triggering
    * @param[in] daysData  In realtime mode this the look back time interval 
    *                      (days) for data files. In archive mode, archive
    *                      begin and end times are used.
    * @param[out] filepath  Path to a new file.
    *
    * @return true if triggered successfully, false for no more triggering.
    */
   static bool filepathTrigger(const ParmMain &parms, const int daysData,
			       std::string &filepath);


   /**
    * Triggering method for ConvWx apps to return a collection of file paths
    * returned one at a time
    *
    * @param[in] parms  Parameters that specify the triggering
    * @param[in] daysData  In realtime mode this the look back time interval 
    *                      (days) for data files. In archive mode, archive
    *                      begin and end times are used.
    * @param[in] currentT  Current data time, used only in real time mode
    * @param[out] filepath  Path to a new file.
    *
    * @return true if triggered successfully, false for no more triggering.
    */
  static bool filepathTriggerRealtime(const ParmMain &parms, 
				      const int daysData, 
				      const time_t &currentT,
				      std::string &filepath);

    /**
    * Triggering method for ConvWx apps to return a collection of file paths
    * returned sequentially from two urls
    * @param[in] parms  Parameters for triggering
    * @param[in] triggerUrl1  Url of input data 
    * @param[out] filepath1   Path to a new file.
    * @param[in] triggerUrl2  Url of input data 
    * @param[out] filepath2   Path to a new file.
    * @param[in] daysData  In realtime mode this the look back time interval 
    *                      (days) for data files. In archive mode, archive
    *                      begin and end times are used.
    * @return true if triggered successfully, false for no more triggering.
    */

   static bool filepathTriggerTwoUrls(const ParmMain &parms,
                                      const std::string triggerUrl1, 
                                      std::string &filepath1,
                                      const std::string triggerUrl2, 
                                      std::string &filepath2,
                                      const int daysData);

  /**
   * Set status of forecast triggering/observation triggering to input.
   *
   * @param[in] isFcst  True for forecast triggering, false for observations
   *                    triggering
   * 
   * This is used when the mode is Trigger::TRIGGER_OBS_OR_FCST
   */
   static void setTriggeringFcst(const bool isFcst);

  /**
   * @return True if the configuration allows subset triggering.
   */
  static bool allowSubsets(void);
    
  /**
   * @return  The triggering type
   */
  static Trigger_t getTriggering(void);

  /**
   * Return the status of forecast triggering/observation triggering.
   *
   * @return True if triggering for forecast data, false if for observations
   *         data
   * 
   *
   * This is used  when the mode is TRIGGER_OBS_OR_FCST
   *
   * @note Should be called after one of the loadConvWxParams() methods
   */
  static bool isForecastTrigger(void);

  /**
   * Set triggering flags
   * 
   * @param[in] triggerType  Type of triggering for this app.
   * @param[in] allowSubsetTriggering  True to show parameters for subsets of
   *                                   generation time and lead time in the
   *                                   parameter file so the user can configure
   *                                   for a subset, false to hardwire to
   *                                   trigger the full set of incoming times
   *
   * @note called from within the ParmAppInit() template function
   */
  static void setTriggeringFlags(const Trigger_t triggerType,
				 const bool allowSubsetTriggering);

  /**
   * Set a main triggering log message to on or off
   *
   * @param[in] printTriggering  True to turn on the main triggering log
   *                             messages
   *
   * The log messages will appear with each triggering event if this status
   * is set to true, otherwise triggering will be silent
   */
  static void setTriggeringPrint(const bool printTriggering);

  /**
   * @return string representation of the triggering mode
   * @param[in] mode  Triggering mode
   */
  static std::string sprintMode(const Trigger::Trigger_t &mode);

protected:
private:  

};

# endif
