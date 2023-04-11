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
 * @file TriggerState.hh
 * @brief The state of triggering
 * @class TriggerState
 * @brief The state of triggering
 *
 * The triggering state is a set of parameters that are set to particular values
 * 
 * @note 'sequence' triggering means individual lead times cause a trigger,
 * 'regular' triggering happens for forecast data only when ALL lead times
 * are present, or when observation data changes.
 */

#ifndef TRIGGER_STATE_H
#define TRIGGER_STATE_H

#include <vector>
#include <string>

//----------------------------------------------------------------
class TriggerState
{
public:

  /**
   * Constructor
   */
  TriggerState(void);

  /**
   * Destructor
   */
  virtual ~TriggerState(void);

  /** 
   * @return True if the current state is archive mode triggering
   */
  inline bool isArchive(void) const {return pIsArchive;}

  /**
   * Return all the information needed to specify archive mode
   *
   * @param[out] archiveTime0  Archive early time
   * @param[out] archiveTime1  Archive latest time
   * @param[out] url  The location of data
   * @param[out] gentimeSubsample  Subset of generation times to process (or
   *                               empty to process all generation times)
   * @param[out] isLeadtimeSubsample  True to use only a subsample of lead times
   *                                  specified by the parameters
   *                                  leadtimeSeconds0,
   *                                  leadtimeSeconds1, leadtimeSecondsDelta
   * @param[out] leadtimeSeconds0  Earliest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSeconds1  Latest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSecondsDelta  Resolution of leads to use when 
   *                                   isLeadtimeSubsample=true
   * @param[out] isFcst  True if triggering forecast data, false for observation
   *                     data
   * @param[out] isSequence  True if doing sequence triggering, false if doing
   *                         regular triggering
   */
  void getArchiveInfo(time_t &archiveTime0, time_t &archiveTime1,
		      std::string &url, 
		      std::vector<int> &gentimeSubsample, 
		      bool &isLeadtimeSubsample,
		      int &leadtimeSeconds0, int &leadtimeSeconds1,
		      int &leadtimeSecondsDelta, bool &isFcst,
		      bool &isSequence) const;

  /**
   * Set internal state for realtime observation data triggering.
   *
   * @param[in] url  The location of the observation data
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   *
   * @return true if the internal state is different than input.
   */
  bool didChange(const std::string &url,
		 const std::vector<int> &gentimeSubsample) const;

  /**
   * Check internal state for ARCHIVE Observation data triggering.
   *
   * @param[in] url  The location of the observation data
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   * @param[in] t0  Archive early time
   * @param[in] t1  Archive latest time
   *
   * @return true if the internal state is different than input.
   */
  bool didChange(const std::string &url,
		 const std::vector<int> &gentimeSubsample,
		 const time_t &t0, const time_t &t1) const;

  /**
   * Check internal state for realtime forecast data triggering.
   *
   * @param[in] url  The location of the triggering data
   * @param[out] isLeadtimeSubsample  True to use only a subsample of lead times
   *                                  specified by the parameters
   *                                  leadtimeSeconds0,
   *                                  leadtimeSeconds1, leadtimeSecondsDelta
   * @param[out] leadtimeSeconds0  Earliest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSeconds1  Latest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSecondsDelta  Resolution of leads to use when 
   *                                   isLeadtimeSubsample=true
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   * @param[in] sequence  True if sequence triggering, false if regular
   *
   * @return true if the internal state is different than input.
   */
  bool didChange(const std::string &url, const bool isLeadtimeSubsample,
		 const int leadtimeSeconds0, const int leadtimeSeconds1,
		 const int leadtimeSecondsDelta,
		 const std::vector<int> &gentimeSubsample, 
		 const bool sequence) const;

