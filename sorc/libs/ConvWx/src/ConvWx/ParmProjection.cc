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
 * @file ParmProjection.cc
 */

//----------------------------------------------------------------
#include <cmath>
#include <string>
#include <vector>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/ConvWxFuzzy.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::vector;

//----------------------------------------------------------------
ParmProjection::ParmProjection(void) :
  pProjection(ParmProjection::FLAT),
  pNx(0),
  pNy(0),
  pNz(0),
  pDx(0),
  pDy(0),
  pDz(0),
  pMinx(0),
  pMiny(0),
  pMinz(0),
  pOriginLat(0),
  pOriginLon(0),
  pLat1(0),
  pLat2(0),
  pOffsetOriginLat(0),
  pOffsetOriginLon(0),
  pRotation(0),
  pEarthRadiusKm(0)
{
}

//----------------------------------------------------------------
ParmProjection::ParmProjection(Proj_t proj, int nx, int ny, double minx,
			       double miny, double dx, double dy,
			       double originLat, double originLon,
			       double offsetOriginLat,
			       double offsetOriginLon, double lat1,
			       double lat2, double rotation,
			       double earthRadiusKm) :
  pProjection(proj),
  pNx(nx),
  pNy(ny),
  pNz(1),
  pDx(dx),
  pDy(dy),
  pDz(0),
  pMinx(minx),
  pMiny(miny),
  pMinz(0),
  pOriginLat(originLat),
  pOriginLon(originLon),
  pLat1(lat1),
  pLat2(lat2),
  pOffsetOriginLat(offsetOriginLat),
  pOffsetOriginLon(offsetOriginLon),
  pRotation(rotation),
  pEarthRadiusKm(earthRadiusKm)
{
  InterfaceLL::setEarthRadiusKm(*this);
}

//----------------------------------------------------------------
ParmProjection::ParmProjection(Proj_t proj, int nx, int ny,int nz,  
			       double minx, double miny, double minz,
			       double dx, double dy, double dz,
			       double originLat, double originLon,
			       double offsetOriginLat,
			       double offsetOriginLon, double lat1,
			       double lat2, double rotation,
			       double earthRadiusKm) :
  pProjection(proj),
  pNx(nx),
  pNy(ny),
  pNz(nz),
  pDx(dx),
  pDy(dy),
  pDz(dz),
  pMinx(minx),
  pMiny(miny),
  pMinz(minz),
  pOriginLat(originLat),
  pOriginLon(originLon),
  pLat1(lat1),
  pLat2(lat2),
  pOffsetOriginLat(offsetOriginLat),
  pOffsetOriginLon(offsetOriginLon),
  pRotation(rotation),
  pEarthRadiusKm(earthRadiusKm)
{
  InterfaceLL::setEarthRadiusKm(*this);
}

//----------------------------------------------------------------
ParmProjection::ParmProjection(const ParmProjection &highResParms,
			       const int res)
{
  *this = highResParms;
  if (res < 2)
  {
    return;
  }

  pNx = pNx/res;
  pNy = pNy/res;
  pDx = pDx*res;
  pDy = pDy*res;
}

//----------------------------------------------------------------
ParmProjection::ParmProjection(const ParmProjection &proj,
			       const int nx, const int ny) :
  pProjection(proj.pProjection),
  pNx(proj.pNx + 2*nx),
  pNy(proj.pNy + 2*ny),
  pNz(proj.pNz),
  pDx(proj.pDx),
  pDy(proj.pDy),
  pDz(proj.pDz),
  pMinx(proj.pMinx - nx*pDx),
  pMiny(proj.pMiny - ny*pDy),
  pMinz(proj.pMinz),
  pOriginLat(proj.pOriginLat),
  pOriginLon(proj.pOriginLon),
  pLat1(proj.pLat1),
  pLat2(proj.pLat2),
  pOffsetOriginLat(proj.pOffsetOriginLat),
  pOffsetOriginLon(proj.pOffsetOriginLon),
  pRotation(proj.pRotation),
  pEarthRadiusKm(proj.pEarthRadiusKm)
{
  if (pNx <= 0 || pNy <= 0)
  {
    ILOGF(ERROR,
	  "Grid shrunk too much took off %d,%d result nx,ny=%d,%d",
	  nx, ny, pNx, pNy);
  }
}

//----------------------------------------------------------------
ParmProjection::~ParmProjection()
{
}

//----------------------------------------------------------------
bool ParmProjection::operator==(const ParmProjection &proj) const
{
  return (pProjection == proj.pProjection &&
	  pNx == proj.pNx &&
	  pNy == proj.pNy &&
	  pNz == proj.pNz &&
	  pDx == proj.pDx &&
	  pDy == proj.pDy &&
	  pDz == proj.pDz &&
	  pMinx == proj.pMinx &&
	  pMiny == proj.pMiny &&
	  pMinz == proj.pMinz &&
	  pOriginLat == proj.pOriginLat &&
	  pOriginLon == proj.pOriginLon &&
	  pLat1 == proj.pLat1 &&
	  pLat2 == proj.pLat2 &&
	  pOffsetOriginLat == proj.pOffsetOriginLat &&
	  pOffsetOriginLon == proj.pOffsetOriginLon &&
	  pRotation == proj.pRotation &&
	  pEarthRadiusKm == proj.pEarthRadiusKm);
}

