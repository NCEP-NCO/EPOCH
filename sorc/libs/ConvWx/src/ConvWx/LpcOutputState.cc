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
 * @file LpcOutputState.cc
 */
#include <cstdio>
#include <string>
#include <ConvWx/LpcOutputState.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
LpcOutputState::LpcOutputState(const int lt) :
  pLtSeconds(lt),
  pAllMissing(false),
  pProcessed(false)
{  
}

//----------------------------------------------------------------
LpcOutputState::~LpcOutputState()
{
}

//----------------------------------------------------------------
string LpcOutputState::sprint(void) const
{
  string ret;
  string stat, proc;
  if (pProcessed)
  {
    proc = "P";
  }
  else
  {
    proc = " ";
  }
  if (pAllMissing)
  {
    stat = "X";
  }
  else
  {
    stat = " ";
  }
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "output[%.2lf] %s %s",
	  static_cast<double>(pLtSeconds)/convWx::DOUBLE_SECS_PER_HOUR,
	  proc.c_str(), stat.c_str());
  ret = buf;
  return ret;
}
