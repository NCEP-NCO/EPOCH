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
 * @file EarthRadius.hh
 * @brief  Static functions that give Low Level interface to infrastructure
 * software that is not part of ConvWx for the radius of the earth, allowing
 * it to be modified.
 * @class EarthRadius
 * @brief  Static functions that give Low Level interface to infrastructure
 * software that is not part of ConvWx for the radius of the earth, allowing
 * it to be modified.
 *
 * @note These are the functions that are implemented differently for different
 *       implementations
 */
# ifndef    EARTH_RADIUS_HH
# define    EARTH_RADIUS_HH


//----------------------------------------------------------------
class EarthRadius
{
public:

  /**
   * This is a placeholder to support a proper link
   */
  static void init(void);

  /**
   * override the default earth radius in km
   * @param[in] earthRadiusKm   The new radius to use 
   */
  static void setEarthRadiusKm(double earthRadiusKm);

protected:
private:  

};

# endif
