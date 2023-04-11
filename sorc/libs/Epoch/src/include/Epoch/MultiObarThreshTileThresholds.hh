/**
 * @file MultiObarThreshTileThresholds.hh
 * @brief The threshold information for a gen time, lead time, all obar thresholds, all tiles
 * @class MultiObarThreshTileThresholds
 * @brief The threshold information for a gen time, lead time, all obar thresholds, all tiles
 */

# ifndef    MultiObarThreshTileThresholds_hh
# define    MultiObarThreshTileThresholds_hh

#include <Epoch/MultiTileThresholdsGenBased.hh>
#include <string>
#include <vector>

//----------------------------------------------------------------
class MultiObarThreshTileThresholds
{
public:

  /**
   * Empty
   */
  MultiObarThreshTileThresholds(void);

  /**
   * Constructor to set initial values
   *
   * @param[in] threshColdstartThresh  coldstart value for this field
   * @param[in] tiling                 tiling info
   * @param[in] obarThresh             the obar thresholds
   */
  MultiObarThreshTileThresholds(double threshColdstartThresh,
				const TileInfo &tiling,
				const std::vector<double> &obarThresh);

  /**
   * Constructor that parses XML input and sets tiling
   * @param[in] xml
   * @param[in] tiling
   */
  MultiObarThreshTileThresholds(const std::string &xml,	const TileInfo &tiling);

  /**
   * Destructor
   */
  virtual ~MultiObarThreshTileThresholds(void);
  
  /**
   * @return The actual thresholds for all tiles for an obar index
   *         Returns an empty vector if the index is bad
   * @param[in] obarThreshIndex
   */
  std::vector<double> getTileThresholds(int obarThreshIndex) const;

  /**
   * @return thresholds and additional information for a particular 
   *         obar index for all tiles, empty if problems 
   * @param[in] gen  Value that gets put into the returned objects
   * @param[in] leadSeconds  Value that gets put into the returned objects
   * @param[in] obarThreshIndex  Index into obar
   */
  std::vector<TileThreshInfoGenBased> getTileThreshInfo(const time_t &gen,
							int leadSeconds,
							int obarThreshIndex) const;
  /**
   * Update local state using input information, for a particular obar index
   * @param[in] info  
   * @param[in] obarThreshIndex
   */
  bool update(const TileThreshInfoGenBased &info, int obarThreshIndex);

  /**
    * @return XML string representing the local state
   */
  std::string toXml(void) const;

  /**
   * set threshold  for one tile at one obar index
   * @param[in] obarThreshIndex
   * @param[in] tileIndex
   * @param[in] value  The threshold
   * @param[in] isColdstart true if it is a coldstart threshold
   */
  void setThresh(int obarThreshIndex, int tileIndex, double value,
		 bool isColdstart);
  
  /**
   * Retrieve tiled grid for a particular gen/lead time
   *
   * @param[in] inputName Name of field that has thresholds
   * @param[in] obarThresh  The obar threshold itself
   * @param[in] tileInfo Specification for tiling
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid with thresholds
   * @param[in] motherOnly If true show only tiles that reverted to the mother file
   *
   * @return true if successful
   */
  bool getTiledGrid(const std::string &inputName, double obarThresh,
		    const TileInfo &tileInfo, double centerWeight,
		    double edgeWeight, int nptSmooth,
		    Grid2d &item, bool motherOnly=false) const;

  /**
   * @return number of obar thresholds
   */
  inline int getNumObarThresh(void) const {return (int)_obarThresh.size();}

  /**
   * @return  the actual obar thresholds
   */
  inline std::vector<double> getObarThresh(void) const {return _obarThresh;}

  /**
   * Debug print
   *
   * @param[in] lt  Lead time seconds
   * @param[in] info  Information about tiling
   * @param[in] verbose  True to print more
   */
  void print(int lt, const TileInfo &info, bool verbose=false) const;

  /**
   * Debug print with limitation on what is printed
   *
   * @param[in] lt  Lead time seconds
   * @param[in] tiles     Tile indices to print, empty to print all tiles
   * @param[in] info  Information about tiling
   * @param[in] verbose  True to print more
   */
  void print(int lt, const std::vector<int> &tiles, const TileInfo &info,
	     bool verbose=false) const;

  /**
   * @return True for good state internally
   */
  inline bool isOk(void) const {return _ok;}

  /**
   * Value of XML tag
   */ 
  static const std::string _tag;

 protected:
 private:  

  bool _ok;              /**< True if everything is set */
  bool _fixedValuesSet;  /**< True if everything except thresholds is set*/
  bool _thresholdsSet;   /**< True if thresholds are set */
  std::vector<double> _obarThresh;  /**< All the obar threshold values */
  /**
   * The thresholds for each obar thresh
   */
  std::vector<MultiTileThresholdsGenBased> _thresholdsForObar;

};

#endif
