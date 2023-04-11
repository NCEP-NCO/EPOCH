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
 * @file DebugPoint.hh 
 * @brief Allow debugging at a particular Grid point for particular lead times
 * @class DebugPoint
 * @brief Allow debugging at a particular Grid point for particular lead times
 */

#ifndef DEBUG_POINT_H
#define DEBUG_POINT_H

//----------------------------------------------------------------
class DebugPoint
{
public:
  /**
   * Empty constructor (no debugging)
   */
  DebugPoint(void);

  /**
   * Destructor
   */
  virtual ~DebugPoint(void);

  /**
   * Activate debugging at the input lead times at a single grid index.
   *
   * @param[in] leadSeconds  The Lead times (seconds) at which to show debug
   *                         information
   * @param[in] x  The grid index (X) at which to show debug information
   * @param[in] y  The grid index (Y) at which to show debug information
   */
  void set(const std::vector<int> &leadSeconds, const int x, const int y);

  /**
   * Activate debugging at a single grid index for all lead times
   * (or for data in which there is not a lead time)
   *
   * @param[in] x  The grid index (X) at which to show debug information
   * @param[in] y  The grid index (Y) at which to show debug information
   */
  void set(const int x, const int y);

  /**
   * @return true if the input is a point/lead time at which debugging is
   *         indicated
   *
   * @param[in] ltSeconds  A lead time (seconds)
   * @param[in] x  Grid index x
   * @param[in] y  Grid index y
   *
   * @note if no lead times are specified in the state, ignores the lead
   *       time input
   */
  bool isDebug(const int ltSeconds, const int x, const int y) const;

  /**
   * @return true if input is a point at which debugging is indicated
   * @param[in] x  Grid index x
   * @param[in] y  Grid index y
   *
   * @note if lead times are specified in the state, returns false
   */
  bool isDebug(const int x, const int y) const;

protected:
private:

  std::vector<int> pLeadTime; /**< Lead times at which show debugging */
  int pX;                     /**< X grid index at which to show debugging */
  int pY;                     /**< Y grid index at which to show debugging */

  /**
   * True if debugging has been activated.
   */
  bool pDebug;

  /**
   * True if lead times should be checked. If false, only the grid point
   * index is relevant and the lead times are not checked.
   */
  bool pLeadTimeCheck;

};

#endif
