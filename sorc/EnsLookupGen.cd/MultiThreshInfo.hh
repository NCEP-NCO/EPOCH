/**
 * @file MultiThreshInfo.hh
 * @brief The information about the thresholds coming from multiple databases
 *        and fixed thresholds
 * @class MultiThreshInfo
 * @brief The information about the thresholds coming from multiple databases
 *       and fixed thresholds
 *
 * One database or fixed threshold per input field
 * One set of thresholds per obar threshold
 */

#ifndef MultiThreshInfo_hh
#define MultiThreshInfo_hh

#include "DbThresh.hh"
#include "ParmsEnsLookupGen.hh"
#include <vector>

class MultiThreshInfo
{
public:
  /**
   * Constructor, 
   * @param[in] parms
   */
  MultiThreshInfo(const ParmsEnsLookupGen &parms);

  /**
   * Destructor
   */
  inline ~MultiThreshInfo(void) {}

  /**
   * Update state when a new gen time triggers
   * @param[in] genTime
   */
  void newGenTime(const time_t &genTime);

  /**
   * Prepare output by appending grids and metadata XML to inputs
   *
   * @param[in,out] mOutGrid  Grids
   * @param[in,out] xml  Metadata XML
   */
  void prepareOutput(//MultiGrid &mOutGrid,
		     std::string &xml) const;


  /**
   * @return reference to the vector member
   */
  inline const std::vector<DbThresh> & getDbThresh(void) const {return _dbThresh;}

protected:
private:

  /**
   * Params
   */
  ParmsEnsLookupGen _parms;   

  /**
   * Inputs.
   * Containers for database and fixed threshold,
   * one per field, each field can have different obar thresholds
   */
  std::vector<DbThresh> _dbThresh; 

  /**
   * Current time for database, which is supposed to synchronize
   * between multiple databases
   */
  time_t _dbGenTime;
};


#endif