//----------------------------------------------------------------
Grid ParmProjection::interior(const ConvWxFuzzy &fuzzyDistToInterest) const
{
  double missingv = convWx::FCST_MISSING;
  Grid ret("interior", "none", pNx, pNy, missingv);

  if (pProjection == ParmProjection::LATLON)
  {
    pInteriorLatlon(fuzzyDistToInterest, missingv, ret);
    return ret;
  }

  // get maximum distance (grid points) from edge in fuzzyDistToInterest
  double maxd = fuzzyDistToInterest.maxX();
  double maxy = pDy*static_cast<double>(pNy-1);
  double maxx = pDx*static_cast<double>(pNx-1);
  for (int y=0; y<pNy; ++y)
  {
    double yd = pInterior(y, pDy, maxd, maxy, missingv);
    for (int x=0; x<pNx; ++x)
    {
      double xd = pInterior(x, pDx, maxd, maxx, missingv);
      double v = pInteriorV(xd, yd, maxd, missingv);
      ret.setv(x, y, v);
    }
  }
  ret.applyFuzzyF(fuzzyDistToInterest);
  return ret;
}

//----------------------------------------------------------------
bool ParmProjection::consistentForTiling(const ParmProjection &proj) const
{
  bool ret = true;
  if (proj.pDx != pDx || proj.pDy != pDy)
  {
    ILOGF(WARNING, "projections differ in dx,dy %lf,%lf vs %lf,%lf",
	  proj.pDx, proj.pDy, pDx, pDy);
    ret = false;
  }
  if (proj.pProjection != pProjection)
  {
    ILOGF(WARNING, "projections differ in type %d vs. %d", 
	  static_cast<int>(proj.pProjection), static_cast<int>(pProjection));
    ret = false;
  }
  return ret;
}

//----------------------------------------------------------------
bool ParmProjection::isSuperset(const ParmProjection &proj) const
{
  bool ret = true;
  if (!consistentForTiling(proj))
  {
    ILOG(WARNING, "superset check failed");
    ret = false;
  }
  if (pMinx > proj.pMinx || pMiny > proj.pMiny ||
      pMinx +  static_cast<double>(pNx)*pDx < 
      proj.pMinx + static_cast<double>(proj.pNx)*proj.pDx ||
      pMiny + static_cast<double>(pNy)*pDy < 
      proj.pMiny + static_cast<double>(proj.pNy)*proj.pDy)
  {
    ILOGF(WARNING,
	  "[%lf,%lf],[%lf,%lf] not a superset of [%lf,%lf],[%lf,%lf]",
	  pMinx, pMiny, pMinx + static_cast<double>(pNx-1)*pDx, 
	  pMiny + static_cast<double>(pNy-1)*pDy, proj.pMinx, proj.pMiny,
	  proj.pMinx+static_cast<double>(proj.pNx-1)*proj.pDx,
	  proj.pMiny+static_cast<double>(proj.pNy-1)*proj.pDy);
    ret = false;
  }
  return ret;
}

//----------------------------------------------------------------
double ParmProjection::kmPerPixel(void) const
{
  if (pProjection == ParmProjection::FLAT || 
      pProjection == ParmProjection::LAMBERT_CONFORMAL2 ||
      pProjection == ParmProjection::LAMBERT_AZIM_EQUAL_AREA)
  {
    return (pDx + pDy)/2.0;
  }
  else if (pProjection == ParmProjection::LATLON)
  {
    // dx and dy are degrees, use an average latitude to convert to km
    // get an approximate latitude and use that to distort things
    double midlat = pMiny + pDy*pNy/2.0;
    midlat = convWx::RADIANS_PER_DEGREE*midlat;
    return (pDx*cos(midlat) + pDy)/2.0/convWx::DEGREES_PER_KILOMETER_AT_EQUATOR;
  }
  else
  {
    return (pDx + pDy)/2.0;
  }
}

//----------------------------------------------------------------
double ParmProjection::kmPerPixelX(void) const
{
  if (pProjection == ParmProjection::FLAT || 
      pProjection == ParmProjection::LAMBERT_CONFORMAL2 ||
      pProjection == ParmProjection::LAMBERT_AZIM_EQUAL_AREA)
  {
    return pDx;
  }
  else if (pProjection == ParmProjection::LATLON)
  {
    double midlat = pMiny + pDy*pNy/2.0;
    midlat = convWx::RADIANS_PER_DEGREE*midlat;
    return pDx*cos(midlat)/convWx::DEGREES_PER_KILOMETER_AT_EQUATOR;
  }
  else
  {
    return pDx;
  }
}

//----------------------------------------------------------------
double ParmProjection::kmPerPixelY(void) const
{
  if (pProjection == ParmProjection::FLAT || 
      pProjection == ParmProjection::LAMBERT_CONFORMAL2 ||
      pProjection == ParmProjection::LAMBERT_AZIM_EQUAL_AREA)
  {
    return pDy;
  }
  else if (pProjection == ParmProjection::LATLON)
  {
    return pDy/convWx::DEGREES_PER_KILOMETER_AT_EQUATOR;
  }
  else
  {
    return pDy;
  }
}

