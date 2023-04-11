/**
 * @file GridDistToNonMissing.cc
 */

#include <ConvWx/GridDistToNonMissing.hh>
#include <toolsa/LogMsg.hh>
#include <cmath>

//----------------------------------------------------------------
GridDistToNonMissing::GridDistToNonMissing(int maxSearch, int searchScale) :
  pMaxSearch(maxSearch), pSearchScale(searchScale), pNx(0), pNy(0)
{
}

//----------------------------------------------------------------
GridDistToNonMissing::~GridDistToNonMissing()
{
}

//----------------------------------------------------------------
void GridDistToNonMissing::update(const GridData &g)
{
  int nx, ny;
  g.getDim(nx, ny);
  if (nx != pNx || ny != pNy)
  {
    pNx = nx;
    pNy = ny;
    pXIndex = GridData(nx, ny, MISSING);
    pYIndex = GridData(nx, ny, MISSING);
    pRebuild(g);
  }
  else
  {
    if (pMissingChanged(g))
    {
      pRebuild(g);
    }
  }
}

//----------------------------------------------------------------
bool GridDistToNonMissing::distanceToNonMissing(const ParmProjection &proj,
						const GridData &data,
						GridData &distOut,
						GridData &valOut)
{      
  // check dimensions for consistency
  if (!data.sizeEqual(distOut) || !data.sizeEqual(valOut))
  {
    LOG(LogMsg::ERROR, "Dimensions inconsistent");
    return false;
  }

  // update if update is needed
  update(data);

  double maxRadius = pMaxSearch*proj.kmPerPixel();

  // At each grid point x,y find and record minimum distances to non-missing
  // data and the non missing data value  
  for (int j = 0; j < pNy; j++)
  {
    for (int i = 0; i < pNx; i++)
    { 
      
      // Taxicab metric distance to non-missing data from (i,j)
      double minDist = maxRadius;   

      // Value of data at minimum distance
      double closeVal;

      // Flag to indicate successful search
      bool distFound = false;
      
      // Check right away if point is not missing or bad
      // since we need not go further and distance = 0 at such points
      if (!data.isMissingAt(i, j))
      { 
	distFound = true;
	minDist = 0.0;
	data.getValue(i, j, closeVal);
      }
      else
      {
	int nearI, nearJ;
	if ((distFound = nearestPoint(i, j, nearI, nearJ)))
	{
	  double di = fabs(nearI - i);
	  double dj = fabs(nearJ - j);
	  if (dj > di)
	  {
	    di = dj;
	  }
	  minDist = di*proj.kmPerPixel();
	  if (!data.getValue(nearI, nearJ, closeVal))
	  {
	    LOGF(LogMsg::WARNING,
		 "Unexpected missing where not expected (%d,%d)", nearI,
		 nearJ);
	    distFound = false;
	  }
	}
      }
      
      if (!distFound)
      {
	// At points where no nearby data is non-missing, set result to max
	distOut.setv(i, j, maxRadius);
	valOut.setToMissing(i, j);
      }
      else
      {
	// Set the minimum distance to non missing data and the value 
	distOut.setv(i, j, minDist);
	valOut.setv(i, j, closeVal);
      }
    } // End for
  }// End for

  return true;
}

//----------------------------------------------------------------
bool GridDistToNonMissing::nearestPoint(int x, int y, int &nearX,
					int &nearY) const
{
  double dx, dy;
  if (pXIndex.getValue(x, y, dx) && pYIndex.getValue(x, y, dy))
  {
    if (dx == HAS_DATA)
    {
      nearX = x;
      nearY = y;
      return true;
    }
    else
    {
      nearX = static_cast<int>(dx);
      nearY = static_cast<int>(dy);
      return true;
    }
  }
  else
  {
    // nothing
    return false;
  }
}

