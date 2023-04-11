/**
 * @file PbarAtLead.hh
 * @brief The pbar information for a gen time, lead time, all thresholds, all tiles
 * @class PbarAtLead
 * @brief The pbar information for a gen time, lead time, all thresholds, all tiles
 */

# ifndef    PbarAtLead_hh
# define    PbarAtLead_hh

#include <string>
#include <vector>
#include <Epoch/PbarAtLeadThresh.hh>

class TileInfo;

//----------------------------------------------------------------
class PbarAtLead
{
public:

  /**
   * Empty
   */
  PbarAtLead(void);

  /**
   * Constructor to set initial values
   *
   * @param[in] tiling             tiling info
   * @param[in] thresh             thresholds
   */
  PbarAtLead(const TileInfo &tiling,
	     const std::vector<double> &thresh);

  /**
   * Constructor that parses XML input and sets tiling
   * @param[in] xml
   * @param[in] tiling
   */
  PbarAtLead(const std::string &xml, const TileInfo &tiling);

  /**
   * Destructor
   */
  virtual ~PbarAtLead(void);
  
  /**
   * @return The actual pbar for all tiles for a thresh index
   *         Returns an empty vector if the index is bad
   * @param[in] threshIndex
   */
  std::vector<double> getTilePbar(int threshIndex) const;

  /**
   * @return The all pbars for one tile
   *         Returns an empty vector if the index is bad
   * @param[in] tileIndex
   */
  std::vector<double> getPbarsForOneTile(int tileIndex) const;

  /**
   * Copy the pbars from one tile to another tile, at all thresholds
   * @param[in] fromTile  Index to tile to get from 
   * @param[in] toTile  Index to tile to put to
   */
  void copyPbarForAllThresh(int fromTile, int toTile);

  /**
   * Set pbar values for a tile (all thresholds)
   * @param[in] tileIndex  Which tile
   * @param[in] pbar  The pbar values for all thresholds
   */
  void setPbarForAllThresh(int tileIndex, const std::vector<double> &pbar);

  /**
    * @return XML string representing the local state
   */
  std::string toXml(void) const;

  /**
   * set pbar for one tile at one thresh index
   * @param[in] threshIndex
   * @param[in] tileIndex
   * @param[in] value  The pbar
   */
  void setPbar(int threshIndex, int tileIndex, double value);
  
  /**
   * Retrieve tiled grid for a particular gen/lead time
   *
   * @param[in] inputName Name of field that has thresholds
   * @param[in] thresh  The obar threshold itself
   * @param[in] tileInfo Specification for tiling
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid with thresholds
   *
   * @return true if successful
   */
  bool getTiledGrid(const std::string &inputName, double thresh,
		    const TileInfo &tileInfo, double centerWeight, double edgeWeight,
		     int nptSmooth, Grid2d &item) const;

  /**
   * @return number of thresholds
   */
  inline int getNumThresh(void) const {return (int)_thresh.size();}

  /**
   * @return  the actual thresholds
   */
  inline std::vector<double> getThresh(void) const {return _thresh;}

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
  bool _fixedValuesSet;  /**< True if everything except pbar is set*/
  bool _thresholdsSet;   /**< True if pbar are set */
  std::vector<double> _thresh;  /**< All the threshold values */
  /**
   * The pbar for each thresh
   */
  std::vector<PbarAtLeadThresh> _pbarAtThresh;

};

#endif
