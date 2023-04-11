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
 * @file LpcModelState.hh 
 * @brief a struct-like class that holds a lead time and an
 *        availability status, used for bookkeeping of various model runs
 *        for the Lagrangian Phase Correction algorithm
 *        
 * @class LpcModelState
 * @brief a struct-like class that holds a lead time and an
 *        availability status, used for bookkeeping of various model runs
 *        for the Lagrangian Phase Correction algorithm
 *
 */
#ifndef LPC_MODEL_STATE_H
#define LPC_MODEL_STATE_H

//----------------------------------------------------------------
class LpcModelState
{
public:
  /**
   * Constructor, set members using inputs
   * @param[in] lt  Lead time seconds
   * @param[in] available  True if data for the lead is available 
   */
  LpcModelState(const int lt, const bool available);

  /**
   * Destructor
   */
  virtual ~LpcModelState(void);

  /**
   * @return A string description of the local state
   */
  std::string sprint(void) const;
  
  /** 
   * Set the local availability state to input
   *
   * @param[in] isAvailable  True if data is presumed available
   */
  void setAvailability(const bool isAvailable);

  /**
   * @return The availability (true if available)
   */
  inline bool available(void) const {return pModelAvailable;}

protected:
private:

  
  int pModelLtSeconds;  /**< Lead time seconds */
  bool pModelAvailable; /**< Available true or false */
};

#endif