//----------------------------------------------------------------
bool GridDistToNonMissing::pMissingChanged(const GridData &g) const
{
  int diff = 0;
  for (int i=0; i<g.getNdata(); ++i)
  {
    // is the new grid data missing at i?
    bool newMissing = g.isMissingAt(i);

    // the state says data was missing at i if either an index is set
    // indicating an offset 
    // (the value is not missing and is not HAS_DATA) or the
    // index is not set (the point has missing data and is out of the
    // max search distance)
    bool oldMissing;
    double v;
    if (pXIndex.getValue(i, v))
    {
      oldMissing = v != HAS_DATA;
    }
    else
    {
      oldMissing = true;
    }
    if (oldMissing != newMissing)
    {
      ++diff;
    }
  }
  if (diff > 0)
  {
    LOGF(LogMsg::DEBUG_VERBOSE, "Rebuild because %d points different", diff);
    return true;
  }
  else
  {
    LOG(LogMsg::DEBUG_VERBOSE, "Do not Rebuild because 0 points different");
    return false;
  }
}

//----------------------------------------------------------------
void GridDistToNonMissing::pRebuild(const GridData &g)
{
  // initialize to MISSING everywhere (MISSING is the missing value)
  pXIndex.setAllMissing();
  pYIndex.setAllMissing();


  // create a copy of the input grid  (gradually to be eroded away)
  GridData s(g);

  for (int r=pSearchScale; r<pMaxSearch; r += pSearchScale)
  {
    LOGF(LogMsg::DEBUG_VERBOSE, "Checking r=%d, nmissing=%d", r, s.nmissing());
    int totalNew=0;
    for (int y=0; y<pNy; ++y)
    {
      for (int x=0; x<pNx; ++x)
      {
	if (!s.isMissingAt(x, y))
	{
	  int nnew = 0;
	  nnew = pRebuild1(r, x, y, g);
	  totalNew += nnew;
	  if (nnew == 0)
	  {
	    // at larger search scales, we will want to skip over this point
	    // because the point was not used at a smaller scale.  This makes
	    // things more efficient.
	    s.setToMissing(x, y);
	  }
	}
      }
    }
    LOGF(LogMsg::DEBUG_VERBOSE, "R=%d had %d new", r, totalNew);
  }

  for (int y=0; y<pNy; ++y)
  {
    for (int x=0; x<pNx; ++x)
    {
      if (!g.isMissingAt(x, y))
      {
	pXIndex.setv(x, y, HAS_DATA);
	pYIndex.setv(x, y, HAS_DATA);
      }
    }
  }
}

//----------------------------------------------------------------
int GridDistToNonMissing::pRebuild1(int r, int x, int y, const GridData &g)
{
  // x,y is the point with data

  int nnew = 0;

  //
  // Search horizontal edges of box surrounding point (x, y)
  // with box lengths 2r
  for (int iy=y-r; iy<=y+r; iy += 2*r)
  {
    if (iy < 0 || iy >= pNy)
    {
      continue;
    }
    for (int ix = x-r; ix<=x+r; ix += pSearchScale)
    {
      if (ix >= 0 && ix < pNx)
      {
	if (g.isMissingAt(ix, iy))  
	{
	  if (pXIndex.isMissingAt(ix, iy))
	  {
	    // not yet set, set now
	    pXIndex.setv(ix, iy, x);
	    pYIndex.setv(ix, iy, y);
	    ++nnew;
	  }
	}
      }
    }
    //
    // Search vertical edges of box surrounding point (i,j)
    // with box lengths 2r
    // 
    for (int ix=x-r; ix<=x+r; ix+= 2*r)
    {
      if (ix < 0 || ix >= pNx)
      {
	continue;
      }
      for (int iy=y-r; iy<=y+r; iy += pSearchScale)
      {
	if (iy >= 0 && iy < pNy)
	{
	  if (g.isMissingAt(ix, iy))
	  {
	    if (pXIndex.isMissingAt(ix, iy))
	    {
	      // not yet set, set now
	      pXIndex.setv(ix, iy, x);
	      pYIndex.setv(ix, iy, y);
	      ++nnew;
	    }
	  }
	}
      }
    }
  }
  return nnew;
}

