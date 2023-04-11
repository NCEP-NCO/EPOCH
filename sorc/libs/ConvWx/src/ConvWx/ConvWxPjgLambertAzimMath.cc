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
 * @file ConvWxPjgLambertAzimMath.cc
 */

#include <ConvWx/ConvWxPjgLambertAzimMath.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <cmath>
using std::ostream;
using std::cerr;
using std::endl;

//------------------------------------------------------------------
ConvWxPjgLambertAzimMath::ConvWxPjgLambertAzimMath(double originLat,
						   double originLon,
						   double earthRadiusKm) :
  ConvWxPjgMath(earthRadiusKm)
  
{
  pProjType = ParmProjection::LAMBERT_AZIM_EQUAL_AREA;

  pOriginLat = originLat;
  pOriginLon = originLon;
  pOffsetLat = pOriginLat;
  pOffsetLon = pOriginLon;
  
  pOriginLatRad = originLat * pDeg2Rad;
  pOriginLonRad = originLon * pDeg2Rad;

  computeSinCos(pOriginLatRad, &pSin0, &pCos0);
  
}

//------------------------------------------------------------------
ConvWxPjgLambertAzimMath::~ConvWxPjgLambertAzimMath(void)
{
}

//------------------------------------------------------------------
void ConvWxPjgLambertAzimMath::print(ostream &out) const
{
  out << "  Projection: LAMBERT_AZIM_EQUAL_AREA" << endl;
  out << "  origin_lon (deg): " << pOriginLon << endl;
  out << "  origin_lat (deg): " << pOriginLat << endl;
  printOffsetOrigin(out);
}

//------------------------------------------------------------------
void ConvWxPjgLambertAzimMath::printDetails(ostream &out) const
{
  print(out);
  out << "  Derived:" << endl;
  out << "    origin_lat_rad: " << pOriginLatRad << endl;
  out << "    origin_lon_rad: " << pOriginLonRad << endl;
  out << "    sin0: " << pSin0 << endl;
  out << "    cos0: " << pCos0 << endl;
}

//------------------------------------------------------------------
void ConvWxPjgLambertAzimMath::latlon2xy(double lat, double lon,
					 double &x, double &y) const
{
  if (lat == pOriginLat && lon == pOriginLon)
  {
    x = pFalseEasting;
    y = pFalseNorthing;
    return;
  }

  
  double sinLat, cosLat;
  computeSinCos(lat*pDeg2Rad, &sinLat, &cosLat);
  
  double deltaLonRad = (lon*pDeg2Rad) - pOriginLonRad;
  double sinDeltaLon, cosDeltaLon;
  computeSinCos(deltaLonRad, &sinDeltaLon, &cosDeltaLon);

  double cc (1.0 + pSin0*sinLat + pCos0*cosLat*cosDeltaLon);

  // indeterminate for point exactly opposite origin
  
  if (cc == 0.0)
  {
    x = pFalseEasting;
    y = pFalseNorthing;
    return;
  }

  double kk = sqrt(2.0 / cc);
  double rk = pEradKm*kk;
  
  double xx = rk*cosLat*sinDeltaLon;
  double yy = rk*(pCos0*sinLat - pSin0*cosLat*cosDeltaLon);
  
  x = xx + pFalseEasting;
  y = yy + pFalseNorthing;

}

//------------------------------------------------------------------
void ConvWxPjgLambertAzimMath::xy2latlon(double x, double y,
                                   double &lat, double &lon) const
{
  x -= pFalseEasting;
  y -= pFalseNorthing;
  
   // Reference: Map Projections used by the USGS
  
  double dist = sqrt(x*x + y*y);
  if (dist == 0.0)
  {
    lat = pOriginLatRad*pRad2Deg;
    lon = pOriginLonRad*pRad2Deg;
    return;
  }
  
  double cc = 2.0 * asin(dist / (2.0*pEradKm));
  double sinCC, cosCC;

  computeSinCos(cc, &sinCC, &cosCC);
  
  double latRad = asin((cosCC*pSin0) + (y * sinCC*pCos0/dist));
  
  double lonRad = 0.0;
  if (pOriginLatRad == convWx::CONVWX_PI/2.0)
  {
    lonRad = pOriginLonRad + atan2(x, -y);
  }
  else if (pOriginLatRad == -convWx::CONVWX_PI/2.0)
  {
    lonRad = pOriginLonRad + atan2(x, y);
  }
  else
  {
    lonRad = pOriginLonRad + atan2(x*sinCC, dist*pCos0*cosCC - y*pSin0*sinCC);
  }
  
  lat = latRad * pRad2Deg;
  lon = lonRad * pRad2Deg;
  lon = conditionRange180(lon);
  conditionLon2Origin(lon);
}
     
