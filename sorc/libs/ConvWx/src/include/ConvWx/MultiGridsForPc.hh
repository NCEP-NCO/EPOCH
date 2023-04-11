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
 * @file MultiGridsForPc.hh
 * @brief zero or more GridsForPc objects
 * @class MultiGridsForPc
 * @brief zero or more GridsForPc objects
 *
 * This class is designed to allow phase correction using more than
 * one input data type, for example phase correcting using both VIL
 * and EchoTops inputs.  The phase correction algorithm supports any
 * number of input forecasts to be used together to produced a phase
 * correction grid.  This class stores each such input data type as
 * a GridsForPc object locally.
 */

# ifndef    MULTI_GRIDS_FOR_PC_HH
# define    MULTI_GRIDS_FOR_PC_HH

#include <vector>
#include <ConvWx/GridsForPc.hh>
class PcScore;
class Grid;
class MultiGrid;

//----------------------------------------------------------------
class MultiGridsForPc 
{
public:

  /**
   * Empty constructor, zero GridsForPc objects
   */
  MultiGridsForPc(void);

  /**
   * Destructor
   */
  virtual ~MultiGridsForPc(void);

  /**
   *@return The number of GridsForPc objects
   */
  inline int num(void) const {return static_cast<int>(pGrids.size());}

  /**
   * @return  Reference to i'th GridsForPc object
   * @param[in] i  Index
   */
  inline GridsForPc &ithGrids(const int i) { return pGrids[i];}

  /**
   * @return  Const reference to i'th GridsForPc object
   * @param[in] i  Index
   */
  inline const GridsForPc &ithConstGrids(const int i) const { return pGrids[i];}

  /** 
   * Append A GridsForPc object to the local state
   * @param[in] g   GridsForPc object to append
   * @return true if successful. It will fail if the local and input Grid
   *         dimensions are not consistent.
   */
  bool append(const GridsForPc &g);

  /**
   * @return true if each GridsForPc has enough verification and forecast data
   *         above the parameterized threshold for that data type 
   * @param[in] minPercentAboveThreshold  The minimum percent of data above
   *                                      the threshold for 'enough'
   */
  bool hasEnoughInput(const double minPercentAboveThreshold) const;

  /**
   * Set phase corrected Grid data to the forecast Grid (i.e. no phase 
   * correction) for all GridsForPc objects
   */
  void setNoPc(void);

  /**
   * Reduce the resolution of all Grid data in the local state.
   *
   * @param[in] res  Reduction, 1 = no resolution reduction, 2 = 
   *                 resolution reduction by a factor of 2, and so on.
   *
   * @return true if successful
   *
   * @note see Grid::reduce()
   */
  bool reduce(const int res);

  /**
   * Test all GridsForPc objects using the fractional area threshold test.
   * If any one of the objects fails the test, return false.
   * @return true if all GridsForPc pass the test
   *
   * @param[in] x0  Lower left corner of a volume
   * @param[in] y0  Lower left corner of a volume
   * @param[in] volSize  Array of length 2, where volSize[0] is the x number
   *                     of points in the volume, and volSize[1] is the y
   *                     number of volume points
   * @param[in] minPcnt Minimum percent 
   *
   * A GridsForPc object passes the test if at least minPcnt of the verification
   * data in the volume exceeds the threshold parameter for that data type.
   */
  bool fractionalAreaTest(const int x0, const int y0, const int *volSize, 
			  const double minPcnt) const;

  /**
   * Construct a vector of PcScore objects, one for each GridsForPc object
   *
   * @param[in] x0  Lower left corner of a volume
   * @param[in] y0  Lower left corner of a volume
   * @param[in] xoff  Offset (number of grid points)
   * @param[in] yoff  Offset (number of grid points)
   * @param[in] volSize  X and y number of points in the volume
   * @param[out] pcscore  The returned PcScore objects
   *
   * @return true if at least one score was built.
   *
   * @note see GridsForPc::buildScore() to see how a PcScore is computed
   * for each GridsForPc
   */
  bool buildScores(const int x0, const int y0, const int xoff, const int yoff,
		   const int *volSize, std::vector<PcScore> &pcscore) const;

  /**
   * Use internal grids as a template to initialize and return a Grid
   *
   * @param[in] name  Name to give the returned grid
   * @param[in] units  Units to give the returned grid
   * @param[in] value   Grid is initialized to this value at all points
   *
   * @return The grid.
   *
   * @note  If there is a problem the returned Grid will have nx=ny=0
   */
  Grid initializeGrid(const std::string &name, const std::string &units,
		      const double value) const;

protected:
private:  

  /**
   * The GridsForPc objects
   */
  std::vector<GridsForPc> pGrids;
};

# endif 
