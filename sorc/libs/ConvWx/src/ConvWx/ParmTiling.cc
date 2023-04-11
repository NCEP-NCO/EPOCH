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
 * @file ParmTiling.cc
 * @brief Algorithm parameters all in one place
 */

//----------------------------------------------------------------
#include <string>
#include <cstdio>
#include <cstring>
#include <vector>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ParmTiling.hh>

//----------------------------------------------------------------
ParmTiling::ParmTiling(void)  : 
  pTiling(false),
  pTileNptX(0),
  pTileNptY(0),
  pTileNptOverlapX(0),
  pTileNptOverlapY(0),
  pMotherIsSubset(false),
  pMotherLowerLeftX(0),
  pMotherLowerLeftY(0),
  pMotherUpperRightX(0),
  pMotherUpperRightY(0)
{
}

//----------------------------------------------------------------
ParmTiling::ParmTiling(bool doTiling, int *npt, int *overlapNpt,
		       bool motherTileIsSubset, int *motherTileLowerLeftIndex,
		       int *motherTileUpperRightIndex) :
  pTiling(doTiling),
  pTileNptX(npt[0]),
  pTileNptY(npt[1]),
  pTileNptOverlapX(overlapNpt[0]),
  pTileNptOverlapY(overlapNpt[1]),
  pMotherIsSubset(motherTileIsSubset),
  pMotherLowerLeftX(motherTileLowerLeftIndex[0]),
  pMotherLowerLeftY(motherTileLowerLeftIndex[1]),
  pMotherUpperRightX(motherTileUpperRightIndex[0]),
  pMotherUpperRightY(motherTileUpperRightIndex[1])
{
}

//----------------------------------------------------------------
ParmTiling::~ParmTiling()
{
}

//----------------------------------------------------------------
void ParmTiling::print(void) const
{    
  printf("\tTiling:%d  tileSize(%d,%d), tileOverlap(%d,%d) MotherSubset:%d  lowerLeft(%d,%d)  UpperRight(%d,%d)\n",
	 pTiling, pTileNptX, pTileNptY, pTileNptOverlapX, pTileNptOverlapY,
	 pMotherIsSubset, pMotherLowerLeftX, pMotherLowerLeftY,
	 pMotherUpperRightX, pMotherUpperRightY);
}

