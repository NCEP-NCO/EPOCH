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
 * @file ConvWxPjgLambertConfMath.cc
 */

#include <ConvWx/ConvWxPjgLambertConfMath.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <cmath>
using std::ostream;
using std::cerr;
using std::endl;

//------------------------------------------------------------------
ConvWxPjgLambertConfMath::ConvWxPjgLambertConfMath(double originLat,
						   double originLon,
						   double lat1,
						   double lat2,
						   double earthRadiusKm) :
  ConvWxPjgMath(earthRadiusKm)
{
  pProjType = ParmProjection::LAMBERT_CONFORMAL2;

  double  t1, t2, t0n, t1n;

  // check illegal values
  
  if (fabs(originLat - 90.0) < convWx::TINY_ANGLE ||
      fabs(originLat + 90.0) < convWx::TINY_ANGLE)
  {
    cerr << "WARNING - ConvWxPjgLambertConfMath" << endl;
    cerr << "  origin lat is at a pole: " << originLat << endl;
    if (fabs(originLat - 90.0) < convWx::TINY_ANGLE)
    {
      originLat -= convWx::TINY_ANGLE;
    }
    else
    {
      originLat += convWx::TINY_ANGLE;
    }
  }
  
  if (fabs(lat1 - 90.0) < convWx::TINY_ANGLE ||
      fabs(lat1 + 90.0) < convWx::TINY_ANGLE)
  {
    cerr << "WARNING - ConvWxPjgLambertConfMath" << endl;
    cerr << "  lat1 is at a pole: " << lat1 << endl;
    if (fabs(lat1 - 90.0) < convWx::TINY_ANGLE)
    {
      lat1 -= convWx::TINY_ANGLE;
    }
    else
    {
      lat1 += convWx::TINY_ANGLE;
    }
  }
  
  if (fabs(lat2 - 90.0) < convWx::TINY_ANGLE ||
      fabs(lat2 + 90.0) < convWx::TINY_ANGLE)
  {
    cerr << "WARNING - ConvWxPjgLambertConfMath" << endl;
    cerr << "  lat2 is at a pole: " << lat2 << endl;
    if (fabs(lat2 - 90.0) < convWx::TINY_ANGLE)
    {
      lat2 -= convWx::TINY_ANGLE;
    }
    else
    {
      lat2 += convWx::TINY_ANGLE;
    }
  }
  
  p2tanLine = true;
  if (fabs(lat2-lat1) < convWx::TINY_ANGLE)
  {
    p2tanLine = false;
  }

  pOriginLat = originLat;
  pOriginLon = originLon;
  pOffsetLat = pOriginLat;
  pOffsetLon = pOriginLon;
  
  pOriginLatRad = originLat * pDeg2Rad;
  pOriginLonRad = originLon * pDeg2Rad;
  pOriginColatRad = (90.0 - pOriginLat) * pDeg2Rad;

  pLat1 = lat1;
  pLat2 = lat2;
  pLat1Rad = pLat1 * pDeg2Rad;
  pLat2Rad = pLat2 * pDeg2Rad;

  if (p2tanLine)
  {
    t1 = tan( M_PI_4 + pLat1Rad / 2);
    t2 = tan( M_PI_4 + pLat2Rad / 2);
    pN = log( cos(pLat1Rad)/cos(pLat2Rad)) / log(t2/t1);
    
    t1n = pow(t1, pN);
    pF = cos(pLat1Rad) * t1n / pN;
    
    t0n = pow( tan(M_PI_4 + pOriginLatRad/2), pN);
    pRho = pEradKm * pF / t0n;

  }
  else
  {
    pSin0 = sin(pLat1Rad);
    pTan0 = tan( M_PI_4 - pLat1Rad / 2);
    pRho = pEradKm / tan(pLat1Rad);
  }
}

