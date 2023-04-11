/**
 * @file HistGenTime.hh
 * @brief The counts and sums for all tiles, for all obar thresh,
 *         for all lead times at a gentime
 * @class HistGenTime
 * @brief The counts and sums for all tiles, for all obar thresh,
 *         for all lead times at a gentime
 */

#ifndef HistGenTime_hh
#define HistGenTime_hh

#include <Epoch/HistLeadTime.hh>
#include <vector>
#include <ctime>

class HistGenTime
{
public:
  /**
   * @constructor
   * @param[in] gen  The gen time
   * @param[in] leadSeconds  The lead seconds to acount for
   * @param[in] numObarThresh  
   * @param[in] numTiles
   */
  HistGenTime(const time_t &gen, std::vector<int> &leadSeconds, int numObarThresh,
	      int numTiles);

  /**
   * @destructor
   */
  inline ~HistGenTime(void) {}

  /**
   * Update the state for a particular lead time, all tiles, using input info
   * @param[in] info  The information for all tiles at this lead time
   * @param[in] ltSec  The lead time (seconds)
   * @param[in] obarThreshIndex  Obar threshold index
   */
  void update(const std::vector<TileThreshInfoGenBased> &info, int ltSec,
	      int obarThreshIndex);

  /**
   * @return the average threshold for all tiles at a lead time
   * If the average was not computed in a tile the returned value is -1
   * for that tile
   * 
   * @param[in] ltSec  The lead time to get the average for seconds
   * @param[in] obarThreshIndex  Obar threshold index
   */
  std::vector<double> getAverages(int ltSec, int obarThreshIndex) const;

protected:
private:

  time_t _genTime;                      /**< The gen time */
  std::vector<int> _leadSeconds;        /**< The set of lead seconds */
  std::vector<HistLeadTime> _histAtLead; /**< The count/sums for all tiles
					  * at each lead time */

  int _leadIndex(int leadTime) const;
};

#endif
