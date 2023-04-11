// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/**
 * @file GridLoopA.cc
 */
#include <ConvWx/GridLoopA.hh>
#include <ConvWx/GridLoopAlg.hh>
#include <ConvWx/GridData.hh>
#include <ConvWxIO/ILogMsg.hh>
// #include <toolsa/LogStream.hh>
#include <cstdlib>

//----------------------------------------------------------------
GridLoopA::GridLoopA(int nx, int ny, int sx, int sy) : 
  pNx(nx), pNy(ny), pSx(sx), pSy(sy)
{
  if (pNx < 2 || pNy < 2)
  {
    ILOGF(FATAL, "too few x,y %d, %d", pNx, pNy);
    exit(1);
  }

  // lower left, initial state
  pX = 0;
  pY = 0;
  pMinx = pX - pSx;
  pMiny = pY - pSy;
  pMaxx = pX + pSx;
  pMaxy = pY + pSy;
  pState = INIT;
}

//----------------------------------------------------------------
GridLoopA::~GridLoopA()
{
}

//----------------------------------------------------------------
void GridLoopA::reinit(void)
{
  // lower left
  pX = 0;
  pY = 0;
  pMinx = pX - pSx;
  pMiny = pY - pSy;
  pMaxx = pX + pSx;
  pMaxy = pY + pSy;
  pState = INIT;
}

//----------------------------------------------------------------
GridLoopA::State_t GridLoopA::getXy(int &x, int &y) const
{
  x = pX;
  y = pY;
  return pState;
}

//----------------------------------------------------------------
bool GridLoopA::getXyAndResult(const GridLoopAlg &alg,
			       int minGood, const GridData &g,
			       int &x, int &y, double &result) const
{
  x = pX;
  y = pY;
  return alg.getResult(minGood, g, x, y, result);
}

//----------------------------------------------------------------
bool GridLoopA::increment(const GridData &g, GridLoopAlg &alg)
{
  switch (pState)
  {
  case INIT:
    //LOG(DEBUG) << "INIT -> full compute at " << pX << "," << pY;
    // initially start moving up in y
    pState = INC_Y;

    // compute the full thing initially
    pFullCompute(g, alg);
    break;
  case INC_Y:
    // keep moving up in y if you can
    if (pY+1 < pNy)
    {
      //LOG(DEBUG) << "IncY " << pX << "," << pY;
      ++pY;
      pSubtractY(pMiny, g, alg);
      pMiny++;
      pMaxy++;
      pAddY(pMaxy, g, alg);
    }
    else
    {
      if (++pX >= pNx)
      {
	// upper right will be last point depending on nx
	return false;
      }
      // move to right when hit the top
      pState = INC_X;
      //LOG(DEBUG) << "IncX " << pX << "," << pY;
      pSubtractX(pMinx, g, alg);
      pMinx++;
      pMaxx++;
      pAddX(pMaxx, g, alg);
    }
    break;
  case DEC_Y:
    // keep moving down in y if you can
    if (pY-1 >= 0)
    {
      //LOG(DEBUG) << "DecY " << pX << "," << pY;
      --pY;
      pSubtractY(pMaxy, g, alg);
      pMiny--;
      pMaxy--;
      pAddY(pMiny, g, alg);
    }
    else
    {
      if (++pX >= pNx)
      {
	// lower right will be last point depending on nx
	return false;
      }
      // start moving to the right
      //LOG(DEBUG) << "IncX " << pX << "," << pY;
      pState = INC_X;
      pSubtractX(pMinx, g, alg);
      pMinx++;
      pMaxx++;
      pAddX(pMaxx, g, alg);
    }
    break;
  case INC_X:
  default:
    // increment x only by one, then move up or down
    // depending on where you are at
    if (pY  >= pNy-1)
    {
      // down we go
      //LOG(DEBUG) << "DecY " << pX << "," << pY;
      pState = DEC_Y;
      --pY;
      pSubtractY(pMaxy, g, alg);
      pMiny--;
      pMaxy--;
      pAddY(pMiny, g, alg);
    }
    else if (pY == 0)
    {
      // up up and away
      pState = INC_Y;
      //LOG(DEBUG) << "IncY " << pX << "," << pY;
      ++pY;
      pSubtractY(pMiny, g, alg);
      pMiny++;
      pMaxy++;
      pAddY(pMaxy, g, alg);
    }
    else
    {
      // ?
      ILOG(ERROR, "state bad");
    }
    break;
  }

  return true;
}

//----------------------------------------------------------------
std::string GridLoopA::printState(const State_t s)
{
  std::string ret;
  switch (s)
  {
  case INIT:
    ret = "INIT";
    break;
  case INC_Y:
    ret = "INC_Y";
    break;
  case INC_X:
    ret = "INC_X";
    break;
  case DEC_Y:
    ret = "DEC_Y";
    break;
  default:
    ret = "UNKNOWN";
    break;
  }
  return ret;
}

//----------------------------------------------------------------
void GridLoopA::pFullCompute(const GridData &g, GridLoopAlg &alg) const
{
  for (int y=pMiny; y<=pMaxy; ++y)
  {
    if (y >= 0 && y < pNy)
    {
      for (int x=pMinx; x<=pMaxx; ++x)
      {
	if (x >= 0 && x < pNx)
	{
	  alg.increment(x, y, g);
	}
      }
    }
  }
}

//----------------------------------------------------------------
void GridLoopA::pAddY(int y, const GridData &g, GridLoopAlg &alg) const
{
  if (y <0 || y >= pNy)
  {
    return;
  }
  for (int x=pMinx; x<=pMaxx; ++x)
  {
    if (x >= 0 && x < pNx)
    {
      alg.increment(x, y, g);
    }
  }
}

//----------------------------------------------------------------
void GridLoopA::pSubtractY(int y, const GridData &g, GridLoopAlg &alg) const
{
  if (y <0 || y >= pNy)
  {
    return;
  }
  for (int x=pMinx; x<=pMaxx; ++x)
  {
    if (x >= 0 && x < pNx)
    {
      alg.decrement(x, y, g);
    }
  }
}

//----------------------------------------------------------------
void GridLoopA::pAddX(int x, const GridData &g, GridLoopAlg &alg) const
{
  if (x <0 || x >= pNx)
  {
    return;
  }
  for (int y=pMiny; y<=pMaxy; ++y)
  {
    if (y >= 0 && y < pNy)
    {
      alg.increment(x, y, g);
    }
  }
}

//----------------------------------------------------------------
void GridLoopA::pSubtractX(int x, const GridData &g,  GridLoopAlg &alg) const
{
  if (x <0 || x >= pNx)
  {
    return;
  }
  for (int y=pMiny; y<=pMaxy; ++y)
  {
    if (y >= 0 && y < pNy)
    {
      alg.decrement(x, y, g);
    }
  }
}


