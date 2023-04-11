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
 * @file ContingencyTableWindowed.hh
 * @brief Manages windowed contingencytable information and performs actions
 *        on MultiGrid data
 * @class ContingencyTableWindowed
 * @brief Manages windowed contingencytable information and performs actions
 *        on MultiGrid data
 *
 */

# ifndef    CONTINGENCY_TABLE_WINDOWED_HH
# define    CONTINGENCY_TABLE_WINDOWED_HH

#include <ConvWx/ContingencyTableWindowedInfo.hh>
#include <vector>

class Grid;
class MultiGrid;
class MultiFcstGrid;

//----------------------------------------------------------------
class ContingencyTableWindowed
{
public:
  
  /**
   * Constructor, does nothing
   */
  ContingencyTableWindowed(void);

  /**
   * Construct from XML by parsing to set state
   * @param[in] xml  The string that should contain XML content for state
   */
  ContingencyTableWindowed(const std::string &xml);

  /**
   * Construct from XML found in the input MultiFcstGrid metadata,
   * making sure the in the input contain the fields that are expected
   * based on this metadata
   *
   * @param[in] mg  Grids that should contain fields associated with
   *                the windowed information found in the metadata
   * @param[in] counts  True if input should contain counts for hits, misses,
   *                    false positives and nones for all fields in the
   *                    metadata, false if input should contain ContingencyTable
   *                    enumerated values for all fields in metadata.
   */
  ContingencyTableWindowed(const MultiFcstGrid &mg, const bool counts);

  /**
   * Destructor
   */
  virtual ~ContingencyTableWindowed(void);

  /**
   * Add information for one windowed contingency table to state
   * @param[in] table
   */
  void add(const ContingencyTableWindowedInfo &table);

  /**
   * @return number of windowed contingency tables
   */
  inline int num(void) const {return static_cast<int>(pTable.size());}

  /**
   * @return reference to the information for one table
   * @param[in] index  Index into the tables
   */
  inline const ContingencyTableWindowedInfo &getInfo(const int index) const
  {
    return pTable[index];
  }

  /**
   * @return pointer to a 'hits' grid in a multigrid, NULL for none
   * @param[in] index  Index to windowed information
   * @param[in] mg  Grids that should contain counts for all windowed tables
   */
  const Grid *ithHitGridPtr(const int index, const MultiGrid &mg) const;

  /**
   * @return pointer to a 'misses' grid in a multigrid, NULL for none
   * @param[in] index  Index to windowed information
   * @param[in] mg  Grids that should contain counts for all windowed tables
   */
  const Grid *ithMissGridPtr(const int index, const MultiGrid &mg) const;

  /**
   * @return pointer to a 'false positive' grid in a multigrid, NULL for none
   * @param[in] index  Index to windowed information
   * @param[in] mg  Grids that should contain counts for all windowed tables
   */
  const Grid *ithFalsepGridPtr(const int index, const MultiGrid &mg) const;

  /**
   * @return pointer to a 'none' grid in a multigrid, NULL for none
   * @param[in] index  Index to windowed information
   * @param[in] mg  Grids that should contain counts for all windowed tables
   */
  const Grid *ithNoneGridPtr(const int index, const MultiGrid &mg) const;

  /**
   * Create and return bias grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] bias  The grid
   *
   * @return true if could access the counts and produce the bias
   */
  bool ithBias(const int index, const MultiGrid &mg, Grid &bias) const;

  /**
   * Create and return CSI grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] csi  The grid
   *
   * @return true if could access the counts and produce the csi
   */
  bool ithCSI(const int index, const MultiGrid &mg, Grid &csi) const;

  /**
   * Create and return HSS grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] hss  The grid
   *
   * @return true if could access the counts and produce the HSS
   */
  bool ithHSS(const int index, const MultiGrid &mg, Grid &hss) const;

  /**
   * Create and return FAR grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] far  The grid
   *
   * @return true if could access the counts and produce the FAR
   */
  bool ithFAR(const int index, const MultiGrid &mg, Grid &far) const;

