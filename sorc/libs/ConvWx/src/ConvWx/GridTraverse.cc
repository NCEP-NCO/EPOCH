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
 * @file GridTraverse.cc
 * @brief GridTraverse main class
 */
#include <cstdlib>
#include <vector>
#include <string>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/GridTraverse.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::vector;
using std::pair;
using std::string;

//----------------------------------------------------------------
GridTraverse::GridTraverse(const int nx, const int ny) :
  pNx(nx),
  pNy(ny),
  // start out in lower left with state INIT
  pCurrentX(0),
  pCurrentY(0),
  pState(GridTraverse::INIT),
  pPreInitState(GridTraverse::INIT)
{
  if (pNx < 2 || pNy < 2)
  {
    ILOGF(ERROR, "too few x,y %d %d", nx, ny);
    exit(convWx::BAD_EXIT);
  }
}

//----------------------------------------------------------------
GridTraverse::~GridTraverse()
{
}

//----------------------------------------------------------------
void GridTraverse::reinit(void)
{
  // start out in lower left with state = INIT
  pCurrentX = 0;
  pCurrentY = 0;
  pPreInitState = GridTraverse::INIT;
  pState = GridTraverse::INIT;
}

//----------------------------------------------------------------
void GridTraverse::setStateInit(void)
{
  // save previous state and set state to INIT
  pPreInitState = pState;
  pState = GridTraverse::INIT;
}

//----------------------------------------------------------------
GridTraverse::State_t GridTraverse::getXy(int &x, int &y) const
{
  x = pCurrentX;
  y = pCurrentY;
  return pState;
}

//----------------------------------------------------------------
bool GridTraverse::increment(void)
{
  bool stat;
  switch (pState)
  {
  case GridTraverse::INIT:
    stat = pIncrementInit();
    break;
  case GridTraverse::INC_Y:
    stat = pIncrementY();
    break;
  case GridTraverse::DEC_Y:
    stat = pDecrementY();
    break;
  case GridTraverse::INC_X:
    stat = pIncrementX();
    break;
  default:
    ILOG(ERROR, "unexpected switch");
    stat = false;
  }
  return stat;
}


//----------------------------------------------------------------
string GridTraverse::printState(const State_t s)
{
  string ret;
  switch (s)
  {
  case GridTraverse::INIT:
    ret = "INIT";
    break;
  case GridTraverse::INC_Y:
    ret = "INC_Y";
    break;
  case GridTraverse::INC_X:
    ret = "INC_X";
    break;
  case GridTraverse::DEC_Y:
    ret = "DEC_Y";
    break;
  default:
    ret = "UNKNOWN";
    break;
  }
  return ret;
}

//----------------------------------------------------------------
bool GridTraverse::pIncrementInit(void)
{
  // set state to whatever it was prior to init
  pState = pPreInitState;
  if (pState == GridTraverse::INIT)
  {
    // assumed back at lower left moving up
    pState = GridTraverse::INC_Y;
    ++pCurrentY;
    return true;
  }
  else
  {
    // increment from the pre-init state
    return increment();
  }
}

//----------------------------------------------------------------
bool GridTraverse::pIncrementY(void)
{
  // try to move up
  if (pCurrentY+1 < pNy)
  {
    ++pCurrentY;
    return true;
  }
  else
  {
    // no more up, move right
    if (++pCurrentX >= pNx)
    {
      // no more period
      return false;
    }
    else
    {
      // moving right
      pState = GridTraverse::INC_X;
      return true;
    }
  }
}

//----------------------------------------------------------------
bool GridTraverse::pDecrementY(void)
{
  // try to move down
  if (pCurrentY-1 >= 0)
  {
    --pCurrentY;
    return true;
  }
  else
  {
    // no more down, move right
    if (++pCurrentX >= pNx)
    {
      // no more period
      return false;
    }
    else
    {
      // moving right
      pState = GridTraverse::INC_X;
      return true;
    }
  }
}

//----------------------------------------------------------------
bool GridTraverse::pIncrementX(void)
{
  // done moving right, either move up or down
  if (pCurrentY >= pNy-1)
  {
    // down it is
    pState = GridTraverse::DEC_Y;
    --pCurrentY;
    return true;
  }
  else if (pCurrentY == 0)
  {
    // up it is
    pState = GridTraverse::INC_Y;
    ++pCurrentY;
    return true;
  }
  else
  {
    ILOG(ERROR, "state bad");
    return false;
  }
}

