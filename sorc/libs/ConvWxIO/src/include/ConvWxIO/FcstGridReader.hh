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
 * @file FcstGridReader.hh
 * @brief Reads FcstGrid data and manages data that has been read in
 * @class FcstGridReader
 * @brief Reads FcstGrid data and manages data that has been read in
 *
 * A FcstGrid is maintained locally, and methods are used to 
 * make particular data available, and to get access to it
 *
 * The FcstGrid can be for observations data (no lead time) or for
 * forecast data with a lead time.
 *
 * In most cases, the access of data is fairly straightforward. The exception
 * is the case of forecast data with latency, which is
 * typically the incoming high resolution model data, for which there is a lag
 * of hours between the model run time and the real time.  A latency adjustment
 * algorithm is part of this class.
 */

# ifndef    FCST_GRID_READER_HH
# define    FCST_GRID_READER_HH

#include <string>
#include <vector>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/FcstGrid.hh>

//----------------------------------------------------------------
class FcstGridReader
{
public:

  /**
   * Constructor..empty
   */
  FcstGridReader(void);

  /**
   * Constructor for observations data, or for forecast data with no
   * latency adjustment.
   *
   * @param[in] isObs  True for observations data, false for forecasts
   * @param[in] parms  The parameters for this data
   * @param[in] proj  Projection for the data
   */
  FcstGridReader(const bool isObs, const ParmFcst &parms,
		 const ParmProjection &proj);

  /**
   * Constructor for forecast data with latency adjustment.
   *
   * @param[in] inputParms  The parameters for the forecast (input) data
   * @param[in] latencyParms  The parameters for the latency adjusted (output)
   *                          data
   * @param[in] proj  Projection for the data
   * @param[in] realtime  True for realtime mode, false for archive mode
   * @param[in] maxWaitSeconds  In realtime, maximum time allowed to wait
   *                            for latency adjusted model data to be present
   *                            before reverting to an older model run,
   *                            ignored in archive mode
   * @param[in] archiveLatencySeconds  Latency to use for the model data
   *                                   in archive mode, ignored in real time
   *                                   mode
   */
  FcstGridReader(const ParmFcst &inputParms, const ParmFcst &latencyParms,
		 const ParmProjection &proj, const bool realtime,
		 const int maxWaitSeconds, const int archiveLatencySeconds);

  /**
   * Destructor
   */
  virtual ~FcstGridReader(void);

  /**
   * Update the internal state as needed for the case of observations data.
   * If the current internal state already contains a FcstGrid at the input
   * time, the internal state is not changed.
   * 
   * @param[in] t  The time of the observations data that is wanted 
   * @param[out] isnew  Set to true if the the internal state has changed,
   *                    i.e. a FcstGrid for time t was retrieved.
   *
   * @return true if the local FcstGrid has data for time t, either when
   *         isnew=true and new data has been put into the state, or when
   *         isnew=false and the data already in the state is for time=t.
   *
   * @note if the constructor was not the one for observations data, false
   *       is returned.
   */
  bool obsUpdateState(const time_t &t, bool &isnew);

  /**
   * Update the internal state of forecast data, with or without latency
   *
   * @param[in] gt  The generation time of the forecast that is wanted.
   *                If this is an object configured for latency adjustment,
   *                it is the latency adjusted generation time of the
   *                forecast data that is wanted
   * @param[in] lt  The lead seconds of the forecast that is wanted. If this
   *                is an object configured for latency adjustment, it is the
   *                latency adjusted lead seconds of the forecast data
   *                that is wanted
   * @param[out] isnew  Set to true if the the internal state has changed,
   *                    i.e. a FcstGrid for time t was retrieved.
   *
   * @return  True if the local FcstGrid has data for the requested time and
   *          lead time, either when isnew=true and new data has been put
   *          into the state, or when isnew=false and the data already in
   *          the state is for the requested forecast.
   *
   * @note If there is no data present, and no latency adjustment required,
   *       an attempt is made to retrieve data for the gt/lt input.
   *
   * @note If there is no data present, and latency adjustment is required,
   *       AND the input gt is different than in previous calls to this method,
   *       a model run must be chosen. If the gt is the same as in previous
   *       calls to this method, the data for the latency adjusted times is
   *       retrieved.
   *
   * @note when the generation time does change for latency adjustment, the
   *       'wait' algorithm is invoked. (see pModelSetGenTime());
   *
   * @note if the constructor was not the one for observations data, false
   *       is returned.
   */
  bool fcstUpdateState(const time_t &gt, const int lt, bool &isnew);

  /**
   * Clear internal state to 'no data is present'
   */
  void setStateNoData(void);

  /**
   * @return  Reference to the local FcstGrid
   */
  inline const FcstGrid &gridRef(void) const { return pCurrentGrid;}
 
