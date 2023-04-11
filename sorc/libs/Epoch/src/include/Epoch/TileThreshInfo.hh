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
 * @file  TileThreshInfo.hh
 * @brief  
 * @class  TileThreshInfo
 * @brief  
 */

# ifndef    TileThreshInfo_H
# define    TileThreshInfo_H

#include <Epoch/FieldThresh.hh>
#include <vector>
#include <map>

class TileInfo;
class ThresholdsAtGenHms;
class SingleTileThresholds;
class FieldTiledThresh;

//----------------------------------------------------------------
class TileThreshInfo  
{
public:

  /**
   * Empty constructor
   */
  inline TileThreshInfo(void) : 
    _ok(false), _bias(-99.99), _coldstart(true), _motherTile(false),
    _generatingTime(0), _obsValue(-99.99), _fcstValue(-99.99),
    _genHour(-1), _genMin(-1), _genSec(-1), _leadTime(0), _tileIndex(0)
  {
  }

  /**
   * Constructor with member values to use passed in
   *
   * @param[in] fieldthresh Fields/thresholds
   * @param[in] obsTime  time of data used to generate thresholds/bias
   * @param[in] genHour  Forecast generation time
   * @param[in] genMin  Forecast generation time
   * @param[in] genSec  Forecast generation time
   * @param[in] leadTime  Forecast lead time
   * @param[in] tileIndex  Tile index 
   * @param[in] motherTile  True if came from  mother tile
   * @param[in] bias  bias value
   * @param[in] obsValue  Observations data value
   * @param[in] fcstValue  Forecast data value
   */
  inline TileThreshInfo(std::vector<FieldThresh> fieldthresh, time_t obsTime,
			 int genHour, int genMin, int genSec,
			 int leadTime, int tileIndex, bool motherTile,
			 double bias=-999.99, double obsValue=-99.99,
			 double fcstValue=-99.99) :
    _ok(true), _thresh(fieldthresh), _bias(bias), _coldstart(false),
    _motherTile(motherTile), _generatingTime(obsTime), _obsValue(obsValue),
    _fcstValue(fcstValue), _genHour(genHour), _genMin(genMin), _genSec(genSec),
    _leadTime(leadTime), _tileIndex(tileIndex)
  {
  }

  /**
   * Constructor with SingleTileThresholds and member values passed in
   *
   * @param[in] mthresh  Thresholds in a tile
   * @param[in] genHour  Forecast generation time
   * @param[in] genMin  Forecast generation time
   * @param[in] genSec  Forecast generation time
   * @param[in] leadTime  Forecast lead time
   * @param[in] tileIndex  Tile index
   */
  TileThreshInfo(const SingleTileThresholds &mthresh, int genHour, int genMin,
		  int genSec, int leadTime, int tileIndex);

  /**
   * Destructor 
   */
  inline ~TileThreshInfo(void) {}

  /**
   * @return true if the is an empty object
   */
  inline bool empty(void) const
  {
    return _genHour == -1 && _genMin == -1 && _genSec == -1;
  }

  /**
   * @return threshold for i'th field
   * @param[in] index Index
   */
  inline double getIthThresh(int index) const
  {
    return _thresh[index].getThresh();
  }

  /**
   * @return field name for i'th field
   * @param[in] index Index
   */
  inline std::string getIthName(int index) const
  {
    return _thresh[index].getField();
  }

  /*
   * Simple get methods
   */
  inline const int & getLeadTime(void) const {return _leadTime;}
  inline const int & getTileIndex(void) const {return _tileIndex;}
  inline std::vector<FieldThresh> getThresh(void) const { return _thresh; }
  inline bool getColdstart(void) const {return _coldstart;}
  inline bool getMotherTile(void) const {return _motherTile;}
  inline time_t getGeneratingTime(void) const {return _generatingTime;}
  inline double getBias(void) const {return _bias;}
  inline double getObs(void) const {return _obsValue;}
  inline double getFcst(void) const {return _fcstValue;}

  /**
   * Debug print
   * @param[in] info  Information about tiling
   */
  void print(const TileInfo &info) const;

