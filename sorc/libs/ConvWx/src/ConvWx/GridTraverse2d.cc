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
 * @file GridTraverse2d.cc
 * @brief GridTraverse2d main class
 */

#include <cstdio>
#include <ConvWx/GridTraverse2d.hh>
#include <ConvWxIO/ILogMsg.hh>

using std::min;
using std::max;

/**
 * use this in the constructor
 */
static GridTraverse2d::Range_t sRange = {0,0,0,0};

//----------------------------------------------------------------
GridTraverse2d::GridTraverse2d(const int nx, const int ny,
			       const int sx, const int sy) :
  GridTraverse(nx, ny),
  pBoxSizeX(sx),
  pBoxSizeY(sy),
  pCurrentBox(sRange),
  pOldBox(sRange),
  pNewBox(sRange),
  pHasNewBox(false),
  pHasOldBox(false)
{
  pCurrentBox.minx = max(pCurrentX-pBoxSizeX, 0);
  pCurrentBox.miny = max(pCurrentY-pBoxSizeY, 0);
  pCurrentBox.maxx = min(pCurrentX+pBoxSizeX, pNx-1);
  pCurrentBox.maxy = min(pCurrentY+pBoxSizeY, pNy-1);
}

//----------------------------------------------------------------
GridTraverse2d::~GridTraverse2d()
{
}

//----------------------------------------------------------------
bool GridTraverse2d::increment2d(void)
{
  if (!GridTraverse::increment())
  {
    return false;
  }
  GridTraverse::State_t s = GridTraverse::getState();

  pCurrentBox.minx = max(0, pCurrentX-pBoxSizeX);
  pCurrentBox.maxx = min(pCurrentX+pBoxSizeX, pNx-1);
  pCurrentBox.miny = max(0, pCurrentY-pBoxSizeY);
  pCurrentBox.maxy = min(pCurrentY+pBoxSizeY, pNy-1);

  bool stat = true;
  switch (s)
  {
  case GridTraverse::INC_Y:
    pIncy();
    break;
  case GridTraverse::DEC_Y:
    pDecy();
    break;
  case GridTraverse::INC_X:
    pIncx();
    break;
  case GridTraverse::INIT:
    ILOGF(ERROR, "incrememented to bad state %d", static_cast<int>(s));
    stat = false;
    break;
  default:
    ILOGF(ERROR, "incrememented to bad state %d", static_cast<int>(s));
    stat = false;
    break;
  }
  return stat;
}

//----------------------------------------------------------------
void GridTraverse2d::pIncy(void)
{
  pHasNewBox = pCurrentY + pBoxSizeY < pNy;
  if (pHasNewBox)
  {
    pNewBox.minx = pCurrentBox.minx;
    pNewBox.maxx = pCurrentBox.maxx;
    pNewBox.miny = pNewBox.maxy = pCurrentBox.maxy;
  }
  pHasOldBox = pCurrentY - pBoxSizeY - 1 >= 0;
  if (pHasOldBox)
  {
    pOldBox.minx = pCurrentBox.minx;
    pOldBox.maxx = pCurrentBox.maxx;
    pOldBox.miny = pOldBox.maxy = pCurrentY - pBoxSizeY - 1;
  }
}

//----------------------------------------------------------------
void GridTraverse2d::pDecy(void)
{
  pHasNewBox = pCurrentY - pBoxSizeY >= 0;
  if (pHasNewBox)
  {
    pNewBox.minx = pCurrentBox.minx;
    pNewBox.maxx = pCurrentBox.maxx;
    pNewBox.miny = pNewBox.maxy = pCurrentBox.miny;
  }
  pHasOldBox = pCurrentY + pBoxSizeY + 1 < pNy;
  if (pHasOldBox)
  {
    pOldBox.minx = pCurrentBox.minx;
    pOldBox.maxx = pCurrentBox.maxx;
    pOldBox.miny = pOldBox.maxy = pCurrentY + pBoxSizeY + 1;
  }
}

//----------------------------------------------------------------
void GridTraverse2d::pIncx(void)
{
  pHasNewBox = pCurrentX + pBoxSizeX < pNx;
  if (pHasNewBox)
  {
    pNewBox.miny = pCurrentBox.miny;
    pNewBox.maxy = pCurrentBox.maxy;
    pNewBox.minx = pNewBox.maxx = pCurrentBox.maxx;
  }
  pHasOldBox = pCurrentX - pBoxSizeX - 1 >= 0;
  if (pHasOldBox)
  {
    pOldBox.miny = pCurrentBox.miny;
    pOldBox.maxy = pCurrentBox.maxy;
    pOldBox.minx = pOldBox.maxx = pCurrentX - pBoxSizeX - 1;
  }
}

//----------------------------------------------------------------
bool GridTraverse2d::isInOldBox(const int x, const int y) const
{
  if (pHasOldBox)
  {
    return (x >= pOldBox.minx && y <= pOldBox.maxy && 
	    y >= pOldBox.miny && y <= pOldBox.maxy);
  }
  else
  {
    return false;
  }
}
