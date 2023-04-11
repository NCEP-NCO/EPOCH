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
 * @file TriggerSequenceState.hh
 * @brief The state of a sequence of triggering generation/lead times
 * 
 * @class TriggerSequenceState
 * @brief The state of a sequence of triggering generation/lead times
 *
 * This is useful with 'sequence triggering' (see InterfaceParm.hh)
 *
 * Typically a sequence of generation/lead times occurs, with lead time changing
 * more rapidly than lead time, for example:
 *  - generation=2011/10/05,18:00:00  lead=0.25 hour
 *  - generation=2011/10/05,18:00:00  lead=0.50 hour
 *  - generation=2011/10/05,18:00:00  lead=0.75 hour
 *  - ...
 *  - generation=2011/10/05,18:00:00  lead=7.75 hour
 *  - generation=2011/10/05,18:00:00  lead=8.00 hour
 *  - generation=2011/10/05,18:15:00  lead=0.00 hour
 *  - generation=2011/10/05,18:15:00  lead=0.25 hour
 *  - generation=2011/10/05,18:15:00  lead=0.50 hour
 * and so on
 *
 * This class detects when the generation time changes, and when all lead times
 * for a generation time have been accounted for
 */

#ifndef TRIGGER_SEQUENCE_STATE_H
#define TRIGGER_SEQUENCE_STATE_H

#include <ConvWx/ParmFcst.hh>

//----------------------------------------------------------------
class TriggerSequenceState
{
public:

  /**
   * @enum Update_t
   * @brief Status of generation time compared to previous generation time
   *
   * When a new generation/lead is put into the state, the returned
   * status is one of these things., either a new generation time (after
   * completing the previous generation time), or a new generation time
   * (without completing the previous one), or the same generation time as
   * before.
   */
  typedef enum
  {
    INIT,               /**< initial state (nothing at all yet) */
    NEW_GEN_COMPLETE,   /**< new generation, with all leads gotten from
			 * previous generation */
    NEW_GEN_INCOMPLETE, /**< new generation, with missing lead times from
			 *  previous generation */
    SAME_GEN            /**< same generation as before, new lead time */
  } Update_t;

  /**
   * Constructor
   *
   * @param[in] parms information about triggering forecasts
   *                  (lead times particularly.)
   */
  TriggerSequenceState(const ParmFcst &parms);

  /**
   * Destructor
   */
  virtual ~TriggerSequenceState(void);

  /**
   * Update state for triggering of new generation/lead
   *
   * @param[in] gt  New generation time
   * @param[in] lt  New lead time
   * @param[out] oldGt  Old (previous) generation time if any
   *
   * @return The enumerated state that results.
   */
  Update_t update(const time_t &gt, const int lt, time_t &oldGt);

  /**
   * Update the state when finished processing a particular lead time. 
   *
   * @param[in] lt the lead time seconds that was just processed.
   *
   * @return true if this is the last lead time in the sequence specified
   * by the state parameters
   */
  bool finish(const int lt);

protected:
private:

  /**
   * Current generation time being processed
   */
  time_t pGenTime;

  /**
   * True if current generation time is considered complete
   */
  bool pGenTimeComplete;

  /**
   * True if it is the initial situation where a random lead time
   * is returned initially (not the 0'th one)
   */
  bool pIsInit;

  /**
   * Triggering forecast information
   */
  ParmFcst pParm;

  /**
   * Status of each lead time configured for in pParm
   * 1 if that lead time has been done for the current gen time, 0 otherwise
   * (int because of rumors that vector<bool> has bugs)
   */
  std::vector<int> pLeadTimeStatus;

  /**
   * @return true if pLeadTimeStatus is all 1's
   */
  bool pHasAllLeadsAtGen(void) const;

  /**
   * Set lead time status to 0 for all lead times
   */
  void pClearLeadStatus(void);
  
};

#endif
