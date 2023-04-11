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
 * @file  TileObarInfo.hh
 * @brief  The information related to obar in one tile
 * @class  TileObarInfo
 * @brief  The information related to obar in one tile
 *
 * THis might have overkill carryovers from cut/pastes
 */

# ifndef    TileObarInfo_H
# define    TileObarInfo_H

#include <string>

class TileInfo;

//----------------------------------------------------------------
class TileObarInfo  
{
public:

  /**
   * Empty constructor, _ok set false
   */
  inline TileObarInfo(void) : 
    _ok(false), _obar(-1),  _coldstart(true), _motherTile(false),
    _tileIndex(0)
  {
  }

  /**
   * Constructor with member values to use passed in
   *
   * @param[in] obar  The obar value
   * @param[in] tileIndex  Tile index 
   * @param[in] motherTile  True if came from  mother tile
   * @param[in] coldstart True for coldstart
   */
  inline TileObarInfo(double obar, int tileIndex, bool motherTile,
		      bool coldstart) :
    _ok(true), _obar(obar), _coldstart(coldstart), _motherTile(motherTile), 
    _tileIndex(tileIndex)
  {
  }


  /**
   * XML contructor - Parse XML into local state
   *
   * @param[in] xml
   */
  TileObarInfo(const std::string &xml);

  /**
   * Destructor 
   */
  inline ~TileObarInfo(void) {}

  /*
   * Simple get methods
   */
  inline int getTileIndex(void) const {return _tileIndex;}
  inline bool getColdstart(void) const {return _coldstart;}
  inline bool getMotherTile(void) const {return _motherTile;}
  inline double getObar(void) const {return _obar;}

  /**
   * Change the tile index in the local state to input
   * @param[in] index
   */
  inline void setTileIndex(int index) {_tileIndex = index;}

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
   * Value of XML tag for this object
   */ 
  static const std::string _tag;

  
  /**
   * @return true if values are set
   */
  inline bool ok(void) const {return _ok;}
  
protected:
private:  

  bool _ok;            /**< True if object is set */
  double _obar;        /**< The Obar value for this tile */
  bool _coldstart;     /**< True if the obar for this tile is coldstart */
  bool _motherTile;    /**< True if the obar for this tile is from 'mothertile' */
  int _tileIndex;      /**< Which tile */
};

# endif     // TileThreshInfo_HH
