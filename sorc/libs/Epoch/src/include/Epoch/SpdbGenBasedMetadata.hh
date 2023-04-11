/**
 * @file SpdbGenBasedMetadata.hh
 * @brief The threshold information for a gen time, all lead times, all obar thresholds,
 * all tiles
 * @class SpdbGenBasedMetadata
 * @brief The threshold information for a gen time, all lead times, all obar thresholds,
 * all tiles
 *
 */

# ifndef    SpdbGenBasedMetadata_hh
# define    SpdbGenBasedMetadata_hh

#include <Epoch/TileInfo.hh>
#include <Epoch/TileThreshInfoGenBased.hh>
#include <Epoch/MultiObarThreshTileThresholds.hh>
#include <string>
#include <vector>

//----------------------------------------------------------------
class SpdbGenBasedMetadata
{
public:
  /**
   * Empty
   */
  SpdbGenBasedMetadata(void);

  /**
   * Constructor to set initial values
   * Thresholds not known, filled in later.
   * Has a fixed field
   *
   * @param[in] genTime
   * @param[in] ltHours
   * @param[in] threshField
   * @param[in] threshColdstartThresh
   * @param[in] fixedField
   * @param[in] fixedThresh
   * @param[in] tiling
   * @param[in] obarThresh
   */
  SpdbGenBasedMetadata(const time_t &genTime,
  		       const std::vector<double> &ltHours,
  		       const std::string &threshField,
  		       double threshColdstartThresh,
   		       const std::string &fixedField,
  		       double fixedThresh,
  		       const TileInfo &tiling,
  		       const std::vector<double> &obarThresh);
  /**
   * Constructor to set initial values
   * Thresholds not known, filled in later.
   * No fixed field
   *
   * @param[in] genTime
   * @param[in] ltHours
   * @param[in] threshField
   * @param[in] threshColdstartThresh
   * @param[in] tiling
   * @param[in] obarThresh
   */
  SpdbGenBasedMetadata(const time_t &genTime,
		       const std::vector<double> &ltHours,
		       const std::string &threshField,
		       double threshColdstartThresh,
		       const TileInfo &tiling,
		       const std::vector<double> &obarThresh);

  /**
   * Destructor
   */
  virtual ~SpdbGenBasedMetadata(void);
  
  /**
   * @return true if the thresholds have been set
   */
  inline bool thresholdsSet(void) const {return _thresholdsSet;}

  /**
   * @return true if everything except the thresholds has been set
   */
  inline bool fixedValuesSet(void) const {return  _fixedValuesSet;}

  /**
   * @return the gen time
   */
  inline time_t getGenTime(void) const {return _genTime;}


  /**
   * @return the lead seconds  
   */
  inline std::vector<int> getLeadSeconds(void) const {return _leadSeconds;}

  /**
   * @return the number of tiles
   */
  inline int getNumTiles(void) const {return _tiling.numTiles();}

  /**
   * @return the number of obar thresholds
   */
  int getNumObarThresh(void) const;

  /**
   * @return the obar thresholds themselves, or an empty vector
   */
  std::vector<double> getObarThresh(void) const;

  /**
   * @return the actual thresholds for all tiles at a lead time and obar threshold
   * @param[in] leadSeconds 
   * @param[in] obarThreshIndex  Index into the obars
   */
  std::vector<double> getTileThresholds(int leadSeconds, int obarThreshIndex) const;


  /**
   * @return the information for all tiles at a lead time and obar threshold
   * @param[in] leadSeconds
   * @param[in] obarThreshIndex  Index into the obars
   */
  std::vector<TileThreshInfoGenBased> getTileThreshInfo(int leadSeconds,
							int obarThreshIndex) const;

  /**
   * Update local state for one obar threshold using input information
   * @param[in] info
   * @param[in] obarThreshIndex
   */
  void update(const TileThreshInfoGenBased &info, int obarThreshIndex);

  /**
   * Parse an XML string into the local state
   * @param[in] xml
   *
   * @return true if successful
   */
  bool fromXml(const std::string &xml);

  /**
    * @return XML string representing the local state
   */
  std::string toXml(void) const;

