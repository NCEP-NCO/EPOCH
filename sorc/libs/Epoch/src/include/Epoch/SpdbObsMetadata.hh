/**
 * @file SpdbObsMetadata.hh
 * @brief The metadata in a database of Obar values
 * @class SpdbObsMetadata
 * @brief The metadata in a database of Obar values
 */

# ifndef    SpdbObsMetadata_hh
# define    SpdbObsMetadata_hh

#include <Epoch/TileInfo.hh>
#include <Epoch/ThreshObarInfo.hh>
#include <string>
#include <vector>

class Grid2d;

//----------------------------------------------------------------
class SpdbObsMetadata
{
public:
  /**
   * Empty
   */
  SpdbObsMetadata(void);

  /**
   * Constructor to set initial values
   *
   * @param[in] field Name of field in obs data
   * @param[in] tiling  Tile information
   * @param[in] thresh  Thresholds to use
   */
  SpdbObsMetadata(const std::string &field, const TileInfo &tiling,
		  const std::vector<double> &thresh);

  /**
   * Destructor
   */
  virtual ~SpdbObsMetadata(void);
  
  /**
   * @return true if values have been set
   */
  inline bool valuesSet(void) const {return _valuesSet;}
  
  /**
   * parse XML string into local data
   * @param[in] xml
   * @return true if successful
   */
  bool fromXml(const std::string &xml);

  /**
    * @return XML string representing the local state
   */
  std::string toXml(void) const;

  /**
   * Update local state using input information, for tile 
   * specificed in info
   *
   * @param[in] threshIndex
   * @param[in] info
   */
  void update(int threshIndex, const TileObarInfo &info);

  /**
   * Update local time with input
   * @param[in] obsTime
   */
  inline void updateTime(const time_t &obsTime) { _obsTime = obsTime;}

  /**
   * Update local state using input information, for tile
   * specified by input
   *
   * @param[in] threshIndex
   * @param[in] tileIndex
   * @param[in] info
   */
  void update(int threshIndex, int tileIndex, const TileObarInfo &info);

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * Simple gets
   */
  inline int getNumThresh(void) const {return (int)_info.size();}
  inline double getIthThresh(int i) const {return _thresh[i];}

  /**
   * @return information about one tile from local state
   *
   * @param[in] obarThreshIndex  which obar threshold
   * @param[in] tileIndex  Which tile
   */
  inline TileObarInfo getObarInfo(int obarThreshIndex, int tileIndex) const
  {
    if (_valuesSet)
    {
      return _info[obarThreshIndex].getObarInfo(tileIndex);
    }
    else
    {
      return TileObarInfo();
    }
  }
  
  /**
   * Retrieve tiled grid for a particular lead time, and obar threshold
   *
   * @param[in] obarThresh  Obar threshold
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid
   *
   * @return true if successful
   */
  bool getTiledGrid(double obarThresh, double centerWeight,
		    double edgeWeight,  int nptSmooth, Grid2d &item) const;

  /**
   * Debug print for a particular set of tiles
   * @param[in] t   The data time
   * @param[in] timeWritten  The SPDB written time
   * @param[in] tiles  Set of tiles to print, or empty for all
   * @param[in] verbose  True to print more things
   */
  void printState( const time_t &t, const time_t &timeWritten,
		   const std::vector<int> &tiles, bool verbose=false) const;

  /**
   * Debug print for all tiles
   * @param[in] t   The data time
   * @param[in] timeWritten  The SPDB written time
   * @param[in] verbose  True to print more things
   */
  void printState(const time_t &t, const time_t &timeWritten,
		   bool verbose) const;

  /**
   * Debug print of tile info, including input time
   * @param[in] time  Time
   */
  void showTilingInfo(const time_t &time) const;

  /**
   * @return the local threshold
   */
  inline std::vector<double> getObarThresh(void) const {return _thresh;}

 protected:
 private:  

  bool _valuesSet;             /**< True if values have been set */
  std::string _field;          /**< Field name tied to this obar */
  std::vector<double> _thresh; /**< the obar thresholds (one for each _info) */
  TileInfo _tiling;            /**< Information about tiles */
  time_t _obsTime;             /**< Observations time=chunk time */

  /**
   * obar info for each obar threshold
   */
  std::vector<ThreshObarInfo> _info;
  
  int _threshIndex(double obarThresh) const;

  std::string _valuesSetToXml(void) const;
  std::string _fieldToXml(void) const;
  std::string _threshToXml(void) const;
  std::string _obsTimeToXml(void) const;
  std::string _infoToXml(void) const;
  
  bool _valuesSetFromXml(const std::string &xml);
  bool _fieldFromXml(const std::string &xml);
  bool _tilingFromXml(const std::string &xml);
  bool _threshFromXml(const std::string &xml);
  bool _obsTimeFromXml(const std::string &xml);
  bool _infoFromXml(const std::string &xml);

};

#endif

