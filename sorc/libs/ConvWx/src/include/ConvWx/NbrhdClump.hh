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
 * @file NbrhdClump.hh
 * @brief Clump index value, an array of Grid indices, and a minimum data value
 * @class NbrhdClump
 * @brief Clump index value, an array of Grid indices, and a minimum data value
 *
 * This is the bookkeeping for a non-grid representation of one clump
 */

# ifndef    NBRHD_CLUMP_H
# define    NBRHD_CLUMP_H

#include <vector>
#include <string>
class Grid;
class ConvWxFuzzy;

/**
 * @def NUM_ARRAY_CLUMP
 * size of array used for efficiency
 */
#define NUM_ARRAY_CLUMP 5000

class NbrhdClump 
{      
public:

  /**
   * Create a clump object with no points, just an index value
   *
   * @param[in] value  The clump index value
   */
  NbrhdClump(const double value);

  /**
   * Destructor
   */
  virtual ~NbrhdClump(void);

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * Add a point to the clump
   *
   * @param[in] i  The one dimensional Grid index
   * @param[in] dataValue  Data value at index i
   */
  void addPoint(const int i, const double dataValue);

  /**
   * @return The clump index value 
   */
  inline double clumpValue(void) const {return pClumpValue;}

  /**
   * Merge points from input into the local state
   *
   * @param[in] mergeData   NbrhdClump to be merged into local state
   */
  void mergePoints(const NbrhdClump &mergeData);

  /**
   * @return  The i'th one dimensional Grid index value i=0,1,.. for the clump
   *
   * @param[in] i  index to value to return
   */
  int ithGridIndex(const int i) const;

  /**
   * @return  The number of points in the clump
   */
  inline int npt(void) const {return pNumPoint;}

  /**
   * @return  The minimum data value in the clump
   */
  inline double minClumpDataValue(void) const {return pMinDataValue;}

  /**
   * Write to a Grid at the clump points
   *
   * @param[in,out] outGrid  Grid to write to
   * @param[in] value  Value to put into the Grid at clump points
   */
  void writeClumpToGridWithValue(Grid &outGrid, const double value) const;

protected:
private:

  /**
   * The index value for this clump
   */
  double pClumpValue;

  /**
   * Number of one dimensional Grid index values
   */
  int pNumPoint;

  /**
   * One dimensional Grid index values (an array for efficiency)
   */
  int pGridIndex[NUM_ARRAY_CLUMP];

  /**
   * Overflow vector of one dimensional Grid index values (for big clumps)
   */
  std::vector<int> pExtraGridIndex;

  /**
   * The minimum data value in the clump
   */
  double pMinDataValue;

  /**
   * Add a point to the clump
   * @param[in] i  One dimensional Grid index
   */
  void pAddPoint(const int i);
};

# endif 