  /**
   * Retrieve tiled grid for a particular lead time, and obar threshold
   *
   * @param[in] leadTime Lead seconds
   * @param[in] obarThresh  Obar threshold
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid
   * @param[in] motherOnly If true show only tiles that reverted to the mother file
   *
   * @return true if successful
   */
  bool getTiledGrid(int leadTime, double obarThresh, double centerWeight,
		    double edgeWeight,  int nptSmooth, Grid2d &item, bool motherOnly=false) const;

  /**
   * set threshold  for one tile at one lead time/obar threshold
   * @param[in] ltSec the lead time seconds
   * @param[in] obarThreshIndex   Index into obar
   * @param[in] tileIndex  index into tiles
   * @param[in] value  The threshold to store
   */
  void setThresh(int ltSec, int obarThreshIndex, int tileIndex, double value);
  
  /**
   * set threshold  for one tile at one lead time to the coldstart value, which is
   * a local member
   * @param[in] ltSec the lead time seconds
   * @param[in] obarThreshIndex
   * @param[in] tileIndex
   */
  void setThreshColdstart(int ltSec, int obarThreshIndex, int tileIndex);
  
  /**
   * @return the tile information
   */
  inline TileInfo getTileInfo(void) const {return _tiling;}

  /**
   * @return the field name being thresholded over tiles
   */
  inline std::string getThreshField(void) const {return _threshField;}
  
  /**
   * Debug print, with filtering of what is printed
   *
   * @param[in] t  The time of this data
   * @param[in] verbose  True for more printing
   * @param[in] timeWritten  The time the data was written
   * @param[in] genHours  Print only gen time hours equal to input, unless 
   *                      empty in which case print all gen times
   * @param[in] leadSeconds  Print only lead time seconds equal to input,
   *                         unless empty in which case print all lead times
   * @param[in] tiles  Print only tile indices equal to input,
   *                   unless empty in which case print all tiles
   */
  void printState(const time_t &t, const time_t &timeWritten, 
		  const std::vector<int> &leadSeconds,
		  const std::vector<int> &tiles, bool verbose=false) const;

  /**
   * Debug print
   *
   * @param[in] t  The time of this data
   * @param[in] verbose  True for more printing
   */
  void printState(const time_t &t, bool verbose=false) const;

  /**
   * Debug print
   *
   * @param[in] t  The time of this data
   * @param[in] timeWritten  The time the data was written
   * @param[in] verbose  True for more printing
   */
  void printState(const time_t &t, const time_t &timeWritten,
		  bool verbose=false) const;

  /**
   * Print the tile information  and input time
   * @param[in] t  Time
   */
  void showTilingInfo(const time_t &t) const;
  

 protected:
 private:  

  bool _fixedValuesSet;       /**< True if everything except thresholds is set*/
  time_t _genTime;            /**< Every chunk is for a gen time */
  std::vector<int> _leadSeconds;  /**< All the lead seconds */
  std::string _threshField;       /**< Field name with varying thresh */
  double _threshColdstartThresh;  /**< Coldstart threshold, thresh */
  bool _hasFixedField;            /**< True if a fixed field is set */
  std::string _fixedField;         /**< Field name, fixed thresh, if _hasFixedField */
  double _fixedThresh;             /**< Threshold, fixed field, if _hasFixedField */
  TileInfo _tiling;               /**< tile information */
  bool _thresholdsSet;            /**< True if thresholds are set */

  /**
   * The thresholds for each lead time
   */
  std::vector<MultiObarThreshTileThresholds> _thresholdsAtLead;

  std::string _fixedValuesSetToXml(void) const;
  std::string _genToXml(void) const;
  std::string _thresholdsSetToXml(void) const;
  std::string _threshFieldToXml(void) const;
  std::string _fixedFieldToXml(void) const;
  std::string _leadsToXml(void) const;
  std::string _thresholdsToXml(void) const;
  bool _fixedValuesSetFromXml(const std::string &xml);
  bool _genFromXml(const std::string &xml);
  bool _thresholdsSetFromXml(const std::string &xml);
  bool _threshFieldFromXml(const std::string &xml);
  bool _fixedFieldFromXml(const std::string &xml);
  bool _leadsFromXml(const std::string &xml);
  bool _tilingFromXml(const std::string &xml);
  bool _thresholdsFromXml(const std::string &xml);
  
  int _leadIndex(int leadTime) const;
};

#endif
