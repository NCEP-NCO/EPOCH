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
 * @file ConvWxProjection.cc
 */

#include <cstdio>
#include <cmath>
#include <ConvWx/ConvWxProjection.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//------------------------------------------------------------------
ConvWxProjection::ConvWxProjection(void) :
  pMath(NULL),
  pProjType(ParmProjection::FLAT),
  pOriginLat(0),
  pOriginLon(0),
  pFalseNorthing(0),
  pFalseEasting(0)
{
}
  
//------------------------------------------------------------------
ConvWxProjection::~ConvWxProjection(void)
{
  if (pMath != NULL)
  {
    delete pMath;
  }
}

//------------------------------------------------------------------
void ConvWxProjection::init(const ParmProjection &p)
{
  pProjParams = p;
  pProjType = p.pProjection;
  pOriginLat = p.pOriginLat;
  pOriginLon = p.pOriginLon;
  if (pMath != NULL)
  {
    delete pMath;
    pMath = NULL;
  }
  switch (pProjType)
  {
  case ParmProjection::FLAT:
    pMath = new ConvWxPjgAzimEquidistMath(pOriginLat, pOriginLon, 
					  p.pEarthRadiusKm, p.pRotation);
    break;
  case ParmProjection::LAMBERT_CONFORMAL2:
    pMath = new ConvWxPjgLambertConfMath(pOriginLat, pOriginLon, p.pLat1,
					 p.pLat2, p.pEarthRadiusKm);
    pMath->setOffsetCoords(pFalseNorthing, pFalseEasting);
    pMath->setOffsetOrigin(p.pOffsetOriginLat, p.pOffsetOriginLon);
    pFalseNorthing = pMath->getFalseNorthing();
    pFalseEasting = pMath->getFalseEasting();
    break;
  case ParmProjection::LAMBERT_AZIM_EQUAL_AREA:
    pMath = new ConvWxPjgLambertAzimMath(pOriginLat, pOriginLon,
					 p.pEarthRadiusKm);
    pMath->setOffsetCoords(pFalseNorthing, pFalseEasting);
    pMath->setOffsetOrigin(p.pOffsetOriginLat, p.pOffsetOriginLon);
    pFalseNorthing = pMath->getFalseNorthing();
    pFalseEasting = pMath->getFalseEasting();
    break;
  case ParmProjection::LATLON:
  default:
    pMath = new ConvWxPjgLatlonMath(p.pEarthRadiusKm, pOriginLon);
    pMath->setOffsetCoords(pFalseNorthing, pFalseEasting);
    break;
  }
}

//------------------------------------------------------------------
void ConvWxProjection::xyIndex2latlon(const int x, const int y, double &lat,
				      double &lon) const
{
  double xx = pProjParams.pMinx + x*pProjParams.pDx;
  double yy = pProjParams.pMiny + y*pProjParams.pDy;
  xy2latlon(xx, yy, lat, lon);
}

//------------------------------------------------------------------
bool ConvWxProjection::latlon2xyIndex(const double lat, const double lon,
				      int &x, int &y) const
{
  double xx, yy;
  latlon2xy(lat, lon, xx, yy);
  return pXy2xyIndex(xx, yy, x, y);
}

//------------------------------------------------------------------
void ConvWxProjection::latlon2xyIndexAny(const double lat, const double lon,
					 int &x, int &y) const
{
  double xx, yy;
  latlon2xy(lat, lon, xx, yy);
  pXy2xyIndexAny(xx, yy, x, y);
}

//------------------------------------------------------------------
void ConvWxProjection::xy2latlon(const double x, const double y, double &lat,
				 double &lon) const
{
  if (pMath == NULL)
  {
    lat = 0.0;
    lon = 0.0;
    return;
  }
  pMath->xy2latlon(x, y, lat, lon);
}

//------------------------------------------------------------------
void ConvWxProjection::latlon2xy(const double lat, const double lon, double &x,
				 double &y) const
{
  if (pMath == NULL)
  {
    x = 0.0;
    y = 0.0;
    return;
  }
  pMath->latlon2xy(lat, lon, x, y);
}

//------------------------------------------------------------------
double ConvWxProjection::xGrid2km(const double x) const
{
  double ret;
  switch (pProjType)
  {
  case ParmProjection::LATLON:
    ret = pXGrid2km(x);
    break;
  default:
    ret = pGrid2km(x, pProjParams.pDx);
    break;
  }
  return ret;
}

//------------------------------------------------------------------
double ConvWxProjection::yGrid2km(const double y) const
{
  double ret;
  switch (pProjType)
  {
  case ParmProjection::LATLON:
    ret = pYGrid2km(y);
    break;
  default:
    ret = pGrid2km(y, pProjParams.pDy);
    break;
  }
  return ret;
}

//------------------------------------------------------------------
void ConvWxProjection::setEarthRadiusKm(double earthRadiusKm)
{
  if (pMath != NULL)
  {
    pMath->setEarthRadiusKm(earthRadiusKm);
  }
}

//------------------------------------------------------------------
bool ConvWxProjection::pXy2xyIndex(double xx, double yy,
				   int &xIndex, int &yIndex) const
{
  bool ret = true;

  pXy2xyIndexAny(xx, yy, xIndex, yIndex);

  if (xIndex < 0)
  {
    xIndex = 0;
    ret = false;
  }
  
  if (xIndex >= pProjParams.pNx)
  {
    xIndex = pProjParams.pNx - 1;
    ret = false;
  }
  
  if (yIndex < 0)
  {
    yIndex = 0;
    ret = false;
  }
  
  if (yIndex >= pProjParams.pNy)
  {
    yIndex = pProjParams.pNy - 1;
    ret = false;
  }
  
  return ret;
}

//------------------------------------------------------------------
void ConvWxProjection::pXy2xyIndexAny(double xx, double yy,
				      int &xIndex, int &yIndex) const
{
  xIndex = static_cast<int>((xx - pProjParams.pMinx) / pProjParams.pDx + 0.5);
  yIndex = static_cast<int>((yy - pProjParams.pMiny) / pProjParams.pDy + 0.5);
}

//------------------------------------------------------------------
double ConvWxProjection::pXGrid2km(const double gridDistance) const
{
  double midLat = pProjParams.pMiny + pProjParams.pDy*pProjParams.pNy/2.0;
  double latitudeFactor = cos(midLat * convWx::RADIANS_PER_DEGREE);
  return gridDistance*
    convWx::KILOMETERS_PER_DEGREE_AT_EQUATOR*latitudeFactor*pProjParams.pDx;
}


//------------------------------------------------------------------
double ConvWxProjection::pYGrid2km(const double gridDistance) const
{
  return gridDistance*convWx::KILOMETERS_PER_DEGREE_AT_EQUATOR*pProjParams.pDy;
}


//------------------------------------------------------------------
double ConvWxProjection::pGrid2km(double gridDistance, double gridDelta) const
{
  return gridDistance*gridDelta;
}