//------------------------------------------------------------------
ConvWxPjgLambertConfMath::~ConvWxPjgLambertConfMath(void)
{
}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::print(ostream &out) const
{
  out << "  Projection: LAMBERT_CONFORMAL2" << endl;
  out << "  origin_lon (deg): " << pOriginLon << endl;
  out << "  origin_lat (deg): " << pOriginLat << endl;
  out << "  lat1 (deg)      : " << pLat1 << endl;
  out << "  lat2 (deg)      : " << pLat2 << endl;
  printOffsetOrigin(out);
}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::printDetails(ostream &out) const
{
  print(out);
  out << "  Derived:" << endl;
  out << "    origin_lat_rad: " << pOriginLatRad << endl;
  out << "    origin_lon_rad: " << pOriginLonRad << endl;
  out << "    origin_colat_rad: " << pOriginColatRad << endl;
  out << "    lat1_rad: " << pLat1Rad << endl;
  out << "    lat2_rad: " << pLat2Rad << endl;
  out << "    n: " << pN << endl;
  out << "    F: " << pF << endl;
  out << "    rho: " << pRho << endl;
  out << "    tan0: " << pTan0 << endl;
  out << "    sin0: " << pSin0 << endl;
  out << "    2tan_line: " << p2tanLine << endl;

}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::latlon2xy(double lat, double lon,
                                   double &x, double &y) const
{
  if (lat == pOriginLat && lon == pOriginLon)
  {
    x = pFalseEasting;
    y = pFalseNorthing;
    return;
  }

  double xx, yy;

  if (p2tanLine)
  {
    pLatlon2xy2tan(lat, lon, xx, yy);
  }
  else
  {
    pLatlon2xy1tan(lat, lon, xx, yy);
  }

  x = xx + pFalseEasting;
  y = yy + pFalseNorthing;

}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::xy2latlon(double x, double y,
                                   double &lat, double &lon) const
{
  x -= pFalseEasting;
  y -= pFalseNorthing;
  
  if (p2tanLine)
  {
    pXy2latlon2tan(x, y, lat, lon);
  }
  else
  {
    pXy2latlon1tan(x, y, lat, lon);
  }
  conditionLon2Origin(lon);
}
     
//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::pLatlon2xy2tan(double lat, double lon,
					      double &x, double &y) const
{
  double latRad = lat * pDeg2Rad;
  double lonRad = lon * pDeg2Rad;
  double theta = pN * (lonRad - pOriginLonRad);
  double tn = pow( tan(M_PI_4 + latRad / 2.0), pN);
  double r = pEradKm * pF / tn;
  double sinTheta, cosTheta;
  computeSinCos(theta, &sinTheta, &cosTheta);
  x = r * sinTheta;
  y = pRho - r * cosTheta;
}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::pLatlon2xy1tan(double lat, double lon,
					      double &x, double &y) const
{
  double latRad = lat * pDeg2Rad;
  double lonRad = lon * pDeg2Rad;
  double tanPhi = tan(M_PI_4 - latRad / 2);
  double f1 = pow((tanPhi/pTan0), pSin0);
  double delLon = lonRad - pOriginLonRad;
  double sinLon, cosLon;
  computeSinCos(delLon * pSin0, &sinLon, &cosLon);
  x = pRho * f1 * sinLon;
  y = pRho * (1 - f1 * cosLon);
}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::pXy2latlon2tan(double x, double y,
					      double &lat, double &lon) const
{

  double r, theta, rn, yd;

  if (pN < 0.0)
  {
    yd = (-pRho + y);
    theta = atan2(-x, yd);
    r = sqrt(x * x + yd * yd);
    r *= -1.0;
  }
  else
  {
    yd = (pRho - y);
    theta = atan2(x, yd);
    r = sqrt(x * x + yd * yd);
  }

  lon = (theta / pN + pOriginLonRad) * pRad2Deg;
  lon = conditionRange180(lon);

  if (fabs(r) < convWx::TINY_DOUBLE)
  {
    lat = ((pN < 0.0) ? -90.0 : 90.0);
  }
  else
  {
    rn = pow( pEradKm * pF / r, 1 / pN);
    lat = (2.0 * atan(rn) - M_PI_2) * pRad2Deg;
  }
  lat = conditionRange180(lat);
}

//------------------------------------------------------------------
void ConvWxPjgLambertConfMath::pXy2latlon1tan(double x, double y,
					      double &lat, double &lon) const
{

  double delLon, sinLon, f1, r;
  double invSin0, toSin0, locX;

  locX = x;

  invSin0 = 1/pSin0;

  if (fabs(locX) < convWx::TINY_DOUBLE)
  {
    locX = convWx::BIG_EPSILON;
  }

  delLon = invSin0*atan2(locX,(pRho - y));
  lon = pOriginLonRad + delLon;
  sinLon = sin(delLon * pSin0);
  r = pRho * sinLon;
  toSin0 = pow((locX/r), invSin0);
  f1 = 2*atan(pTan0 * toSin0);
  lon = conditionRange180(lon*pRad2Deg);
  lat = (M_PI_2 - f1) * pRad2Deg;
  lat = conditionRange180(lat);
}

