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
 * @file ConvWxPjgAzimEquidistMath.cc
 */

#include <ConvWx/ConvWxPjgAzimEquidistMath.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <cmath>
using std::ostream;
using std::endl;

//------------------------------------------------------------------
ConvWxPjgAzimEquidistMath::ConvWxPjgAzimEquidistMath(double originLat,
						     double originLon,
						     double earthRadiusKm,
						     double rotation) :
  ConvWxPjgMath(earthRadiusKm)
{
  pProjType = ParmProjection::FLAT;
  pOriginLat = originLat;
  pOriginLon = originLon; 
  pOffsetLat = pOriginLat;
  pOffsetLon = pOriginLon;
  pRotation = rotation;
  pOriginLatRad = originLat * pDeg2Rad;
  pOriginLonRad = originLon * pDeg2Rad;
  pRotationRad = rotation * pDeg2Rad;
  pOriginColatRad = (90.0 - pOriginLat) * pDeg2Rad;
  computeSinCos(pOriginLatRad, &pSinOriginLat, &pCosOriginLat);
  computeSinCos(pOriginColatRad, &pSinOriginColat, &pCosOriginColat);
}

//------------------------------------------------------------------
ConvWxPjgAzimEquidistMath::~ConvWxPjgAzimEquidistMath(void)
{
}

//------------------------------------------------------------------
void ConvWxPjgAzimEquidistMath::print(ostream &out) const

{
  out << "  Projection: FLAT" << endl;
  out << "  origin_lon (deg): " << pOriginLon << endl;
  out << "  origin_lat (deg): " << pOriginLat << endl;
  out << "  rotation (deg)  : " << pRotation << endl;
  printOffsetOrigin(out);
}

//------------------------------------------------------------------
void ConvWxPjgAzimEquidistMath::printDetails(ostream &out) const
{
  print(out);
  out << "  Derived: " << endl;
  out << "    rotation (deg): " << pRotation << endl;
  out << "    origin_lat_rad: " << pOriginLatRad << endl;
  out << "    origin_lon_rad: " << pOriginLonRad << endl;
  out << "    origin_colat_rad: " << pOriginColatRad << endl;
  out << "    rotation_rad: " << pRotationRad << endl;
  out << "    sin_origin_lat: " << pSinOriginLat << endl;
  out << "    cos_origin_lat: " << pCosOriginLat << endl;
  out << "    sin_origin_Colat: " << pSinOriginColat << endl;
  out << "    cos_origin_Colat: " << pCosOriginColat << endl;
}

//------------------------------------------------------------------
void ConvWxPjgAzimEquidistMath::latlon2xy(double lat, double lon,
                                    double &x, double &y) const
{
  if (lat == pOriginLat && lon == pOriginLon)
  {
    x = pFalseEasting;
    y = pFalseNorthing;
    return;
  }

  double xx, yy;

  if (fabs(pRotation) > convWx::EPSILON)
  {
    // if we have a grid rotation, use the old code
    double r, thetaRad;
    pLatlon2RTheta(lat, lon, r, thetaRad);
    
    double gridTheta = thetaRad - pRotationRad;
    double sinTheta, cosTheta;
    computeSinCos(gridTheta, &sinTheta, &cosTheta);
    
    xx = r * sinTheta;
    yy = r * cosTheta;

  }
  else
  {
    // Reference: Map Projections used by the USGS
    double sinLat, cosLat;
    computeSinCos(lat * pDeg2Rad, &sinLat, &cosLat);
    
    double deltaLonRad = (lon * pDeg2Rad) - pOriginLonRad;
    double sinDeltaLon, cosDeltaLon;
    computeSinCos(deltaLonRad, &sinDeltaLon, &cosDeltaLon);
    
    double cosCC = (pSinOriginLat * sinLat +
                    pCosOriginLat * cosLat * cosDeltaLon);

    double cc = acos(cosCC);
    double sinCC = sin(cc);

    if (sinCC == 0.0)
    {
      x = pFalseEasting;
      y = pFalseNorthing;
      return;
    }
    
    double kk = cc / sinCC;
    double rk = pEradKm * kk;
    
    xx = rk * cosLat * sinDeltaLon;
    yy = rk * (pCosOriginLat * sinLat -
               pSinOriginLat * cosLat * cosDeltaLon);

  }

  x = xx + pFalseEasting;
  y = yy + pFalseNorthing;
  
}

