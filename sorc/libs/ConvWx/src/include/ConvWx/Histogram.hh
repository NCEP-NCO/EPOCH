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
 * @file Histogram.hh
 * @brief  Histogram sorts model grid indices into bins according to 
 *         value at the grid point and user specified bin bounds for grid 
 *         values. It returns the sorted histogram of indices, histogram 
 *         bin totals, the number of missing data values, or values above 
 *         a user specified threshold.
 * @class Histogram
 * @brief Histogram sorts model grid indices into bins according to 
 *         value at the grid point and user specified bin bounds for grid 
 *         values. It returns the sorted histogram of indices, histogram 
 *         bin totals, the number of missing data values, or values above 
 *         a user specified threshold.
 */

#ifndef HISTOGRAM_HH
#define HISTOGRAM_HH

#include <vector>
#include <list>

#include <ConvWx/Grid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/IndexGridVal.hh>
#include <ConvWx/IndexGridValCmpPtr.hh>

class Histogram
{
public:

  /** 
   *   Create histogram
   *   @param[in]  binBounds  Array of length n+1 containing the upper and 
   *                           lower bounds of bins
   *   @param[in]  valThresh  Count kept of values above this threshold when 
   *                           sorting data 
   */
  Histogram(const std::vector<float> binBounds, const float valThresh);

  /** 
   *   Create histogram
   *   @param[in]  binBounds  Array of length n+1 containing the upper and 
   *                           lower bounds of bins
   */
  Histogram(const std::vector<float> binBounds);

  /**
   * Destructor
   */ 
  virtual ~Histogram(void);

  
  /** 
   *   Create histogram of a grid of data based on user specified histogram 
   *   bin bounds.
   *   @param[in] dataGrid  Grid indices will be sorted into histogram bins. 
   *                        Each bin is then sorted by data value.
   *   @param[out] hist  Histogram of data values sorted in ascending order 
   *                     within each bin (vector of vectors)
   *   @param[out] numMissing  Number of missing data points will be recorded
   *   @param[out] numOverThresh  Number of data points above user specified 
   *                               threshold will recorded.
   */
  void binData(const Grid &dataGrid, std::vector < std::vector <int> > &hist, 
			  int &numMissing, int &numOverThresh)const;

  /**
   *   Create histogram of a grid of data based on user specified histogram
   *   bin bounds.
   *   @param[in] dataGrid  Grid indices will be sorted into histogram bins.
   *                        Each bin is then sorted by data value.
   *   @param[out] histBinTot  Vector of histogram bin totals
   *   @param[out] numMissing  Number of missing data points will be recorded
   *   @param[out] numOverThresh  Number of data points above user specified
   *                               threshold will recorded.
   */
  void binData(const Grid &dataGrid, std::vector < int > &histBinTot,
                          int &numMissing, int &numOverThresh)const;

 /** 
   *   Create histogram of a grid of data based on user specified histogram 
   *   bin bounds.
   *   @param[in] indexGrid  Contains indices from larger grid making up 
   *                          sub grid
   *   @param[in] dataGrid  Subset of the data from this grid (specified by 
   *                        indexGrid) will be sorted into histogram bins. 
   *                        Each bin is then sorted by data value.
   *   @param[out] hist  Histogram of data values sorted in ascending order 
   *                     within each bin (vector of vectors)
   *   @param[out] numMissing  Number of missing data points will be recorded
   *   @param[out] numOverThresh  Number of data points above user specified 
   *                               threshold will recorded.
   */
  void binData(const Grid &indexGrid, const Grid &dataGrid, 
	       std::vector < std::vector <int> > &hist, 
	       int &numMissing, int &numOverThresh)const;

  /** 
   *   Create histogram bin totals of data based on user specified histogram 
   *   bin bounds.
   *   @param[in] indexGrid  Contains indices from larger grid making up 
   *                          sub grid
   *   @param[in] dataGrid  Subset of the data from this grid (specified by 
   *                         indexGrid) will be sorted into histogram bins. 
   *                         Each bin is then sorted by data value.
   *   @param[out] histBinTot  Vector of int values representing histogram bin 
   *                           totals 
   *                           
   *   @param[out] numMissing  Number of missing data points will be recorded
   *   @param[out] numOverThresh  Number of data points above user specified 
   *                               threshold will recorded.
   */
  void binData(const Grid &indexGrid, const Grid &dataGrid,  
	       std::vector <int> & histBinTot, 
	       int &numMissing, int &numOverThresh) const;

  /** 
   *   Create histogram bin totals of data based on user specified histogram 
   *   bin bounds.
   *   @param[in] indexGrid  Contains indices from larger grid making up 
   *                          sub grid
   *   @param[in] dataGrid  Subset of the data from this grid (specified by 
   *                         indexGrid) will be sorted into histogram bins. 
   *                         Each bin is then sorted by data value.
   *   @param[out] histBinTot  Histogram of int values representing bin total 
   *                           number of elements
   */
  void binData(const Grid &indexGrid, const Grid &dataGrid,  
	       std::vector <int> & histBinTot) const;

  /** 
   *
   * @return pbinBounds  Vector of floats representing the histogram bin 
   *                     bounds
   */
  inline const std::vector <float> &getBinBounds(void) const
  {
    return pBinBounds;
  } 

protected:

private:
 
  /**
   * Constant for missing data 
   */
  static const int histMissing;

  /** 
   * Vector of floats representing the histogram bin bounds
   */
  std::vector <float> pBinBounds;
  
  /** 
   * Count is kept of values above this threshold when sorting data into bins 
   */
  float pValThresh;  
};

#endif
