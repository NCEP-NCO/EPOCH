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
 * @file FcstWait.hh 
 * @brief Wait for forecasts to become available.
 * @class FcstWait
 * @brief a class designed to wait for forecasts to become available.
 *
 * It checks for state changes at specified intervals up to a max wait.
 * If state is changing it might wait the entire maxWait, if state is
 * unchanging, it might wait less before giving up and using what is present. 
 * 
 * Waits a 'reasonable' amount of time in some situations.
 *
 * Rules:
 * - 1. Cannot switch to a generation time older than one that was previously used.
 * - 2. Wait up to the full alloted time only when either of:
 *   -  a. The newest possible model generation time has changed from 15 minutes ago
 *   -  b. It is an imperfect state (the newest possible generation time is not
 *         completely present) and there is some change in the state from 15
 *         minutes ago
 * - 3. Stop waiting when any one of:
 *   -  a. Things are now 'perfect' (the newest possible generation time is completely
 *         present)
 *   -  b. The full waiting time is up
 *   -  c. It is archive mode processing.
 *
 * Uses a fairly elaborate scheme to optimize tradeoffs between
 * waiting a long time and getting on with things using older data.
 *
 * @note Designed for High resolution model data which has known latencies in
 * its model runs of 2 or more hours, and is known to regularly miss some
 * forecasts within individual model runs.
 *
 * This allows the choice to 'go on' when it seems like that is the best
 * thing to do, and wait when that seems like the best thing to do.
 * 
 */

#ifndef FCST_WAIT_HH
#define FCST_WAIT_HH

#include <ConvWxIO/FcstWithLatencyState.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/ParmProjection.hh>

class ParmPc;

//----------------------------------------------------------------
class FcstWait
{
public:

  /**
   * Constructor
   */
  FcstWait(void);

  /**
   * Destructor
   */
  virtual ~FcstWait(void);

  /**
   * Set the internal state regarding realtime (if it is not realtime we don't
   * actually wait around for anything)
   *
   * @param[in] isRealtime  True for realtime, false for archive mode
   */
  void setRealtime(const bool isRealtime);

  /**
   * Set the internal parameters from inputs.
   *
   * @param[in] parms  ParmPc object, which has needed input/output/projection
   *                   parameters.
   * @param[in] maxWaitSec  Maximum seconds to wait for model data
   * @param[in] waitSleepSec  The sleep amount between checks for model data
   */
  void setParms(const ParmPc &parms, const int maxWaitSec,
		const int waitSleepSec);

  /**
   * Set the internal parameters from input.
   *
   * @param[in] inputParm Parameters specifying input data
   * @param[in] outputParm Parameters specifying output data (latency adjusted)
   * @param[in] proj  Projection information
   * @param[in] maxWaitSeconds  Maximum seconds to wait for model data
   * @param[in] sleepSeconds The sleep amount between checks for model data
   */
  void setParms(const ParmFcst &inputParm, const ParmFcst &outputParm,
		const ParmProjection &proj, const int maxWaitSeconds,
		const int sleepSeconds);

  /**
   * Wait for model data to be available for a range of possible generation times,
   * not knowing which might be available, based on the real time and on
   * an allowed latency range.
   *
   * Given a real time t and a range of latencies, look for model runs
   * generated within t-latency[0] and t-latency[1], and apply the waiting
   * algorithm in an attempt to come up with a good model run.
   *
   * @param[in] t  Real time
   * @param[in] modelLatencyHours  The range of model latencies first being
   *                               smallest latency, second being largest
   * @return true for success (waiting yielded something useful)
   */
  bool waitForModelData(const time_t &t, const int modelLatencyHours[2]);

  /**
   * Wait for model data to be available for one known generation time
   * @param[in] t  Real time
   * @param[in] gt  Model generation time to wait for
   *
   * @return true for success (waiting yielded something useful)
   */
  bool waitForModelData(const time_t &t, const time_t &gt);

  /**
   * Load data for a particular lead time, giving the returned data
   * latency adjusted generation and lead time values
   *
   * @param[in] leadTimeIndex  The index 0,1,2,.. to the latency adjusted
   *                           (output) values.
   * @param[out] outGrid  The returned data, with latency adjusted generation and 
   *                      lead times.
   * @param[out] latencyHours  The latency of the data (hours).
   * @param[out] hasData  True if outGrid has valid data, false if it is all
   *                      missing data.
   *
   * @return true if data was loaded.
   *
   * @note queries server to retrieve the data
   */
  bool loadLatencyAdjustedFcst(const int leadTimeIndex, FcstGrid &outGrid,
			       double &latencyHours,  bool &hasData) const;

