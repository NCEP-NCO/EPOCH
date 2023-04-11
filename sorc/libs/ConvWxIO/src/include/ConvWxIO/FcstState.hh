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
 * @file  FcstState.hh
 * @brief  The state of model forecast availability at a particular generation
 *         time.
 * @class  FcstState
 * @brief  The state of model forecast availability at a particular generation
 *         time.
 *
 * State members are generation time (of model data), all the lead times
 * and whether each lead time is available, as well as some parameters
 * describing input
 */

#ifndef FCST_STATE_H
#define FCST_STATE_H

#include <vector>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/ILogMsg.hh>


class ParmProjection;
class FcstGrid;

//----------------------------------------------------------------
class FcstState
{
public:

  /**
   * Empty constructor
   */
  FcstState(void);

  /**
   * Constructor, dynamic construction using a server
   *
   * @param[in] gt  Model data generation time
   * @param[in] parms   Parameters for the model data
   *
   * @note  The constructor queries the server to fill in the availability
   * values
   */
  FcstState(const time_t &gt, const ParmFcst &parms);

  /**
   * Constructor, static constructor using status values
   *
   * @param[in] gt  Model data generation time
   * @param[in] parms   Parameters for the model data
   * @param[in] status  Pairs of Status information for all forecasts,
   *                    first = lead, second = forecast is available or not
   */
  FcstState(const time_t &gt, const ParmFcst &parms,
	    const std::vector<std::pair<int,bool> > &status);


  /**
   * Destructor
   */
  virtual ~FcstState(void);

  /**
   * Debug print to a string
   * @return  The string
   */
  std::string sprint(void) const;

  /**
   * Debug print to stdout
   */
  void print(void) const;

  /**
   * Debug print at a particular severity
   * @param[in] severity  The severity
   */
  void print(LogStream::Log_t severity) const;

  /**
   * @return True if the generation time matches an input value
   *
   * @param[in] gt  Forecast data generation time to compare against
   */
  inline bool genTimeEquals(const time_t &gt) const
  {
    return gt == pGenTime;
  }

  /**
   * @return The model generation time
   */
  inline time_t getGenTime(void) const {return pGenTime;}

  /**
   * @return  the lead time seconds for a particular index
   * @param[in] i  Index 0,1,.. into lead times
   */
  inline int getIthLeadSeconds(const int i) const
  {
    if (i >= 0 && i < static_cast<int>(pLeadStatus.size()))
    {
      return pLeadStatus[i].first;
    }
    else
    {
      return -1;
    }
  }

  /**
   * @return  Total number of forecast lead times
   */
  inline int numLeadTimes(void) const {return pParm.nlead();}

  /**
   * @return The number of forecasts for which there is no data available
   */
  int numUnavailableFcsts(void) const;

  /**
   * @return The number of forecasts for which there is no latency adjusted
   *         data available.
   *
   * @param[in] outputGt  Latency adjusted generation time
   * @param[in] outputParm  Forecast parameters associated with latency
   *                        adjustment
   *
   * @note the outputGt plus the internal state is used to get the latency,
   *       and the outputParm is used to determine those forecasts that are
   *       the wanted ones after latency adjustment.
   */
  int numUnavailableLatencyAdjustedFcsts(const time_t &outputGt,
					 const ParmFcst &outputParm) const;

  /**
   * @return True if all forecasts are available.
   */
  bool allFcstsAreAvailable(void) const;

  /**
   * @return  True if number of missing forecasts is 0, with latency adjustment.
   *
   * @param[in] outputGt  Latency adjusted generation time
   * @param[in] outputParm  Forecast parameters associated with latency
   *                        adjustment
   *
   * @note the outputGt plus the internal state is used to get the latency,
   *       and the outputParm is used to determine those forecasts that are
   *       the wanted ones after latency adjustment.
   */
  bool allLatencyAdjustedFcstsAreAvailable(const time_t &outputGt,
					   const ParmFcst &outputParm) const;

  /**
   * @return  True if all forecasts are unavailable
   */
  bool noFcstsAreAvailable(void) const;

  /**
   * @return  True if all forecasts are unavailable, after latency adjustment
   * @param[in] outputGt  Latency adjusted generation time
   * @param[in] outputParm  Forecast parameters associated with latency
   *                        adjustment
   * @note the outputGt plus the internal state is used to get the latency,
   *       and the outputParm is used to determine those forecasts that are
   *       the wanted ones after latency adjustment.
   *
   * @note This does use the server.
   *
   */
  bool noLatencyAdjustedFcstsAreAvailable(const time_t &outputGt,
					  const ParmFcst &outputParm) const;

  /**
   * Retrieve a FcstGrid for a particular lead time, with latency adjustment
   *
   * @param[in] outputGt  Latency adjusted generation time
   * @param[in] outputParm  Forecast parameters associated with latency
   *                        adjustment
   * @param[in] proj  Projection parameters
   * @param[in] leadTimeIndex  Index 0,1,..into the outputParm lead times
   * @param[out] latencyHours   Returned latency hours
   * @param[out] outputGrid  Returned FcstGrid, with the latency adjusted
   *                         generation and lead
   * 
   * @return  True if successful
   *
   * @note the outputGt plus the internal state is used to get the latency,
   *       and the outputParm is used to determine which forecasts is
   *       referred to by leadTimeIndex after latency adjustment.
   *
   * @note This does use the server.
   */  
  bool getLatencyAdjustedGrid(const time_t &outputGt,
			      const ParmFcst &outputParm,
			      const ParmProjection &proj,
			      const int leadTimeIndex,
			      double &latencyHours, FcstGrid &outputGrid) const;

