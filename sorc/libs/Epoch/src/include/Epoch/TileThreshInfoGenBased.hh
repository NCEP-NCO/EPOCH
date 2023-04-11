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
 * @file  TileThreshInfoGenBased.hh
 * @brief  All thresholds information for one tile
 * @class  TileThreshInfoGenBased
 * @brief  All thresholds information for one tile
 */

# ifndef    TileThreshInfoGenBased_H
# define    TileThreshInfoGenBased_H

#include <string>

class SingleTileThresholdsGenBased;

//----------------------------------------------------------------
class TileThreshInfoGenBased  
{
public:

  /**
   * Empty constructor
   */
  inline TileThreshInfoGenBased(void) : 
    _ok(false), _bias(-99.99), _coldstart(true), _motherTile(false),
    _genTime(0), _leadTime(0), _tileIndex(0)
  {
  }

  /**
   * Constructor with member values to use passed in
   *
   * @param[in] thresh The threshold
   * @param[in] genTime  which gen time this is
   * @param[in] leadTime  which Forecast lead time this is
   * @param[in] tileIndex  Tile index 
   * @param[in] motherTile  True if came from  mother tile
   * @param[in] coldstart  True if this is coldstart
   * @param[in] bias  bias value
   */
  inline TileThreshInfoGenBased(double thresh, time_t genTime,
				int leadTime, int tileIndex, bool motherTile,
				bool coldstart, double bias=-999.99)
    :
    _ok(true), _thresh(thresh), _bias(bias),
    _coldstart(coldstart), _motherTile(motherTile), 
    _genTime(genTime), _leadTime(leadTime), _tileIndex(tileIndex)
  {
  }

  /**
   * Constructor with SingleTileThresholds and member values passed in
   *
   * @param[in] mthresh  Threshold information for the tile
   * @param[in] genTime Forecast generation time
   * @param[in] leadTime  Forecast lead time
   * @param[in] tileIndex  Tile index
   */
  TileThreshInfoGenBased(const SingleTileThresholdsGenBased &mthresh,
			 const time_t &genTime, int leadTime, int tileIndex);

  /**
   * Destructor 
   */
  inline ~TileThreshInfoGenBased(void) {}

  /**
   * @return true if the is an empty object
   */
  inline bool empty(void) const
  {
    return _genTime == 0;
  }

  /*
   * Simple get methods
   */
  inline std::string getField(void) const {return _field;}
  inline double getThresh(void) const {return _thresh;}
  inline const int & getLeadTime(void) const {return _leadTime;}
  inline const int & getTileIndex(void) const {return _tileIndex;}
  inline bool getColdstart(void) const {return _coldstart;}
  inline bool getMotherTile(void) const {return _motherTile;}
  inline double getBias(void) const {return _bias;}

  /**
   * Change the tile index local value to input value
   * @param[in] index
   */
  inline void setTileIndex(int index) {_tileIndex = index;}

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * @return true if values are set
   */
  inline bool ok(void) const {return _ok;}
  
protected:
private:  

  bool _ok;                /**< True if object is set */
  std::string _field;      /**< Field name */
  double _thresh;          /**< threshold */
  double _bias;            /**< Bias value */
  bool _coldstart;         /**< True if the thresh are set to coldstart */
  bool _motherTile;        /**< True if the thresh are set from 'mother tile' */
  time_t _genTime;         /**< Forecast Gen Time */
  int _leadTime;           /**< Forecast lead time */
  int _tileIndex;          /**< Index into tiles */

};

# endif     // TileThreshInfo_HH



