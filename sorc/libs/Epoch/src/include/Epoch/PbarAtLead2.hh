/**
 * @file PbarAtLead2.hh
 * @brief The pbar information for a gen time, lead time, all threshold pairs, all tiles
 * @class PbarAtLead2
 * @brief The pbar information for a gen time, lead time, all threshold pairs, all tiles
 */

# ifndef    PbarAtLead2_hh
# define    PbarAtLead2_hh

#include <string>
#include <vector>
#include <Epoch/PbarAtLeadThresh2.hh>

class TileInfo;

//----------------------------------------------------------------
class PbarAtLead2
{
public:

  /**
   * Empty
   */
  PbarAtLead2(void);

  /**
   * Constructor to set initial values
   *
   * @param[in] tiling             tiling info
   * @param[in] thresh1            thresholds, field1
   * @param[in] thresh2            thresholds, field2
   */
  PbarAtLead2(const TileInfo &tiling,
	      const std::vector<double> &thresh1,
	      const std::vector<double> &thresh2);

  /**
   * Constructor that parses XML input and sets tiling
   * @param[in] xml
   * @param[in] tiling
   */
  PbarAtLead2(const std::string &xml, const TileInfo &tiling);

  /**
   * Destructor
   */
  virtual ~PbarAtLead2(void);
  
  /**
   * @return the second fields pbars for a particular tile (at all thresholds)
   * @param[in] tileIndex Index into the tiles
   * @param[in] index1  first field's index into the 2d storage
   */
  std::vector<double> getPbars2ForOneTile(int tileIndex, int index1) const;
  
  /**
   * @return The actual pbar for all tiles for a thresh index pair
   *         Returns an empty vector if the index is bad
   * @param[in] threshIndex1
   * @param[in] threshIndex2
   */
  std::vector<double> getTilePbar(int threshIndex1, int threshIndex2) const;

  /**
   * Copy the pbars from one tile to another tile, at all thresholds
   * @param[in] fromTile  Index to tile to get from 
   * @param[in] toTile  Index to tile to put to
   */
  void copyPbarForAllThresh(int fromTile, int toTile);

  /**
   * Set pbar values for a tile (all thresholds, ordered 1st field moves fastest)
   * @param[in] tileIndex  Which tile
   * @param[in] pbar  The pbar values for all thresholds, ordered as indicated
   */
  void setPbarForAllThresh(int tileIndex, const std::vector<double> &pbar);

  /**
    * @return XML string representing the local state
   */
  std::string toXml(void) const;

  /**
   * set pbar for one tile at one thresh index pair into the state
   * 
   * @param[in] threshIndex1
   * @param[in] threshIndex2
   * @param[in] tileIndex
   * @param[in] value  The pbar
   */
  void setPbar(int threshIndex1, int threshInde2, int tileIndex, double value);
  
  /**
   * Retrieve tiled grid for a particular gen/lead time
   *
   * @param[in] fieldName name for the returned field
   * @param[in] thresh1  threshold itself
   * @param[in] thresh2  threshold itself
   * @param[in] tileInfo Specification for tiling
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid with thresholds
   *
   * @return true if successful
   */
  bool getTiledGrid(const std::string &fieldName, double thresh1, double thresh2,
		    const TileInfo &tileInfo, double centerWeight, double edgeWeight,
		    int nptSmooth, Grid2d &item) const;

  /**
   * Retrieve tiled grid for a particular gen/lead time, different naming
   *
   * @param[in] fieldName1 name1
   * @param[in] fieldName2 name2
   * @param[in] thresh1  threshold itself
   * @param[in] thresh2  threshold itself
   * @param[in] tileInfo Specification for tiling
   * @param[in] centerWeight  Weight at center of each tile
   * @param[in] edgeWeight  Weight at edge of each tile
   * @param[in] nptSmooth Smoothing to do in the tiled grid, number of points
   * @param[out] item   Returned information, one grid with thresholds
   *
   * @return true if successful
   */
  bool getTiledGrid2(const std::string &fieldName, const std::string &fieldName2,
		     double thresh1, double thresh2,
		    const TileInfo &tileInfo, double centerWeight, double edgeWeight,
		    int nptSmooth, Grid2d &item) const;

  /**
   * @return number of thresholds
   * @param[in] which 1 or 2
   */
  inline int getNumThresh(int which) const
  {
    if (which == 1)
    {
      return (int)_thresh1.size();
    }
    else if (which == 2)
    {
      return (int)_thresh2.size();
    }
    else
    {
      return 0;
    }
  }
  
  /**
   * @return  the actual thresholds
   * @param[in] which  1 or 2
   */
  inline std::vector<double> getThresh(int which) const
  {
    if (which == 1)
    {
      return _thresh1;
    }
    else if (which == 2)
    {
      return _thresh2;
    }
    else
    {
      std::vector<double> x;
      return x;
    }
  }

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
  std::vector<double> _thresh1;  /**< All the threshold values */
  std::vector<double> _thresh2;  /**< All the threshold values */

  /**
   * The pbar for each threshold pair, all tiles, 2 dimensional list
   */
  std::vector<PbarAtLeadThresh2> _pbarAtThresh;

  int _index2d(int i1, int i2) const;
  int _indexFromThresh(double thresh1, double thresh2) const;
};

#endif
