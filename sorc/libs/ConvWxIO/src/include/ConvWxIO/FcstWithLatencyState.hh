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
 * @file  FcstWithLatencyState.hh
 * @brief  A set of FcstState objects, one for each in a set of different
 *         latencies.
 * @class  FcstWithLatencyState
 * @brief  A set of FcstState objects, one for each in a set of different
 *         latencies.
 *
 * This represents the forecasts for some time into the past, with one
 * FcstState for each generation time in the past, which shows how much of that
 * forecast is available.
 *
 * This class is used to keep track of what is available and make an
 * informed decision as to which model run to use in real time.
 *
 * State is maintained through time.
 */

#ifndef FCST_WITH_LATENCY_STATE_H
#define FCST_WITH_LATENCY_STATE_H

#include <vector>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ParmFcst.hh>

class Grid;
class FcstState;
class FcstGrid;

//----------------------------------------------------------------
class FcstWithLatencyState 
{
public:

  /**
   * No state information
   */
  FcstWithLatencyState(void);

  /**
   * Build from a fixed known time and latency.  The result is the state
   * for one latency.
   *
   * @param[in] t  A time
   * @param[in] latencyHours  Latency of the forecast
   * @param[in] inputParm  The input forecast data parameter settings
   * @param[in] outputParm  The output (latency adjusted) forecast data
   *                        parameter settings
   *
   * @note Queries server to set the state
   */
  FcstWithLatencyState(const time_t &t, const double latencyHours,
		       const ParmFcst &inputParm, const ParmFcst &outputParm);

  /**
   * Destructor
   */
  virtual ~FcstWithLatencyState(void);

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * Debug print for a given severity
   *
   * @param[in] severity The severity
   */
  void print(LogStream::Log_t severity) const;

  /**
   * Rebuild state using the existing state and new information.
   *
   * @param[in] latencyAdjustedGt  The latency adjusted generation time i.e.
   *                               the current time.
   * @param[in] gt  The set of model generation times to include.
   * @param[in] parmIn  Input (model) data parameters
   * @param[in] parmOut  Output (latency adjusted model) parameters
   * @param[out] didChange  Set to true if change to the internal state
   *                        occurred.
   * @param[out] isComplete  True if the newest model in the state has data 
   *                         available for all the wanted latency adjusted
   *                         lead times.
   *
   * @note Queries a server to set state availability values
   */
  void rebuild(const time_t &latencyAdjustedGt, const std::vector<time_t> &gt,
	       const ParmFcst &parmIn, const ParmFcst &parmOut,
	       bool &didChange, bool &isComplete);

  /**
   * Determine the best state (model run) based on internal settings.
   * Minimize the number of unavailable wanted forecasts, and maximize 
   * the model run generation time
   *
   * @note  Sets the internal state value: pFcstUsedIndex
   *
   * @note  This method does NOT query the server, it uses existing state
   *        availability values
   *
   * @return true if successful
   */
  bool determineBest(void);

  /**
   * Get the FcstGrid for inputs, using pFcstUsedIndex to choose
   * which model run to use. 
   *
   * @note This does query the server to retrieve the FcstGrid
   *
   * @param[in] proj  Parameters for data access
   * @param[in] leadTimeIndex  Index value 0,1,..to the latency adjusted lead
   *                           times
   * @param[out] latencyHours  Latency (hours)
   * @param[out] outGrid  FcstGrid containing latency adjusted generation/lead times.
   * @param[out] hasData  True if returned FcstGrid has actual data, false if
   *                      it contains all missing data
   * @return true for success
   */  
  bool getLatencyAdjustedGrid(const ParmProjection &proj,
			      const int leadTimeIndex, double &latencyHours,
			      FcstGrid &outGrid, bool &hasData) const;

  /**
   * @return The model generation time for the model run specified by pFcstUsedIndex,
   *         or BAD_TIME if none
   *
   * @note This time is for the actual model data, i.e. it is not latency
   *       adjusted.
   */
  time_t getModelGenTimeUsed(void) const;

  /**
   * @return The newest model generation time in the state, or BAD_TIME if none
   *
   * @note  This time is for the actual model data, i.e. it is not latency
   *        adjusted.
   */
  time_t getModelGenTimeNewest(void) const;

  /**
   * @return  True if all latency adjusted model forecasts from the newest
   *          model run in the state are available.
   *
   * @note Uses the pParmOut and pLatencyAdjustedGenTime values.
   */
  bool isComplete(void) const;

  /**
   * Return a copy of a particular FcstState 
   *
   * @return  True if the wanted FcstState was found
   *
   * @param[in] gt  Model generation time (actual) to get the FcstState for
   * @param[out] state  Returned FcstState
   */
  bool findMatchingModelGt(const time_t &gt, FcstState &state) const;
  
  /**
   * Returns information to be put into HTML output, using the model
   * run given by pFcstUsedIndex
   *
   * @param[out] gt  String 'hh:mm' for the model generation time
   * @param[out] latency  String latency hours of model data
   * @param[out] missing  String list of missing lead hours
   * @param[out] outGt  String  'hh:mm' for the latency adjusted generation time
   * @param[out] outMissing  List of missing latency adjusted lead hours
   */
  void getLogInfo(std::string &gt, std::string &latency, std::string &missing,
		  std::string &outGt, std::string &outMissing)const;

  /**
   * @return The latency hours for the model run specified by pFcstUsedIndex,
   * or BAD_LATENCY_HOURS if none.
   */
  double getLatencyHours(void) const;
  
  /**
   * @return  True if no model data is available for the model run specified
   *          by pFcstUsedIndex
   */
  bool noneAvailable(void) const;

protected:
private:

  /**
   * The current time (the latency adjusted generation time)
   */
  time_t pLatencyAdjustedGenTime;

  /**
   * Index to the 'best' FcstState
   */
  int pFcstUsedIndex;

  /**
   * The forecast states, one for each of a set of latencies
   * These have the actual generation/lead times without latency adjustment
   */
  std::vector<FcstState> pState;

  /**
   * Output forecast parameters
   */
  ParmFcst pParmOut;

  /**
   * Rebuild the local state for a particular model run
   *
   * @param[in] gt  Model generation time
   * @param[in] parmIn  Parameters for model data
   * @param[out] didChange  True if state has changed
   *
   * @return Rebuilt state
   *
   * @note This method queries the server to look for state changes
   */
  FcstState pRebuild(const time_t &gt, const ParmFcst &parmIn,
		     bool &didChange);
  /**
   * @return  A string printout of the local state
   */
  std::string pPrint(void) const;

  /**
   * Print out the local state
   * @param[in] severity  The severity to give the message
   */
  void pPrint(LogStream::Log_t severity) const;
};

#endif
