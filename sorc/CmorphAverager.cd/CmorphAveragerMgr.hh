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
 * @file CmorphAveragerMgr.hh
 * @brief 
 * 
 * @class CmorphAveragerMgr
 * @brief
 */

#ifndef CmorphAveragerMgr_HH
#define CmorphAveragerMgr_HH

#include "ParmsCmorphAverager.hh"

class CmorphAveragerMgr
{
  
public:
  
  /**
   * Constructor 
   * Initialize data members, set up archive or real-time triggering based on
   * user defined parameters. Log the start time of the application. Exit if
   * driver does not initialize properly.
   *
   * @param[in] params  The user defined algorithm parameters
   * @param[in] cleanExit  Cleanup function to call at exit: Free memory, 
   *                       inform monitoring processes that program is 
   *                       terminating, return signal to operating system
   */
  CmorphAveragerMgr (const ParmsCmorphAverager &params, void cleanExit(int) );
  
  /**
   *  Destructor
   */
  ~CmorphAveragerMgr(void);
  
  /**
   * Run the application.
   * @return true or false
   */ 
  bool run(void) const;
  
protected:
  
private:
  
  /**
   *  User defined parameters
   */
  ParmsCmorphAverager  pParams;

  /**
   *  Load observations at this time. Static calibration may be applied.
   *  @param[in] obsTime  Observation time
   *  @param[in] data  Grid to hold observation data
   *  @return true or false
   */
  bool pLoadObs(const time_t obsTime, Grid &data) const;

  /**
   *  Write observation data to observation output URL.
   *  @param[in] validTime  Observation data time
   *  @param[in] grid  Reference to FcstGrid object containing observation 
   *                    data
   */ 
  void pWriteObs(const time_t validTime, Grid &grid) const;
};

#endif

