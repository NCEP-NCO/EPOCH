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
 * @file ParmTiling.hh
 * @brief Parameters for tiling
 * @class ParmTiling
 * @brief Parameters for tiling
 *
 *
 */

# ifndef    PARM_TILING_HH
# define    PARM_TILING_HH

//---------------------------------------------------------------
class ParmTiling
{
public:

  /**
   * default constructor. values are not set
   */
  ParmTiling(void);

  /**
   * Constructor with all values passed in, except Dxkm and DyKm
   * which are set through another method (storeGridDim()).
   * 
   * @param[in] doTiling  True to do tiling, false to not
   * @param[in] npt  x and y number of points per tile
   * @param[in] overlapNpt x and y number of overlap points per tile
   * @param[in] motherTileIsSubset  True if mother tile is a subset
   * @param[in] motherTileLowerLeftIndex  Lower left x,y, when subset
   * @param[in] motherTileUpperRightIndex  Upper right x,y when subset
   */
  ParmTiling(bool doTiling, int *npt, int *overlapNpt, bool motherTileIsSubset,
	     int *motherTileLowerLeftIndex, int *motherTileUpperRightIndex);


  /**
   * Destructor
   */
  virtual ~ParmTiling(void);

  /**
   * Debug print
   */
  void print(void) const;


  bool pTiling;         /**< True to do tiling */
  int pTileNptX;        /**< Number of points per tile, x */
  int pTileNptY;        /**< Number of points per tile, y */
  int pTileNptOverlapX; /**< Number of overlap points per tile, x */
  int pTileNptOverlapY; /**< Number of overlap points per tile, Y */
  bool pMotherIsSubset; /**< True if mother tile is a subset of the grid */
  int pMotherLowerLeftX; /**< Lower left grid index, x, when pMotherIsSubset */
  int pMotherLowerLeftY; /**< Lower left grid index, y, when pMotherIsSubset */
  int pMotherUpperRightX; /**< UpperRight grid index, x, when pMotherIsSubset */
  int pMotherUpperRightY; /**< UpperRight grid index, y, when pMotherIsSubset */


protected:
private:  
};

# endif