  /**
   * Load data for a particular lead time, giving the returned data
   * latency adjusted generation and lead time values, with time checks.
   *
   * Checks that the internal state generation time matches an input value, and that
   * the internal latency matches the input time - the generation time.
   *
   * @param[in] t  Input time used to check latency
   * @param[in] gt  Expected generation time in the internal state.
   * @param[in] leadTimeIndex  The index 0,1,2,.. to the latency adjusted
   *                           (output) values.
   * @param[out] outGrid  The returned data with latency adjusted generation and lead
   *                      times.
   * @param[out] hasData  True if outGrid has valid data, false if it is all
   *                       missing data.
   *
   * @return true if checks succeed and data was loaded, false otherwise.
   */
  bool loadLatencyAdjustedFcstWithTimeCheck(const time_t &t,
					    const time_t &gt, 
					    const int leadTimeIndex,
					    FcstGrid &outGrid, bool &hasData);

  /** 
   * @return A reference to the internal FcstWithLatencyState object
   */
  inline const FcstWithLatencyState &getStateConst(void) const
  {
    return pState;
  }

  /**
   * @return The model generation time for the 'best' state, BAD_TIME for none.
   */
  inline time_t getModelGenTime(void) const
  {
    return pState.getModelGenTimeUsed();
  }


  /**
   * @return The model latency hours for 'best' state, BAD_LATENCY_HOURS for
   *         none.
   */
  inline double getLatencyHours(void) const
  {
    return pState.getLatencyHours();
  }

protected:
private:

  bool pRealtime;              /**< True for realtime mode */
  ParmFcst pInputParms;        /**< Parameters for input model data */
  ParmFcst pOutputParms;       /**< Parameters for output (latency adjusted)
				*   model data */
  ParmProjection pProj;        /**< Projection parameters */
  int pRtWaitMaxSeconds;       /**< Maximum wait seconds */
  int pRtWaitSleepSeconds;     /**< Seconds to sleep between tests */
  FcstWithLatencyState pState; /**< State, which is maintained through time */
  bool pFull;                  /**< True if doing full waiting right now */

  /**
   * True if the state of model data has changed compared to the previous
   * data time
   */
  bool pDidChange;

  /**
   * True if the most recent model data has enough lead times to allow complete
   * lead time outputs, when latency is taken into account.
   */
  bool pPerfect;

  /**
   * Retrieve the last model data used and last model data available
   *
   * @param[in] t  Current data time
   * @param[out] gen0  Model data time that has been used before (gotten
   *                   from pState)
   * @param[out] gen1  Newest model data available (gotten from pState)
   */
  void pWaitForModelInit(const time_t &t, time_t &gen0, time_t &gen1) const;

  /**
   * Waiting algorithm.. wait for model data to 'resolve', and return when
   * things are perfect, or ran out of wait time.
   *
   * @param[in] gen0  Previously used model generation time 
   * @param[in] gen1  Newest generation time with at least some model data
   * @param[in] gt  All possible generation times to check
   * @param[in] t  Current data time.
   * @return True if there is a generation time that can be used
   */
  bool pWaitAlg(const time_t &gen0, const time_t &gen1,
		const std::vector<time_t> &gt, const time_t &t);

  /**
   * Build the initial state using an input set of generation times and the current
   * time.
   *
   * @param[in] gen0  Previously used model generation time 
   * @param[in] gen1  Newest generation time with at least some model data
   * @param[in] gt  All possible generation times
   * @param[in] t  Current data time.
   */
  void pSetStateBeforeWaiting(const time_t &gen0, const time_t &gen1, 
			      const std::vector<time_t> &gt, const time_t &t);

  /**
   * Invoke an algorithm to determine whether 'full' waiting should happen.
   * Do full waiting only if something changed from last time.
   *
   * @param[in] previousUsedGen  Previously used generation time (last cycle)
   * @param[in] previousNewestGen  Previously newest generation time with data (last
   *                               cycle)
   * @param[in] newestGen  Current newest generation time that has data
   * @return True for full waiting
   */
  bool pFullWaitingTest0(const time_t &previousUsedGen,
			 const time_t &previousNewestGen,
			 const time_t &newestGen);

  /**
   * @return  True if more waiting is still possible and is still needed
   *
   * @param[in] waitSeconds  The number of seconds total spent waiting so far
   */
  bool pNeedToWait(const int waitSeconds) const;

  /**
   * Adjust the state to reflect a new situation after one iteration of waiting
   *
   * @param[in] t  Current data time
   * @param[in] gt  The set of all possible model generation times
   */
  void pAdjustStateAfterWaiting(const time_t &t, const std::vector<time_t> &gt);

  /**
   * @return True if the input number of seconds is below a parameterized
   * maximum
   *
   * @param[in] elapsedSeconds  Total seconds spent waiting
   */
  bool pShouldWait(const int elapsedSeconds) const;

  /**
   * Sleep a parameterized number of seconds, and increase a counter
   *
   * @param[in,out] waitSeconds The counter for total number of seconds spent
   *                            sleeping
   */
  void pWait(int &waitSeconds) const;
};

#endif
