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
 * @file ParmPcAlg.cc
 */

//----------------------------------------------------------------
#include <ConvWx/ParmPcAlg.hh>

//----------------------------------------------------------------
ParmPcAlg::ParmPcAlg(void) :
  pUseHighSmooth(false),
  pSmoothWhereCorrected(false),
  pZeroResolution(0),
  pAlwaysPhaseCorrect(true),
  pExcludeWhenSmoothing(false),
  pSmoothExclude(0.0)
{
  pSmooth[0] = 0;
  pSmooth[1] = 0;
  pGapfill[0] = 0;
  pGapfill[1] = 0;
}

//----------------------------------------------------------------
ParmPcAlg::ParmPcAlg(const int *smooth, const bool useHighSmooth,
		     const bool smoothWhereCorrected, 
		     const int *gapfill, const double zeroRes,
		     const bool alwaysPhaseCorrect,
		     const bool excludeWhenSmoothing,
		     const double smoothExclude) :
  pUseHighSmooth(useHighSmooth),
  pSmoothWhereCorrected(smoothWhereCorrected),
  pZeroResolution(zeroRes),
  pAlwaysPhaseCorrect(alwaysPhaseCorrect),
  pExcludeWhenSmoothing(excludeWhenSmoothing),
  pSmoothExclude(smoothExclude)
  
{
  pSmooth[0] = smooth[0];
  pSmooth[1] = smooth[1];
  pGapfill[0] = gapfill[0];
  pGapfill[1] = gapfill[1];
}

//----------------------------------------------------------------
ParmPcAlg::~ParmPcAlg()
{
}

