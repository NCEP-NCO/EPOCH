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
 * @file NbrhdClumps.hh
 * @brief Zero or more NbrhdClump objects, with merging
 * @class NbrhdClumps
 * @brief Zero or more NbrhdClump objects, with merging
 *
 * This is a non-grid representation of a set of clumps used by Nbrhd.
 * It was developed due to a need for efficiency (so as to not have to
 * move through the entire grid).
 *
 * State includes one clump that is the current one, and a set of clumps
 * that is to be removed.
 */

# ifndef    NBRHD_CLUMPS_H
# define    NBRHD_CLUMPS_H

#include <vector>
#include <ConvWx/NbrhdClump.hh>

class NbrhdClumps 
{      
public:

  /**
   * Constructor.. no clumps
   */
  NbrhdClumps(void);

  /**
   * Destructor
   */
  virtual ~NbrhdClumps(void);

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * Clear so there are no clumps
   */
  void clear(void);

  /**
   * Start a new clump using the input clump value and no points.
   * The new clump becomes the current clump.
   *
   * @param[in] value  The clump value
   */
  void startClump(const double value);

  /**
   * Add a point to the current clump
   *
   * @param[in] index  Grid 1 dimensional index
   * @param[in] value  Data value at the point to be added to the clump
   */
  void markPoint(const int index, const double value);

  /**
   * Merge multiple clumps in the internal local state
   *
   * @param[in] clumps  The multiple clump values that are to be merged together
   * @param[in] minClumpNumPts  Minimum number of points in the merged clump.
   *                            If the merged clump has less than this many
   *                            points, it becomes a clump marked to remove in
   *                            the internal state
   */
  void merge(const std::vector<int> &clumps, const int minClumpNumPts);

  /**
   * Set a Grid to a missing data value at all points in the 'remove' clumps.
   *
   * @param[in,out]  dataGrid  Grid to filter
   * @param[in] missingValue  The missing data value to use
   */
  void remove(Grid &dataGrid, const double missingValue) const;

  /**
   * Set a scaling Grid.  The scale is set to 1.0 at all grid points except for
   * points that are part of clumps that have been marked for remove. 
   * For those points, the minimum data value in that clump is mapped to
   * a scale using the input fuzzy function.  That is the scale that is set
   * for all points in the clump.
   *
   * @param[in] valueToScale  Function from data values to a number in
   *                          range [0,1]
   * @param[in,out] scaleGrid  The grid in which to put the scale values
   *
   */
  void rescale(const ConvWxFuzzy &valueToScale, Grid &scaleGrid) const;

protected:
private:

  /**
   * The clumps
   */
  std::vector<NbrhdClump> pClump;

  /**
   * Iterator to the current clump
   */
  std::vector<NbrhdClump>::reverse_iterator pIclump;

  /**
   * Pointers to those clumps marked for remove
   */
  std::vector<const NbrhdClump *> pRemoveClump; 

  /**
   * @return The single NbrhdClump gotten by merging all clumps indicated
   * by the input indices.  Remove the clumps that were merged from pClump
   *
   * @param[in] clumps  Clump index values
   *
   * @note The resultant merged clump is given the index value equal to the
   *       minimum of all the input index values
   *
   * @note The returned pointer is to something in the state, so don't
   *       delete it
   */
  const NbrhdClump *pMergedClumps(const std::vector<int> &clumps);

  /**
   * @return Pointer to the clump with the input index value, or NULL if none
   *
   * @param[in] clumpValue  Index to look for in internal state
   *
   * @note The returned pointer is to something in the state, so don't
   *       delete it
   */
  const NbrhdClump *pMatchingClump(const int clumpValue) const;

  /**
   * Merge points from the local clump that has an index value into a clump
   * and remove that clump whose points were merged from the local state
   *
   * @param[in] value  A clump index value
   * @param[in,out] nnew  Object to merge into
   */
  void pMergePointsAndRemove(double value, NbrhdClump &nnew);

  /**
   * At all points within a clump, set a scale value by mapping the
   * minimum data value in that clump to a scale using the input fuzzy
   * function.  That is the scale that is set into the scaleGrid for all
   * points in the clump.
   *
   * @param[in] valueToScale  Function from data values to a number in
   *                          range [0,1]
   * @param[in] clump  The clump to use
   * @param[in,out] scaleGrid  The grid in which to put the scale values
   *
   */
  void pRescale(const ConvWxFuzzy &valueToScale, const NbrhdClump &clump, 
		Grid &scaleGrid) const;
};

# endif 
