/**
 * @file EdgeFinder.cc
 */
#include <cmath>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/EdgeFinder.hh>
using std::pair;

static const int sBadInd = -99999;

//------------------------------------------------------------------
EdgeFinder::EdgeFinder(const GridData &grid) :
  //pEdge(grid),
  pX0(sBadInd), pX1(sBadInd), pY0(sBadInd), pY1(sBadInd), pLastX(sBadInd),
  pLastY(sBadInd), pBad(0.0), pGood(1.0)
{
  grid.getDim(pNx, pNy);
}

//------------------------------------------------------------------
EdgeFinder::~EdgeFinder()
{
}

//------------------------------------------------------------------
void EdgeFinder::add(const int x, const int y)
{
  pPoints.push_back(pair<int,int>(x, y));
  if (pX0 == sBadInd)
  {
    pX0 = pX1 = x;
    pY0 = pY1 = y;
  }
  else
  {
    if (x < pX0)
    {
      pX0 = x;
    }
    if (y < pY0)
    {
      pY0 = y;
    }
    if (x > pX1)
    {
      pX1 = x;
    }
    if (y > pY1)
    {
      pY1 = y;
    }
  }
  pFillGaps(x, y);
  pLastX = x;
  pLastY = y;

}

//------------------------------------------------------------------
bool EdgeFinder::bad(void) const
{
  return (pX0 == sBadInd);
}
  
//------------------------------------------------------------------
void EdgeFinder::putToGrid(GridData &edgeData)
{
  pEdge = GridData(pX1-pX0+1, pY1-pY0+1, pBad);

  int nx, ny;
  edgeData.getDim(nx, ny);
  if (nx != pNx || ny != pNy)
  {
    ILOGF(ERROR, "Dimensions unequal %d,%d  %d,%d", nx, ny,
	  pNx, pNy);
    return;
  }
  for (size_t i=0; i<pPoints.size(); ++i)
  {
    pEdge.setv(pPoints[i].first - pX0, pPoints[i].second - pY0, pGood);
    if (edgeData.inRange(pPoints[i].first, pPoints[i].second))
    {
      double v;
      if (edgeData.getValue(pPoints[i].first, pPoints[i].second, v))
      {
	if (v < pGood)
	{
	  edgeData.setv(pPoints[i].first, pPoints[i].second, pGood);
	}
      }
      else
      {
	edgeData.setv(pPoints[i].first, pPoints[i].second, pGood);
      }
    }
  }
}

//------------------------------------------------------------------
bool EdgeFinder::notEdge(const int x, const int y) const
{
  return pEdge.isMissingAt(x, y);
}

//------------------------------------------------------------------
bool EdgeFinder::dataGridIndex(const int x, const int y,
			       int &xi, int &yi) const
{
  xi = x + pX0;
  yi = y + pY0;
  return (xi >= 0 && xi < pNx && yi >= 0 && yi < pNy);
}

//------------------------------------------------------------------
void EdgeFinder::pFillGaps(const int x, const int y)
{
  if (pLastX == sBadInd)
  {
    return;
  }

  // make a line equation
  bool vertical = (pLastX  == x);
  double slope=1.0, intercept=1.0;
  if (!vertical)
  {
    slope = static_cast<double>(y-pLastY)/static_cast<double>(x-pLastX);
    intercept = static_cast<double>(y) - slope*static_cast<double>(x);
  }
  bool moreVert = (vertical || (!vertical && fabs(slope) > 1.0));
  if (moreVert)
  {
    int y0, y1;
    if (y > pLastY)
    {
      y0 = pLastY+1;
      y1 = y - 1;
    }
    else
    {
      y0 = y+1;
      y1 = pLastY - 1;
    }
    for (int iy=y0; iy<=y1; ++iy)
    {
      int ix;
      if (vertical)
      {
	ix = x;
      }
      else
      {
	double v = (static_cast<double>(iy)-intercept)/slope;
	ix = static_cast<int>(rint(v));
      }
      ILOGF(DEBUG_VERBOSE, "Filled gap point %d,%d", ix, iy);
      pPoints.push_back(pair<int,int>(ix, iy));
      // pEdge.setv(ix, iy, pGood);
    }
  }
  else
  {
    int x0, x1;
    if (x > pLastX)
    {
      x0 = pLastX+1;
      x1 = x - 1;
    }
    else
    {
      x0 = x + 1;
      x1 = pLastX - 1;
    }
    for (int ix=x0; ix<=x1; ++ix)
    {
      int iy;
      double v;
      v = static_cast<double>(ix)*slope+intercept;
      iy = static_cast<int>(rint(v));
      ILOGF(DEBUG_VERBOSE, "Filled gap point %d,%d", ix, iy);
      pPoints.push_back(pair<int,int>(ix, iy));
    }
  }
}

