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
 * @file ConvWxPjgMath.cc
 */


#include <ConvWx/ConvWxPjgMath.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWxIO/EarthRadius.hh>
#include <cmath>
using std::ostream;
using std::endl;

double ConvWxPjgMath::pEradKm = 6371.204;
//double ConvWxPjgMath::pEradKm = 6378.137;  // maybe switch to this
const double ConvWxPjgMath::pRad2Deg = 180.0 / M_PI;
const double ConvWxPjgMath::pDeg2Rad = 1.0 / ConvWxPjgMath::pRad2Deg;

//------------------------------------------------------------------
ConvWxPjgMath::ConvWxPjgMath(double earthRadiusKm) :
  pProjType(ParmProjection::FLAT),
  pOriginLat(0.0),
  pOriginLon(0.0),
  pLat1(0.0),
  pLat2(0.0),
  pRotation(0.0),
  pOffsetLat(0.0),
  pOffsetLon(0.0),
  pFalseNorthing(0.0),
  pFalseEasting(0.0)
{
  pEradKm = earthRadiusKm;
  EarthRadius::setEarthRadiusKm(earthRadiusKm);
}

//------------------------------------------------------------------
ConvWxPjgMath::~ConvWxPjgMath()
{
  return;
}

//------------------------------------------------------------------
ConvWxPjgMath *ConvWxPjgMath::newDeepCopy()
{
  // Create a new object, as a deep copy of the existing one
  // This copies at the derived class level

  const ConvWxPjgAzimEquidistMath *azimEquidist =
    dynamic_cast< const ConvWxPjgAzimEquidistMath* >(this);
  if (azimEquidist != 0)
  {
    return new ConvWxPjgAzimEquidistMath(*azimEquidist);
  }
  
  const ConvWxPjgLambertConfMath *lambertConf =
    dynamic_cast< const ConvWxPjgLambertConfMath* >(this);
  if (lambertConf != 0)
  {
    return new ConvWxPjgLambertConfMath(*lambertConf);
  }
    
  const ConvWxPjgLambertAzimMath *lambertAzim =
    dynamic_cast< const ConvWxPjgLambertAzimMath* >(this);
  if (lambertAzim != 0)
  {
    return new ConvWxPjgLambertAzimMath(*lambertAzim);
  }
    
  const ConvWxPjgLatlonMath *latlon =
    dynamic_cast< const ConvWxPjgLatlonMath* >(this);
  if (latlon != 0)
  {
    return new ConvWxPjgLatlonMath(*latlon);
  }
  
  return new ConvWxPjgMath(*this);
    
}

//------------------------------------------------------------------
bool ConvWxPjgMath::operator==(const ConvWxPjgMath &other) const
{
  
  if (pProjType != other.pProjType) {
    return false;
  }
  if (pOriginLat != other.pOriginLat) {
    return false;
  }
  if (pOriginLon != other.pOriginLon) {
    return false;
  }
  if (pLat1 != other.pLat1) {
    return false;
  }
  if (pLat2 != other.pLat2) {
    return false;
  }
  if (pRotation != other.pRotation) {
    return false;
  }
  if (pOffsetLat != other.pOffsetLat) {
    return false;
  }
  if (pOffsetLon != other.pOffsetLon) {
    return false;
  }
  if (pFalseNorthing != other.pFalseNorthing) {
    return false;
  }
  if (pFalseEasting != other.pFalseEasting) {
    return false;
  }
  
  return true;

}
  
//------------------------------------------------------------------
bool ConvWxPjgMath::operator!=(const ConvWxPjgMath &other) const
{
  return !(*this == other);
}
  
//------------------------------------------------------------------
void ConvWxPjgMath::setOffsetOrigin(double offsetLat, double offsetLon)
  
