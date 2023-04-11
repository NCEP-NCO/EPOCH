/**
 * @file GriddedThresh.hh
 * @brief The information about the thresholds coming from multiple databases
 *        and fixed thresholds
 * @class GriddedThresh
 * @brief The information about the thresholds coming from multiple databases
 *       and fixed thresholds
 *
 * One database or fixed threshold per input field
 * One set of thresholds per obar threshold
 */

#ifndef GriddedThresh_hh
#define GriddedThresh_hh

#include "ThreshForOneObar.hh"
#include "DbThresh.hh"
#include "ParmsEnsLookupGen.hh"
#include <vector>

class MultiThreshInfo;

class GriddedThresh
{
public:
  /**
   * Constructor, 
   * @param[in] parms
   */
  GriddedThresh(const ParmsEnsLookupGen &parms);

  /**
   * Destructor
   */
  inline ~GriddedThresh(void) {}

  /**
   * Update state when a new forecast triggers
   * @param[in] genTime
   * @param[in] lt  Lead seconds
   * @param[in] m  The database threshold infomation
   */
  void newLeadTime(const time_t &genTime, int lt,
		   const MultiThreshInfo &m);

  /**
   * Update the ensemble sums when all fields have data that is above the thresholds
   * @param[in] values  The values from an ensemble for each field, thresholded and fixed
   * @param[in] index Index into data grid (one dimensional)
   * @param[in] x  Index into data grid (2 dimensional)
   * @param[in] y  Index into data grid (2 dimensional)
   */
  void updateCount(const std::vector<double> &values, int index, int x, int y);

  /**
   * Prepare output by appending grids and metadata XML to inputs
   *
   * @param[in,out] mOutGrid  Grids
   * @param[in,out] xml  Metadata XML
   */
  void prepareOutput(MultiGrid &mOutGrid);//, std::string &xml);


  /**
   * Set the ensemble sum to missing at a point
   *
   * @param[in] index Index into data grid (one dimensional)
   */
  void setEnsembleSumMissing(int index);

  /**
   * Normalize the ensemble sum by dividing by a count
   *
   * @param[in] index Index into data grid (one dimensional)
   * @param[in] count  The normalizing value
   * @param[in] debug  True to debug
   */
  void normalizeEnsembleSum(int index, double count, bool debug);

protected:
private:

  /**
   * Params
   */
  ParmsEnsLookupGen _parms;   

  /**
   * Outputs.
   * containers for gridded thresholds, one per output obar threshold
   * that being the special output obar threshold set
   */
  std::vector<ThreshForOneObar> _obarThresh; 

  /**
   * Inputs.
   * Containers for database and fixed threshold,
   * one per field, each field can have different obar thresholds
   */
  std::vector<DbThresh> _dbThresh; 

  /**
   * Current time for database, which is supposed to synchronize
   * between multiple databases
   */
  time_t _dbGenTime;
};


#endif
