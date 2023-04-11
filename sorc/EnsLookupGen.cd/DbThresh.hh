/**
 * @file DbThresh.hh
 * @brief The information about the thresholds coming from one database
 * @class DbThresh
 * @brief The information about the thresholds coming from one database
 *
 * Also fixed threshold fields
 */

#ifndef DbThresh_hh
#define DbThresh_hh

#include "ParmsEnsLookupGen.hh"
#include <Epoch/SpdbGenBasedThreshHandler.hh>
#include <Epoch/ThresholdDatabaseParms.hh>
#include <euclid/Grid2d.hh>
#include <vector>
#include <string>

class DbThresh
{
public:


  /**
   * Constructor
   * @param[in] fieldParm  Params for this one field
   * @param[in] parm  Full param set
   */
  DbThresh(const ThresholdDatabaseParms &fieldParm, const ParmsEnsLookupGen &parm);

  /**
   * Destructor
   */
  inline ~DbThresh(void) {}

  /**
   * Update state due to a change in gen time
   * @param[in] genTime
   * @param[out] gt the gen time that was gotten from the database
   * @return true if successful
   */
  bool newGenTime(const time_t &genTime, time_t &gt);

  /**
   * Create and return a thresholds grid, either from the database, or
   * all one value if it is a fixed threshold
   * @param[in] gt Not used
   * @param[in] lt  Lead seconds, used to access data base
   * @param[in] oparms  Used if the local state is for the output obar.
   *                    This is confusing, need to debug.
   * @param[out] grid  Returned grid
   * @param[out] doOutput   set to true if the returned grid should be output
   */
  void createThresholdGrid(const time_t &gt, int lt,
			   const ObarThreshParms &oparms,
			   Grid2d &grid,  bool &doOutput) const;


  /**
   * append information from local state to XML
   * @param[in,out] xml
   */
  void prepareOutput(std::string &xml) const;

protected:
private:
  ParmsEnsLookupGen _parms;   /**< Params */
  ThresholdDatabaseParms _dbParms;  /**< Parms for this field */
  SpdbGenBasedThreshHandler _spdb;  /**< The database handling object */

   /**
    * True if this is coldstart (no threshold mapping in SPDB)
    */
   bool _coldstart;

  /**
   * The chunk time from the thresholds mapping
   */
  time_t _threshChunkTime;

  Grid2d _expandedGrid(const Grid2d &g, double outsideThresh) const;
};


#endif
