/**
 * @file PbarAtLeadThresh2.hh
 * @brief One set of thresholds (one for each tile)
 * @class PbarAtLeadThresh2
 * @brief One set of thresholds (one for each tile)
 */

# ifndef    PbarAtLeadThresh2_hh
# define    PbarAtLeadThresh2_hh

#include <string>
#include <vector>

// class TileThreshInfoGenBased;
class TileInfo;
class Grid2d;

//----------------------------------------------------------------
class PbarAtLeadThresh2
{
public:

  /**
   * Empty Constructor, but with tiling
   * @param[in] tiling Info on tiling
   */
  PbarAtLeadThresh2(const TileInfo &tiling);


  /**
   * Constructor from XML string, as from toXml()
   *
   * @param[in] xml   String to parse
   * @param[in] tiling  Corraborating tiling information expected in xml
   */
  PbarAtLeadThresh2(const std::string &xml, const TileInfo &tiling);


  /**
   * Destructor
   */
  virtual ~PbarAtLeadThresh2(void);

  /**
   * @return the pbar as set for each tile (2 dimensional list)
   */
  std::vector<double> pbar(void) const;

  /**
   * @return the pbar for a particular tile 
   * @param[in] tileIndex
   */
  inline double pbarForTile(int tileIndex) const {return _pbar[tileIndex];}
  
  /**
   * @return XML repesentation of local state
   * 
   * @param[in] indent  Number of tabs to indent the xml
   */
  std::string toXml(int indent=0) const;

   /**
    * Construct and return a Grid2d that contains tiled pbars
    * with averaging in the overlap, using pbars found locally.
    *
    * @param[in] fieldName  name of field for which to return thresholds
    * @param[in] tiling  Tiling information
    * @param[in] centerWeight  Weight at center of each tile
    * @param[in] edgeWeight  Weight at edge of each tile
    * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
    * @param[out]  grid   The constructed grid
    *
    * @return true for success, false for inputs were wrong
    */
   bool constructTiledGrid(const std::string &fieldName, const TileInfo &tiling,
   			  double centerWeight, double edgeWeight, int nptSmooth,
   			  Grid2d &grid) const;

  /**
   * Set the pbar for a tile
   * @param[in] tileIndex  Which tile
   * @param[in] v The pbar
   */
  void setPbar(int tileIndex, double v);

  /**
   * Debug print
   * @param[in] lt  Lead time seconds
   * @param[in] thresh1  The first threshold for this pbar
   * @param[in] thresh2  The second threshold for this pbar
   * @param[in] info  Tile information
   * @param[in] verbose  True for more print
   */
  void print(int lt, double thresh1, double thresh2, const TileInfo &info,
	     bool verbose=false) const;
  /**
   * Debug print for a subset of tiles
   * @param[in] lt  Lead time seconds
   * @param[in] thresh1  The first threshold for this pbar
   * @param[in] thresh2  The second threshold for this pbar
   * @param[in] tiles The particular tiles to print, empty to print all
   * @param[in] info  Tile information
   * @param[in] verbose  True for more print
   */
  void print(int lt, double thresh1, double thresh2, const std::vector<int> &tiles,
	     const TileInfo &info, bool verbose=false) const;

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

  /**
   *  pbar
   */
  std::vector<double> _pbar;

};

# endif
