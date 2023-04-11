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
 * @file VerifThresh.hh 
 * @brief Thresholds for verification related activities.
 * @class VerifThresh 
 * @brief Thresholds for verification related activities.
 * 
 * The representation of names at a threshold can be real valued or 
 * integer valued as regards the threshold. 
 * This class exists for backward compatibility for when all fields
 * being verified were integer valued.
 * In particular, two string representations of a threshold exist, one
 * for integer values one for floating point
 *
 * This class has been extended to allow any precision in the floating point
 * representation. The default is 2 digits, i.e.:  0.05.
 */

#ifndef VERIF_THRESH_H
#define VERIF_THRESH_H
#include <string>

//----------------------------------------------------------------
class VerifThresh
{
public:

  /**
   * Constructor
   *
   * @param[in] isIntegerValued  True if threshold is integer valued
   * @param[in] threshold  The threshold (if isIntegerValued it is truncated).
   * @param[in] digitsPrecision   The number of digits in the floating
   *                              point representation when non-integer valued
   */
  VerifThresh(const bool isIntegerValued, const double threshold,
	      const int digitsPrecision=2);

  /**
   * Destructor
   */
  virtual ~VerifThresh(void);

  /**
   * Check for equality
   * @param[in] v
   */
  bool operator==(const VerifThresh &v) const;

  /**
   * @return the field name associated with the input and local state
   *
   * @param[in] name  A name to use in the field name
   * @note returns a string with the input name and the threshold appended
   */
  std::string fieldName(const std::string &name) const;

  /**
   * @return the local threshold
   */
  inline double getThresh(void) const {return pThresh;}

protected:
private:

  bool pIsIntegerValued; /**< True if integer 'mode' */
  double pThresh;        /**< The threshold */
  int pDigitsPrecision;  /**< Number of digits in precision when not integers */
};

#endif
