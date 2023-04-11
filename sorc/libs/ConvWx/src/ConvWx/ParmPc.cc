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
 * @file ParmPc.cc
 */

//----------------------------------------------------------------
#include <string>
#include <vector>
#include <ConvWx/ParmPc.hh>
#include <ConvWx/Grid.hh>
using std::string;
using std::pair;
using std::vector;

//----------------------------------------------------------------
ParmPc::ParmPc(void) : 
  ParmPcAlg(),
  pModel(),
  pIncomingUv(),
  pVFieldname(),
  pOutput(),
  pProj()
{
  pDxdy[0] = 0;
  pDxdy[1] = 0;
}

//----------------------------------------------------------------
ParmPc::ParmPc(const ParmFcst &modelParms, const ParmFcst &uvParms,
	       const char *vFieldname, const ParmFcst &outputParms,
	       const ParmProjection &proj, 
	       const int *smooth, const bool useHighSmooth,
	       const bool smoothWhereCorrected, 
	       const int *gapfill, const double zeroRes,
	       const bool alwaysPhaseCorrect, const bool excludeWhenSmoothing,
	       const double smoothExclude) :
  ParmPcAlg(smooth, useHighSmooth, smoothWhereCorrected, gapfill, zeroRes,
	    alwaysPhaseCorrect, excludeWhenSmoothing, smoothExclude),
  pModel(modelParms),
  pIncomingUv(uvParms),
  pVFieldname(vFieldname),
  pOutput(outputParms),
  pProj(proj)
{
  pDxdy[0] = pProj.pDx;
  pDxdy[1] = pProj.pDy;
}

//----------------------------------------------------------------
ParmPc::~ParmPc()
{
}

//----------------------------------------------------------------
Grid ParmPc::uvGrid(const string &fieldname) const
{
  Grid g = Grid(fieldname, pIncomingUv.pUnits, pProj.pNx, pProj.pNy,
		convWx::UV_MISSING);
  g.setAllToValue(0.0);
  return g;
}

