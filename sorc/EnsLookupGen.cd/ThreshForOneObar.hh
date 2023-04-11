/**
 * @file ThreshForOneObar.hh
 * @brief Information for a particular obar threshold, and storage for
 *        threshold grids for that obar threshold
 * @class ThreshForOneObar
 * @brief Information for a particular obar threshold, and storage for
 *        threshold grids for that obar threshold
 */

#ifndef ThreshForOneObar_hh
#define ThreshForOneObar_hh

#include "ParmsEnsLookupGen.hh"
#include <Epoch/ThresholdDatabaseParms.hh>
#include <ConvWx/Grid.hh>
#include <euclid/Grid2d.hh>
#include <vector>

class DbThresh;

class ThreshForOneObar
{
public:
  /**
   * Constructor
   * @param[in] obarParm  For this obar thresh
   * @param[in] parm  Main params
   */
  ThreshForOneObar(const ObarThreshParms &obarParm,
		   const ParmsEnsLookupGen &parm);

  /**
   * Destructor
   */
  inline ~ThreshForOneObar(void) {}

  /**
   * Create thresholds grids 
   * @param[in] genTime  Not used
   * @param[in] lt  Used to access thresholds from database
   * @param[in] dbThresh  The database spec for all fields
   */
  void createThresholdGrids(const time_t &genTime, int lt,
			    const std::vector<DbThresh> &dbThresh);

  /**
   * Set the ensemble sum to missing at a point
   * @param[in] index  Grid index
   */
  void setEnsembleSumMissing(int index);

  /**
   * Normalize the ensemble sum by dividing by count at an index
   * @param[in] index  Grid index
   * @param[in] count  Count to divide by
   * @param[in] debug  True to show debug info
   */
  void normalizeEnsembleSum(int index, double count, bool debug);
  
  /**
   * Update the ensemble sum when all fields have data that is above the threshold
   * for that field at that point
   * 
   * @param[in] values  The values from an ensemble for each field including fixed
   * @param[in] index Index into data grid (one dimensional)
   * @param[in] x  Index into data grid (2 dimensional)
   * @param[in] y  Index into data grid (2 dimensional)
   */
  void updateCount(const std::vector<double> &values, int index, int x, int y);

  /**
   * Prepare output by appending grids
   *
   * @param[in,out]  mOutGrid  Grids
   */
  void prepareOutput(MultiGrid &mOutGrid) const;

protected:
private:

  ObarThreshParms _oparms;           /**< params for this obar */
  ParmsEnsLookupGen _parms;    /**< main params */

  /**
   * expanded gridded thresholds from a tile or from a fixed threshold,
   * one per field
   */
  std::vector<Grid2d> _thresholdGrids;

  /**
   * Matches thresholdGrids, true to output this grid
   */
  std::vector<bool> _doOutputThresholdsGrid;

  /**
   * Sum of ensemble members that passed the threshold test at each point
   */
  Grid _ensembleSum;

};


#endif
