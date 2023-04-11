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
 * @file  ContingencyTable.hh
 * @brief  The possible results of comparing a forecast and verifying truth
 * @class  ContingencyTable
 * @brief  The possible results of comparing a forecast and verifying truth
 *
 * The possibilities are 'hit', 'miss', 'false positive', 'correct none'.
 * This simple class contains only enumerated values for these, and a static
 * method to convert from the enumerated value to a string
 *
 * This class has only static members, no state.
 */

# ifndef    CONTINGENCY_TABLE_HH
# define    CONTINGENCY_TABLE_HH
#include <string>

//----------------------------------------------------------------
class ContingencyTable
{
public:

  /**
   * @enum Type_t 
   * @brief The results of comparing observations and forecasts.
   */
  typedef enum
  {
    MISSING_VALUE = -1, /**< Missing observation or forecast data */
    UNKNOWN = 0,        /**< An error */
    NONE = 1,           /**< Observation <  threshold,  forecast <  threshold */
    FALSEP = 2,         /**< Observation <  threshold,  forecast >= threshold */
    MISS = 3,           /**< Observation >= threshold, forecast <  threshold */
    HIT = 4             /**< Observation >= threshold, forecast >= threshold */
  } Type_t;

  /**
   * @return A string describing a particular type
   * @param[in] type  The type to describe
   */
  static std::string descriptor(const ContingencyTable::Type_t type);

protected:
private:  

};

# endif
