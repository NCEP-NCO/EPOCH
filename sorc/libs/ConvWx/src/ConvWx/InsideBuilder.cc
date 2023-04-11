/**
 * @file InsideBuilder.cc
 */

#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/InsideBuilder.hh>
#include <ConvWx/EdgeFinder.hh>

//------------------------------------------------------------------
InsideBuilder::InsideBuilder(const EdgeFinder &e)
{
  // get a range
  e.getRange(pX0, pX1, pY0, pY1);
  
  // set our subset and make a boolean array for that
  pNx = pX1 - pX0 + 1;
  pNy = pY1 - pY0 + 1;
  pIsInside = new bool[pNx*pNy];

  ILOGF(DEBUG_VERBOSE, "Range (%d,%d) to (%d,%d) so nx,ny=%d,%d",
	pX0, pY0, pX1, pY1, pNx, pNy);


  // start with inside everywhere
  for (int i=0; i<pNx*pNy; ++i)
  {
    pIsInside[i] = true;
  }

  // erode away in all 4 search directions.
  Inside_t *dirX = new Inside_t[pNx*pNy];
  Inside_t *dirY = new Inside_t[pNx*pNy];

  ILOG(DEBUG_VERBOSE, "Setting INSIDE/OUTSIDE at each point");
  for (int iy=0; iy<pNy; ++iy)
  {
    ILOGF(DEBUG_VERBOSE, "Checking y=%d left to right start OUT", iy);
    bool out = true;
    for (int ix=0; ix<pNx; ++ix)
    {
      if (e.notEdge(ix, iy))
      {
	if (out)
	{
	  //ILOGF(DEBUG_VERBOSE, "[%d,%d] = OUT", ix, iy);
	  dirX[pPtr(ix, iy)] = OUT;
	}
	else
	{
	  ILOGF(DEBUG_VERBOSE, "[%d,%d] = MAYBE", ix, iy);
	  dirX[pPtr(ix, iy)] = MAYBE;
	}
      }
      else
      {
	out = false;
	ILOGF(DEBUG_VERBOSE, "[%d,%d] = IN", ix, iy);
	dirX[pPtr(ix, iy)] = IN;
      }
    }
    ILOGF(DEBUG_VERBOSE, "Checking y=%d  right to left start OUT", iy);
    out = true;
    for (int ix=pNx-1; ix>=0; --ix)
    {
      if (e.notEdge(ix, iy))
      {
	if (out)
	{
	  //ILOGF(DEBUG_VERBOSE, "[%d,%d] = OUT", ix, iy);
	  dirX[pPtr(ix, iy)] = OUT;
	}
      }
      else
      {
	out = false;
      }
    }
  }

  for (int ix=0; ix<pNx; ++ix)
  {
    ILOGF(DEBUG_VERBOSE, "Checking x=%d bottom to top start OUT", ix);
    bool out = true;
    for (int iy=0; iy<pNy; ++iy)
    {
      if (e.notEdge(ix, iy))
      {
	if (out)
	{
	  //ILOGF(DEBUG_VERBOSE, "[%d,%d] = OUT", ix, iy);
	  dirY[pPtr(ix, iy)] = OUT;
	}
	else
	{
	  ILOGF(DEBUG_VERBOSE, "[%d,%d] = MAYBE", ix, iy);
	  dirY[pPtr(ix, iy)] = MAYBE;
	}
      }
      else
      {
	out = false;
	ILOGF(DEBUG_VERBOSE, "[%d,%d] = YES", ix, iy);
	dirY[pPtr(ix, iy)] = IN;
      }
    }
    ILOGF(DEBUG_VERBOSE, "Checking x=%d top to bottom start out OUT",
	  ix);
    out = true;
    for (int iy=pNy-1; iy>=0; --iy)
    {
      if (e.notEdge(ix, iy))
      {
	if (out)
	{
	  // ILOGF(DEBUG_VERBOSE, "[%d,%d] = OUT", ix, iy);
	  dirY[pPtr(ix, iy)] = OUT;
	}
      }
      else
      {
	out = false;
      }
    }
  }

  ILOG(DEBUG_VERBOSE, "Fill inside/outside values now");
  for (int i=0; i<pNx*pNy; ++i)
  {
    if (dirY[i] == IN || dirX[i] == IN)
    {
      pIsInside[i] = true;
    }
    else if (dirY[i] == OUT || dirX[i] == OUT)
    {
      pIsInside[i] = false;
    }
    else
    {
      // at these 'ambiguous' points we leave things inside i.e. true
      // 'maybe' in both directions
      ILOGF(DEBUG_VERBOSE, "AMBIGUOUS Point %d", i);
    }
  }

  delete [] dirX;
  delete [] dirY;
}

//------------------------------------------------------------------
InsideBuilder::~InsideBuilder(void)
{
  delete [] pIsInside;
}

//------------------------------------------------------------------
bool InsideBuilder::isInside(const int x, const int y) const
{
  return pIsInside[pPtr(x, y)];
}

//------------------------------------------------------------------
int InsideBuilder::pPtr(const int x, const int y) const
{
  return y*pNx + x;
}
