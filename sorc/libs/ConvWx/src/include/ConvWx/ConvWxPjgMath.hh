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
 * @file ConvWxPjgMath.hh
 * @brief  Back-end math package for projective geometry, base class for
 *         each supported projection.
 *
 * @class ConvWxPjgMath
 * @brief  Back-end math package for projective geometry, base class for
 *         each supported projection.
 *
 */

#ifndef CONVWX_PJG_MATH_HH
#define CONVWX_PJG_MATH_HH

#include <iostream>
#include <ConvWx/ParmProjection.hh>

class ConvWxPjgMath
{
public:

  /**
   * Constructor
   * @param[in] earthRadiusKm  Radius of earth in kilometers
   */
  ConvWxPjgMath(double earthRadiusKm);
  
  /**
   * Destructor
   */
  virtual ~ConvWxPjgMath(void);
  
  /**
   * Create a new deep copy, at the derived class level
   *
   * @return the derived class pointer
   */
  ConvWxPjgMath *newDeepCopy(void);

  /**
   * equality
   *
   * @param[in] other  Object to compare against.
   */
  bool operator==(const ConvWxPjgMath &other) const;

  /**
   * inequality
   *
   * @param[in] other  Object to compare against.
   */
  bool operator!=(const ConvWxPjgMath &other) const;

  /**
   * Set offset origin by specifying latitude/longitude.
   *
   * Normally the offset origin and projection origin are co-located
   * This will compute false_northing and false_easting.
   *
   * - X = x_from_proj_origin + false_easting
   * - Y = y_from_proj_origin + false_northing
   *
   * @param[in] offsetLat  Latitude of offset point
   * @param[in] offsetLon  Longitude of offset point
   */
  virtual void setOffsetOrigin(double offsetLat,double offsetLon);
  
  /**
   * Set offset origin by specifying false_northing and false_easting.
   *
   * Normally the offset origin and projection origin are co-located
   * This will compute offset latitude and longitude, which is the point:
   *  (x = -falseNorthing, y = -falseEasting)
   *
   * @param[in] falseNorthing  Distance offset x
   * @param[in] falseEasting   Distance offset y
   */
  virtual void setOffsetCoords(double falseNorthing, double falseEasting);
  
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
   * Condition longitude to be in same hemisphere as origin longitude
   * @param[in,out] lon  to modify
   */
  void conditionLon2Origin(double &lon) const;

  /**
   * Condition longitude to be in same hemisphere as reference longitude
   *
   * @param[in] lon  to condition
   * @param[in] refLon  Reference longitude
   *
   * @return conditioned longitude
   */
  static double conditionLon2Ref(double lon, double refLon);
  
  /**
   * condition angle so it is between 0 and 360
   * @param[in] a  Angle to condition
   * @return conditioned angle
   */
  static double conditionRange360(double a);
  
  /**
   * condition angle so it is between -180 and 180
   * @param[in] a  Angle to condition
   * @return conditioned angle
   */
  static double conditionRange180(double a);
  
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
  
  /**
   * Print out the offset origin
   * @param[in] out  Stream to write to
   */
  void printOffsetOrigin(std::ostream &out) const;

  /**
   * compute sin and cos together for efficiency
   * @param[in] radians  An agrument for sine and cosine
   * @param[out] sinVal  The sine(radians)
   * @param[out] cosVal  The cosine(radians)
   */
  static void computeSinCos(double radians, double *sinVal, double *cosVal);

  /**
   * override the default earth radius in km
   * @param[in] earthRadiusKm   The new radius to use 
   */
  void setEarthRadiusKm(double earthRadiusKm);

  /**
   * @return the origin latitude
   */
  inline double getOriginLat(void) const { return pOriginLat; }

  /**
   * @return the origin longitude
   */
  inline double getOriginLon(void) const { return pOriginLon; }

  /**
   * @return standard latitude 1
   * 
   * Used for Lambert Conformal Conic, Albers Equal Area Conic
   */
  inline double getLat1(void) const { return pLat1; }

  /**
   * @return standard latitude 2
   * 
   * Used for Lambert Conformal Conic, Albers Equal Area Conic
   */
  inline double getLat2(void) const { return pLat2; }

  /**
   * @return rotation relative to True North (deg)
   *
   * @note optional on Azimuthal Equidistant (FLAT)
   */
  inline double getRotation(void) const { return pRotation; }

  /**
   * @return the false northing value
   */
  inline double getFalseNorthing(void) const { return pFalseNorthing; }

  /**
   * @return the false easting value
   */
  inline double getFalseEasting(void) const { return pFalseEasting; }

protected:

  static double pEradKm;  /**< Earth radius (km), default is 6378.137 */
  static const double pRad2Deg; /**< Radians to degrees conversion */
  static const double pDeg2Rad; /**< Degrees to radians conversion */

  ParmProjection::Proj_t pProjType;  /**< The projection type */

  double pOriginLat; /**< latitude of origin (deg) */
  double pOriginLon; /**< longitude of origin (deg) */

  double pLat1;  /**< standard latitude1 for conic projections */
  double pLat2;  /**< standard lat2 for conic projections */
  
  double pRotation; /**< rotation relative to TN - deg */

  double pOffsetLat; /**< latitude of offset origin (deg) */
  double pOffsetLon; /**< longitude of offset origin (deg) */

  double pFalseNorthing; /**< Value added to y coord computed from origin */
  double pFalseEasting;  /**< Value added to x coord computed from origin */

private:

};

#include <ConvWx/ConvWxPjgAzimEquidistMath.hh>
#include <ConvWx/ConvWxPjgLambertConfMath.hh>
#include <ConvWx/ConvWxPjgLambertAzimMath.hh>
#include <ConvWx/ConvWxPjgLatlonMath.hh>

#endif











