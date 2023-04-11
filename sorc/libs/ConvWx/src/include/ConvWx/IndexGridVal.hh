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
 * @file IndexGridVal.hh
 * @brief IndexGridVal holds grid index and data value at the grid index.
 * @class IndexGridVal
 * @brief IndexGridVal holds grid index and data value at the grid index.
 */

#ifndef INDEXGRIDVAL_HH
#define INDEXGRIDVAL_HH

#include <utility> 

class  IndexGridVal{

public:

  /** 
   * Constructor
   * @param[in]  index  Array index of grid point
   * @param[in]  gridVal  Data value at grid index
   */
  inline IndexGridVal(const double index, const double gridVal)
  {
    pIndex = index; pGridVal = gridVal;
  }
  

  /**
   * Default destructor  
   */
  inline virtual ~IndexGridVal(void) {} 

  /** 
   *  @return pIndex  Model grid array index 
   */
  inline double getIndex(void)const {return pIndex;}

  /** 
   * @return pGridVal  Data value at model grid index 
   */
  inline double getValue(void)const {return pGridVal;}

  /** 
   * @param[in] a  Reference to an IndexGridVal object
   * @return true if pGridVal is less than input argument's pGridVal member. 
   *           Else return false.
   */
  inline bool operator <(const IndexGridVal& a) const
  {
    return pGridVal  < a.pGridVal;
  }


protected: 

private:

  /** 
   * Array index of grid point.
   */
  double pIndex;

  /** 
   * Data value at grid index.
   */
  double pGridVal;
};

#endif

