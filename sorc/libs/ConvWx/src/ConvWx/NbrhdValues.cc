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
 * @file NbrhdValues.cc
 */
#include <ConvWx/NbrhdValues.hh>
#include <ConvWx/Grid.hh>

const double NbrhdValues::pBad = -2.0;
const double NbrhdValues::pNotMarked = -1.0;
const double NbrhdValues::pMarked = 0.0;
const double NbrhdValues::pInitialClumpValue = 1.0;

//----------------------------------------------------------------
NbrhdValues::NbrhdValues(void) :
  pCurrentClumpValue(pInitialClumpValue),
  pCurrentClumpArea(0.0)
{
}

//----------------------------------------------------------------
NbrhdValues::~NbrhdValues(void)
{
}

//----------------------------------------------------------------
bool NbrhdValues::pointProcessed(const Grid &clumpGrid, const int x,
				 const int y) const
{
  return clumpGrid.isGtAt(x,  y, pNotMarked);
}

//----------------------------------------------------------------
bool NbrhdValues::markPoint(const int x, const int y, Grid &clumpGrid)
{
  if (!clumpGrid.isEqualAt(x, y, pCurrentClumpValue))
  {
    // not yet marked..mark now
    clumpGrid.setv(x, y, pCurrentClumpValue);
    ++pCurrentClumpArea;
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
void NbrhdValues::markPointNotWanted(const int x, const int y,
				     Grid &clumpGrid) const
{
  // not yet marked
  clumpGrid.setv(x, y, pMarked);
}

//----------------------------------------------------------------
void NbrhdValues::init(Grid &clumpGrid)
{
  clumpGrid.setAllToValue(pNotMarked);
  pCurrentClumpValue = pInitialClumpValue;
  pCurrentClumpArea = 0.0;
}

//----------------------------------------------------------------
bool NbrhdValues::isInAClump(const Grid &clumpGrid, const int x,
			     const int y) const
{
  // clump values are anything > pMarked
  return clumpGrid.isGtAt(x, y, pMarked);
}

//----------------------------------------------------------------
bool NbrhdValues::isInAClump(const double v)
{
  return v > pMarked;
}

//----------------------------------------------------------------
int NbrhdValues::numClumps(void) const
{
  // number of clumps is based on the current clump value and the initial
  // clump value.  The current clump value is actually the 'next' clump,
  // so if initial, there are no clumps, if initial+1 there are 1, and so
  // on

  return static_cast<int>(pCurrentClumpValue - pInitialClumpValue);
}


