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
 * @file BlendStatus.hh
 * @brief Status of blending data at a lead time
 * @class BlendStatus
 * @brief Status of blending data at a lead time
 *
 */

#ifndef BLEND_STATUS_H
#define BLEND_STATUS_H

//----------------------------------------------------------------
class BlendStatus
{
public:

  /**
   * Status values
   */
  typedef enum {ERROR, NONE, ALL_MISSING, GOOD} Status_t;

  /**
   * Constructor , no lead time and status = NONE
   */
  inline BlendStatus(void) : pLeadSeconds(-1), pStatus(NONE)  {}
  
  /**
   * Constructor 
   * @param[in] lt  Lead time
   */
  inline BlendStatus(int lt) : pLeadSeconds(lt), pStatus(NONE)  {}


  /**
   * Destructor
   */
  inline virtual ~BlendStatus (void) {}

  /**
   * @return true if the lead time has been processed
   */
  inline bool processed(void) const {return pStatus != NONE;}

  /**
   * @return lead seconds
   */
  inline int getLeadSeconds(void) const {return pLeadSeconds;}

  /**
   * Set state based on availability of data
   *
   * @param[in] available  True if data is available at the lead time,
   *                       false if not
   */
  inline void initialize(bool available)
  {
    if (available)
    {
      pStatus = GOOD;
    }
    else
    {
      pStatus = NONE;
    }
  }

  /**
   * Set state to NONE
   */
  inline void setNone(void) { pStatus = NONE;}

  /**
   * Increment input counter unless status is GOOD
   * @param[in,out] nmissing
   */
  inline void incCounters(int &nmissing) const
  {
    if (pStatus != GOOD)
    {
      nmissing++;
    }
  }

  /**
   * Increment counters for output based on local state
   *
   * @param[in,out] nmissing incremented with ERROR and NONE and ALL_MISSING
   * @param[in,out] nerror  incremented with ERROR and NONE
   */
  inline void incOutputCounters(int &nmissing, int &nerror) const
  {
    if (pStatus == ERROR || pStatus == NONE)
    {
      nmissing++;
      nerror++;
    }
    else if (pStatus == ALL_MISSING)
    {
      nmissing++;
    }
  }

  /**
   * @return true if status is GOOD
   */
  inline bool isGood(void) const {return pStatus == GOOD;}
  
  /**
   * @return true if input lead time matches local value
   * @param[in] lt  Lead time
   */
  inline bool matchingLead(int lt) const {return lt == pLeadSeconds;}

  inline void setStatusAllMissing(void) {pStatus = ALL_MISSING; }
  inline void setStatusGood(void) {pStatus= GOOD; }
  inline void setStatusNone(void) {pStatus = NONE; }
  inline void setStatusError(void) {pStatus = ERROR; }

protected:
private:

  int pLeadSeconds;     /**< Lead time seconds */
  Status_t pStatus;     /**< Status value */

};

#endif
