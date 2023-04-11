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
 * @file  GridAverage.hh 
 * @brief Averaging of data from more than one grid, with or without weights
 * @class GridAverage 
 * @brief Averaging of data from more than one grid, with or without weights
 */

#ifndef GRID_AVERAGE_H
#define GRID_AVERAGE_H

#include <ConvWx/ParmProjection.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/MultiGrid.hh>

class ParmFcstIO;

//----------------------------------------------------------------
class GridAverage
{
public:

  /**
   * Constructor
   *
   */
  GridAverage(void);

  /**
   * Constructor
   *
   * @param[in] p  Projection used to build local grids
   * @param[in] num Total number of grids that will be averaged
   */
  GridAverage(const ParmProjection &p, const int num);


  /**
   * Destructor
   */
  virtual ~GridAverage(void);

  /**
   * Accumulate weights values to state from one of the input weights grids,
   * using lookback to find some data
   *
   * @param[in] gt  Gen time
   * @param[in] lt  Lead seconds
   * @param[in] input  Input spec
   * @param[in] toleranceSeconds  Max xeconds to look for weights data < gt
   */
  void accumulateWeightsInfo(const time_t &gt, int lt, const ParmFcstIO &input,
			     int toleranceSeconds);

  /**
   * Accumulate weights values to state from one of the input weights grids,
   * using input values for gen and lead
   *
   * @param[in] gt  Weights Gen time to use as is
   * @param[in] lt  Lead seconds to use
   * @param[in] url  Url with weights data
   * @param[in] field  Field name of weights
   * @param[in] remap  True to remap using local projection on read
   */
  void accumulateWeightsInfo(const time_t &gt, const int lt,
			     const std::string &url,
			     const std::string &field, const bool remap);

  /**
   * Finish accumulating  weights information
   */
  void finishAccumulateWeightsInfo(void);

  /**
   * Set background weight either as a fuzzy mapping output, or as 1.0
   * @param[in] v  Value to send to fuzzy function
   * @param[in] f  Fuzzy function, or an empty mapping
   */
  void setBackgroundWeight(double v,  const ConvWxFuzzy &f);

  /**
   * Set background weight to a constant value
   * @param[in] v  Value to use as background weight
   */
  void setBackgroundWeight(double v);

  /**
   * Initialize prior to averaging
   */
  void init(void);

  /**
   * Add to average, use background weight
   * @param[in] grid  Grid to add to averaging state
   *
   * @return true for success
   */
  bool accumulate(const MultiGrid &grid);

  /**
   * Add to average, use input weight if possible, or background weight,
   * or average
   * @param[in] grid  Grid to add to averaging state
   * @param[in] weights  Grid with weights to use if possible
   *
   * @return true for success
   */
  bool accumulate(const MultiGrid &grid, const Grid &weights);

  /**
   * Take average of what was accumulated
   * @param[in] minFcsts  Minimum number of grids to average, if not that many
   *                      output is all missing
   * @param[out] mg  The averaged data fields
   * @return true for all went well
   */
  bool average(const int minFcsts, MultiGrid &mg);

  /**
   * If possible load the newest weights data with gen time in range 
   * [gt-toleranceSeconds, gt] at the lead time
   *
   * @param[in] gt  Gen time
   * @param[in] lt  Lead time
   * @param[in] input Parameters for input weights data
   * @param[in] proj  Projection information
   * @param[in] toleranceSeconds   Window
   * @param[out] wtg  Weights grid
   * @param[in] debug  True for debugging
   *
   * @return true for success
   */
  static bool loadWeightsData(const time_t &gt, int lt, const ParmFcstIO &input,
			      const ParmProjection &proj, int toleranceSeconds,
			      Grid &wtg, bool debug=false);

  /**
   * If possible find the newest weights data gen time in range 
   * [gt-toleranceSeconds, gt] with the maximum number of lead times available
   *
   * @param[in] gt  Gen time
   * @param[in] lt  Not used
   * @param[in] url  Location of input weights data
   * @param[in] toleranceSeconds   Window
   * @param[out] weightsGt  Chosen gen time
   *
   * @return true for success
   */
  static bool findBestWeightsGenTime(const time_t &gt, int lt,
				     const std::string &url,
				     int toleranceSeconds,
				     time_t &weightsGt);
protected:

private:

  ParmProjection pProj;  /**< Projection, used to read in data */

  int pNumGrids;     /**< Number of grids to average */
  int  pNumSum;      /**< Number of things averaged so far */
  MultiGrid pNum;    /**< Counts at each point */
  MultiGrid pSum;    /**< Sum of values at each point */
  MultiGrid pCount;  /**< Count of how many things are averaged*/


  double pBackgroundWeight;  /**< Background weight value */

  bool pWeights; /**< Set to true when pWtNum and pWtAve are completely set */
  bool pNoWeights; /**< True when pWtNum and pWtAve are not at all set */
  Grid pWtNum; /**< Grid with number of weights grids with non-missing
		* data at each point, when weights are in play */
  Grid pWtAve; /**< Grid with average value for all weights grids data
		* at each point, when weights are in play */

  /**
   * At each gridpoint in input grid with data, increment count by one,
   * num by the local background weight, and sum by value*(local background wt)
   *
   * @param[in] g  Grid to traverse
   * @param[in,out]  count  Grid to update
   * @param[in,out]  num    Grid to update
   * @param[in,out]  sum    Grid to update
   */
  void pAccumulate(const Grid &g, Grid &count, Grid &num, Grid &sum);

  /**
   * At each gridpoint in input grid with data, determine the weight at that
   * point by calling pDetermineWeight() with input weights as an argument.
   * If the weight is > 0
   *   increment count by one, num by the weight, and sum by value*weight
   *
   * @param[in] g  Grid to traverse
   * @param[in] weight  Weights Grid to use 
   * @param[in,out]  count  Grid to update
   * @param[in,out]  num    Grid to update
   * @param[in,out]  sum    Grid to update
   */
  void pAccumulate(const Grid &g, const Grid &weight, Grid &count, Grid &num,
		   Grid &sum);

  /**
   * Initialize the members pNum pSum pCount to have one grid per input grid,
   * initialized to all missing
   * @param[in] grid  Grids to create off of
   */
  void pInit(const MultiGrid &grid);

  /**
   * At point i, return the weight.
   * @param[in] i  Index
   * @param[in] weight Weights grid input
   *
   * @return if no grids have a weight at point i, return the background weight
   *         if ALL grids have a weight at point i, return the input weight at i
   *         if some grids have a weight at point i, use input weight if can,
   *         be used, otherwise use the average weight at i  (pWtAve)
   */

  double pDetermineWeight(const int i, const Grid &weight) const;

  /**
   * Compute the average value 
   * @param[in] minForecasts  Minimum count at a point to compute, 
   * @param[in] count  The grid with how many things in average
   * @param[in] num  The counts grid
   * @param[in,out] sum  On input it is sum, on output it is the averages
   * @param[out] valset  True on exit if at least one point got an average
   *
   * Sum at each point i is set to sum[i] = sum[i]/num[i]
   */
  void pAverage(const int minForecasts, const Grid &count,
		const Grid &num, Grid &sum, bool &valset);
};

#endif