{
  
  pOffsetLat = offsetLat;
  pOffsetLon = offsetLon;

  if(pOriginLat == offsetLat && pOriginLon == offsetLon)
  {
    pFalseEasting = 0.0;
    pFalseNorthing = 0.0;
    return;
  }
    
  double xx, yy;
  latlon2xy(pOffsetLat, pOffsetLon, xx, yy);
  pFalseEasting = -xx;
  pFalseNorthing = -yy;

}

//------------------------------------------------------------------
void ConvWxPjgMath::setOffsetCoords(double falseNorthing, double falseEasting)
{
  if (falseEasting == 0.0 && falseNorthing == 0.0)
  {
    pFalseNorthing = 0;
    pFalseEasting = 0;
    pOffsetLat = pOriginLat;
    pOffsetLon = pOriginLon;
    return;
  }

  xy2latlon(-falseEasting, -falseNorthing, pOffsetLat, pOffsetLon);
  pFalseNorthing = falseNorthing;
  pFalseEasting = falseEasting;
  
}
  
//------------------------------------------------------------------
void ConvWxPjgMath::print(ostream &out) const
{
  out << "  Projection: (int) " << pProjType << endl;
  printOffsetOrigin(out);
}

//------------------------------------------------------------------
void ConvWxPjgMath::printDetails(ostream &out) const
{
  print(out);
}

//------------------------------------------------------------------
void ConvWxPjgMath::printOffsetOrigin(ostream &out) const
{
  if (pFalseNorthing != 0.0 || pFalseEasting != 0.0)
  {
    out << "  Offset lon (deg): " << pOffsetLon << endl;
    out << "  Offset lat (deg): " << pOffsetLat << endl;
    out << "  False northing: " << pFalseNorthing << endl;
    out << "  False easting: " << pFalseEasting << endl;
  }
}

//------------------------------------------------------------------
void ConvWxPjgMath::latlon2xy(double lat, double lon,
                        double &x, double &y) const
{
  y = lat;
  x = lon;
}

//------------------------------------------------------------------
void ConvWxPjgMath::xy2latlon(double x, double y,
                        double &lat, double &lon) const
{
  lat = y;
  lon = x;
}

//------------------------------------------------------------------
void ConvWxPjgMath::conditionLon2Origin(double &lon) const
{
  double diff = pOriginLon - lon;
  if (fabs(diff) > 180.0)
  {
    if (diff > 0)
    {
      lon += 360.0;
    }
    else
    {
      lon -= 360.0;
    }
  }
}

//------------------------------------------------------------------
double ConvWxPjgMath::conditionLon2Ref(double lon, double ref)
{
  double diff = ref - lon;
  if (fabs(diff) > 180.0)
  {
    if (diff > 0)
    {
      return lon + 360.0;
    }
    else
    {
      return lon - 360.0;
    }
  }
  return lon;
}

//------------------------------------------------------------------
double ConvWxPjgMath::conditionRange360(double a)
{
  while (a < 0.)
  {
    a += 360.;
  }
  while (a > 360.)
  {
    a -= 360.;
  }
  return a;
}

//------------------------------------------------------------------
double ConvWxPjgMath::conditionRange180(double a)
{
  while (a < -180.)
  {
    a += 360.;
  }
  while (a > 180.)
  {
    a -= 360.;
  }
  return a;
}


//------------------------------------------------------------------
void ConvWxPjgMath::computeSinCos(double radians, double *sinVal, double *cosVal)
{
  double cosv, sinv, interval;

  cosv = cos(radians);
  *cosVal = cosv;

  // compute sine magnitude 

  sinv = sqrt(1.0 - cosv * cosv);

  // set sine sign from location relative to PI

  interval = floor(radians / M_PI);
  if (fabs(fmod(interval, 2.0)) == 0)
  {
    *sinVal = sinv;
  }
  else
  {
    *sinVal = -1.0 * sinv;
  }
}

//----------------------------------------------------------------------
void ConvWxPjgMath::setEarthRadiusKm(double earthRadiusKm)
{
  pEradKm = earthRadiusKm;
  EarthRadius::setEarthRadiusKm(earthRadiusKm);
}

