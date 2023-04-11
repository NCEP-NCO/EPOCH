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
 * @file LtInterpPoint.hh
 * @brief Information about one of two lead times that are interpolated between
 * @class LtInterpPoint
 * @brief Information about one of two lead times that are interpolated between
 *
 * Public members intentional (struct like class).
 * This is a simple class with a lead time and a weight and that is it.
 */

#ifndef LT_INTERP_POINT_HH
#define LT_INTERP_POINT_HH

#include <string>


class LtInterpPoint
{
 public:

  /**
   * Empty constructor (no lead time or weight, sets pOk to false)
   */
  LtInterpPoint(void);

  /**
   * Normal constructor, sets the two member values
   *
   * @param[in] lt  Lead time seconds
   * @param[in] weight  Weight
   */
  LtInterpPoint(int lt, double weight);

  /**
   * Destructor
   */
  virtual ~LtInterpPoint(void);

  /**
   * @return a diagnostic string for the interpolation object
   */
  std::string ltString(void) const;

  /**
   * @return a different diagnostic string for the interpolation object
   */
  std::string ltString2(void) const;

  /**
   * Parse a string created using ltString2(), and create and
   * return a LtInterpPointObject based on that
   *
   * @return true for success
   * @param[in] s  String to parse
   * @param[out] point  The created object
   * @param[out] last = index to last string position that was parsed
   */
  static bool parse2(const std::string &s, LtInterpPoint &point, int &last);

  bool pOk;         /**< true if the item has a lead and weight set */
  int pLeadSeconds; /**< Lead time seconds */
  double pWeight;   /**< Weight */

protected:
private:

};


#endif
