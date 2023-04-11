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
 * @file MdvMetadata.hh
 * @brief metadata associated with forecast thresholds
 * @class MdvMetadata
 * @brief metadata associated with forecast thresholds
 *
 * Includes threshold over tiles for zero or more 
 * gen times in the state
 */

# ifndef    MdvMetadata_H
# define    MdvMetadata_H

#include <vector>
#include <string>
#include <Epoch/TiledFcstThreshInfo.hh>
#include <Epoch/FixedFcstThreshInfo.hh>
#include <Epoch/FieldThresh.hh>
#include <Epoch/FieldTiledThresh.hh>
#include <Epoch/GenTimeAndOlder.hh>

class TileThreshInfo;

//----------------------------------------------------------------
class MdvMetadata
{
public:

  /**
   * Constructor, only number of tiles is known
   * @param[in] numTiles  Total number of tiles
   */
  MdvMetadata(int numTiles=1);
		     

  /**
   * Constructor from parsing XML
   *
   * @param[in] xml  String to parse
   * @param[in] needGen  True if expect a gen time in the XML
   * @param[in] needFieldThresh True if expect tiled thresholds
   *                            in the XML
   */
  MdvMetadata(const std::string &xml, bool needGen,
		     bool needFieldThresh);


  /**
   * Destructor
   */
  virtual ~MdvMetadata(void);

  /**
   * @return number of tiles
   */
  inline int numTiles(void) const {return _numTiles;}

  /**
   * Add input information to state..called by EnsFcstGenMultiThresh for write.
   * Tiled thresholds.
   *
   * @param[in] gen  Gen time information of inputs
   * @param[in] lead  Lead seconds being processed
   * @param[in] fieldName  field
   * @param[in] threshold  Threshold for the field, one per tile, in order
   */
  void add(const GenTimeAndOlder &gen, int lead, const std::string &fieldName,
	   const std::vector<double> &threshold);

  /**
   * Add input information to state..called by EnsFcstGenMultiThresh for write.
   * Fixed threshold.
   *
   * @param[in] fieldName  field
   * @param[in] threshold  Threshold for the field, fixed.
   */
  void add(const std::string &fieldName, double threshold);

  /**
   * Compute and set the average thresholds from local state, store average into
   * local state
   */
  void setAverage(void);

  /**
   * @return XML representation of state
   */
  std::string getXml(void) const;

  /**
   * Merge input object into local object , called to add new stuff to state
   *
   * @param[in] genTime  Gen time from SPDB
   * @param[in] info  The metadata to add in
   * @param[in] isFirst  True if this is the initial data, false if not
   *
   * isFirst- THIS IS CONFUSING!! because of the funny way
   * things are done in ClimoState., effectively if isFirst, the merge has
   * already happened.. Sort of.  REFACTOR CANDIDATE.
   */
  void merge(const GenTimeAndOlder &genTime,
	     const MdvMetadata &info,  bool isFirst);

  /**
   * Erase all data for a particular gen time from state
   *
   * @param[in] gt  Gen time
   */
  void erase(const time_t &gt);

  /**
   * If averages have been set, and the input name exists, change argument
   * matchThresh to the average for the input tile.
   * If averages have not been set, do not modify matchThresh
   *
   * @param[in] tileIndex  Index into tiles
   * @param[in] fullname  Name to try and match
   * @param[in,out] matchThresh  Threshold value to modify when possible.
   *                             if modified it gets set to the average
   *                             value for that tile.
   *
   * ANOTHER REFACTOR CANDIDATE.   See ThreshChooser/ParmsThreshChooser.cc
   */
  void changeThreshIfMatch(int tileIndex, const std::string &fullname,
 			   double &matchThresh) const;
  /**
   * Change 'best' value to input values, and if averages are set locally,
   * change the threshold within 'best' to the average. If averages not set
   * locally, leave threshold in best to what it was.
   *
   * @param[in] tileIndex  Tile to modify
   * @param[in] bias     new value
   * @param[in] obsTime  new value
   * @param[in] obsMean  new value
   * @param[in] fcstMean new value
   * @param[in,out] best Object to modify
   *
   * ANOTHER REFACTOR CANDIDATE.  See ThreshChooser/ThreshChooser.cc
   */
  void updateBest(int tileIndex, double bias, const time_t &obsTime,
		  double obsMean, double fcstMean, TileThreshInfo &best) const;

  /**
   * @return those times that are in the state but are too old based
   * on number of days to keep
   *
   * @param[in] genTime  Current new gen time
   * @param[in] daysInClimo  Maximum number of days to keep
   */
  std::vector<time_t> tooOld(const time_t &genTime, int daysInClimo) const;

  /**
   * @return The XML tag
   */
  inline static std::string fcstThreshMetadataTag(void) {return _tag;}

  /**
   * @return true if local state contains a particular gen time
   * @param[in] gentime
   */
  bool contains(const time_t &gentime) const;

  /**
   * Debug print
   * @param[in] verbose  Flag for more 
   */
  void print(bool verbose=false) const;

protected:
private:  

  bool _needGen;     /**< Expect state to have at least one gen time if true */
  bool _needFieldThresh;  /**< Expect state to have at least one tiled
			   * thresholds if true */

  int _numTiles;  /**< Number of tiles, must remain consistent */

  /**
   * The tiled threshold field names in the state, used for averaging
   */
  std::vector<std::string> _fieldNameTiledThresh;

  /**
   * Tile based Forecast thresholds information for any number of
   * fields, at any number of gen/lead times..  
   */
  std::vector<TiledFcstThreshInfo> _tiledThreshInfo;

  /**
   * Fixed forecast thresholds information for any number of fields
   */
  std::vector<FixedFcstThreshInfo> _fixedThreshInfo;

  /**
   * True if _averageThresh is set
   */
  bool _averageTiledThreshInfoIsSet;
  
  /**
   * One tiled threshold set for each _tiledThreshInfo field,
   * which is the average of all the information in _tiledThreshInfo,
   * correct only if _averageTiledThreshIsSet is true
   */
  std::vector<FieldTiledThresh> _averageTiledThreshInfo;

  /**
   * Generation times (and actual gen times for tiled thresholds) in the state,
   * redundant with info in _tiledThreshInfo.  SPDB gen information.
   */
  std::vector<GenTimeAndOlder> _gen;

  /** 
   * The fixed XML tag
   */
  static const std::string _tag;

  void _setFieldsFromInfo(void);
  FieldTiledThresh _getAverageThresh(const std::string &name) const;
  
};

# endif     // MdvMetadata_HH
