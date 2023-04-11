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
 * @file ObarForEachThresh.hh
 * @brief Obar information, for each threshold
 * @class ObarForEachThresh
 * @brief Obar information, for each threshold
 */

#ifndef ObarForEachThresh_hh
#define ObarForEachThresh_hh

#include "ObarForEachTile.hh"
#include <vector>

class ObarForEachThresh
{
public:
  
  /**
   * Constructor, creates numTiles empty obar info objects numThresh thresholds
   *
   * Nothing is known about the threshold values, or obar within each tile
   *
   * @param[in] numThresh
   * @param[in] numTiles
   */
  ObarForEachThresh(int numThresh, int numTiles);

  /**
   * Destructor
   */
  ~ObarForEachThresh(void);

// #ifdef NOTYET
//   /**
//    * Clear so each tile has an empty obar
//    */
//   void clear(void);

//   /**
//    * Try to set the obar info for a tile to that from the tile below
//    * @param[in] tileIndex  The tile index
//    * @param[in] belowTile Index to the tile below
//    * @param[out] data  The info from the below tile with index set to
//    *                   tileIndex
//    *
//    * @return true for success
//    */
//   bool useTileBelow(int tileIndex, int belowTile, TileObarInfo  &data);

// #endif

  /**
   * Update the metadata object using local state, at each obar threshold
   *
   * @param[in,out] spdb  Object that gets updated.
   */
  void update(SpdbObsMetadata &spdb) const;


  /**
   * @return reference to i'th indexed obar set for all tiles
   * @param[in] i  Threshold index
   */
  inline ObarForEachTile & operator[](size_t i) {return _obarInfo[i];}

  /**
   * @return reference to i'th indexed obar set for all tiles
   * @param[in] i
   */
  inline const ObarForEachTile & operator[](size_t i) const {return _obarInfo[i];}

  /**
   * @return number of objects with obar set for all tiles (number of thresholded obars)
   */
  inline size_t size(void) const {return _obarInfo.size();}
  
protected:
private:

  /**
   * Information about obar, one per obar threshold, in order
   */
  std::vector<ObarForEachTile> _obarInfo;
};


#endif

