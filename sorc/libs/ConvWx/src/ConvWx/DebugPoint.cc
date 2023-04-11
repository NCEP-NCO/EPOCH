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
 * @file DebugPoint.cc
 */
#include <vector>
#include <algorithm>
#include <ConvWx/DebugPoint.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::vector;

//----------------------------------------------------------------
DebugPoint::DebugPoint() :
  pLeadTime(),
  pX(convWx::BAD_INDEX),
  pY(convWx::BAD_INDEX),
  pDebug(false),
  pLeadTimeCheck(false)
{  
}

//----------------------------------------------------------------
DebugPoint::~DebugPoint() 
{
}

//----------------------------------------------------------------
void DebugPoint::set(const std::vector<int> &leadSeconds, const int x,
		     const int y)
{
  pLeadTime = leadSeconds;
  pX = x;
  pY = y;
  pDebug = true;
  pLeadTimeCheck = true;
}

//----------------------------------------------------------------
void DebugPoint::set(const int x, const int y)
{
  pLeadTime.clear();
  pX = x;
  pY = y;
  pDebug = true;
  pLeadTimeCheck = false;
}

//----------------------------------------------------------------
bool DebugPoint::isDebug(const int ltSeconds, const int x, const int y) const
{
  if (!pDebug)
  {
    return false;
  }
  if (pLeadTimeCheck)
  {
    if (find(pLeadTime.begin(), pLeadTime.end(), ltSeconds) == pLeadTime.end())
    {
      return false;
    }
  }
  return x == pX && y == pY;
}

//----------------------------------------------------------------
bool DebugPoint::isDebug(const int x, const int y) const
{
  if (!pDebug)
  {
    return false;
  }
  if (pLeadTimeCheck)
  {
    return false;
  }
  return x == pX && y == pY;
}
