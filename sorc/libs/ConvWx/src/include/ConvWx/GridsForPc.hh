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
 * @file GridsForPc.hh
 * @brief  Grid data used in the phase correction algorithm, plus some counters
 * @class GridsForPc
 * @brief  Grid data used in the phase correction algorithm, plus some counters
 *
 * The Grid data:
 *  -  verification
 *  -  forecast
 *  -  smooth verification
 *  -  smooth forecast
 *  -  phase corrected forecast
 */

# ifndef    GRIDS_FOR_PC_HH
# define    GRIDS_FOR_PC_HH

#include <ConvWx/PcInput.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/ParmPcFcst.hh>

class MultiGrid;
class ParmSetUV;
class PcScore;
class UvOutput;

//----------------------------------------------------------------
class GridsForPc 
{
public:

  /**
   * Default constructor
   *
   * @param[in] inputData  Verification and forecast data which is copied in
   * @param[in] algParms  Algorithm parameters used to create the smoothed
   *                      verification and smoothed forecast data.
   */
  GridsForPc(const PcInput &inputData, const ParmSetUV &algParms);
  
  /**
   * Destructor
   */
  virtual ~GridsForPc(void);

  /**
   * Reduce resolution of grids by a factor res
   *
   * @param[in] res  Reduction amount, 1=none, 2=factor of 2, and so on
   *
   * @return true if success
   *
   * @note see GridData::reduce()
   */
  bool reduce(const int res);

  /**
   * Set local state so that no phase correction is present (set the
   * pPhaseCorrectedGrid to the non-phase corrected forecast
   */
  void setNoPc(void);

  /**
   * Set the phase corrected Grid to missing at a point
   * @param[in] x  Index at which to set data missing
   * @param[in] y  Index at which to set data missing
   */
  void setPcToMissing(const int x, const int y);

  /**
   * Phase shift using the input phase correction vectors to convert the
   * forecast into the phase corrected forecast locally
   *
   * @param[in] uv  Phase correction vectors
   */
  void shiftFcst(const UvOutput &uv);

  /**
   * Phase shift using the input phase correction vectors to convert the
   * forecast into the phase corrected forecast locally, then smooth the
   * results
   *
   * @param[in] uv  Phase correction vectors
   * @param[in] gapFill  Gap filling radius
   * @param[in] smooth  Smoothing radii
   * 
   * @note the smoothing filters are applied in the order of the radii
   *       in the smooth vector
   */
  void shiftFcstAndSmooth(const UvOutput &uv, const int gapFill,
			  const std::vector<int> &smooth);

  /**
   * Move data from the forecast grid to the phase corrected grid locally
   * shifting the location
   *
   * @param[in] fromX   X location in the forecast grid to read
   * @param[in] fromY   Y location in the forecast grid to read
   * @param[in] toX  X location in phase corrected grid to write
   * @param[in] toY  Y location in phase corrected grid to write
   */
  void moveFcstForPc(const int fromX, const int fromY, const int toX,
		     const int toY);

  /**
   * Apply an n by n smoothing filter to the phase corrected grid only at
   * points where data is initially missing in the phase corrected grid.
   *
   * @param[in] n  Smoothing filter radius
   */
  void fillGapsInPc(const int n);

  /**
   * Apply an n by n smoothing filter to the phase corrected grid at all
   * points.
   *
   * @param[in] n  Smoothing filter radius
   */
  void smoothPc(const int n);

  /**
   * @return true if the input object has Grid data with the same dimensions
   *         as local Grid data
   *
   * @param[in] g  Object to compare against
   */
  bool dimensionsConsistent(const GridsForPc &g) const;

  /**
   * Check if both verification and forecast data have enough content above
   * a threshold
   *
   * @param[in] minPcnt  Minimum percent of data above the threshold for success
   *
   * @return true if both grids have enough content
   *
   * @note uses a threshold from local parameter object for this data type.
   */
  bool hasEnoughInput(const double minPcnt) const;

  /** 
   * Look in a particular volume to see what percent of the verification data
   * exceeds a threshold, and report on whether it passes this test
   *
   * @param[in] x0  Volume lower left corner
   * @param[in] y0  Volume lower left corner.
   * @param[in] volSize  Values defining the volume (number of grid points x
   *                     and y)
   * @param[in] minPcnt  The percent to check against
   * 
   * @return true if at least minPcnt of data in the volume exceed a threshold
   *
   * @note uses the ParmPcFcst threshold value fractionalAreaDataThresh
   */
  bool fractionalAreaTest(const int x0, const int y0, const int *volSize,
			  const double minPcnt) const;

  /**
   * Set PcScore variables using the inputs and the local state 
   *
   * @param[in] x0  Lower left of a volume box
   * @param[in] y0  Lower left of a volume box
   * @param[in] xoff  X shift for phase correction
   * @param[in] yoff  Y shift for phase correction
   * @param[in] volSize  Array of length 2 with the x,y dimensions of the volume
   * @param[out] score  PcScore variables, filled in on exit
   *
   * @return true if enough data in the box exists to do the calculation,
   *         false if there is not enough data to do the calculation.
   *
   * The PcScore variables are used to compute the phase correction quality
   */
  bool buildScore(const int x0, const int y0, const int xoff, const int yoff,
		  const int *volSize, PcScore &score) const;

  /**
   * Use the internal Grid data as a template to create a Grid
   *
   * @param[in] name  Name to give the returned Grid
   * @param[in] units  Units to give the returned Grid
   * @param[in] initialValue  Value to initialize the returned Grid to at all
   *                          points
   * @return  The Grid
   */
  Grid initializeGrid(const std::string &name, const std::string &units,
		      const double initialValue) const;

  /**
   * @return reference to the smooth verification grid
   */
  inline const Grid &getSmoothVerifGrid(void) const {return pSmoothVerifGrid;}

  /**
   * @return reference to the smooth forecast grid
   */
  inline const Grid &getSmoothFcstGrid(void) const {return pSmoothFcstGrid;}

protected:
private:  
   
  Grid pVerifGrid;      /**< Original verification */
  Grid pFcstGrid;       /**< Original forecast */
  Grid pSmoothVerifGrid; /**< Smoothed verification */
  Grid pSmoothFcstGrid;  /**< Smoothed forecast */
  Grid pPhaseCorrectedGrid;      /**< Phase corrected */
  std::string pName;      /**< Name of this data type */
  ParmPcFcst pParm;         /**< parameters that associate the two grids */
};

# endif
