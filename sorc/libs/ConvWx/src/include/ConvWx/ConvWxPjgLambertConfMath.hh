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
 * @file ConvWxPjgLambertConfMath.hh
 * @brief  Low-level math for Lambert Conformal Conic projection
 *
 * @class ConvWxPjgLambertConfMath
 * @brief  Low-level math for Lambert Conformal Conic projection
 *
 * @note Do not include this file directly. Include only the base class file,
 *       ConvWxPjgMath.hh
 */

#ifndef CONVWX_PJG_LAMBERT_CONF_MATH_HH
#define CONVWX_PJG_LAMBERT_CONF_MATH_HH

#include <ConvWx/ConvWxPjgMath.hh>

class ConvWxPjgLambertConfMath : public ConvWxPjgMath
{

public:

  /**
   * Constructor
   *
   * @param[in] originLat  Latitude of 0,0
   * @param[in] originLon  Longitude of 0,0
   * @param[in] lat1  Latitude 1 for the projection
   * @param[in] lat2  Latitude 2 for the projection
   * @param[in] earthRadiusKm  The earth radius in kilometers
   */
  ConvWxPjgLambertConfMath(double originLat, double originLon,
			   double lat1, double lat2, double earthRadiusKm);
  
  /**
   * Destructor
   */
  virtual ~ConvWxPjgLambertConfMath(void);
  
  /**
   * Convert from latitude/longitude (degrees) to x,y (km) 
   * @param[in] lat  Latitude degrees
   * @param[in] lon  Longitude degrees
   * @param[out] x  Kilometers X
   * @param[out] y  Kilometers Y
   */
  virtual void latlon2xy(double lat, double lon, double &x, double &y) const;

  /**
   * Convert from x,y (km)  to latitude/longitude (degrees)
   * @param[in] x  Kilometers X
   * @param[in] y  Kilometers Y
   * @param[out] lat  Latitude degrees
   * @param[out] lon  Longitude degrees
   */
  virtual void xy2latlon(double x, double y, double &lat, double &lon) const;

  
  /**
   * Print the object,  basic params only
   * @param[in] out  Stream to write to
   */
  virtual void print(std::ostream &out) const; 

  /**
   * Print the object,  details
   * @param[in] out  Stream to write to
   */
  virtual void printDetails(std::ostream &out) const;
  
protected:
  
  double pOriginLatRad; /**< Radians of origin latitude */
  double pOriginLonRad; /**< Radians of origin longitide */
  double pOriginColatRad; /**< Radians of origin co-latitude */
  double pLat1Rad;        /**< Latitude1, radians */
  double pLat2Rad;        /**< Latitude2, radians */

  double pN;              /**< Workspace varaible */
  double pF;              /**< Workspace varaible */
  double pRho;            /**< Workspace varaible */
  double pTan0;           /**< Workspace varaible */
  double pSin0;           /**< Workspace varaible */
  bool p2tanLine;         /**< Workspace varaible */
  
  /**
   * Compute x and y for a latitude longitude for the 2 tangent line case
   *
   * @param[in] lat  Latitude
   * @param[in] lon  Longitude
   * @param[out] x  X location
   * @param[out] y  Y location
   */
  void pLatlon2xy2tan(double lat, double lon, double &x, double &y) const;
  
  /**
   * Compute x and y for a latitude longitude for the 1 tangent line case
   *
   * @param[in] lat  Latitude
   * @param[in] lon  Longitude
   * @param[out] x  X location
   * @param[out] y  Y location
   */
  void pLatlon2xy1tan(double lat, double lon, double &x, double &y) const;
  
  /**
   * Compute latitude and longitude from x,y for the 2 tangent line case
   *
   * @param[in] x  X location
   * @param[in] y  Y location
   * @param[out] lat  Latitude
   * @param[out] lon  Longitude
   */
  void pXy2latlon2tan(double x, double y, double &lat, double &lon) const;
  
  /**
   * Compute latitude and longitude from x,y for the 1 tangent line case
   *
   * @param[in] x  X location
   * @param[in] y  Y location
   * @param[out] lat  Latitude
   * @param[out] lon  Longitude
   */
  void pXy2latlon1tan(double x, double y, double &lat, double &lon) const;

private:

};

#endif











