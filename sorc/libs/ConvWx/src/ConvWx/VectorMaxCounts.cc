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
 * @file VectorMaxCounts.cc
 */

//----------------------------------------------------------------
#include <cstring>
#include <ConvWx/VectorMaxCounts.hh>
using std::string;

//----------------------------------------------------------------
VectorMaxCounts::VectorMaxCounts(void) :
  pNumNoPreviousStat(0),
  pNumAtLowestValue(0),
  pNumWherePreviousIsOldest(0),
  pNumWherePreviousNotOldest(0),
  pNumTotal(0),
  pNumWithRedo(0)
{
}

//----------------------------------------------------------------
VectorMaxCounts::~VectorMaxCounts()
{
}

//----------------------------------------------------------------
void VectorMaxCounts::init(void)
{
  pNumNoPreviousStat = 0;
  pNumAtLowestValue = 0;
  pNumWherePreviousIsOldest = 0;
  pNumWherePreviousNotOldest = 0;
  pNumTotal = 1;
  pNumWithRedo = 1;
}

//----------------------------------------------------------------
void VectorMaxCounts::log(LogStream::Log_t s) const
{
  ILOGFV(s, "percent redo =         %.4lf",  pNumWithRedo/pNumTotal);
  ILOGFV(s, "pcnt no previous stats:%.4lf",  pNumNoPreviousStat/pNumTotal);
  ILOGFV(s, "pcnt value=lowest:     %.4lf",  pNumAtLowestValue/pNumTotal);
  ILOGFV(s, "pcnt prev=oldest (redo) %.4lf", pNumWherePreviousIsOldest/pNumTotal);
  ILOGFV(s, "pcnt prev not oldest    %.4lf",
	pNumWherePreviousNotOldest/pNumTotal);
}
