/**
 * @file HistLeadTime.hh
 * @brief counts and sums for all obar thresholds for all tiles at a lead time
 * @class HistLeadTime
 * @brief counts and sums for all obar thresholds for all tiles at a lead time
 */

#ifndef HistLeadTime_hh
#define HistLeadTime_hh

#include <Epoch/HistOneObarThresh.hh>
#include <vector>

class TileThreshInfoGenBased;

class HistLeadTime
{
public:
  /**
   * Constructor
   * @param[in] leadSeconds  The lead time (seconds)
   * @param[in] numObarThresh  The number of obar thresholds
   * @param[in] numTiles   
   */
  inline HistLeadTime(int leadSeconds, int numObarThresh,  int numTiles) :
    _leadSeconds(leadSeconds), _numObarThresh(numObarThresh)
  {
    for (int i=0; i<numObarThresh; ++i)
    {
      _histOneObarThresh.push_back(HistOneObarThresh(numTiles));
    }
  }

  /**
   * Destructor
   */
  inline ~HistLeadTime(void) {}
  
  /**
   * Update state using input
   * @param[in] info  The information for each tile
   * @param[in] obarThreshIndex
   */
  void update(const std::vector<TileThreshInfoGenBased> &info,
	      int obarThreshIndex);

  /**
   * @return the average threshold for all tiles for some obar threshold
   *
   * @param[in] obarThreshIndex  Index into obar threshold based history information
   * 
   * If the average was not computed in a tile the returned value is -1
   * for that tile
   */
  std::vector<double> getAverages(int obarThreshIndex) const;

protected:
private:

  int _leadSeconds;    /**< The lead time seconds */
  int _numObarThresh;  /**< Number of obar thresholds */
  /**
   * * The tiled based count/sums at each obar thresh
   */
  std::vector<HistOneObarThresh> _histOneObarThresh;  
};

#endif