  /** 
   * Retrieve ALL forecasts. At each lead time, if the forecast is available
   * it is returned, if not, a forecast with all missing data is returned.
   *
   * @param[in] proj  Projection info.
   * @param[out] outGrids  The returned vector of FcstGrid, in lead time order.
   *
   * @return True if at least one FcstGrid was available and returned.
   *
   * @note This does use the server to get and fill outGrids.
   */
  bool retrieveAllFcsts(const ParmProjection &proj,
			std::vector<FcstGrid> &outGrids) const;

  /**
   * Check for forecast availability and update the state if there was a
   * change (something  became available when it was unavailable).
   *
   * @note  This method assumes nothing ever goes from 'available'
   *        to 'not available' in the time frames of real time change.
   *
   * @return  True if there was a change in state
   *
   * @note queries the server
   */
  bool updateAvailability(void);

  /**
   * Set a particular lead time to 'unavailable'
   *
   * @param[in] ltSeconds  Lead time (seconds) of forecast
   * @return  True if successful
   */
  bool setFcstUnavailable(const int ltSeconds);

  /**
   * Set a particular lead time to 'unavailable' based on index
   *
   * @param[in] i  The index to the lead time 0,1,..
   *
   * @return true for success
   */
  bool setIthFcstUnavailable(const int i);

  /**
   * @return true if a particular lead time is available
   *
   * @param[in] ltSeconds  The lead time (seconds)
   */
  bool fcstIsAvailable(const int ltSeconds) const;

  /**
   * @return true if a particular lead time is available based on index
   *
   * @param[in] i  The index to the lead time 0,1,..
   */
  bool ithFcstIsAvailable(const int i) const;

  /**
   * Get status of a particular forecast
   *
   * @param[in] i  Index to the forecast 0,1,2,..
   * @param[out] leadHour  Lead time (hours) of this entry
   * @param[out] available  True if this forecast is available
   *
   * @return true if could set values (i was in range)
   */
  bool getIthFcstStatus(const int i, double &leadHour, bool &available) const;

  /**
   * Return strings to use for HTML 
   *
   * @param[out] gen  Generation time 'hh:mm'
   * @param[out] missing  Information about missing forecasts (string)
   *
   * @note The missing string is a list of lead hours (floating point) for
   *       which there is no forecast, or "None" if all forecasts are available.
   */
  void getLogInfo(std::string &gen, std::string &missing) const;

  /**
   * Return strings to use for HTML, with latency adjustment.
   *
   * @param[in] outputGt  Latency adjusted generation time
   * @param[in] outputParm  Forecast parameters associated with latency
   *                        adjustment
   * @param[out] gt  Generation time 'hh:mm'
   * @param[out] latency  The latency in hours (string)
   * @param[out] missing  Information about missing forecasts (string)
   * @param[out] outGt   Latency adjusted generation time 'hh:mm'
   * @param[out] outMissing  Information about missing latency adjusted
   *                         forecasts (string)
   * @note missing and outMissing are list of lead hours (floating 
   *       point) for which there is no forecast, or "None" if all forecasts
   *       are available.
   */
  void getLogInfo(const time_t &outputGt, const ParmFcst &outputParm,
		  std::string &gt, std::string &latency, std::string &missing,
		  std::string &outGt, std::string &outMissing)const;
protected:
private:

  
  time_t pGenTime; /**< generation time of the forecast (actual data) */

  /**
   *  Pairs to tell whether a lead time is available or not.
   *  For each pair:
   *   -  first = lead seconds
   *   -  second = available yes or no
   * this is ALL the lead times for this forecast, with respect to pParm
   */
  std::vector<std::pair<int,bool> > pLeadStatus;

  ParmFcstIO pParm;   /**<  Forecast parameters */

  /**
   * Update the state using the sever to see what is available at the
   * generation time
   *
   * @return true for success
   */
  bool pUpdate(void);

  /**
   * @return  Index to the pLeadStatus vector for the input lead time seconds,
   *          or BAD_INDEX for none
   *
   * @param[in] ltSec  A lead time (seconds)
   */
  int pMatch(const int ltSec) const;

  /**
   * Print out the state into a string
   *
   * @return  The string with printout of state
   */
  std::string pPrint(void) const;

  /**
   * Print out the state to logging output
   *
   * @param[in] severity  The severity to put into the logged message
   */
  void pPrint(LogStream::Log_t severity) const;


  /**
   * Retrieve a FcstGrid (if available) or create an all missing FcstGrid if not
   *
   * @param[in] available  True if the data should be available for the lead
   *                       time
   * @param[in] lt  The lead time (seconds)
   * @param[in] proj  The projection information
   * @param[in] severity  The logging severity (the method does write log
   *                      messages)
   * @param[out] outGrid  The returned FcstGrid
   *
   * @return True if the grid was filled with data, false if filled with 
   *         all missing values
   */
  bool pFillGrid(const bool available, const int lt,
		 const ParmProjection &proj,
		 LogStream::Log_t severity,
		 FcstGrid &outGrid) const;
};

#endif