  /**
   * @return  Generation time of the local FcstGrid (actual)
   *
   * Actual means without any latency adjustments
   */
  inline int getActualDataGenTime(void) const {return pGtActual;}

  /**
   * @return  Lead time seconds of the local FcstGrid (actual)
   *
   * Actual means without any latency adjustments
   */
  inline int getActualDataLeadSeconds(void) const {return pLtActual;}

  /**
   * @return  Latency (hours) of the local FcstGrid.
   */
  double getLatencyHours(void) const;

protected:
private:

  bool pIsObsData;  /**<  True for observation data, false for forecast data */
  bool pIsLatencyAdjusted; /**< True for the latency adjustment situation
			    * (forecasts only) */
  ParmFcstIO pReadParms; /**< Parameters for the actual data to be read */

  /**
   * Parameters for forecasts after latency adjustment (maybe a different
   * set of lead times). This has meaning only when pIsLatencyAdjusted=true
   */
  ParmFcst pReadParmsLatencyAdjusted; 

  ParmProjection pProj; /**< Projection information */
  FcstGrid pCurrentGrid; /**< The current data */
  time_t pGtLatencyAdjusted; /**< Latency adjusted generation time of the data, this
			      * is the time that is requested in update() */
  int pLtLatencyAdjusted; /**< Latency adjusted lead seconds (forecast data
			   * only), this is the lead time that is requested
			   * in update() */
  time_t pGtActual;  /**< Input (actual) generation time of the data */
  int pLtActual;     /**< Input (actual) lead seconds (forecast data only) */

  bool pGridIsCurrent; /**< True if the data is valid (the data content
			* agrees with  the generation and lead time values) */
  bool pIsRealtime;  /**< True for real time , false for archive */
  int pArchiveLatencySeconds; /**< In archive mode, the latency to use */
  int pMaxWaitSeconds; /**< In realtime mode, the maximum time to wait for
			* data */

  /**
   * Retrieve forecast data at pGtActual, pLtActual
   *
   * @return true if successful
   */
  bool pLoadFcstData(void);

  /**
   * When doing latency adjustment set the input and latency adjusted model generation
   * times using one of two latency adjustment techniques (realtime or archive)
   *
   * @return  true if successful
   */
  bool pSetModelGenTime(void);

  /**
   * When doing latency adjustment set the input and latency adjusted model generation
   * times using the real time latency adjustment technique (includes waiting)
   *
   * @return  true if successful
   *
   * - Look back at generation times so that the oldest such generation time at its maximum
   *   (actual) lead time gives data valid at the maximum (latency adjusted)
   *   lead time from the requested (latency adjusted) generation time.
   * - Get all generation times from that oldest one up to the present that are
   *   available right now.
   * - Repeat
   *     - pSetModelGen1()
   * - until successful or run out of time
   */
  bool pSetModelGenTimeRealtime(void);

  /**
   * Try one time to choose a model generation time.  If haven't waited too long
   * test only the newest model generation time in the input set of generation times,
   * otherwise, adjust to an older model generation time
   *
   * @param[in] availableModelGt  Vector of model generation times that are available
   * @param[in] minValidT  The smallest valid time needed
   * @param[in] maxValidT  The largest valid time needed
   * @param[in,out] waitSeconds Number of seconds spent waiting for something to
   *                            change, updated by this method
   *
   * @return true if the model generation time pGtActual has been set
   *
   * Use the newest input availableModelGt and range of valid times to
   * check if all lead times in the range that will cover this valid time
   * range are available or not.  If they are all available, that newest
   * generation time is used. Otherwise, the algorithm sleeps and returns
   * false, or calls pAdjustToOlderModelGen() if can't sleep any more.
   */
  bool pSetModelGen1(const std::vector<time_t> &availableModelGt,
		     const time_t &minValidT, const time_t &maxValidT,
		     int &waitSeconds);

  /**
   * set pGt to use the best older model run it can possibly fine
   *
   * @param[in] availableModelGt  Vector of model generation times that are available
   * @param[in] minValidT  The smallest valid time needed
   * @param[in] maxValidT  The largest valid time needed
   * @param[in] origNlead  Number of lead times that were available
   *                       in the most current model run (not a full set)
   * @param[in] maxNlead  The number of lead times needed after latency
   *                      adjustment to cover the valid time range specified
   *
   * On entry, origNlead < maxNlead (thats why this is called)
   *
   * pGt is ALWAYS set on exit
   */
  void pAdjustToOlderModelGen(const std::vector<time_t> &availableModelGt,
			      const time_t &minValidT, const time_t &maxValidT,
			      const int origNlead, const int maxNlead);
  /**
   * When doing latency adjustment set the input and latency adjusted model generation
   * times using a latency parameter
   *
   * @return  true if successful
   */
  bool pSetModelGenTimeArchive(void);

};

# endif 
