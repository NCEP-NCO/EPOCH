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
 * @file BlendingInputData.hh 
 * @brief Blending algorithm input data access, reads FcstGrid input with wait
 * @class BlendingInputData
 * @brief Blending algorithm input data access, reads FcstGrid input with wait
 *
 * This class is responsible for filling a FcstGrid object with data,
 * allowing a waiting period for the data to become available, up to a maximum.
 * 
 * The fillFcstGridWithWait method is called once per forecast and the total
 * time spent waiting for forecasts is incremented by each such call to method
 * by the amount of time spent waiting.  If the
 * upper wait limit is exceeded, the FcstGrid is not filled with data,
 * and that call and subsequent calls to the method return false.
 *
 */

#ifndef BLENDING_INPUT_DATA_HH
#define BLENDING_INPUT_DATA_HH

#include <string>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include <ConvWx/ParmProjection.hh>
class FcstGrid;

class BlendingInputData
{
 public:

  /**
   * Constructor
   *
   * @param[in] name  A name to use in logging messages
   * @param[in] proj  The projection parameters
   * @param[in] parms  The parameters specifying the input data
   * @param[in] maxWaitSeconds  The maximum time (seconds) to wait for
   *                            data that is not available before giving up.
   */
  BlendingInputData(const std::string &name, const ParmProjection &proj,
		    const ParmFcst &parms, const int maxWaitSeconds);

  /**
   * Destructor
   */
  virtual ~BlendingInputData(void);

  /**
   * Fill a FcstGrid with data for a particular generation and lead time,
   * with waiting up to a maximum total wait limit.
   *
   * @param[in] gt  Forecast generation time requested
   * @param[in] lt  Forecast Lead seconds requested.
   * @param[in,out] secondsSpentWaiting  Total number of seconds spent waiting
   *                                     prior to this call (on entry), then
   *                                     incremented by this method by the
   *                                     number of seconds spent waiting for
   *                                     the forecast that was requested.
   * @param[out] outGrid  The FcstGrid to be filled with data
   * @param[out] allMissing  True if the returned FcstGrid was filled with all
   *                         missing data
   *
   * @return  true if the FcstGrid was filled, false if not (no more waiting).
   *
   * @note will wait if data is not available until either data becomes 
   *       available, or the upper limit for time spent waiting is reached.
   *
   * @note allMissing will be set to true if either the method returns false,
   *       or data was read in but the FcstGrid contained only missing data.
   */
  bool fillFcstGridWithWait(const time_t &gt, int lt, int &secondsSpentWaiting,
			    FcstGrid &outGrid, bool &allMissing) const;

  /**
   * Fill a FcstGrid with data for a particular generation and lead time,
   * with waiting up to a maximum total wait limit.
   *
   * @param[in] gt  Forecast generation time requested
   * @param[in] lt  Forecast Lead seconds requested.
   * @param[in,out] secondsSpentWaiting  Total number of seconds spent waiting
   *                                     prior to this call (on entry), then
   *                                     incremented by this method by the
   *                                     number of seconds spent waiting for
   *                                     the forecast that was requested.
   * @param[out] outGrid  The FcstGrid to be filled with data
   * @param[out] allMissing  True if the returned FcstGrid was filled with all
   *                         missing data
   * @param[in,out] threads Threading which is locked and ulocked within 
   *                        the method
   * @return  true if the FcstGrid was filled, false if not (no more waiting).
   *
   * @note will wait if data is not available until either data becomes 
   *       available, or the upper limit for time spent waiting is reached.
   *
   * @note allMissing will be set to true if either the method returns false,
   *       or data was read in but the FcstGrid contained only missing data.
   */
  bool fillFcstGridWithWait(const time_t &gt, int lt, int &secondsSpentWaiting,
			    FcstGrid &outGrid, bool &allMissing,
			    ConvWxThreadMgr &threads) const;

  /**
   * @return true if input generation/lead times have data for the configured
   * parameters
   * @param[in] gt
   * @param[in] lt
   */
  inline bool hasData(const time_t &gt, int lt) const
  {
    return pParm.hasData(gt, lt);
  }
  
protected:


private:

  std::string pName;     /**< Debug name */
  ParmProjection pProj;  /**< Projection info */
  ParmFcstIO pParm;      /**< Input data info */
  int pMaxWaitSeconds;   /**< Maximum seconds to wait for data */

  static const int pWaitTick; /**< Seconds sleep between read attempts */

  /**
   * One attempt at filling a FcstGrid with data, no waiting.
   *
   * @param[in] gt  Gen time of read request
   * @param[in] lt  Lead seconds of read request
   * @param[out] outGrid  The FcstGrid that is to be filled with data
   * @param[out] allMissing  True if the FcstGrid was filled with all missing
   *                         data
   * @return  true if the FcstGrid contains data for the gt/lt, false if not.
   *
   */
  bool pLoadData1(const time_t &gt, int lt, FcstGrid &outGrid,
		  bool &allMissing) const;

  /**
   * Wait for pWaitTick seconds unless totalWaitSeconds exceeds maximum
   * waiting limit, then return.
   * 
   * @param[in,out] totalWaitSeconds  Total number of seconds spent waiting.
   *                                  Incremented by this method.
   * @return  true if waiting happened, false if the limit has been exceeded
   */
  bool pWait(int &totalWaitSeconds) const;
};


#endif
