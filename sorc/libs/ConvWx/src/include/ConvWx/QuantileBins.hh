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

/**
 * @file  QuantileBins.hh
 * @brief QuantileBins sorts model grid indices into bins quantiles. 
 *        It returns the sorted histogram of indices, the number of 
 *        missing data values, or values above a user specified threshold.
 * @class QuantileBins
 * @brief QuantileBins sorts model grid indices into bins quantiles. 
 *        It returns the sorted histogram of indices, the number of 
 *        missing data values, or values above a user specified threshold.
 */

#ifndef QUANTILE_BINS_HH
#define QUANTILE_BINS_HH

#include <vector>
#include <list>

#include <ConvWx/Grid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/IndexGridVal.hh>
#include <ConvWx/IndexGridValCmpPtr.hh>

class QuantileBins
{
public:

  /** 
   *   Create histogram
   *   @param[in]  qQuantiles Integer indicating number of equal size bins
   *                          in which to sort the data
   *   @param[in]  valThresh  Count kept of values above this threshold when 
   *                          sorting data 
   */
  QuantileBins(const int qQuantiles, const float valThresh);

  /** 
   *   Create histogram
   *   @param[in]  qQuantiles Integer indicating number of equal size bins
   *                          in which to sort the data
   */
  QuantileBins(const int qQuantiles);


  /**
   * Destructor
   */ 
  virtual ~QuantileBins(void);

  /** 
   *   Create histogram of a grid of data based on user specified q-quantile.
   *   @param[in] indexGrid  Contains indices from larger grid making up 
   *                         sub grid
   *   @param[in] dataGrid  Subset of the data from this grid (specified by 
   *                        indexGrid) will be sorted into histogram bins. 
   *                        Each bin is then sorted by data value.
   *   @param[out] hist  QuantileBins of data values sorted in ascending order 
   *                     within each bin (vector of vectors)
   *   @param[out] binAvg  Vector of quantile bin averages
   *   @param[out] numMissing  Number of missing data points will be recorded
   *   @param[out] numOverThresh  Number of data points above user specified 
   *                               threshold will recorded.
   *   @param[in] lowerBound  Lower bound on data values in histogram
   */
  void binData(const Grid &indexGrid, const Grid &dataGrid, 
	       std::vector < std::vector <int> > &hist, 
	       std::vector < float> &binAvg,
	       int &numMissing, int &numOverThresh, float lowerBound)const;

   /**
   *   Create histogram of a grid of data based on user specified q-quantile.
   *   @param[in] dataGrid  Subset of the data from this grid (specified by
   *                        indexGrid) will be sorted into histogram bins.
   *                        Each bin is then sorted by data value.
   *   @param[out] hist  QuantileBins of data values sorted in ascending order
   *                     within each bin (vector of vectors)
   *   @param[out] binAvg  Vector of quantile bin averages
   *   @param[out] numMissing  Number of missing data points will be recorded
   *   @param[out] numOverThresh  Number of data points above user specified
   *                               threshold will recorded.
   *   @param[in] lowerBound  Lower bound on data values in histogram
   */
  void binData( const Grid &dataGrid,
		std::vector < std::vector <int> > &hist,
		std::vector < float> &binAvg,
		int &numMissing, int &numOverThresh, float lowerBound)const;


protected:

private:
 
  /**
   * Constant for missing data 
   */
  static const int histMissing;

  /**
   * Integer indicating number of equal size bins  in which to sort the data 
   */
  int pQQuantiles;

  /** 
   * Count is kept of values above this threshold when sorting data into bins 
   */
  float pValThresh;  
};

#endif
