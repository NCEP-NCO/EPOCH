/**
 * @file DynamicWeightsHandler.hh
 * @brief Manage and maintain dynamic weights from one source
 * @class DynamicWeightsHandler
 * @brief Manage and maintain dynamic weights from one source
 *
 *
 * Weights Grid data from two lead times is stored locally, as well as
 * information about interpolating the values from each of the two Grid data
 * objects.
 */

#ifndef DYNAMIC_WEIGHTS_HANDLER_HH
#define DYNAMIC_WEIGHTS_HANDLER_HH

#include <ConvWx/FcstGrid.hh>
class LtInterp;

class DynamicWeightsHandler
{
 public:

  /**
   *
   * Constructor, sets the range of thresholds and weights in internal state,
   * thresholds are computable.
   *
   * @param[in] threshRange  Array of length 2 with minimum and maximum
   *                         threshold values
   * @param[in] weightRange  Array of length 2 with minimum and maximum weight
   *                         values
   */
  DynamicWeightsHandler(const double *threshRange, const double *weightRange);

  /**
   *
   * Constructor, thresholds are not computable.
   *
   */
  DynamicWeightsHandler(void);

  /**
   * Destructor
   */
  virtual ~DynamicWeightsHandler(void);

  /**
   * Set internal state values using inputs
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead time interpolation information
   *
   * Sets state so that no data is assumed present
   */
  void setState(const time_t &gt, const LtInterp &lt);

  /**
   * Get the interpolated weight at a grid point
   *
   * @param[in] ipos  Grid position (1 dimensional)
   * @param[out] weight   The weight
   *
   * @return true if successful
   *
   * Uses input state to decide whether to call pInterp() or pOne().
   */
  bool getWeight(int ipos, double &weight) const;  

  /**
   * Get the maximum weight at a grid point
   *
   * @param[in] ipos  Grid position (1 dimensional)
   * @param[out] weight   The weight
   *
   * @return true if successful
   *
   * Uses input state to decide whether to call pInterp() or pOne().
   */
  bool getMaxWeight(int ipos, double &weight) const;  

  /**
   * Get the minimum weight at a grid point.  If one or more is missing,
   * return false
   *
   * @param[in] ipos  Grid position (1 dimensional)
   * @param[out] weight   The weight
   *
   * @return true if successful
   *
   * Uses input state to decide whether to call pInterp() or pOne().
   */
  bool getMinWeight(int ipos, double &weight) const;  

  /**
   * Get the interpolated threshold at a grid point
   *
   * @param[in] ipos  Grid position (1 dimensional)
   * @param[out] thresh   The threshold
   * @return true if successful
   *
   * calls getWeight(), the uses that weight and calls getThreshFromWeight()
   */
  bool getThresh(int ipos, double &thresh) const;

  /**
   * Compute the threshold from a weight
   *
   * @param[in] weight  Value to compute with
   * @param[out] thresh  Resultant threshold
   *
   * @return true if successful
   *
   * Does a linear mapping from weight to threshold, using pWeightRangeMin,
   * pWeightRangeMax, pThreshRangeMin and pThreshRangeMax
   *
   */
  bool getThreshFromWeight(double weight, double &thresh) const;

  /**
   * @return The configured minimum threshold value pThreshRangeMin
   */
  inline double getThreshRangeMin(void) const { return pThreshRangeMin;}

  /**
   * Change the range of weights in state to inputs
   * @param[in] min  Minimum weight
   * @param[in] max  Maximum weight
   */
  inline void changeWeightsRange(double min, double max)
  {
    pWeightRangeMin = min;
    pWeightRangeMax = max;
  }    

  /**
   * Set grid argument values to maximum of the 2 grids (when 2) or 
   * equal to the  1 grid (when 1)
   * @return true if able to set grid, false if no grids to take max of
   *
   * @param[out] ret  Returned grid
   */
  bool maxGrid(Grid &ret) const;

  /**
   * @return xml description of state, with dataType as part of key
   * @param[in] dataType  String that goes into key
   */
  std::string xml(const std::string &dataType) const;

  /**
   * @return true if object is set correctly
   */
  inline bool isOk(void) const {return pOk;}

protected:

  bool pOk;                 /**< Flag for object correctness */

  time_t pGt;               /**< Current Dynamic weights generation time */

  bool pHas0;               /**< True if weights data for the earlier lead time
			     *  is present */
  FcstGrid pDynamicWeight0; /**< Weights data for the earlier lead time */
  int pLeadTime0;           /**< Earlier Lead time seconds */
  double pWeight0;          /**< Interpolating weight for earlier lead time */

  bool pHas1;               /**< True if weights data for the later lead time
			     *  is present */
  FcstGrid pDynamicWeight1; /**< Weights data for the later lead time */
  int pLeadTime1;           /**< Later Lead time seconds */
  double pWeight1;          /**< Interpolating weight for later lead time */

private:

  bool pHasThresh;         /**< True if limits are set and can be use to compute
			    * thresholds */
  double pThreshRangeMin;  /**< Lower limit threshold value */
  double pThreshRangeMax;  /**< Upper limit threshold value */
  double pWeightRangeMin;  /**< Lower limit weight value */
  double pWeightRangeMax;  /**< Upper limit weight value */

  /**
   * Set the dynamic weight using one input instead of interpolating 
   *
   * @param[in] ipos  The Grid index from which to get the dynamic weight
   *                  (one dimensional indexing)
   * @param[in] interpWeight  The interpolation weight for this data
   * @param[in] grid  The Grid that contains the dynamic weights
   * @param[out] weight  The returned dynamic weight
   *
   * @return true if a valid (non missing) value was returned in weight
   */
  bool pOne(int ipos, double interpWeight, const FcstGrid &grid,
	    double &weight) const;

  /**
   * Set the dynamic weight using the interpolation technique at a point
   *
   * @param[in] ipos  The index from which to get values from the two 
   *                  grids, one dimensional indexing.
   * @param[out] weight  The dynamic weight value 
   *
   * @return true if a value was returned in weight
   *
   * weight is set to 
   * pWeight0*(value from grid 0) + pWeight1*(value from grid 1)
   */
  bool pInterp(int ipos, double &weight) const;

  /**
   * @return maximum of the weights from the two lead times at a position,
   * @param[in] ipos  The position
   * @param[out] weight  Returned weight
   * 
   * Return false if neither lead time has data at the point
   */
  bool pMax(int ipos, double &weight) const;

  /**
   * @return minimum of the weights from the two lead times at a position,
   * @param[in] ipos  The position
   * @param[out] weight  Returned weight
   * 
   * Return false if one or both lead times has no data at the point
   */
  bool pMin(int ipos, double &weight) const;
};


#endif
