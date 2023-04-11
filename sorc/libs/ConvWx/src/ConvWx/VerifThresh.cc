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
 * @file VerifThresh.cc
 */
#include <cstdio>
#include <string>
#include <ConvWx/VerifThresh.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
VerifThresh::VerifThresh(const bool isIntegerValued, const double threshold,
			 const int digitsPrecision) :
  pIsIntegerValued(isIntegerValued),
  pThresh(threshold),
  pDigitsPrecision(digitsPrecision)
{
}

//----------------------------------------------------------------
VerifThresh::~VerifThresh() 
{
}

//----------------------------------------------------------------
bool VerifThresh::operator==(const VerifThresh &v) const
{
  return (v.pIsIntegerValued == pIsIntegerValued &&
	  v.pThresh == pThresh &&
	  v.pDigitsPrecision == pDigitsPrecision);
}

//----------------------------------------------------------------
string VerifThresh::fieldName(const string &name) const
{
  char buf[convWx::ARRAY_LEN_SHORT];
  if (pIsIntegerValued)
  {
    int it = static_cast<int>(pThresh);
    sprintf(buf, "%s_%d", name.c_str(), it);
  }
  else
  {
    char format[convWx::ARRAY_LEN_SHORT];
    std::string sformat = "%s_%.";
    sprintf(format, "%d", pDigitsPrecision);
    sformat += format;
    sformat += "lf";
    sprintf(buf, sformat.c_str(), name.c_str(), pThresh);
  }
  string ret = buf;
  return ret;
}

