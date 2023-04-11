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
 * @file LpcOutputState.hh 
 * @brief a struct like class for with three fields helping give status for
 *        Lagrangian Phase Correction forecast data.
 *        
 * @class LpcOutputState
 * @brief a struct like class for with three fields helping give status for
 *        Lagrangian Phase Correction forecast data.
 *
 * The three fields are:
 *   - lead time seconds
 *   - processed yes or no
 *   - all missing data yes or no
 */

#ifndef LPC_OUTPUT_STATE_H
#define LPC_OUTPUT_STATE_H

//----------------------------------------------------------------
class LpcOutputState
{
public:
  /**
   * Constructor for a lead time.
   * Sets the 'all missing' and 'is processed' flags to false, and
   * sets the lead time to the input.
   *
   * @param[in] lt  Lead time seconds
   */
  LpcOutputState(const int lt);

  /**
   * Destructor
   */
  virtual ~LpcOutputState(void);

  /**
   * @return  A string description of the local state
   */
  std::string sprint(void) const;

  /** 
   * Set state for 'all missing' to true
   */
  inline void setMissing(void) {pAllMissing = true;}

  /**
   * Set state for 'processed' to true
   */
  inline void setProcessed(void) {pProcessed = true;}

  /**
   * @return true if state indicates 'all missing'
   */
  inline bool allMissing(void) const {return pAllMissing;}

  /**
   * @return true if state indicates 'processed'
   */
  inline bool isProcessed(void) const {return pProcessed;}

protected:
private:

  int pLtSeconds;    /**< Lead time (seconds) */
  bool pAllMissing;  /**< All missing flag */
  bool pProcessed;   /**< Processed flag */
};

#endif
