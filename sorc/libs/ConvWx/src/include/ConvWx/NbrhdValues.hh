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
 * @file NbrhdValues.hh
 * @brief Values used in the clumping algorithm in Nbrhd, and the current
 *        clump size
 * @class NbrhdValues
 * @brief Values used in the clumping algorithm in Nbrhd, and the current
 *        clump size
 *
 * Clump values are index values one per clump, values 1,2,3,..
 * which is pInitialClumpValue=1, pInitialClumpValue+1,
 * pInitialClumpValue+2, ...
 *
 * The current clump being built up by the algorithm has index
 * pCurrentClumpValue
 *
 * Extra values are used to mark points in ways used by the algorithm:
 *  - pBad        Error 
 *  - pNotMarked  Not yet looked at
 *  - pMarked     Looked at and determined to not be part of any clump
 */

# ifndef    NBRHD_VALUES_H
# define    NBRHD_VALUES_H

class Grid;

class NbrhdValues 
{      
public:

  /**
   * Constructor, sets current clump value to pClumpInitialValue,no points (yet)
   */
  NbrhdValues(void);

  /**
   * Destructor
   */
  virtual ~NbrhdValues(void);

  /**
   * @return True if a Grid at a point has a value indicating its already been
   * looked at
   * @param[in] clumpGrid  Grid with clump values in it
   * @param[in] x  Point in Grid
   * @param[in] y  Point in Grid
   */
  bool pointProcessed(const Grid &clumpGrid, const int x, const int y) const;

  /**
   * If not yet set, set clumpGrid[x,y] to pCurrentClumpValue
   * @param[in] x   Point in a Grid
   * @param[in] y   Point in a Grid
   * @param[in,out] clumpGrid  Grid with clump values to modify
   *
   * @return true if clumpGrid was changed
   *
   * @note also increments current area  pCurrentClumpArea
   */
  bool markPoint(const int x, const int y, Grid &clumpGrid);

  /**
   * Set clumpGrid[x,y] to the special value 'not part of a clump' pMarked
   *
   * @param[in] x   Point in a Grid
   * @param[in] y   Point in a Grid
   * @param[in,out] clumpGrid  Grid with clump values to modify
   */
  void markPointNotWanted(const int x, const int y, Grid &clumpGrid) const;

  /**
   * Reset the local state.  The current clump index value pCurrentClumpValue
   * is set to pInitialClumpValue, the area is set to 0, and
   * the Grid is filled with the 'not yet looked at' value  pNotMarked
   *
   * @param[in,out] clumpGrid  Grid to fill with pNotMarked
   */
  void init(Grid &clumpGrid);

  /**
   * Start a new clump .
   *
   * Increment the current clump value pCurrentClumpValue and initialize
   * to zero area
   */
  inline void incrementClumpValue(void)
  {
    pCurrentClumpValue ++;
    pCurrentClumpArea = 0.0;
  }

  /**
   * @return true if a Grid is part of some clump at a particular point
   * @param[in] clumpGrid   Grid with clump values
   * @param[in] x   Point in grid
   * @param[in] y   Point in grid
   */
  bool isInAClump(const Grid &clumpGrid, const int x, const int y) const;

  /**
   * @return true if the input value is a valid clump value, i.e.
   * a number greater or equal to pInitialClumpValue
   *
   * @param[in] v  Value to check
   */
  static bool isInAClump(const double v);

  /**
   * @return the value pBad
   */
  inline double dBad(void) const {return static_cast<double>(pBad);}

  /**
   * @return the value pNotMarked
   */
  inline double dNotMarked(void) const {return static_cast<double>(pNotMarked);}

  /**
   * @return the value pMarked
   */
  inline double dMarked(void) const {return static_cast<double>(pMarked);}

  /**
   * @return he current clump value  pCurrentClumpValue
   */
  inline double dCurrentClumpValue(void) const {return pCurrentClumpValue;}

  /**
   * @return the area of the current clump
   */
  inline double currentClumpArea(void) const {return pCurrentClumpArea;}

  /**
   * @return the number of clumps, based on the internal state
   */
  int numClumps(void) const;

protected:
private:

  double pCurrentClumpValue;      /**< current clump value */
  double pCurrentClumpArea;       /**< the current clump area (# of points) */
  static const double pBad;       /**< bad data value */
  static const double pNotMarked; /**< data value not yet looked at */
  static const double pMarked;    /**< data value looked at and marked as
				   *   'uninteresting'*/
  static const double pInitialClumpValue;    /**< initial clump value */
};

# endif 
