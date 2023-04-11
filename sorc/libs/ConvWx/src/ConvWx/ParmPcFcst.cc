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
 * @file ParmPcFcst.cc
 */

//----------------------------------------------------------------
#include <string>
#include <ConvWx/ParmPcFcst.hh>
#include <ConvWx/InterfaceLL.hh>
using std::string;

//----------------------------------------------------------------
ParmPcFcst::ParmPcFcst(void) :
  pName("bad"),
  pThresh(0),
  pVerif(),
  pFcst(),
  pFractionalAreaDataThresh(0),
  pAlpha(0),
  pVariance(0),
  pVerifAndFcstSet(false),
  pVerifIsObs(false)
{
}

//----------------------------------------------------------------
ParmPcFcst::ParmPcFcst(const char *name, const double thresh,
		       const double dthresh, const double alpha,
		       const double variance) :
  pName(name),
  pThresh(thresh),
  pVerif(),
  pFcst(),
  pFractionalAreaDataThresh(dthresh),
  pAlpha(alpha),
  pVariance(variance),
  pVerifAndFcstSet(false),
  pVerifIsObs(false)
{
}

//----------------------------------------------------------------
ParmPcFcst::~ParmPcFcst()
{
}

//----------------------------------------------------------------
void ParmPcFcst::setVerifAndFcst(const ParmFcst &verifParms,
				 const ParmFcst &fcstParms,
				 const bool verifIsObs)
{
  pVerif = verifParms;
  pFcst = fcstParms;
  pVerifIsObs = verifIsObs;
  pVerifAndFcstSet = true;
}

