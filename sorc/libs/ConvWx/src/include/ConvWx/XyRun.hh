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
 * @file XyRun.hh
 * @brief Runs of grid values above a threshold in x or y,used for convergence
 * @class XyRun
 * @brief Runs of grid values above a threshold in x or y,used for convergence
 *
 * The algorithm finds 'runs' of data above a threshold, then uses the
 * length of the run to compute an expanded size using a fuzzy function.
 * The expanded size is the width of a window of data points in one dimension.
 * For all data points for which the window touches the original 'run', the
 * center point output data value is replaced with the average of the data
 * values within the window.
 */

# ifndef    XY_RUN_HH
# define    XY_RUN_HH

#include <ConvWx/ConvWxFuzzy.hh>
class Grid;

//----------------------------------------------------------------*/
class XyRun
{
public:

  /**
   * Default constructor
   *
   * @param[in] isX  True for x run, false for y run
   * @param[in] thresh  The threshold to use
   * @param[in] nptToExpansion  Fuzzy function from number of points in the run
   *            to an expansion
   * @param[in] nx  Grid dimension 
   * @param[in] ny  Grid dimension 
   */
  XyRun(const bool isX, const double thresh, const ConvWxFuzzy &nptToExpansion,
	const int nx, const int ny);

  /**
   *  destructor
   */
  virtual ~XyRun(void);

  /**
   * Process the data at x, y.
   *
   * @param[in] x  Grid point x dimension
   * @param[in] y  Grid point y dimension
   * @param[in] v  The data value at x,y
   * @param[in] duv  The dU (x runs) or dV (y runs) partial derivative grid
   * @param[out] g  The grid in which to write the averaged results
   *
   * If the data at x,y is above threshold, a run is started or added to.
   * If the data at x,y is below threshold, a run is ended or there is no action
   * (if there is no run to end)
   *
   * @note It is assumed if it an x run, this method is called with x
   * incrementing faster than y, and vice versa
   */
  void process(const int x, const int y, const double v, const Grid &duv,
	       Grid &g);

  /**
   * End the current run at the point x,y if there is a run, otherwise
   * there is no action.
   *
   * @param[in] x  Grid point x dimension
   * @param[in] y  Grid point y dimension
   * @param[in] duv  The dU (x runs) or dV (y runs) partial derivative grid
   * @param[out] g  The grid in which to write the averaged results
   *
   * The algorithm that does the averaging within the run is done here.
   */
  void endRun(const int x, const int y, const Grid &duv, Grid &g);

protected:
private:  

  bool pIsX;               /**< true for x, false for y run dimension */
  double pConvThresh;      /**< threshold for building runs */
  ConvWxFuzzy pNptToExpansion;  /**< function used in the algorithm */
  int pNx;                 /**< grid dimension */
  int pNy;                 /**< grid dimension */

  bool pInside;            /**< True or false */
  int pMin;                /**< minimum index of run */
  int pMax;                /**< maximum index of run */

  double pNum;  /**< workspace for accumulating run info */
  double pSum;  /**< workspace for accumulating run info */

  /**
   * Extend the run to the point x,y , or start a run at x,y
   *
   * @param[in] x  Grid point x dimension
   * @param[in] y  Grid point y dimension
   */
  void pExtendRun(const int x, const int y);

  /**
   * Process an existing run.
   *
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] indMax   If pX=true, it is nx otherwise ny
   * @param[in] data  The grid with data to evaluate
   * @param[out] g  The grid with filtered results (runs are replaced
   *                with average in the run)
   *
   * This method sets up the expansion factor, and does the averaging into g.
   */
  void pProcessRun(const int otherInd, const int indMax, const Grid &data,
		   Grid &g);

  /**
   * One step of run building. This is a moving window, the value at the
   * tail end is subtracted from the average, the value at the high end
   * is added into the average.
   *
   * @param[in] iptc  Window center index, if pX=true an x index, otherwise y
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] indMax  If pX=true it is nx, otherwise ny
   * @param[in] data  Data with runs in it
   * @param[out] g  Filtered data
   * @param[in,out] ipt0  Window bottom index, if pX=true an x index,otherwise y
   * @param[in,out] ipt1  Window top index, if pX=true an x index, otherwise y
   *
   * Write out average of window to output g, then adjust average and ipt0,ipt1
   */
  void pRunStep(const int iptc, const int otherInd, const int indMax,
		const Grid &data, Grid &g, int &ipt0, int &ipt1);

  /**
   * Debug show results of a run
   *
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] data  Data grid
   * @param[in] g  Filtered data grid
   */
  void pProcessRunDebugOutput(const int otherInd, const Grid &data,
			      Grid &g) const;

  /**
   * Increment average member pNum,pSum values using data at input index
   * @param[in] ind   If pX=true it is the x index, otherwise y index
   * @param[in] indMax  If pX=true it is nx, otherwise ny
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] data  The input data
   */
  void incSum(const int ind, const int indMax, const int otherInd,
	      const Grid &data);

  /**
   * Decrement average member pNum,pSum values using data at input index
   * @param[in] ind   If pX=true it is the x index, otherwise y index
   * @param[in] indMax  If pX=true it is nx, otherwise ny
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] data  The input data
   */
  void decSum(const int ind, const int indMax, const int otherInd,
	      const Grid &data);

  /**
   * @return true if data has a value at input indices
   * @param[in] ind   If pX=true it is the x index, otherwise y index
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] data  The input data
   * @param[out] v  The data value at ind,otherInd
   */
  bool getValue(const int ind, const int otherInd, const Grid &data,
		double &v) const;

  /**
   * Set data into grid at an index
   *
   * @param[in] ind  If pX=true it is the x index, otherwise y index
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[in] v  The data value to set 
   * @param[out] g  The grid to write to
   */
  void setValue(const int ind, const int otherInd, const double v,
		Grid &g) const;

  /**
   * Set missing data value at an index
   *
   * @param[in] ind  If pX=true it is the x index, otherwise y index
   * @param[in] otherInd  If pX=true it is the y index, otherwise x index
   * @param[out] g  The grid to write to
   */
  void setToMissing(const int ind, const int otherInd, Grid &g) const;
};


#endif
