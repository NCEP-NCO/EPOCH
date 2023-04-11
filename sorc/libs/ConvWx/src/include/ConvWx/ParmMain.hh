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
 * @file ParmMain.hh
 * @brief Parameters used for commonly shared aspects of ConvWx applications
 *        such as logging, process registration and triggering
 *  @class ParmMain
 * @brief Parameters used for commonly shared aspects of ConvWx applications
 *        such as logging, process registration and triggering
 */

# ifndef    PARM_MAIN_HH
# define    PARM_MAIN_HH

#include <string>
#include <vector>

//----------------------------------------------------------------
class ParmMain
{
public:
  
  /**
   * @enum Trigger_t 
   * @brief Triggering has one of 2 modes
   */
  typedef enum
  {
    REALTIME=0,  /**< wait for inputs at new time*/
    ARCHIVE=1    /**< trigger a fixed set of pre-existing input times */
  } Trigger_t;
    
  /**
   * Default constructor. values are not set
   */
  ParmMain(void);

  /**
   * Constructor REALTIME triggering.
   *
   * @param[in] procname   Process name
   * @param[in] debug   True to set normal debugging logging messages
   * @param[in] debugVerbose   True to set verbose debugging logging messages
   * @param[in] debugTrigger   True to set triggering debugging logging messages
   * @param[in] debugShowRealtime  True to show time in logging messages
   * @param[in] debugShowClassAndMethod  True to show information about the
   *                                     file and method in logging message
   * @param[in] instance  Process instance, used for registration
   * @param[in] registrationIntervalSeconds  How often process heart beats,
   *                                          used for registration
   * @param[in] triggerUrl   The triggering data input URL
   * @param[in] numSubsetGenTimeMinutes  Number of subset generation time
   *                                     minutes (can be 0)
   * @param[in] subsetGenTimeMinutes  The generation time subsets to trigger 
   * @param[in] isSubsetLeadTimes  True if a subset of lead times is all that
   *                               should trigger
   * @param[in] leadTimeHour0 Earliest lead time hour to trigger when
   *                          isSubsetLeadTimes
   * @param[in] leadTimeHour1 Latest lead time hour to trigger when
   *                          isSubsetLeadTimes
   * @param[in] leadTimeHourDelta Difference (hours) between subset lead
   *                              times (when isSubsetLeadTimes)
   */
  ParmMain(const std::string &procname, const bool debug,
	   const bool debugVerbose, const bool debugTrigger,
	   const bool debugShowRealtime, const bool debugShowClassAndMethod,
	   const std::string &instance, const int registrationIntervalSeconds,
	   const std::string &triggerUrl, const int numSubsetGenTimeMinutes,
	   const int *subsetGenTimeMinutes, const bool isSubsetLeadTimes,
	   const double leadTimeHour0, const double leadTimeHour1, 
	   const double leadTimeHourDelta);


  /**
   * Constructor ARCHIVE triggering.
   *
   * @param[in] procname   Process name
   * @param[in] debug   True to set normal debugging logging messages
   * @param[in] debugVerbose   True to set verbose debugging logging messages
   * @param[in] debugTrigger   True to set triggering debugging logging messages
   * @param[in] debugShowRealtime  True to show time in logging messages
   * @param[in] debugShowClassAndMethod  True to show information about the
   *                                     file and method in logging message
   * @param[in] instance  Process instance, used for registration
   * @param[in] registrationIntervalSeconds  How often process heart beats,
   *                                          used for registration
   * @param[in] triggerUrl   The triggering data input URL
   * @param[in] numSubsetGenTimeMinutes  Number of subset generation time
   *                                     minutes (can be 0)
   * @param[in] subsetGenTimeMinutes  The generation time subsets to trigger 
   * @param[in] isSubsetLeadTimes  True if a subset of lead times is all that
   *                               should trigger
   * @param[in] leadTimeHour0 Earliest lead time hour to trigger when
   *                          isSubsetLeadTimes
   * @param[in] leadTimeHour1 Latest lead time hour to trigger when
   *                          isSubsetLeadTimes
   * @param[in] leadTimeHourDelta Difference (hours) between subset lead
   *                              times (when isSubsetLeadTimes)
   * @param[in] t0  Earliest allowed archive time
   * @param[in] t1  Latest allowed archive time
   */
  ParmMain(const std::string &procname, const bool debug,
	   const bool debugVerbose, const bool debugTrigger,
	   const bool debugShowRealtime, const bool debugShowClassAndMethod,
	   const std::string &instance, const int registrationIntervalSeconds, 
	   const std::string &triggerUrl, const int numSubsetGenTimeMinutes,
	   const int *subsetGenTimeMinutes, const bool isSubsetLeadTimes,
	   const double leadTimeHour0, const double leadTimeHour1, 
	   const double leadTimeHourDelta, const time_t &t0,
	   const time_t &t1);

  /**
   * Destructor
   */
  virtual ~ParmMain(void);

  /**
   * @return true if the  mode is ParmMain::REALTIME
   */
  inline bool isRealtime(void) const
  {
    return pTriggerType != ARCHIVE;
  }

  /**
   * When lead time subsampling is activated (pIsLtSubsample=true),
   * return the expected lead times.
   * When lead time subsampling is not activated, return the empty vector
   *
   * @return lead time seconds, in ascending order
   */
  std::vector<int> getLeadTimes(void) const;

  bool pOk;           /**< Indicate when object is set properly */
  std::string pOkErr; /**< string as to what is wrong when not pOk */
  std::string pProcessName; /**< name of process (for process registration) */
  bool pDebug;             /**< true to log messages of ILogMsg::DEBUG */
  bool pDebugVerbose;     /**< true to log messages of ILogMsg::DEBUG_VERBOSE */
  bool pDebugTrigger;     /**< true to log messages of ILogMsg::TRIGGER */
  bool pDebugShowRealtime;/**< true to put logged time into log messages */
  bool pDebugShowClassAndMethod; /**< true to put file and method information
				  * into logged messages */
  std::string pInstance;   /**< instance of process (for process registration)*/
  int pRegistrationSeconds; /**< seconds between process registration */
  ParmMain::Trigger_t pTriggerType; /**< type of triggering  */
  std::string pTriggerUrl;          /**< where triggering data comes from  */
  std::vector<int> pGtSubsampleMin;   /**< Subsample of generation time minutes
				       * within the hour, unless empty, in
				       * which case all triggering generation
				       * times are used*/
  bool pIsLtSubsample; /**< true for subsampling of leads */
  int pLt0;            /**< subsample seconds of earliest trigger lead time  */
  int pLt1;            /**< subsample seconds of latest trigger lead time  */
  int pDlt;            /**< subsample seconds increment between lead times */
  time_t pArchiveT0; /**< earliest time of triggering (ARCHIVE mode) */
  time_t pArchiveT1; /**< latest time of triggering (ARCHIVE mode) */


protected:
private:  

};

# endif
