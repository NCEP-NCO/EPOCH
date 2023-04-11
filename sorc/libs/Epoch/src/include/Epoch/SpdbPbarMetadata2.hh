/**
 * @file SpdbPbarMetadata2.hh
 * @brief The pbar information for a gen time, all lead times, all thresholds,
 * all tiles, for the case of a 1 dimensional and 2 dimensional pbar set
 * @class SpdbPbarMetadata2
 * @brief The pbar information for a gen time, all lead times, all thresholds,
 * all tiles, for the case of a 1 dimensional and 2 dimensional pbar set
 *
 */

# ifndef    SpdbPbarMetadata2_hh
# define    SpdbPbarMetadata2_hh

#include <Epoch/TileInfo.hh>
#include <Epoch/PbarAtLead.hh>
#include <Epoch/PbarAtLead2.hh>
#include <string>
#include <vector>

//----------------------------------------------------------------
class SpdbPbarMetadata2
{
public:
  /**
    * Empty
    */
   SpdbPbarMetadata2(void);

  /**
   * Constructor to set initial values
   *
   * @param[in] genTime
   * @param[in] ltHours
   * @param[in] tiling
   * @param[in] threshField1
   * @param[in] threshField2
   * @param[in] thresholds1
   * @param[in] thresholds2
   * @param[in] hasFixedField1
   * @param[in] fixedField1
   * @param[in] double fixedThresh1
   * @param[in] hasFixedField2
   * @param[in] fixedField2
   * @param[in] fixedThresh2
   *
   * @note
   * field 1 is for 1 dim and the 1st field in the 2 dim
   * field 2 is for the 2nd field in the 2 dim
   */
  SpdbPbarMetadata2(const time_t &genTime,
		    const std::vector<double> &ltHours,
		    const TileInfo &tiling,
		    const std::string &threshField1,
		    const std::string &threshField2,
		    const std::vector<double> &thresholds1,
		    const std::vector<double> &thresholds2,
		    bool hasFixedField1,
		    const std::string &fixedField1,
		    double fixedThresh1,
		    bool hasFixedField2,
		    const std::string &fixedField2,
		    double fixedThresh2);

  /**
   * Destructor
   */
  virtual ~SpdbPbarMetadata2(void);
  
  /**
   * @return the gen time
   */
  inline time_t getGenTime(void) const {return _genTime;}

  /**
   * @return the lead seconds  
   */
  inline std::vector<int> getLeadSeconds(void) const {return _leadSeconds;}

  /**
   * @return field name 1
   */
  inline std::string getField1(void) const {return _threshField1;}

  /**
   * @return field name 2
   */
  inline std::string getField2(void) const {return _threshField2;}
  
  /**
   * @return the number of tiles
   */
  inline int getNumTiles(void) const {return _tiling.numTiles();}

  /**
   * @return the number of thresholds
   * @param[in] which  Should be 1 or 2
   */
  int getNumThresh(int which) const;

  /**
   * @return the thresholds themselves
   * @param[in] which Should be 1 or 2
   */
  std::vector<double> getThresh(int which) const;

  /**
   * @return info about fixed field1
   * @param[out] isFixed
   * @param[out] fixedFieldName
   * @param[out] fixedThresh
   */
  void getFixed1(bool &isFixed, std::string &fixedFieldName, double &fixedThresh) const;

  /**
   * @return info about fixed field2
   * @param[out] isFixed
   * @param[out] fixedFieldName
   * @param[out] fixedThresh
   */
  void getFixed2(bool &isFixed, std::string &fixedFieldName, double &fixedThresh) const;

  /**
   * @return the 1 dimensional pbars for all tiles at a lead time and threshold
   *         for all tiles
   * @param[in] leadSeconds 
   * @param[in] threshIndex  Index into the thresholds
   */
  std::vector<double> getTilePbar1(int leadSeconds, int threshIndex) const;

  /**
   * @return the 1 dimensional pbars for a particular tile at a lead time, 
   *         all thresholds
   * @param[in] leadSeconds 
   * @param[in] tileIndex
   */
  std::vector<double> getPbar1(int leadSeconds, int tileIndex) const;

  /**
   * @return the 2nd dimension pbars for a particular tile at a lead time, 
   *         all 2nd thresholds, for a particular 1st index
   * @param[in] leadSeconds 
   * @param[in] tileIndex
   * @param[in] index1
   */
  std::vector<double> getPbar2(int leadSeconds, int tileIndex,
			       int index1) const;

  /**
   * @return the 2 dimensional pbars for all tiles at a lead time and threshold
   *         for all tiles at a particular pair of thresh indices
   * @param[in] leadSeconds 
   * @param[in] threshIndex1  Index into the thresholds
   * @param[in] threshIndex2  Index into the thresholds
   */
  std::vector<double> getTilePbar2(int leadSeconds, int threshIndex1,
				   int threshIndex2) const;

