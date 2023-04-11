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
 * @file ConvThresh.cc
 * @brief the convergence threshold algorithm to reduce UV
 */
#include <cstring>
#include <cstdio>
#include <vector>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ConvThresh.hh>
#include <ConvWx/MultiGrid.hh>
using std::string;

//----------------------------------------------------------------
ConvThresh::ConvThresh() : pParm()
{  
}

//----------------------------------------------------------------
ConvThresh::ConvThresh(const ParmConv &parm) :  pParm(parm)
{
}

//----------------------------------------------------------------
ConvThresh::~ConvThresh() 
{
}

//----------------------------------------------------------------
void ConvThresh::apply(const UvOutput &unfilteredPcv,
		       const ParmProjection &proj,
		       UvOutput &filteredPcv)
{
  // initialize output to input
  filteredPcv = unfilteredPcv;

  // debugging here
  double minv, maxv;
  filteredPcv.getRange(minv, maxv);
  ILOGF(DEBUG_VERBOSE, "Before conv range = %lf to %lf", minv, maxv);

  // Compute initial pre-thresholded convergence partials du, dv,
  // and the convergence
  Grid du, dv, convergence;
  filteredPcv.convergence(convergence, du, dv, proj);

  // do the convergence technique using du,dv which reduces uv in the grids.
  filteredPcv.convThreshFilter(pParm.pConvThresh, pParm.pNptToExpansion,
			       du, dv);
  filteredPcv.getRange(minv, maxv);
  ILOGF(DEBUG_VERBOSE, "After conv range = %lf to %lf", minv, maxv);
}

