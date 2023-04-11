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
 * @file ConvWxPjgLatlonMath.hh
 * @brief  Low-level math for Latitude longitude projection
 *
 * @class ConvWxPjgLatlonMath
 * @brief  Low-level math for Latitude longitude projection
 *
 * @note Do not include this file directly. Include only the base class file,
 *       ConvWxPjgMath.hh
 */

#ifndef CONVWX_PJG_LATLON_MATH_HH
#define CONVWX_PJG_LATLON_MATH_HH

#include <ConvWx/ConvWxPjgMath.hh>

class ConvWxPjgLatlonMath : public ConvWxPjgMath
{

public:

  /**
   * Constructor
   *
   * @param[in] earthRadiusKm  Radius of earth in km
   * @param[in] originLon  Longitude of origin (degrees)
   */
  ConvWxPjgLatlonMath(double earthRadiusKm, double originLon = 0.0);
		      

  /**
   * Destructor
   */
  virtual ~ConvWxPjgLatlonMath(void);
  
  /**
   * Convert from latitude/longitude (degrees) to x,y (km) 
   *
   * @param[in] lat  Latitude degrees
   * @param[in] lon  Longitude degrees
   * @param[out] x  Kilometers X
   * @param[out] y  Kilometers Y
   */
  virtual void latlon2xy(double lat, double lon, double &x, double &y) const;

  /**
   * Convert from x,y (km)  to latitude/longitude (degrees)
   *
   * @param[in] x  Kilometers X
   * @param[in] y  Kilometers Y
   * @param[out] lat  Latitude degrees
   * @param[out] lon  Longitude degrees
   */
  virtual void xy2latlon(double x, double y, double &lat, double &lon) const;

  
  /**
   * Print the object,  basic params only
   *
   * @param[in] out  Stream to write to
   */
  virtual void print(std::ostream &out) const; 

  /**
   * Print the object,  details
   *
   * @param[in] out  Stream to write to
   */
  virtual void printDetails(std::ostream &out) const;
  
  /**
   * Override offset origin method which is not applicable to 
   * This projection
   *
   * @param[in] offsetLat
   * @param[in] offsetLon
   */
  virtual void setOffsetOrigin(double offsetLat, double offsetLon);
  
  /**
   * Override offset coordinates method which is not applicable to 
   * This projection
   *
   * @param[in] falseNorthing
   * @param[in] falseEasting
   */
  virtual void setOffsetCoords(double falseNorthing, double falseEasting);
  
protected:

private:

};

#endif











