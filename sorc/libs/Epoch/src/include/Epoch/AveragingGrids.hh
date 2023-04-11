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
 * @file  AveragingGrids.hh
 * @brief Count and sum grids stored locally and accumulated into 
 *        to maintain ability to take an average
 * @class  AveragingGrids
 * @brief Count and sum grids stored locally and accumulated into 
 *        to maintain ability to take an average
 */

# ifndef    AVERAGING_GRIDS_HH
# define    AVERAGING_GRIDS_HH

#include <ctime>
#include <string>
#include <vector>
#include <ConvWx/Grid.hh>

//----------------------------------------------------------------
class AveragingGrids
{
public:

  /**
   * Create counts/sums grids using input template and a fieldname
   *
   * @param[in] fieldName
   * @param[in] templateGrid
   */
  AveragingGrids(const std::string &fieldName, const Grid &templateGrid);

  /**
   * Destructor
   */
  virtual ~AveragingGrids(void);

  /**
   * Set counts and sums to 0
   */
  void initialize(void);

  /**
   * Increment the 'counts' grid at index by 1
   * @param[in] index
   */
  void incrementCount(int index);

  /**
   * Increment the 'sums' grid at index by 1
   * @param[in] index
   */
  void increment(int index);

  /**
   * Divide the sums grid by the counts grid, and rename the result back to
   * the original name
   */
  void normalize(void);

  /**
   * @return reference to the sums grid
   */
  inline Grid &dataGridRef(void) { return _sums;}

  /**
   * @return reference to the sums grid
   */
  inline const Grid &dataGridRef(void) const { return _sums;}

protected:
private:  

  const std::string _fieldName;  /**< Input field name */
  Grid _counts;                  /**< Running counts */
  Grid _sums;                    /**< Running sums */
};

# endif     // AveragingGrids HH
