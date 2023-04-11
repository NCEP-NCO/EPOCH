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
 * @file ObarForEachTile.hh
 * @brief Obar information for each tile
 * @class ObarForEachTile
 * @brief Obar information for each tile
 */

#ifndef ObarForEachTile_hh
#define ObarForEachTile_hh

#include <Epoch/TileObarInfo.hh>
#include <vector>

class SpdbObsMetadata;

class ObarForEachTile
{
public:
  
  /**
   * Constructor, creates numTiles empty obar info objects
   *
   * @param[in] numTiles
   */
  ObarForEachTile(int numTiles);

  /**
   * Destructor
   */
  ~ObarForEachTile(void);

  /**
   * Clear so each tile has an empty obar
   */
  void clear(void);

  /**
   * Try to set the obar info for a tile to that from the tile below
   *
   * @param[in] tileIndex  The tile index
   * @param[in] belowTile Index to the tile below
   * @param[out] data  The info from the below tile with index set to
   *                   tileIndex
   *
   * @return true for success
   */
  bool useTileBelow(int tileIndex, int belowTile, TileObarInfo  &data);

  /**
   * Update the metadata object using local state
   *
   * @param[in] threshIndex  Index to an obar threshold (that goes with local state)
   * @param[in,out] spdb  Object to update.
   */
  void update(int threshIndex, SpdbObsMetadata &spdb) const;

  /**
   * @return reference to i'th tiles obar information
   * @param[in] i  Tile index
   */
  inline TileObarInfo & operator[](size_t i) {return _obarInfo[i];}

  /**
   * @return reference to i'th tiles obar information
   * @param[in] i  Tile index
   */
  inline const TileObarInfo & operator[](size_t i) const {return _obarInfo[i];}

  /**
   * @return number of tiled obar information objects locally
   */
  inline size_t size(void) const {return _obarInfo.size();}

protected:
private:

  /**
   * Information about obar, one per tile, in order
   */
  std::vector<TileObarInfo> _obarInfo;
};


#endif

