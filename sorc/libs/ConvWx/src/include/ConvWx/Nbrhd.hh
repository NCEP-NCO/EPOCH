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
 * @file Nbrhd.hh
 * @brief Clumping of points in a Grid
 * @class Nbrhd
 * @brief Clumping of points in a Grid
 *
 * Nbrhd is a class that performs clumping of a data Grid.  A 'clump' is
 * connected set of points (x,y pixel locations).  Each clump in a Grid
 * has a clump 'index' value'.  The clump information is stored in a Grid
 * with these index values and some special values, plus there are additional
 * vectors (NbrhdValues, NbrhdClumps) put in for efficiency.
 *
 * More and more points become checked as the grid is examined, and
 * clumps are built.
 *
 * Each clump can be assigned a 'scale' or weight value that
 * can be returned, based on clump size (small clumps will get a 
 * small scale value).
 */

# ifndef    NBRHD_H
# define    NBRHD_H

#include <vector>
#include <string>
#include <ConvWx/Grid.hh>
#include <ConvWx/NbrhdClumps.hh>
#include <ConvWx/NbrhdValues.hh>
#include <ConvWx/Merge.hh>
class ConvWxFuzzy;

class Nbrhd 
{      
public:

  /**
   * Constructor for clumps, with clumps built at all data points that are
   * not missing data
   *
   * @param[in] nx  Grid number of x
   * @param[in] ny  Grid number of y
   * @param[in] maxClumpPoints  Maximum number of points in a clump
   *                            (to prevent stack overflow)
   *
   * @note does not build the clumps
   */
  Nbrhd(const int nx, const int ny, const int maxClumpPoints);

  /**
   * Constructor for clumps, with clumps built at all data points above
   * a threshold
   *
   * @param[in] nx  Grid number of x
   * @param[in] ny  Grid number of y
   * @param[in] clumpThreshold  Minimum data value in a clump
   * @param[in] maxClumpPoints  Maximum number of points in a clump
   *                            (to prevent stack overflow)
   *
   * @note does not build the clumps
   */
  Nbrhd(const int nx, const int ny, const double clumpThreshold,
	const int maxClumpPoints);

  /**
   * Destructor
   */
  virtual ~Nbrhd(void);

  /**
   * Debug print to stdout
   */
  void print(void) const;

  /**
   * Write clumps (each with a unique clump index value) to a Grid
   *
   * @param[out] clumpGrid   Grid to write to
   */
  void clumpsToImage(Grid &clumpGrid) const;

  /**
   * Write scale values for each clump to a Grid
   *
   * @param[out] scaleGrid  Grid to write to
   */
  void scaleToImage(Grid &scaleGrid) const;

  /**
   * Build all the disjoint clumps, removing clumps that are too small.
   * Does not set the scaling grid.
   *
   * @param[in] dataGrid  Data Grid to make clumps from
   * @param[in] minClumpNumPoints  Minimum size of clump (# of points) to keep.
   *                               Clumps smaller than this are removed.
   *
   * On exit, a Grid is available with each clump having a unique clump
   * index value.  This Grid can be retrieved by calling clumpsToImage().
   */
  void buildClumps(const Grid &dataGrid, double minClumpNumPoints);

  /**
   * Build all the disjoint clumps, keeping all clumps.
   * Create a scaling Grid in which scaling value inside clumps that are small 
   * is small, and the scaling value everywhere else is 1.0
   *
   * @param[in] dataGrid  Data Grid to make clumps from
   * @param[in] minClumpNumPoints  Minimum size of clump (# of points) to keep
   *                               scaling of 1.0. Clumps smaller than this are
   *                               re-scaled.
   * @param[in] valueToScale  Mapping used to set the scaling, from the minimum
   *                          data value in a clump to a scaling value.
   *
   * On exit, a Grid is available with each clump having a unique clump
   * index value.  This Grid can be retrieved by calling clumpsToImage().
   *
   * On exit, a second Grid is available which has a scaling value at each
   * point. This Grid can be retrieved by calling scaleToImage().
   *
   */
  void buildClumps(const Grid &dataGrid, double minClumpNumPoints, 
		   const ConvWxFuzzy &valueToScale);

protected:
private:

  NbrhdValues pValues; /**< keeps track of clump values and clump size,
			* this is a non-grid representation */
  NbrhdClumps pClump;  /**< objects with the clump points in them */
  Grid pClumpGrid;     /**< Grid representation for clumping, used to build the
			*   clumps. The index values are stored at each point */
  Grid pScaleGrid;     /**< Scaling grid for when scaling values are set. */
  bool pHasThresh;     /**< True if thresholds are used (from constructor)*/
  double pThresh;      /**< clump threshold value, if pHasThresh */
  int pNx;             /**< grid dimensions */
  int pNy;             /**< grid dimensions */
  int pMaxClumpPts;    /**< max points per clump when clumping (this prevents
			*   stack overflow) */

