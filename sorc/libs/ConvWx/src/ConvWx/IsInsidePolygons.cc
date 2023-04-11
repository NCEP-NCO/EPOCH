/**
 * @file IsInsidePolygons.cc
 */
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/IsInsidePolygons.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/MetaData.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <cstdio>
#include <vector>
using std::pair;
using std::vector;

#define MISSING_VALUE -1.0
#define MASK_VALUE 1.0

//---------------------------------------------------------
IsInsidePolygons::IsInsidePolygons(void) : pMaskIsSet(false),
					   pXexpand(0), pYexpand(0)
{

}

//---------------------------------------------------------
IsInsidePolygons::~IsInsidePolygons(void)
{
}

//---------------------------------------------------------
bool IsInsidePolygons::readParms(const std::string &fname)
{
  FILE *fp = fopen(fname.c_str(), "r");
  if (fp == NULL)
  {
    ILOGF(ERROR, "Opening %s", fname.c_str());
    return false;
  }
  double lat, lon;
  vector<pair<double,double> > latlon;
  while (fscanf(fp, "%lf,%lf", &lat, &lon) == 2)
  {
    latlon.push_back(pair<double,double>(lat,lon));
  }
  fclose(fp);
  if (latlon.empty())
  {
    ILOGF(ERROR, "Scanning pairs from %s", fname.c_str());
    return false;
  }
  else
  {
    pLatlon.push_back(latlon);
    return true;
  }
}

//---------------------------------------------------------
void
IsInsidePolygons::addPolygon(const std::vector<std::pair<double,double> > &l)
{
  pLatlon.push_back(l);
}

//---------------------------------------------------------
bool IsInsidePolygons::createMask(const ParmProjection &proj,
				  const int nx, const int ny)
{
  pXexpand = nx;
  pYexpand = ny;
  if (nx == 0 && ny == 0)
  {
    pProj = proj;
  }
  else
  {
    pProj = ParmProjection(proj, pXexpand, pYexpand);
  }

  pMask = Grid("mask", "none", pProj.pNx, pProj.pNy, MISSING_VALUE);
  pEdges = Grid("edge", "none", pProj.pNx, pProj.pNy, MISSING_VALUE);

  pMaskIsSet = InterfaceLL::polygonsToGrid(pProj, pLatlon,
					   MASK_VALUE, pMask, pEdges);
  return pMaskIsSet;
};

//---------------------------------------------------------
bool IsInsidePolygons::consistent(const ParmProjection &proj) const
{
  bool good;
  if (pXexpand == 0 && pYexpand == 0)
  {
    good = proj == pProj;
  }
  else
  {
    ParmProjection ptest(proj, pXexpand, pYexpand);
    good = ptest == pProj;
  }
  if (!good)
  {
    ILOG(ERROR, "Projection in use for test not correct");
    return false;
  }
  else
  {
    return true;
  }
}

//---------------------------------------------------------
bool IsInsidePolygons::isInside(const int x, const int y) const
{
  // convert x,y to the bigger grid
  int ix = x + pXexpand;
  int iy = y + pYexpand;

  if (!pMask.inRange(ix, iy))
  {
    // not in the grid means not inside a polygon
    return false;
  }
  else
  {
    double v;
    if (pMask.getValue(ix, iy, v))
    {
      // if v == MASK_VALUE, are inside
      return v == MASK_VALUE;
    }
    else
    {
      // not inside a polygon for sure
      return false;
    }
  }
}

//---------------------------------------------------------
void IsInsidePolygons::appendGrid(const std::string &fieldName,
				  const ParmProjection &proj, MultiGrid &mg)
{
  // create a grid in the input projection
  Grid g(fieldName, "none", proj.pNx, proj.pNy, MISSING_VALUE);

  // for each point in this grid
  for (int y=0; y<proj.pNy; ++y)
  {
    for (int x=0; x<proj.pNx; ++x)
    {
      // convert to a lat/lon, then get where that is in the local projection
      double lat, lon;
      proj.gridIndexToLatlon(x, y, lat, lon);
      int ix, iy;
      if (pProj.latlonToGridIndex(lat, lon, ix, iy))
      {
	// get the value at that point, and set it into output
	double v;
	if (pMask.getValue(ix, iy, v))
	{
	  g.setv(x, y, v);
	}
      }
    }
  }
  mg.append(g);
}

//---------------------------------------------------------
void IsInsidePolygons::writeGrids(const time_t &t, const std::string &url)
{
  MetaData md;
  MultiGrid mg;
  mg.append(pMask);
  mg.append(pEdges);
  InterfaceIO::write(t, url, pProj, mg, md);
}
