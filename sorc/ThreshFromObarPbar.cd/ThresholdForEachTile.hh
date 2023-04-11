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
 * @file ThresholdForEachTile.hh
 * @brief Information about thresholds for each tile
 * @class ThresholdForEachTile
 * @brief Information about thresholds for each tile
 */

# ifndef    THRESHOLD_FOR_EACH_TILE_H
# define    THRESHOLD_FOR_EACH_TILE_H

#include <Epoch/TileThreshInfoGenBased.hh>
#include <vector>

class SpdbGenBasedThreshHandler;

//----------------------------------------------------------------
class ThresholdForEachTile
{
public:

  /**
   * Default constructor - each tiles information set to empty
   * @param[in] numTiles
   */
  ThresholdForEachTile(int numTiles);
  
  /**
   *  Destructor
   */
  virtual ~ThresholdForEachTile(void);

  /**
   * Set each tiles information to empty
   */
  void clear(void);

  /**
   * Copy information from belowTile to tileIndex
   *
   * @param[in] tileIndex  Index to tile 
   * @param[in] belowTile  Index to tile right below this one
   * @param[out] data  Returned information, which is the tile below with
   *                   the tile index changed to the index above
   * @return true for success
   */
  bool useTileBelow(int tileIndex, int belowTile, TileThreshInfoGenBased &data);

  /**
   * @return reference to an individual tiles info.
   * @param[in] i  Index
   */
  inline TileThreshInfoGenBased& operator[](size_t i)
  {
    return _thresholdInfo[i];
  }
  /**
   * @return reference to an individual tiles info.
   * @param[in] i  Index
   */
  inline const TileThreshInfoGenBased& operator[](size_t i) const
  {
    return _thresholdInfo[i];
  }

  /**
   * @return number of tiles with info
   */
  inline size_t size(void) const
  {
    return _thresholdInfo.size();
  }

  /**
   * Debug print
   */
  void print(void) const;
  
  /**
   * Update the database using local state, for a partiular obar threshold
   *
   * @param[in] spdb  Database object
   * @param[in] obarThreshIndex
   */
  void update(SpdbGenBasedThreshHandler &spdb, int obarThreshIndex) const;
  
protected:
private:  

  /**
   * All the information for each tile, which is filled by the algorithm
   *
   * It is stored like this so you can in fact use the tile below when you 
   * go too far north.
   */
  std::vector<TileThreshInfoGenBased> _thresholdInfo;

};

# endif     // ThreshChooser_HH
