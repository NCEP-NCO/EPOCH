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
 * @file Merge.hh
 * @brief merging of integer indexed items.
 * @class Merge
 * @brief merging of integer indexed items.
 *
 * For the case where there are some number of indexed items, 
 * 0,1,2,... and subsets of which can be associated. The associations
 * are initialized one pair at a time using the update() method, then
 * merging combines all the values that can be combined.  
 * This leads to a smaller set of* merged items, each which has 1 or more
 * original index values. As an example
 *
 * -  m = Merge(10):  10 items, none can be merged
 * -  m.update(3,4)   3 and 4 are associated
 * -  m.update(7,9)   7 and 9 are associated
 * -  m.merge();
 * -  m.numMerged() = 8
 * -  m.getIthMerged(0) = <0>
 * -  m.getIthMerged(1) = <1>
 * -  m.getIthMerged(2) = <2>
 * -  m.getIthMerged(3) = <3,4>
 * -  m.getIthMerged(4) = <5>
 * -  m.getIthMerged(5) = <6>
 * -  m.getIthMerged(6) = <7,8>
 * -  m.getIthMerged(7) = <9>
 * -  m.update(3,9)   associate 3 and 9
 * -  m.merge();      re-merge, this should make 3,4,7,9 all associate
 * -  m.numMerged() = 7
 * -  m.getIthMerged(0) = <0>
 * -  m.getIthMerged(1) = <1>
 * -  m.getIthMerged(2) = <2>
 * -  m.getIthMerged(3) = <3,4,7,8>
 * -  m.getIthMerged(4) = <5>
 * -  m.getIthMerged(5) = <6>
 * -  m.getIthMerged(6) = <9>
 */

# ifndef    MERGE_H
# define    MERGE_H

#include <vector>
#include <string>

class Merge 
{      
public:

  /**
   * Merge constructor for empty
   */
  Merge(void);

  /**
   * Merge constructor for the case of a fixed set of indexed items
   *
   * @param[in] numItems  The number of indexed items.
   *
   * @note the items should fall in range, 0,1,..,n-1
   */
  Merge(const int numItems);

  /**
   * Destructor
   */
  virtual ~Merge(void);

  /**
   * print out the indexed lists
   */
  void print(void) const;

  /**
   * print out the merged lists
   */
  void printMerged(void) const;

  /**
   * Clear to empty
   */
  void clear(void);

  /**
   * Associate two index values
   *
   * @param[in] index1  One index
   * @param[in] index2  The other index
   */
  void update(const int index1, const int index2);

  /**
   * Do the merging algorithm that reduces sets to maximally merged.
   */
  void merge(void);

  /**
   * @return number of merged items
   */
  int numMerged(void) const;

  /**
   * return the i'th set of index values after merging
   *
   * @param[in] i index
   *
   * @return vector with the merged index values
   */
  std::vector<int> getIthMerged(const int i) const;

protected:
private:

  /**
   * Two dimensional square matrix of associations
   *  - pR[i][j] = true if jth item is part of i'th set
   *
   * @note pR dimensions are pN-1 both first and second.
   * @note vector<bool> does not work, according to our coding standards
   */
  bool **pMatch;

  /**
   * Number of items
   */
  int pNumMatch;

  /**
   * Merged items after merging
   */
  std::vector<std::vector<int> > pMerged;

  /**
   * Add a row to the state
   *
   * @param[in] i  The row index
   * @param[in,out] done  Array status for all rows
   * @param[in,out] v  Index vector to add to 
   */
  void pAddRow(const int i, bool *done, std::vector<int> &v) const;

};

# endif