//------------------------------------------------------------------
void ConvWxPjgAzimEquidistMath::xy2latlon(double x, double y,
                                    double &lat, double &lon) const
{
  x -= pFalseEasting;
  y -= pFalseNorthing;

  if (fabs(pRotation) > convWx::EPSILON)
  {

    // if we have a grid rotation, use the old code

    double r = sqrt(x * x + y * y);
    
    double thetaRad = 0.0;
    if (x == 0.0 && y == 0.0)
    {
      thetaRad = pRotationRad;
    }
    else
    {
      thetaRad = atan2(x, y) + pRotationRad; // rel to TN
    }
    
    pLatlonPlusRTheta(r, thetaRad, lat, lon);

  }
  else
  {
    // Reference: Map Projections used by the USGS

    double dist = sqrt(x * x + y * y);
    if (dist == 0.0)
    {
      lat = pOriginLatRad * pRad2Deg;
      lon = pOriginLonRad * pRad2Deg;
      return;
    }
    
    double cc = dist / pEradKm;
    double sinCC, cosCC;
    computeSinCos(cc, &sinCC, &cosCC);
    
    double latRad = asin((cosCC * pSinOriginLat) +
                         (y * sinCC * pCosOriginLat / dist));
    
    double lonRad = 0.0;
    if (pOriginLatRad == M_PI_2)
    {
      lonRad = pOriginLonRad + atan2(x, -y);
    }
    else if (pOriginLatRad == -M_PI_2)
    {
      lonRad = pOriginLonRad + atan2(x, y);
    }
    else
    {
      lonRad = pOriginLonRad +
        atan2(x * sinCC,
              dist * pCosOriginLat * cosCC - y * pSinOriginLat * sinCC);
    }
    
    lat = latRad * pRad2Deg;
    lon = lonRad * pRad2Deg;
    lon = conditionRange180(lon);
    conditionLon2Origin(lon);
  }
}
     
//------------------------------------------------------------------
void ConvWxPjgAzimEquidistMath::pLatlonPlusRTheta(double r, double thetaRad,
					    double &lat2, double &lon2) const
{
  double darc, colat2;
  double denom, deltaLon;
  double cosColat2, sinColat2;
  double sinDarc, cosDarc;
  double sinTheta, cosTheta;
  double xx;

  darc = r / pEradKm;
  computeSinCos(darc, &sinDarc, &cosDarc);
  computeSinCos(thetaRad, &sinTheta, &cosTheta);

  xx = pCosOriginColat * cosDarc + pSinOriginColat * sinDarc * cosTheta;
  if (xx < -1.0)
  {
    xx = -1.0;
  }
  if (xx > 1.0)
  {
    xx = 1.0;
  }
  colat2 = acos(xx);
  computeSinCos(colat2, &sinColat2, &cosColat2);
  lat2 = 90.0 - colat2 * pRad2Deg;
  
  denom = pSinOriginColat * sinColat2;
  
  if ( fabs(denom) <= convWx::TINY_DOUBLE)
  {
    deltaLon = 0.0;
  }
  else
  {
    xx = (cos(darc) - pCosOriginColat * cosColat2) / denom;
    if (xx < -1.0)
    {
      xx = -1.0;
    }
    if (xx > 1.0)
    {
      xx = 1.0;
    }
    deltaLon = acos(xx);
  }

  // reverse sign if theta is west
  if (sinTheta < 0.0)
  {
    deltaLon *= -1.0;
  }
  
  lon2 = (pOriginLonRad + deltaLon) * pRad2Deg;

  if (lon2 < -180.0)
  {
    lon2 += 360.0;
  }
  if (lon2 > 180.0)
  {
    lon2 -= 360.0;
  }
  return;
  
}

//------------------------------------------------------------------
void ConvWxPjgAzimEquidistMath::pLatlon2RTheta(double lat, double lon,
					 double &r, double &thetaRad) const
{
  double darc, colat2, delon, denom, therad;
  double cosColat2, sinColat2;
  double sinDarc, cosDarc;
  double xx;

  colat2 = (90.0 - lat) * pDeg2Rad;

  computeSinCos(colat2, &sinColat2, &cosColat2);

  delon = (lon - pOriginLon) * pDeg2Rad;
  
  if (delon < -M_PI)
  {
    delon += 2.0 * M_PI;
  }
  
  if (delon > M_PI)
  {
    delon -= 2.0 * M_PI;
  }
  
  xx = pCosOriginColat * cosColat2 +
    pSinOriginColat * sinColat2 * cos(delon);
  if (xx < -1.0)
  {
    xx = -1.0;
  }
  if (xx > 1.0)
  {
    xx = 1.0;
  }
  darc = acos(xx);
  computeSinCos(darc, &sinDarc, &cosDarc);
  
  r = darc* pEradKm;
  
  denom = pSinOriginColat * sinDarc;

  if ((fabs(pOriginColatRad) <= convWx::TINY_ANGLE) ||
      (fabs(denom) <= convWx::TINY_DOUBLE))
  {
    therad = 0.0;
  } 
  else
  {
    xx = (cosColat2 - pCosOriginColat * cosDarc) / denom;
    if (xx < -1.0)
    {
      xx = -1.0;
    }
    if (xx > 1.0)
    {
      xx = 1.0;
    }
    therad = acos(xx);
  }
  
  if ((delon < 0.0) || (delon > M_PI))
  {
    therad *= -1.0;
  }
  
  thetaRad = therad;

  return;

}

