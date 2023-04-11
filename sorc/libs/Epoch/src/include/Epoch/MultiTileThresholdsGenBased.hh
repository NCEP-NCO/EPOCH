/**
 * @file MultiTileThresholdsGenBased.hh
 * @brief One set of thresholds (one for each tile)
 * @class MultiTileThresholdsGenBased
 * @brief One set of thresholds (one for each tile)
 */

# ifndef    MultiTileThresholdsGenBased_hh
# define    MultiTileThresholdsGenBased_hh

#include <Epoch/SingleTileThresholdsGenBased.hh>
#include <string>
#include <vector>

class TileThreshInfoGenBased;
class TileInfo;
class Grid2d;

//----------------------------------------------------------------
class MultiTileThresholdsGenBased
{
public:

  /**
   * Empty Constructor, but with tiling
   * @param[in] tiling Info on tiling
   */
  MultiTileThresholdsGenBased(const TileInfo &tiling);


  /**
   * Constructor from XML string, as from toXml()
   *
   * @param[in] xml   String to parse
   * @param[in] tiling  Corraborating tiling information expected in xml
   */
  MultiTileThresholdsGenBased(const std::string &xml, const TileInfo &tiling);

  /**
   * Coldstart constructor
   *
   * @param[in] coldstartThresh the coldstart threshold
   * @param[in] tiling  Info for the tiles.
   */
  MultiTileThresholdsGenBased(double coldstartThresh, const TileInfo &tiling);

  /**
   * Destructor
   */
  virtual ~MultiTileThresholdsGenBased(void);

  /**
   * @return true if values are set
   */
  inline bool valuesSet(void) const {return _valuesSet;}
  
  /**
   * @return the thresholds as set for each tile
   */
  std::vector<double> thresholds(void) const;

  /**
   * @return the thresholds as set for each for which the mother tile was used, 0 for all other tiles
   */
  std::vector<double> thresholdsMotherOnly(void) const;

  /**
   * Build up the thresholds info for each tile and return
   * @return information as set
   * @param[in] gen  Gen time to put into the information
   * @param[in] lead  Lead time seconds to put into the information
   */
  std::vector<TileThreshInfoGenBased> getTileThreshInfo(const time_t &gen,
							int lead) const;


  /**
   * Update local state using input
   *
   * @param[in] item Object with values to put into state
   *
   * @return true if update happened without error, false otherwise
   */
  bool update(const TileThreshInfoGenBased &item);

  /**
   * @return XML repesentation of local state
   * 
   * @param[in] indent  Number of tabs to indent the xml
   */
  std::string toXml(int indent=0) const;

   /**
    * Construct and return a Grid2d that contains tiled thresholds 
    * with averaging in the overlap, using tile thresholds found locally.
    * If there is no tiling, return a grid with a single threshold at all points
    *
    * @param[in] fieldName  name of field for which to return thresholds
    * @param[in] tiling  Tiling information
    * @param[in] centerWeight  Weight at center of each tile
    * @param[in] edgeWeight  Weight at edge of each tile
    * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
    * @param[out]  grid   The constructed grid
    * @param[in] motherOnly If true show only tiles that reverted to the mother file
    *
    * @return true for success, false for inputs were wrong
    */
   bool constructTiledGrid(const std::string &fieldName, const TileInfo &tiling,
   			  double centerWeight, double edgeWeight, int nptSmooth,
			   Grid2d &grid, bool motherOnly=false) const;

  /**
   * Set the threshold for a tile
   * @param[in] tileIndex  Which tile
   * @param[in] v The threshold
   * @param[in] isColdstart  True if the threshold is for coldstart
   */
  void setThresh(int tileIndex, double v, bool isColdstart);

  /**
   * Debug print of local state along with inputs
   * @param[in] lt  Lead secvonds
   * @param[in] obarThresh  Obar value
   * @param[in] info  Information object to pull print stuff out of
   * @param[in] verbose  True for more debug output
   */
  void print(int lt, double obarThresh, const TileInfo &info,
	     bool verbose=false) const;

  /**
   * Debug print of local state along with inputs
   * @param[in] lt  Lead secvonds
   * @param[in] obarThresh  Obar value
   * @param[in] tiles spec of tile indices
   * @param[in] info  Information object to pull print stuff out of
   * @param[in] verbose  True for more debug output
   */
  void print(int lt, double obarThresh, const std::vector<int> &tiles,
  	     const TileInfo &info,
  	     bool verbose=false) const;

  /**
   * @return true if set
   */
  inline bool ok(void) const {return _ok;}

  /**
   * Value of XML tag
   */ 
  static const std::string _tag;

protected:
private:  

  bool _ok;        /**< True if object is set */
  bool _valuesSet; /**< True if values set actually using obs verification*/

  /**
   *  threshold information for each tile
   */
  std::vector<SingleTileThresholdsGenBased> _thresh;

  /**
   * @return pointer to the SingleTileThresholds that goes with the tile if
   * there is one, or NULL if no such tile in the map
   * @param[in] tileIndex
   */
  SingleTileThresholdsGenBased *_mapFromTileIndex(int tileIndex);

  /**
   * @return pointer to the SingleTileThresholds that goes with the tile if
   * there is one, or NULL if no such tile in the map
   * @param[in] tileIndex
   */
  const SingleTileThresholdsGenBased *_constMapFromTileIndex(int tileIndex) const;

};

# endif
