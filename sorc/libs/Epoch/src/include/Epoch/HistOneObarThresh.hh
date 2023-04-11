/**
 * @file HistOneObarThresh.hh
 * @brief counts and sums for all tiles at an obar threshold
 * @class HistOneObarThresh
 * @brief counts and sums for all tiles at an obar threshold
 */

#ifndef HistOneObarThresh_hh
#define HistOneObarThresh_hh

#include <Epoch/HistOneTile.hh>
#include <vector>

class TileThreshInfoGenBased;

class HistOneObarThresh
{
public:
  /**
   * Constructor
   * @param[in] leadSeconds  The lead time (seconds)
   * @param[in] numTiles     Number of tiles
   */
  inline HistOneObarThresh(int numTiles) : _numTiles(numTiles)
  {
    for (int i=0; i<numTiles; ++i)
    {
      _histOneTile.push_back(HistOneTile(i));
    }
  }

  /**
   * Destructor
   */
  inline ~HistOneObarThresh(void) {}
  
  /**
   * Update state using input
   * @param[in] info  The information for each tile
   */
  void update(const std::vector<TileThreshInfoGenBased> &info);

  /**
   * @return the average threshold for all tiles
   * If the average was not computed in a tile the returned value is -1
   * for that tile
   */
  std::vector<double> getAverages(void) const;

protected:
private:

  int _numTiles;                           /**< Number of tiles */
  std::vector<HistOneTile> _histOneTile;   /**< The tiled based count/sums*/
};

#endif