  /**
   * Check internal state for ARCHIVE Forecast data triggering.
   * @param[in] url  The location of the triggering data
   * @param[out] isLeadtimeSubsample  True to use only a subsample of lead times
   *                                  specified by the parameters
   *                                  leadtimeSeconds0,
   *                                  leadtimeSeconds1, leadtimeSecondsDelta
   * @param[out] leadtimeSeconds0  Earliest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSeconds1  Latest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSecondsDelta  Resolution of leads to use when 
   *                                   isLeadtimeSubsample=true
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   * @param[in] sequence  True if sequence triggering, false if regular
   * @param[in] t0  Archive early time
   * @param[in] t1  Archive latest time
   *
   * @return true if the internal state is different than input.
   */
  bool didChange(const std::string &url, const bool isLeadtimeSubsample,
		 const int leadtimeSeconds0, const int leadtimeSeconds1,
		 const int leadtimeSecondsDelta,
		 const std::vector<int> &gentimeSubsample, const bool sequence,
		 const time_t &t0, const time_t &t1) const;

  /**
   * Set internal state for realtime observation data triggering.
   *
   * @param[in] url  The location of observation data
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   */
  void setObsTriggering(const std::string &url,
			const std::vector<int> &gentimeSubsample);

  /**
   * Set internal state for ARCHIVE Observation data triggering.
   *
   * @param[in] url  The location of observation data
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   * @param[in] t0  Archive early time
   * @param[in] t1  Archive latest time
   */
  void setObsTriggering(const std::string &url,
			const std::vector<int> &gentimeSubsample,
			const time_t &t0, const time_t &t1);

  /**
   * Set internal state for realtime forecast data triggering.
   *
   * @param[in] url  The location of the triggering data
   * @param[out] isLeadtimeSubsample  True to use only a subsample of lead times
   *                                  specified by the parameters
   *                                  leadtimeSeconds0,
   *                                  leadtimeSeconds1, leadtimeSecondsDelta
   * @param[out] leadtimeSeconds0  Earliest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSeconds1  Latest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSecondsDelta  Resolution of leads to use when 
   *                                   isLeadtimeSubsample=true
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   * @param[in] sequence  True if sequence triggering, false if regular
   */
  void setFcstTriggering(const std::string &url, const bool isLeadtimeSubsample,
			 const int leadtimeSeconds0,
			 const int leadtimeSeconds1,
			 const int leadtimeSecondsDelta,
			 const std::vector<int> &gentimeSubsample,
			 const bool sequence);

  /**
   * Set internal state for ARCHIVE Forecast data triggering.
   *
   * @param[in] url  The location of the triggering data
   * @param[out] isLeadtimeSubsample  True to use only a subsample of lead times
   *                                  specified by the parameters
   *                                  leadtimeSeconds0,
   *                                  leadtimeSeconds1, leadtimeSecondsDelta
   * @param[out] leadtimeSeconds0  Earliest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSeconds1  Latest lead to use when isLeadtimeSubsample
   * @param[out] leadtimeSecondsDelta  Resolution of leads to use when 
   *                                   isLeadtimeSubsample=true
   * @param[in] gentimeSubsample  Subset of generation times to process (or
   *                              empty to process all generation times)
   * @param[in] sequence  True if sequence triggering, false if regular
   * @param[in] t0  Archive early time
   * @param[in] t1  Archive latest time
   */
  void setFcstTriggering(const std::string &url, const bool isLeadtimeSubsample,
			 const int leadtimeSeconds0,
			 const int leadtimeSeconds1,
			 const int leadtimeSecondsDelta,
			 const std::vector<int> &gentimeSubsample,
			 const bool sequence, 
			 const time_t &t0, const time_t &t1);

protected:
private:

  /**
   * location of triggering data
   */
  std::string pUrl;               

  /**
   * subset of generation times (empty for all)
   */
  std::vector<int>  pGentimeSubsample; 

  /**
   * true if doing subset of lead times
   */
  bool pIsLeadtimeSubsample;    


  /**
   * earliest lead time (seconds) if pIsLeadtimeSubsample
   */
  int pLeadtimeSeconds0;  

  /**
   * latest lead time (seconds) if pIsLeadtimeSubsample
   */
  int pLeadtimeSeconds1;            

  /**
   * delta lead time (seconds) if pIsLeadtimeSubsample
   */
  int pLeadtimeSecondsDelta;        

  /**
   * true if triggering off of forecast data, false for observation data
   */
  bool pIsFcst;        

  time_t pArchiveTime0;       /**< earliest time if archive data */
  time_t pArchiveTime1;       /**< latest time if archive data */
  bool pIsArchive;  /**< true if archive data */
  bool pIsSequence; /**< true if sequence triggering*/
};

#endif
