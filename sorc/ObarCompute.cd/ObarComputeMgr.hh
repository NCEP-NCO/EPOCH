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
 * @file ObarComputeMgr.hh
 * @brief Driver for the algorithm
 * @class ObarComputeMgr
 * @brief Driver for the algorithm
 *
 * Reads obs data, computes obar on tiles at any number of thresholds,
 * write to SPDB
 */
#ifndef ObarComputeMgr_hh
#define ObarComputeMgr_hh


#include "ParmsObarComputeIO.hh"
#include "ObarForEachThresh.hh"
#include <Epoch/SpdbObsHandler.hh>
#include <string>

class Grid;
class TileRange;

class ObarComputeMgr
{
public:
  /**
   * Constructor
   * @param[in] parms
   * @param[in] tidyAndExit
   */
  ObarComputeMgr(const ParmsObarComputeIO &parms, void tidyAndExit(int));

  /**
   * Destructor
   */
  ~ObarComputeMgr(void);

  /**
   * Run the app
   */
  void run(void);

protected:
private:

  ParmsObarComputeIO _parms;   /**< Params */
  time_t _obsTime;             /**< Current obs time */
  SpdbObsHandler _spdb;        /**< database object */
  ObarForEachThresh _obarInfo;   /**< Storage for local changes */

  bool _motherFail;            /**< True if mother tile could not be computed */
  bool _motherSet;             /**< True if _motherInfo is set */
  TileObarInfo _motherInfo;    /**< Information for the mother tile */

  void _process(const time_t &obsTime);
  void _processTiles(const Grid &obsGrid);
  TileObarInfo _setupAndRunAlg(const Grid &obsGrid, int tileIndex,
			       int threshIndex, bool isMotherTile);
  bool _obsSetup(const TileRange &r, double thresh, const Grid &obsGrid, double &oBar);
  TileObarInfo _setToMotherOrColdstart(int tileIndex, bool isMotherTile);
};


#endif
