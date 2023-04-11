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
 * @file LpcModelState.cc
 */
#include <cstdio>
#include <string>
#include <ConvWx/LpcModelState.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
LpcModelState::LpcModelState(const int lt, const bool available) :
  pModelLtSeconds(lt),
  pModelAvailable(available)
{  
}

//----------------------------------------------------------------
LpcModelState::~LpcModelState()
{
}

//----------------------------------------------------------------
string LpcModelState::sprint(void) const
{
  string ret;
  string stat;
  if (pModelAvailable)
  {
    stat = "  ";
  }
  else
  {
    stat = "NO";
  }
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "model[%5.2lf] %s",
	  static_cast<double>(pModelLtSeconds)/convWx::DOUBLE_SECS_PER_HOUR,
	  stat.c_str());
  ret = buf;
  return ret;
}

//----------------------------------------------------------------
void LpcModelState::setAvailability(const bool isAvailable)
{
  pModelAvailable = isAvailable;
}
