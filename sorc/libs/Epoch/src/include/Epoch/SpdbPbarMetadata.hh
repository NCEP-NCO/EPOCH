/**
 * @file SpdbPbarMetadata.hh
 * @brief The pbar information for a gen time, all lead times, all thresholds,
 * all tiles
 * @class SpdbPbarMetadata
 * @brief The pbar information for a gen time, all lead times, all thresholds,
 * all tiles
 *
 */

# ifndef    SpdbPbarMetadata_hh
# define    SpdbPbarMetadata_hh

#include <Epoch/TileInfo.hh>
#include <Epoch/PbarAtLead.hh>
#include <string>
#include <vector>

//----------------------------------------------------------------
class SpdbPbarMetadata
{
public:
  /**
   * Empty
   */
  SpdbPbarMetadata(void);

  /**
   * Constructor to set initial values
   * Thresholds not known, filled in later.
   *
   * @param[in] genTime
   * @param[in] ltHours
   * @param[in] threshField
   * @param[in] tiling
   * @param[in] thresholds
   */
  SpdbPbarMetadata(const time_t &genTime,
		   const std::vector<double> &ltHours,
		   const std::string &threshField,
		   const TileInfo &tiling,
		   const std::vector<double> &thresholds);

  /**
   * Destructor
   */
  virtual ~SpdbPbarMetadata(void);
  
  /**
   * @return true if the pbars have been set
   */
  inline bool pbarSet(void) const {return _pbarSet;}


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
   * @return the number of thresholds
   */
  int getNumThresh(void) const;

  /**
   * @return the thresholds themselves, or an empty vector
   */
  std::vector<double> getThresh(void) const;

  /**
   * @return the actual pbars for all tiles at a lead time and threshold
   * @param[in] leadSeconds 
   * @param[in] threshIndex  Index into the thresholds
   */
  std::vector<double> getTilePbar(int leadSeconds, int threshIndex) const;

  /**
   * Copy the pbars from one tile to another tile, at all thresholds
   * @param[in] leadSeconds 
   * @param[in] fromTile  Tile index to copy from
   * @param[in] toTile  Tile index to copy into
   */
  void copyPbarForAllThresh(int leadSeconds, int fromTile, int toTile);

  /**
   * Set pbar values for a tile (all thresholds)
   * @param[in] leadSeconds 
   * @param[in] tileIndex  Tile index tomodify
   * @param[in] pbar The pbar values for all thresholds
   */
  void setPbarForAllThresh(int leadSeconds, int tileIndex,
			   const std::vector<double> &pbar);

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
   * @param[in] thresh
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid
   *
   * @return true if successful
   */
  bool getTiledGrid(int leadTime, double thresh, double centerWeight,
		    double edgeWeight,  int nptSmooth, Grid2d &item) const;

  /**
   * set pbar for one tile at one lead time/threshold
   * @param[in] ltSec the lead time seconds
   * @param[in] threshIndex   Index into obar
   * @param[in] tileIndex  index into tiles
   * @param[in] value  The threshold to store
   */
  void setPbar(int ltSec, int threshIndex, int tileIndex, double value);
  
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
  std::vector<double> _thresh;   /**< All thresholds */
  std::string _threshField;       /**< Field name with varying thresh */
  TileInfo _tiling;               /**< tile information */
  bool _pbarSet;            /**< True if thresholds are set */

  /**
   * The pbar info at all thresholds at each lead time
   */
  std::vector<PbarAtLead> _pbarAtLead;

  std::string _genToXml(void) const;
  std::string _threshToXml(void) const;
  std::string _pbarSetToXml(void) const;
  std::string _threshFieldToXml(void) const;
  std::string _leadsToXml(void) const;
  std::string _pbarToXml(void) const;
  bool _genFromXml(const std::string &xml);
  bool _threshFromXml(const std::string &xml);
  bool _pbarSetFromXml(const std::string &xml);
  bool _threshFieldFromXml(const std::string &xml);
  bool _leadsFromXml(const std::string &xml);
  bool _tilingFromXml(const std::string &xml);
  bool _pbarFromXml(const std::string &xml);
  
  int _leadIndex(int leadTime) const;
};

#endif
