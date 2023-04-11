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
 * @file LtInterp.cc
 */

#include <ConvWx/LtInterp.hh>
#include <iostream>
using std::string;

//-----------------------------------------------------------------
LtInterp::LtInterp()
{
  pPoint[0] = LtInterpPoint();
  pPoint[1] = LtInterpPoint();
}

//-----------------------------------------------------------------
LtInterp::LtInterp(int lt0, double weight0)
{
  pPoint[0] = LtInterpPoint(lt0, weight0);
  pPoint[1] = LtInterpPoint();
}

//-----------------------------------------------------------------
LtInterp::LtInterp(int lt0, double weight0, int lt1, double weight1)
{
  pPoint[0] = LtInterpPoint(lt0, weight0);
  pPoint[1] = LtInterpPoint(lt1, weight1);
}

//-----------------------------------------------------------------
LtInterp::LtInterp(const LtInterpPoint &lt0, const LtInterpPoint &lt1)
{
  pPoint[0] = lt0;
  pPoint[1] = lt1;
}

//-----------------------------------------------------------------
LtInterp::~LtInterp()
{
}

//-----------------------------------------------------------------
string LtInterp::ltString(void) const
{
  string ret = "";

  if (pPoint[0].pOk)
  {
    ret += pPoint[0].ltString();
    if (pPoint[1].pOk)
    {
      ret += ",";
      ret += pPoint[1].ltString();
    }
  }
  else
  {
    if (pPoint[1].pOk)
    {
      ret += pPoint[1].ltString();
    }
    else
    {
      ret = "no l.t.";
    }
  }
  return ret;
}

//-----------------------------------------------------------------
string LtInterp::ltString2(void) const
{
  string ret = "";

  ret += pPoint[0].ltString2();
  ret += ",";
  ret += pPoint[1].ltString2();
  return ret;
}

//-----------------------------------------------------------------
bool LtInterp::parse2(const std::string &s, LtInterp &lt)
{
  LtInterpPoint p0, p1;
  int ilast;

  // expect <first point>,<second point>
  // parse the first point
  if (LtInterpPoint::parse2(s, p0, ilast))
  {
    // skip past last, and past an assumed ','
    string s2 = s.substr(ilast+2);

    // parse the 2nd point
    if (LtInterpPoint::parse2(s2, p1, ilast))
    {
      // form the object  and return
      lt = LtInterp(p0, p1);
      return true;
    }
  }
  // problems along the way
  return false;
}

