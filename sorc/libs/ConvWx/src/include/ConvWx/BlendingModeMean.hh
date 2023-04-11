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
 * @file BlendingModeMean.hh
 * @brief  Allowed modes for mean blending
 * @class BlendingModeMean
 * @brief  Allowed modes for mean blending
 *
 * This is a class with static members only (no internal state).
 *
 */

# ifndef    BLENDING_TYPE_MEAN_HH
# define    BLENDING_TYPE_MEAN_HH
#include <string>
#include <vector>

//----------------------------------------------------------------
class BlendingModeMean
{
public:

  /**
   * @enum Mode_t
   * @brief The types of blending
   */
  typedef enum
  {
    UNKNOWN=-1,      /**< Type is not known */
    WEIGHTS = 0,     /**< blended with weights input for extrap and model */
    INITIATION_PROB=1, /**< WEIGHTS, plus initiation probability field */
    INITIATION_MASK=2, /**< WEIGHTS, plus initiation mask field */
    INITIATION_PROB_AND_MASK=3 /**< WEIGHTS, plus initiation mask field, 
				* and initiation probability field */
  } Mode_t;


  /**
   * Set and the mode based on inputs.
   * @param[in] extrapWeightInput  True if extrap weight input is expected
   * @param[in] modelWeightInput  True if extrap weight input is expected
   * @param[in] initiationProbInput  True if inititation probability input
   *                                 is expected
   * @param[in] initiationMaskInput  True if inititation mask input
   *                                 is expected
   * @param[out] mode   The mode which is returned
   * @return true if consistent inputs and mode is returned, false for 
   * inconsistent input values
   */
  static bool setMode(bool extrapWeightInput, bool modelWeightInput,
		      bool initiationProbInput, bool initiationMaskInput,
		      Mode_t &mode);

  /**
   * @return true if the  mode is one that has an initiation probability
   *         input
   * @param[in] m  The mode
   */
  inline static bool hasInitiationProb(Mode_t m)
  {
    return m == INITIATION_PROB || m == INITIATION_PROB_AND_MASK;
  }

  /**
   * @return true if the  mode is one that has an initiation mask input
   * @param[in] m  The mode
   */
  inline static bool hasInitiationMask(Mode_t m)
  {
    return m == INITIATION_MASK || m == INITIATION_PROB_AND_MASK;
  }

  /**
   * @return true if the  mode is one that has an initiation mask input
   * @param[in] m  The mode
   */
  inline static bool hasCiFlag(Mode_t m)
  {
    return m == INITIATION_MASK || m == INITIATION_PROB_AND_MASK;
  }

protected:
private:  
};

# endif     // BlendingModeMean_HH
