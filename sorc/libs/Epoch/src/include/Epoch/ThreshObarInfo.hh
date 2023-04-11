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
 * @file  ThreshObarInfo.hh
 * @brief  The information related to obar at a particular obs data threshold
 * @class  ThreshObarInfo
 * @brief  The information related to obar at a particular obs data threshold
 *
 * THis might have overkill carryovers from cut/pastes
 */

# ifndef    ThreshObarInfo_H
# define    ThreshObarInfo_H

#include <Epoch/TileObarInfo.hh>
#include <string>
#include <vector>

class TileInfo;
class Grid2d;

//----------------------------------------------------------------
class ThreshObarInfo  
{
public:

  /**
   * Empty constructor, _ok set false
   */
  inline ThreshObarInfo(void) : 
    _ok(false)
  {
  }

  /**
   * Constructor with member values to use passed in.
   * Sets the individual tile obar information for all tiles to 'not set'
   *
   * @param[in] thresh  The data threshold
   * @param[in] numTiles  Number of tiles
   */
  inline ThreshObarInfo(double thresh, int numTiles) :
    _ok(true), _thresh(thresh)
  {
    for (int i=0; i<numTiles; ++i)
    {
      _info.push_back(TileObarInfo());
    }
  }


  /**
   * XML contructor - Parse XML into local state
   *
   * @param[in] xml
   */
  ThreshObarInfo(const std::string &xml);

  /**
   * Destructor 
   */
  inline ~ThreshObarInfo(void) {}

  /**
   * @return XML represntation of local state
   * @param[in] indent how much to indent the XML
   */
  std::string toXml(int indent) const;

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * Debug print version 2
   * @param[in] tiling  Tile information
   * @param[in] verbose  True to print more
   */
  void print(const TileInfo &tiling, bool verbose=false) const;

  /**
   * Debug print version 3, for a subset of tiles
   * @param[in] tiles The specific tiles, or empty to do all tiles
   * @param[in] info  Tile information
   * @param[in] verbose  True to print more
   */
  void print(const std::vector<int> &tiles, const TileInfo &info,
	     bool verbose=false) const;
  /**
   * Set the tile information locally using input info.  The
   * info for the tile index specified by info is replaced locally with info
   *
   * @param[in] info  Information used , including which tile it is
   */
  void update(const TileObarInfo &info);

  /**
   * Set the tile information locally using input info, checking input tile 
   * index for correctness. The info for the tile index locally is replaced
   * with input.
   *
   * @param[in] tileIndex  Expected tile index in info
   * @param[in] info  Information used , including which tile it is
   */
  void update(int tileIndex, const TileObarInfo &info);

  /**
   * Value of XML tag
   */ 
  static const std::string _tag;

  /**
   * @return true if values are set
   */
  inline bool ok(void) const {return _ok;}
  
  /**
   * @return information about one tile from local state
   * @param[in] tileIndex  Which tile
   */
  inline TileObarInfo getObarInfo(int tileIndex) const
  {
    if (_ok)
    {
      return _info[tileIndex];
    }
    else
    {
      return TileObarInfo();
    }
  }
  
  /**
   * Retrieve tiled grid for a particular inputs
   *
   * @param[in] field  Name of field that has thresholds
   * @param[in] obarThresh  The obar threshold itself
   * @param[in] tiling Specification for tiling
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid with thresholds
   *
   * @return true if successful
   */
  bool getTiledGrid(const std::string &field, double obarThresh,
		    const TileInfo &tiling,
		    double centerWeight,  double edgeWeight, int nptSmooth,
		    Grid2d &item) const;

protected:
private:  

  bool _ok;                         /**< True if object is set */
  double _thresh;                   /**< The Obar threshold value */
  std::vector<TileObarInfo> _info;  /**< Tile based obar values */
};

# endif
