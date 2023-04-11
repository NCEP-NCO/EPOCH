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
 * @file PcScore.cc
 * @brief PcScore class
 */

//----------------------------------------------------------------
#include <cstdio>
#include <string>
#include <ConvWx/PcScore.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
PcScore::PcScore() :
  pName("Unknown"),
  pDiffsq(0.0),
  pNum(0.0),
  pAlpha(0.0),
  pDvar(0.0)
{

}

//----------------------------------------------------------------
PcScore::~PcScore()
{
}

//----------------------------------------------------------------
void PcScore::init(const double variance, const double alpha,
		   const string &name)
{
  pName = name;
  pDiffsq = pNum = 0.0;
  pDvar = variance;
  pAlpha = alpha;
}

//----------------------------------------------------------------
void PcScore::incrementVariance(const double data, const double mean)
{
  // no action because pDvar hardwired to a constant
}

//----------------------------------------------------------------
void PcScore::incrementDiffsq(const double fdata, const double ddata)
{
  pDiffsq += (fdata - ddata)*(fdata - ddata);
  ++pNum;
}

//----------------------------------------------------------------
bool PcScore::incrementScoreVars(double &sumScore, double &sumAlpha,
				 string &debugMsg) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  bool stat;

  if (pNum > 0)
  {
    sumAlpha += pAlpha*pNum;
    sumScore += pAlpha*pDiffsq/(pDvar*pDvar);
    sprintf(buf, "%sscore:[num:%.0lf  diffsq:%lf  lvar:%lf] ",
	    pName.c_str(), pNum, pDiffsq, pDvar);
    stat = true;
  }
  else
  {
    sprintf(buf, "%sscore:[num:%.0lf  diffsq:%lf  lvar:%lf] ",
	    pName.c_str(), pNum, pDiffsq, pDvar);
    stat = false;
  }
  string s = buf;
  debugMsg += s;
  return stat;
}