  /**
   * Debug log message to DEBUG
   */
  void logDebug(void) const;

  /**
   * @return string with hour/min/sec
   */
  std::string hmsString(void) const;

  /**
   * @return true if input names match local state in the same order
   *
   * @param[in] names  Field names
   * @param[in] printErrors  true to log an error when something doesn't match
   */
  bool namesMatch(const std::vector<std::string> &names,
		  bool printErrors=true) const;

  /**
   * @return true if gen time hour/minute/second in local state is same
   * as that of input
   * @param[in] m
   */
  bool hmsMatch(const ThresholdsAtGenHms &m) const;

  /**
   * Change local values for tile index and for obsMean to inputs
   * @param[in] tileIndex
   * @param[in] obsMean
   */
  void adjust(int tileIndex, double obsMean);

  /**
   * Set the tile index local value to input
   * @param[in] index
   */
  inline void setTileIndex(int index) {_tileIndex = index;}

  /**
   * return the FieldThresh for a particular field
   *
   * @param[in] fieldName  
   * @param[out] item  Has the actual threshold and the name
   *
   * @return true if fieldName was found locally and item was set
   */
  bool get(const std::string &fieldName, FieldThresh &item) const;

  /**
   * Change local values to inputs
   *
   * @param[in] bias  Bias
   * @param[in] obsTime  The generating time
   * @param[in] obsValue  Observations value
   * @param[in] fcstValue  Forecast value
   * @param[in] thresholds a set of names and actual thresholds for all tiles
   * @param[in] tileIndex a tile Index
   */
  void
  update(double bias, const time_t &obsTime, double obsValue, double fcstValue,
	 const std::vector<FieldTiledThresh> thresholds, int tileIndex);

  /**
   * Change local values to inputs
   *
   * @param[in] bias  Bias
   * @param[in] obsTime  The generating time
   * @param[in] obsValue  Observations value
   * @param[in] fcstValue  Forecast value
   */
  void
  update(double bias, const time_t &obsTime, double obsValue, double fcstValue);


  /**
   * @return  A field name associated with this data.
   *
   * This is the name that gets written to files as a fixed field name.
   * It is a single string with one or more name/threshold pairs.
   *
   * @param[in] precision  Number of digits precision in threshold string
   */
  std::string fieldName(int precision=2) const;

  /**
   * @return  A field name associated with this data.
   *
   * This is retricted to a wanted subset of fields, so not all fields are
   * in the string.
   *
   * This is the name that gets written to files as a fixed field name.
   * It is a single string with one or more name/threshold pairs.
   *
   * @param[in] ignore  Full names of the fields to ignore, can be empty
   * @param[in] precision  Number of digits precision in threshold string
   */
  std::string fieldNameLimited(const std::vector<std::string> &ignore,
			       int precision=2) const;

  /**
   * Add new fields to the state
   *
   * @param[in] nameThresh  a set of names and thresholds
   * 
   * @return true if added, false if already in state
   *
   * Uses each threshold for both of the state threshold values
   */
  bool add(const std::vector<std::pair<std::string,double> > &nameThresh);

  /**
   * @return true if named field is one of the fields in the local state
   *
   * @param[in] name
   */
  bool hasField(const std::string &name) const;

  /**
   * @return true if values are set
   */
  inline bool ok(void) const {return _ok;}
  
protected:
private:  

  bool _ok;                /**< True if object is set */

  std::vector<FieldThresh> _thresh;  /**< Fieldname/thresholds */
  double _bias;            /**< Bias value */
  bool _coldstart;         /**< True if the thresh are set to coldstart */
  bool _motherTile;        /**< True if the thresh are set from 'mother tile' */
  time_t _generatingTime;  /**< Time of data used to generate thresholds*/
  double _obsValue;        /**< Value from observations data */
  double _fcstValue;       /**< Value from forecast data */
  int _genHour;            /**< Forecast gen time */
  int _genMin;             /**< Forecast gen time */
  int _genSec;             /**< Forecast gen time */
  int _leadTime;           /**< Forecast lead time */
  int _tileIndex;          /**< Index into tiles */

};

# endif     // TileThreshInfo_HH
