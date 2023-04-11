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
 * @file LpcState.hh 
 * @brief State of a particular model run, including latency adjustment, as 
 *        used by the Lagrangian Phase Correction algorithm.
 * @class LpcState
 * @brief State of a particular model run, including latency adjustment, as 
 *        used by the Lagrangian Phase Correction algorithm.
 *
 * The state information describes a particular model run, and is used by the
 * Lagrangian Phase Correction algorithm.
 *
 * The state consists of two vectors of forecast status objects.
 *
 * The first is regarding the model run itelf, for each configured for lead
 * time the model data is either available or not.  Availability is passed
 * into the LpcState object during construction or when reInitialize is called
 *
 * The second is regarding the latency adjusted model data created from
 * the input.  For this the status is whether each lead time has been
 * processed and whether the data for that lead time is 'all missing' or not.
 * These values are set for a particular lead time using LpcState methods.
 * 
 */

#ifndef LPC_STATE_H
#define LPC_STATE_H

#include <string>
#include <vector>
#include <ConvWx/LpcModelState.hh>
#include <ConvWx/LpcOutputState.hh>
#include <ConvWx/ConvWxConstants.hh>
class ParmFcst;

//----------------------------------------------------------------
class LpcState
{
public:

  /**
   * Empty constructor
   */
  LpcState(void);

  /**
   * Constructor
   *
   * @param[in] inputModelParms  Input model data parameters. The lead times
   *                             are used to build the input status vector.
   * @param[in] outputModelParms  Output (latency adjusted) model data
   *                              parameters. The lead times are used to build
   *                              the output status vector.
   * @param[in] modelGt  Input model generation time.
   * @param[in] outputGt  Output latency adjusted generation time.
   * @param[in] availableLeadSeconds  The list of lead times (seconds) that
   *                                  are currently available for the data
   *                                  specified by inputModelParms at modelGt
   */
  LpcState(const ParmFcst &inputModelParms, const ParmFcst &outputModelParms,
	   const time_t &modelGt, const time_t &outputGt,
	   const std::vector<int> &availableLeadSeconds);

  /**
   * Destructor
   */
  virtual ~LpcState(void);

  /**
   * Re initialize for new times
   *
   * @param[in] inputModelParms  Input model data parameters. The lead times
   *                             are used to build the input status vector.
   * @param[in] outputModelParms  Output (latency adjusted) model data
   *                              parameters. The lead times are used to build
   *                              the output status vector.
   * @param[in] modelGt  Input model generation time.
   * @param[in] outputGt  Output latency adjusted generation time.
   * @param[in] availableLeadSeconds  The list of lead times (seconds) that
   *                                  are currently available for the data
   *                                  specified by inputModelParms at modelGt
   */
  void reInitialize(const ParmFcst &inputModelParms,
		    const ParmFcst &outputModelParms,
		    const time_t &modelGt, const time_t &outputGt,
		    const std::vector<int> &availableLeadSeconds);


  /**
   * Debug print state
   */
  void print(void) const;

  /**
   * @return The current latency adjusted generation time
   */
  inline time_t outputTime(void) const {return pOutputGt;}

  /**
   * @return The current latency (hours) between output and input model data
   */
  inline double getLatencyHours(void) const
  {
    return static_cast<double>(pOutputGt - pModelGt)/
      convWx::DOUBLE_SECS_PER_HOUR;
  }

  /**
   * Set the input model data availability status (yes or no) for one lead time
   *
   * @param[in] lt  Model lead seconds for which to change input availability
   * @param[in] available  Availability status value
   */
  void setModelAvailability(const int lt, const bool available);

  /**
   * Set output model data status to 'all missing' for one lead time
   *
   * @param[in] lt  The latency adjusted (output) lead time seconds
   */
  void setOutputMissing(const int lt);

  /**
   * Set output model data status to 'processed' for one lead time
   *
   * @param[in] lt  The latency adjusted (output) lead time seconds
   */
  void setOutputProcessed(const int lt);

  /**
   * @return  The number of latency adjusted output lead times that have been
   *          configured for in this object
   */
  inline int noutput(void) const {return static_cast<int>(pOutput.size());}

  /**
   * @return  True if the output lead time as refered to by an index has
   *          the 'processed' status indicating 'yes'
   *
   * @param[in] i  An index into the latency adjusted output lead times, where
   *               0 is the first lead time, 1 is the second lead time,
   *               and so on.
   */
  inline bool ithOutputIsProcessed(const int i) const
  {
    return pOutput[i].second.isProcessed();
  }

  /**
   * @return  The output lead time seconds refered to by an index.
   * @param[in] i  An index into the latency adjusted output lead times, where
   *               0 is the first lead time, 1 is the second lead time,
   *               and so on.
   */
  inline int ithOutputLeadSeconds(const int i) const
  {
    return pOutput[i].first;
  }

  /**
   * @return  True if all output lead times have status 'processed' set to true
   */
  bool outputComplete(void) const;

  /**
   * Return strings describing the internal state
   *
   * @param[in] isRestart  True if the app just restarted.
   * @param[out] modelGt  Model generation time string in "hh:mm" format.
   * @param[out] latency  Model latency string (hours)
   * @param[out] missingModelLeadTimes  String with all the missing input model
   *                                    data lead times as a comma separated
   *                                    list of hour values. These are the
   *                                    input lead times for which 'available'
   *                                    is false, or the string "none" if
   *                                    all input lead times are available.
   * @param[out] outGt  Output generation time string in "hh:mm" format.
   * @param[out] missingOutputLeadTimes  String with all the missing output
   *                                     latency adjusted lead times, as a comma
   *                                     separated list of hour values. These
   *                                     are the output lead times for which
   *                                     'all missing' is true, or the string
   *                                     "none" if no output data lead times
   *                                     are 'all missing'.
   */
  void getLogInfo(const bool isRestart, std::string &modelGt,
		  std::string &latency, std::string &missingModelLeadTimes,
		  std::string &outGt,
		  std::string &missingOutputLeadTimes) const;

protected:
private:

  /**
   * The model generation time
   */
  time_t pModelGt;

  /**
   * Pairs of lead time and associated state for the input model data
   */
  std::vector<std::pair<int, LpcModelState> >  pModel;

  /** 
   * The output (latency adjusted) generation time
   */
  time_t pOutputGt;

  /**
   * Pairs of lead time and associated state for the output data
   * (latency adjusted lead times).
   */
  std::vector<std::pair<int, LpcOutputState> > pOutput;
};

#endif