  /**
   * Create and return POD grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] pod  The grid
   *
   * @return true if could access the counts and produce the POD
   */
  bool ithPOD(const int index, const MultiGrid &mg, Grid &pod) const;

  /**
   * Create and return PODno grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] podNo  The grid
   *
   * @return true if could access the counts and produce the PODno
   */
  bool ithPODno(const int index, const MultiGrid &mg, Grid &podNo) const;

  /**
   * Create and return Obs Frequency grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] obsFreq  The grid
   *
   * @return true if could access the counts and produce the obsFreq
   */
  bool ithObsFreq(const int index, const MultiGrid &mg, Grid &obsFreq) const;

  /**
   * Create and return Fcst Frequency grid for one of the windowed tables
   *
   * @param[in] index  Index into windowed tables
   * @param[in] mg  Grids that should contain counts for all windowed tables
   * @param[out] fcstFreq  The grid
   *
   * @return true if could access the counts and produce the fcstFreq
   */
  bool ithFcstFreq(const int index, const MultiGrid &mg, Grid &fcstFreq) const;

  /**
   * Form and return an XML string representing the state
   * @return  The XML string
   */
  std::string metadata(void) const;

  /**
   * @return grids to hold counts for hits, misses, false positives and none
   * for each of the fields indicated by the local object, i.e. each windowed
   * table
   *
   * @param[in] nx  Grid dimension
   * @param[in] ny  Grid dimension
   */
  MultiGrid createCounterGrids(const int nx, const int ny) const;

  /**
   * Accumulate into hit/miss/falsep/none counts grids using input data grids.
   *
   * @param[in] mg  Data grid with assumed ContingencyTable enumerated values,
   *                one grid per pTable entry
   * @param[in,out] sum  Grids in which counter fields exist for each pTable
   *                     entry for hits, misses, false positives, and nones
   */
   void accumulate(const MultiGrid &mg, MultiGrid &sum) const;

  /**
   * Accumulate into hit/miss/falsep/none counts grids using input data grids,
   * with a weight
   *
   * @param[in] mg  Data grid with assumed ContingencyTable enumerated values,
   *                one grid per pTable entry
   * @param[in] weight  Weight
   * @param[in,out] sum  Grids in which weighted count fields exist for each
   *                     pTable entry for hits, misses, false positives,
   *                     and nones
   * @param[in,out] counts  Grids in which counter fields exist for each pTable
   *                     entry for hits, misses, false positives, and nones
   */
  void accumulateWeighted(const MultiGrid &mg, double weight, 
			  MultiGrid &sum, MultiGrid &counts) const;

  /**
   * Filter so that instances where the sum of hit/miss/false/none counts
   * for a pTable is less than a percentage of the total number these counts
   * are set to missing data
   *
   * @param[in] num  Total number
   * @param[in] minp  Min percent allowed
   * @param[in,out] sum Grids in which counter fields exist for each pTable
   *                    entry for hits, misses, false positives, and nones
   */
  void filter(const int num, const double minp, MultiGrid &sum) const;

  /**
   * Filter so that instances where the sum of hit/miss/false/none counts
   * for a pTable is less than a percentage of the total number these counts
   * are set to missing data in the weighted sums
   *
   * @param[in] num  Total number
   * @param[in] minp  Min percent allowed
   * @param[in]  counts Grids in which counter fields exist for each pTable
   *                    entry for hits, misses, false positives, and nones
   * @param[in,out] sum Grids in which weighted count fields exist for each
   *                    pTable entry for hits, misses, false positives,
   *                    and nones
   */
  void filterWeighted(const int num, const double minp, const MultiGrid &counts,
		      MultiGrid &sum) const;

protected:
private:  

  /**
   * Contingency table info for all instances
   */
  std::vector<ContingencyTableWindowedInfo> pTable;

  /**
   * Set state using input string which should have XML for state
   * @param[in] xml   String
   * @return true if could parse input string successfully
   */
  bool pSetFromXml(const std::string &xml);

};

# endif     // ContingencyTableWindowed_HH