//----------------------------------------------------------------
bool ParmProjection::latlonToGridIndex(const double lat,
				       const double lon,
				       int &x, int &y) const
{
  return InterfaceLL::latlonToXyIndex(*this, lat, lon, x, y);
}

//----------------------------------------------------------------
void ParmProjection::latlonToGridIndexAny(const double lat,
					  const double lon,
					  int &x, int &y) const
{
  return InterfaceLL::latlonToXyIndexAny(*this, lat, lon, x, y);
}

//----------------------------------------------------------------
void ParmProjection::latlonToGridIndexAnyD(const double lat, const double lon,
					   double &x, double &y) const
{
  InterfaceLL::latlonToXy(*this, lat, lon, x, y);
}


//----------------------------------------------------------------
bool ParmProjection::latlonToGridIndexD(const double lat,
					const double lon,
					double &x, double &y) const
{
  return InterfaceLL::latlonToXy(*this, lat, lon, x, y);
}

//----------------------------------------------------------------
void ParmProjection::latlonToXyKm(const double lat, const double lon, double &x,
				  double &y) const
{
  // note this computes x,y even when outside the grid
  InterfaceLL::latlonToXy(*this, lat, lon, x, y);

  x = x*kmPerPixelX();
  y = y*kmPerPixelY();
}

//----------------------------------------------------------------
void ParmProjection::gridIndexToLatlon(const int x, const int y,
				       double &lat, double &lon) const
{
  return InterfaceLL::xyIndexToLatlon(*this, x, y, lat, lon);
}

//----------------------------------------------------------------
void ParmProjection::gridIndexDToLatlon(const double x, const double y,
					double &lat, double &lon) const
{
  return InterfaceLL::xyToLatlon(*this, x, y, lat, lon);
}

//----------------------------------------------------------------
void ParmProjection::distanceKm( const int indexI1,const int indexJ1, 
				 const int indexI2, const int indexJ2,
				 double &dist) const
{
  return InterfaceLL::distanceKm(*this,indexI1,indexJ1,indexI2,indexJ2, dist);
}

//----------------------------------------------------------------
void ParmProjection::distanceKmD( const double indexI1,const double indexJ1, 
				  const double indexI2, const double indexJ2,
				  double &dist) const
{
  return InterfaceLL::distanceKmD(*this,indexI1,indexJ1,indexI2,indexJ2, dist);
}

//----------------------------------------------------------------
void ParmProjection::distanceKm( const int arrayIndex1, const int arrayIndex2,
				 double &dist) const
{
  return InterfaceLL::distanceKm(*this,arrayIndex1,arrayIndex2,dist);
}

//----------------------------------------------------------------
double ParmProjection::pInterior(const int y, const double delta, 
				 const double maxd, const double maxy,
				 const double missingv) const
{
  double yd;
  double dy = static_cast<double>(y)*delta;
  if (dy < maxd)
  {
    yd = dy;
  }
  else if (maxy - dy < maxd)
  {
    yd = maxy - dy;
  }
  else
  {
    yd = missingv;
  }
  return yd;
}

//----------------------------------------------------------------
double ParmProjection::pInteriorV(const double xd, const double yd,
				  const double maxd,
				  const double missingv) const
{
  double v;
  if (xd == missingv && yd == missingv)
  {
    v = maxd;
  }
  else if (xd == missingv && yd != missingv)
  {
    v = yd;
  }
  else if (xd != missingv && yd == missingv)
  {
    v = xd;
  }
  else
  {
    if (xd <= yd)
    {
      v = xd;
    }
    else
    {
      v = yd;
    }
  }
  return v;
}

//----------------------------------------------------------------
void ParmProjection::pInteriorLatlon(const ConvWxFuzzy &fuzzyF,
				     const double missingv,
				     Grid &ret) const
{
  // Calculate the grid size in kilometers

  double midlat = (pMiny + pDy*pNy/2.0) * convWx::RADIANS_PER_DEGREE;
  double dxKm = (pDx * cos(midlat)) / convWx::DEGREES_PER_KILOMETER_AT_EQUATOR;
  double dyKm = pDy / convWx::DEGREES_PER_KILOMETER_AT_EQUATOR;
  
  // get maximum distance (grid points) from edge in fuzzyF
  double maxd = fuzzyF.maxX();
  double maxy = dyKm*static_cast<double>(pNy-1);
  double maxx = dxKm*static_cast<double>(pNx-1);
  for (int y=0; y<pNy; ++y)
  {
    double yd = pInterior(y, dyKm, maxd, maxy, missingv);
    for (int x=0; x<pNx; ++x)
    {
      double xd = pInterior(x, dxKm, maxd, maxx, missingv);
      double v = pInteriorV(xd, yd, maxd, missingv);
      ret.setv(x, y, v);
    }
  }
  ret.applyFuzzyF(fuzzyF);
}

