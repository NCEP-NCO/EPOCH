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
 * @file VerifNames.hh 
 * @brief Standard names for verification entities
 * @class VerifNames
 * @brief Standard names for verification entities
 *
 * The verification entities are hits, misses, false positives and nulls.
 * Each is given a fixed name string.
 *
 * Static methods/members only.
 *
 */

#ifndef VERIF_NAMES_H
#define VERIF_NAMES_H
#include <string>

//----------------------------------------------------------------
class VerifNames
{
public:

  /**
   * @return fixed 'hit' name
   */
  inline static std::string hitName(void) {return pHitName;}

  /**
   * @return fixed 'miss' name
   */
  inline static std::string missName(void) {return pMissName;}

  /**
   * @return fixed 'false positive' name
   */
  inline static std::string falsepName(void) {return pFalsepName;}

  /**
   * @return fixed 'null' name
   */
  inline static std::string nullName(void) {return pNullName;}

protected:
private:

  static const std::string pHitName;   /**< fixed 'hit' name */
  static const std::string pMissName;  /**< fixed 'miss' name */
  static const std::string pFalsepName;/**< fixed 'false positive' name */
  static const std::string pNullName;  /**< fixed 'null' name */
};

#endif
