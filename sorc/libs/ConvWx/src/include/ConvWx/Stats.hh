// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file Stats.hh 
 * @brief Compute and manage statistical scoring of forecast vs. observation
 * @class Stats
 * @brief compute and manage statistical scoring of forecast vs. observation
 *
 * Used by phase correction algorithms to decide whether corrections are
 * worthwhile or not.
 *
 * Computes measure of improvement for a set of thresholds, then does a
 * weighted average of these improvements.
 */

#ifndef STATS_H
#define STATS_H
#include <string>
#include <vector>
#include <ConvWx/Stat.hh>

class FcstGrid;
class Grid;

//----------------------------------------------------------------
class Stats
{
public:

  /**
   * Empty constructor
   */
  Stats(void);

  /**
   * Destructor
   */
  virtual ~Stats(void);

  /**
   * Initialize so that stats can be computed from data
   *
   * @param[in] threshWeightPairs  Pairs of threshold/weight values
   * @param[in] type  The statistic to compute (CSI or CORRELATION)
   */
  void init(const std::vector<std::pair<double, double> > &threshWeightPairs,
	    const Stat::Stat_t type);

  /**
   * Compute the statistical measure for input forecasts with and without
   * phase correction.
   *
   * @param[in] noPcGrid  Forecast without phase correction
   * @param[in] yesPcGrid  Forecast with phase correction
   * @param[in] obsGrid  Observation data
   *
   * Computes each individual statistic, then does the weighted average
   */
  void compute(const Grid &noPcGrid, const Grid &yesPcGrid,
	       const Grid &obsGrid);

  /**
   * Clear object so no statistics are in the internal state
   */
  void clear(void);

  /**
   * Return metadata, Metadata is in XML  format.
   *
   * @return String with metadata
   */
  std::string metadata(void) const;

  /** 
   * @return  True if phase correction statistics are better than 
   *          non phase corrected.
   */
  inline bool keepPc(void) const {return pKeepPc;}

  /**
   * @return  Score from phase corrected forecast
   */
  inline double getPcScore(void) const {return pPcScore;}

  /**
   * @return  Score from non phase corrected forecast
   */
  inline double getScore(void) const {return pScore;}

  /**
   * @return  Phase corrected score divided by non phase corrected score
   */
  inline double skillRatio(void) const
  {
    return (pPcScore + 0.00001)/(pScore + 0.00001);
  }

protected:
private:

  /**
   * Pairs of values, with first = threshold and second = weight
   */
  std::vector<std::pair<double, double> > pThreshWeight;

  Stat::Stat_t pType;      /**< type of statistic CSI or CORRELATION */
  std::vector<Stat> pStat; /**< the individual statistics, one per 
			    *   threshold/weight */
  double pPcScore;         /**< score with phase correction */
  double pScore;           /**< score without phase correction */
  bool pKeepPc;            /**< set to true if phase correction improved score*/

  /**
   * Compute overall statistical score for inputs
   *
   * @param[in] fcstGrid   Forecast Grid
   * @param[in] obsGrid  Observation Grid
   * @param[in] thresh  Threshold at which to compute statistic
   *
   * @return statistical score
   */
  double pComputeStat(const Grid &fcstGrid, const Grid &obsGrid,
		      const double thresh);
};

#endif
