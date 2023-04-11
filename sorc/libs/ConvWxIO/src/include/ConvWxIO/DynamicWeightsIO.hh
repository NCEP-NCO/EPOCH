/**
 * @file DynamicWeightsIO.hh
 * @brief This class updates the base class (DynamicsWeightsHandler) state by
 *        performing data input actions.
 * @class DynamicWeightsIO
 * @brief This class updates the base class (DynamicsWeightsHandler) state by
 *        performing data input actions.
 */

#ifndef DYNAMIC_WEIGHTS_IO_HH
#define DYNAMIC_WEIGHTS_IO_HH

#include <ConvWx/DynamicWeightsHandler.hh>

class LtInterp;
class ParmProjection;
class ParmFcstIO;
class ConvWxThreadMgr;

class DynamicWeightsIO  : public DynamicWeightsHandler
{
 public:

  /**
   *
   * Constructor, sets the range of thresholds and weights in internal state,
   * thresholds can be computed.
   *
   * @param[in] threshRange  Array of length 2 with minimum and maximum
   *                         threshold values
   * @param[in] weightRange  Array of length 2 with minimum and maximum weight
   *                         values
   */
  DynamicWeightsIO(const double *threshRange, const double *weightRange);

  /**
  * Constructor, calls update()
  * 
  * @param[in] dw  base class object to use
  * @param[in] gt  Generation time
  * @param[in] lt  Lead time interpolation information
  * @param[in] proj  Projection information
  * @param[in] parms  Input data parameters
  * @param[in] setMissingToZero  True to set value to 0 at all points where
  *                              data is missing in the internal grids.
  * @param[in] debug  True to print out extra debug output
  * @param[in,out] threads  If not NULL, thread locking and unlocking
  *                         when accessing server. If NULL, not thread safe.
  */
  DynamicWeightsIO(const DynamicWeightsHandler &dw,
		   const time_t &gt, const LtInterp &lt,
		   const ParmProjection &proj, const ParmFcstIO &parms,
		   bool setMissingToZero=false, bool debug=false,
		   ConvWxThreadMgr *threads=NULL);

  /**
   *
   * Constructor, thresholds cannot be computed.
   *
   */
  DynamicWeightsIO(void);

  /**
   * Destructor
   */
  virtual ~DynamicWeightsIO(void);

  /**
   * Update internal state using inputs.
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead time interpolation information
   * @param[in] proj  Projection information
   * @param[in] parms  Input data parameters
   * @param[in] setMissingToZero  True to set value to 0 at all points where
   *                              data is missing in the internal grids.
   * @param[in] debug  True to print out extra debug output
   * @param[in,out] threads  If not NULL, thread locking and unlocking
   *                         when accessing server. If NULL, not thread safe.
   *
   * @return true for success
   *
   * Data is loaded into internal state grids and internal status values
   * are set based on the results.  The lead times and interpolating weights
   * for the two grids (pLeadTime0, pWeight0, pLeadTime1, pW1) are pulled from
   * LtInterp input.
   * The local pGt is set to input gt. The data for the generation and lead
   * times is loaded using pLoadData(), filling in state for pHas0,
   * pDynamicWeights0, pLeadTime0 and pHas1, pDynamicWeights1, and pLeadTime1.
   */
  bool update(const time_t &gt, const LtInterp &lt, const ParmProjection &proj,
	      const ParmFcstIO &parms, bool setMissingToZero=false,
	      bool debug=false, ConvWxThreadMgr *threads=NULL);

protected:
private:

  /**
   * Load data into a FcstGrid object
   *
   * @param[in] gt  Generation time for which to load data
   * @param[in] lt  Lead time seconds for which to load data
   * @param[in] proj  Projection information to use in retrieving data
   * @param[in] parms  Input data parameters (where it is, etc).
   * @param[out] grid  FcstGrid filled in on exit.
   * @param[in] setMissingToZero  True to set value to 0 at all points where
   *                              data is missing in the internal grids.
   * @param[in] debug  True to print out extra debug output
   * @param[in,out] threads  If not NULL, thread locking and unlocking
   *                         when accessing server. If NULL, not thread safe.
   *
   * @return true if data was successfully put into grid
   */
  bool pLoadData(const time_t &gt, int lt, const ParmProjection &proj,
		 const ParmFcstIO &parms, FcstGrid &grid,
		 bool setMissingToZero=false, bool debug=false,
		 ConvWxThreadMgr *threads=NULL) const;
};


#endif
