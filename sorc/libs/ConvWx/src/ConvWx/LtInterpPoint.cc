// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// � University Corporation for Atmospheric Research (UCAR) 2009-2010. 
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
 * @file LtInterpPoint.cc
 */
#include <ConvWx/LtInterpPoint.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWxIO/ILogMsg.hh>

#include <iostream>
#include <cstdio>
#include <algorithm>

using std::string;

//-----------------------------------------------------------------
LtInterpPoint::LtInterpPoint(void) :
  pOk(false),
  pLeadSeconds(-1),
  pWeight(-1.0)
{
}

//-----------------------------------------------------------------
LtInterpPoint::LtInterpPoint(int lt, double weight) :
  pOk(true),
  pLeadSeconds(lt),
  pWeight(weight)
{
}

//-----------------------------------------------------------------
LtInterpPoint::~LtInterpPoint()
{
}

//-----------------------------------------------------------------
string LtInterpPoint::ltString(void) const
{
  string ret;
  char buf[convWx::ARRAY_LEN_SHORT];

  if (pOk)
  {
    sprintf(buf, "%d[%lf]", pLeadSeconds, pWeight);
    ret = buf;
  }
  else
  {
    ret = "";
  }
  return ret;
}

//-----------------------------------------------------------------
string LtInterpPoint::ltString2(void) const
{
  string ret;
  char buf[convWx::ARRAY_LEN_SHORT];

  if (pOk)
  {
    sprintf(buf, "[%d,%.2lf]", pLeadSeconds, pWeight);
    ret = buf;

    // remove any spaces:
    ret.erase(remove_if(ret.begin(), ret.end(), isspace), ret.end());
  }
  else
  {
    ret = "[-1,0.00]";
  }
  return ret;
}

//-----------------------------------------------------------------
bool LtInterpPoint::parse2(const std::string &s, LtInterpPoint &point,
			   int &last)
{
  // expect [<int>,<float>]
  
  size_t p0 = s.find_first_of("[");
  size_t p1 = s.find_first_of("]");
  size_t pc = s.find_first_of(",");
  if (p0 == string::npos || p1 == string::npos || pc == string::npos)
  {
    ILOGF(ERROR, "format of string %s is incorrect", s.c_str());
    return false;
  }
  if (!(p0 < pc && pc < p1))
  {
    ILOGF(ERROR, "format of string %s is incorrect", s.c_str());
    return false;
  }

  // the stuff between p0 and pc is an int
  string i = s.substr(p0+1, pc-p0-1);
  int lt;
  if (sscanf(i.c_str(), "%d", &lt) != 1)
  {
    ILOGF(ERROR, "Parsing %s as an int", i.c_str());
    return false;
  }
  
  // the stuff between pc and p1 is a float
  i = s.substr(pc+1, p1-pc-1);
  float pct;
  if (sscanf(i.c_str(), "%f", &pct) != 1)
  {
    ILOGF(ERROR, "Parsing %s as a float", i.c_str());
    return false;
  }
  
  // create the point, set last, and return
  point = LtInterpPoint(lt, pct);
  last = p1;
  return true;
}