  /**
   * Copy the pbars from one tile to another tile, at all thresholds
   * one and two dimensional
   *
   * @param[in] leadSeconds 
   * @param[in] fromTile  Tile index to copy from
   * @param[in] toTile  Tile index to copy into
   */
  void copyPbarForAllThresh(int leadSeconds, int fromTile, int toTile);

  /**
   * Set 1 or 2d pbar values for a tile (all thresholds)
   * @param[in] leadSeconds 
   * @param[in] tileIndex  Tile index tomodify
   * @param[in] pbar The pbar values for all thresholds
   * @param[in] which  1 or 2
   */
  void setPbarForAllThresh(int leadSeconds, int tileIndex,
			    const std::vector<double> &pbar,
			    int which);

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
   * Retrieve tiled grid for a particular lead time and threshold, field1
   *
   * @param[in] leadTime Lead seconds
   * @param[in] thresh
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid
   *
   * @return true if successful
   */
  bool getTiledGrid1(int leadTime, double thresh, double centerWeight,
		     double edgeWeight,  int nptSmooth, Grid2d &item) const;

  /**
   * Retrieve tiled grid for a particular lead time and thresholds, 2d
   *
   * @param[in] leadTime Lead seconds
   * @param[in] thresh1 threshold for field 1
   * @param[in] thresh2 threshold for field 2
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid
   *
   * @return true if successful
   */
  bool getTiledGrid2(int leadTime, double thresh1, double thresh2, double centerWeight,
		     double edgeWeight,  int nptSmooth, Grid2d &item) const;

  /**
   * Retrieve tiled grid for a particular lead time and thresholds, 2d,
   * different field naming than the other get method
   *
   * @param[in] leadTime Lead seconds
   * @param[in] thresh1 threshold for field 1
   * @param[in] thresh2 threshold for field 2
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid
   *
   * @return true if successful
   */
  bool getTiledGrid2_2(int leadTime, double thresh1, double thresh2, double centerWeight,
		     double edgeWeight,  int nptSmooth, Grid2d &item) const;

  /**
   * set pbar for one tile at one lead time/threshold, field1
   * @param[in] ltSec the lead time seconds
   * @param[in] threshIndex   Index into obar
   * @param[in] tileIndex  index into tiles
   * @param[in] value  The threshold to store
   */
  void setPbar1(int ltSec, int threshIndex, int tileIndex, double value);

  /**
   * set pbar for one tile at one lead time/threshold pair, 2d
   * @param[in] ltSec the lead time seconds
   * @param[in] threshIndex1
   * @param[in] threshIndex2
   * @param[in] tileIndex  index into tiles
   * @param[in] value  The threshold to store
   */
  void setPbar2(int ltSec, int threshIndex1, int threshIndex2,
		int tileIndex, double value);
  
  /**
   * @return the tile information
   */
  inline TileInfo getTileInfo(void) const {return _tiling;}


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

  

 protected:
 private:  

  time_t _genTime;            /**< Every chunk is for a gen time */
  std::vector<int> _leadSeconds;  /**< All the lead seconds */
  TileInfo _tiling;               /**< tile information */
  std::vector<double> _thresh1;   /**< All thresholds, field1 */
  std::string _threshField1;      /**< Field name with varying thresh */
  std::vector<double> _thresh2;   /**< All thresholds, field1 */
  std::string _threshField2;      /**< Field name with varying thresh */
  bool _hasFixedField1;            /**< True if a fixed field is set */
  std::string _fixedField1;         /**< Field name, fixed thresh, if _hasFixedField */
  double _fixedThresh1;             /**< Threshold, fixed field, if _hasFixedField */
  bool _hasFixedField2;            /**< True if a fixed field is set */
  std::string _fixedField2;         /**< Field name, fixed thresh, if _hasFixedField */
  double _fixedThresh2;             /**< Threshold, fixed field, if _hasFixedField */

  

  /**
   * The pbar info at all thresholds (1d) at each lead time
   */
  std::vector<PbarAtLead> _pbarAtLead;

  /**
   * Pbar info at all threshold pairs (2d) at each lead time
   */
  std::vector<PbarAtLead2> _pbarAtLead2;
  
  std::string _genToXml(void) const;
  std::string _threshToXml(void) const;
  std::string _pbarSetToXml(void) const;
  std::string _fixedToXml(void) const;
  std::string _threshFieldToXml(void) const;
  std::string _leadsToXml(void) const;
  std::string _pbarToXml(void) const;
  bool _genFromXml(const std::string &xml);
  bool _threshFromXml(const std::string &xml);
  bool _fixedFromXml(const std::string &xml);
  bool _pbarSetFromXml(const std::string &xml);
  bool _threshFieldFromXml(const std::string &xml);
  bool _leadsFromXml(const std::string &xml);
  bool _tilingFromXml(const std::string &xml);
  bool _pbarFromXml(const std::string &xml);
  
  int _leadIndex(int leadTime) const;
};

#endif