  /**
   * Merge object, created and used to merge clumps
   */
  Merge *pMerge;


  /**
   * @return true if a point adjacent to another point is to be checked for
   *         growing a clump
   *
   * @param[in] ix  Index (x) of point in question
   * @param[in] iy  Index (y) of point in question
   * @param[in] x  Center index (x)
   * @param[in] y  Center index (y)
   *
   * @note The center point has already been checked and growth is happening.
   */   
  inline bool pGrowOk(const int ix, const int iy, const int x,
		      const int y) const
  {
    // if the clump is too big already, it is not ok to grow it more
    if (pValues.currentClumpArea() >= pMaxClumpPts)
    {
      return false;
    }

    // The center point has been dealt with, it is not ok to use it.
    if ( ix == x && iy == y )
    {
      return(false);
    }

    // If point out of range of data, it is not ok to use it.
    if ( ix < 0 || ix >= pNx)
    {
      return(false);
    }
    if ( iy < 0 || iy >= pNy)
    {
      return(false);
    }

    // Finally, if the point hasn't been processed, we can use it!
    return (!pValues.pointProcessed(pClumpGrid, ix, iy));
  }

  /**
   * Make a point part of the current clump.
   *
   * @param[in] img  Grid with data values
   * @param[in] x  Grid index
   * @param[in] y  Grid index
   */
  inline void pMarkPoint(const Grid &img, int x, int y)
  {
    // mark the point with the current clump value into the pValues Grid
    if (pValues.markPoint(x, y, pClumpGrid))
    {
      // make this point part of the current clump (non-image representation)
      pClump.markPoint(pClumpGrid.index(x,y), img.returnValue(x,y));
    }
  }

  /**
   * @return true if a point has been checked already.
   *
   * @param[in] ix grid index 
   * @param[in] iy grid index 
   */
  inline bool pPointChecked(const int ix, const int iy) const
  {
    if (ix < 0 || ix >= pNx || iy < 0 || iy >= pNy)
    {
      return true;
    }
    // check if point in the grid has already been checked based on value there
    return (pValues.pointProcessed(pClumpGrid, ix, iy));
  }

  /**
   * Make the internal state empty and unchecked everywhere.
   */
  void pSetToEmpty(void);

  /**
   * Build clumps, without removing any points, only marking points for removal
   *
   * @param[in] dataGrid  Data Grid to make clumps from
   * @param[in] minClumpNumPoints  Minimum size of clump (# of points) to keep.
   *                               Clumps smaller than this are marked
   *                               for removal.
   */
  void pBuildWithoutRemove(const Grid &dataGrid, double minClumpNumPoints);


  /**
   * Try to build a clump starting at a point
   *
   * @param[in] dataGrid  Grid with data values
   * @param[in] ix  Starting point x
   * @param[in] iy  Starting point y
   *
   * @return true if a clump was built with this initial point
   */
  bool pBuildClump(const Grid &dataGrid, int ix, int iy);

  /**
   * Merge all adjacent clumps, mark all small clumps after merging for
   * removal or rescaling, depending on the configuration.
   *
   * @param[in] minClumpNumPoints  Minimum size of clump (# of points) to keep.
   *                               Clumps smaller than this are marked
   *                               for removal (or re-scale).
   */
  void pMergeAdjacent(const int minClumpNumPoints);

  /**
   * Recursively grow a clump, adding a point, and attempting to add adjacent
   * points by making recursive calls
   *
   * @param[in] dataGrid  Grid with data values
   * @param[in] x  Point to add to the clump
   * @param[in] y  Point to add to the clump
   */
  void pGrowRecursive(const Grid &dataGrid, int x, int y);

  /**
   * Merge multiple clumps into one clump, mark for delete or re-scale if the
   * resulting clump is too small
   * @param[in] clumps  Clump Index values to merge
   * @param[in] minClumpNumPoints  Minimum size of clump (# of pixels) to
   *                               keep after the merging. Clumps smaller
   *                               than this are marked for removal (or
   *                               re-scale)
   */
  void pDoMerge(const std::vector<int> &clumps, const int minClumpNumPoints);

  /**
   * For all points that are adjacent to a clump point but are in a different
   * clump, merge the clumps
   *
   *
   * @param[in] ix  A clump point
   * @param[in] iy  A clump point
   *
   * A static local Merge pointer is involved
   */
  void pMergeAdjacent(const int ix, const int iy);

  /**
   * @return true if a data value at a particular point can be used for
   *         clumping
   * @param[in] dataGrid  Grid with data values
   * @param[in] ipt  One dimensional Grid index 
   *
   * To return true, the value must be non missing, and must be at least
   * pThresh (when pHasThresh is true)
   */
  bool pStatus1(const Grid &dataGrid, const int ipt);
};

# endif 
