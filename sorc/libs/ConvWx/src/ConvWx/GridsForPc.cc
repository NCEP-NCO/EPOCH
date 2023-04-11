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
 * @file GridsForPc.cc
 */
//----------------------------------------------------------------
#include <string>
#include <vector>
#include <ConvWx/GridsForPc.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/PcScore.hh>
#include <ConvWx/UvOutput.hh>

using std::string;
using std::vector;

//----------------------------------------------------------------
/**
 * Get value when you can
 * @param[in] ix  Location from which to get data
 * @param[in] iy  Location from which to get data
 * @param[in] g   Grid with data
 * @param[out] v  Value returned
 * @return true if value was pulled from grid
 */
static bool sGetVal(const int ix, const int iy, const Grid &g, double &v)
{
  if (g.inRange(ix, iy))
  {
    return g.getValue(ix, iy, v);
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
/**
 * Increment some values in input PcScore object
 * @param[in] ddata Data value at a point
 * @param[in] dmean  Mean data value
 * @param[in] fx  location data is from
 * @param[in] fy  location data is from
 * @param[in] g   Grid with data
 * @param[in] thresh  Threshold at which collapse values to 0.0
 * @param[in,out] pcs  The object to modify
 */
static void sPcScoreIncrement(const double ddata, const double dmean,
			      const int fx, const int fy, const Grid &g,
			      const double thresh, PcScore &pcs)
{
  pcs.incrementVariance(ddata, dmean);

  double fdata;
  if (sGetVal(fx, fy, g, fdata))
  {
    if (fdata < thresh)
    {
      fdata = 0.0;
    }
    pcs.incrementDiffsq(fdata, ddata);
  }
}

//----------------------------------------------------------------
GridsForPc::GridsForPc(const PcInput &inputData, const ParmSetUV &algParms) :
  pVerifGrid(*inputData.getVerifPtr()),
  pFcstGrid(*inputData.getFcstPtr()),
  // make copy of the two inputs into the three local grids
  pSmoothVerifGrid(pVerifGrid),
  pSmoothFcstGrid(pFcstGrid),
  pPhaseCorrectedGrid(pFcstGrid),
  pName(inputData.getName()),
  pParm(inputData.getParms())
{
  // do the smoothing here using parameters
  ILOG(DEBUG_VERBOSE, "Smoothing");
  pSmoothVerifGrid.fastsmooth((const int)algParms.pVerifInputSmooth,
			      (const int)algParms.pVerifInputSmooth);
  pSmoothFcstGrid.fastsmooth((const int)algParms.pFcstInputSmooth,
			     (const int)algParms.pFcstInputSmooth);
  ILOG(DEBUG_VERBOSE, "Done Smoothing");
}

//----------------------------------------------------------------
GridsForPc::~GridsForPc()
{
}

//----------------------------------------------------------------
bool GridsForPc::reduce(const int res)
{
  bool stat = true;
  if (!pVerifGrid.reduce(res))
  {
    stat = false;
  }
  if (!pFcstGrid.reduce(res))
  {
    stat = false;
  }
  if (!pSmoothVerifGrid.reduce(res))
  {
    stat = false;
  }
  if (!pSmoothFcstGrid.reduce(res))
  {
    stat = false;
  }
  if (!pPhaseCorrectedGrid.reduce(res))
  {
    stat = false;
  }
  return stat;
}

//----------------------------------------------------------------
void GridsForPc::setNoPc(void)
{
  pPhaseCorrectedGrid = pFcstGrid;
}

//----------------------------------------------------------------
void GridsForPc::setPcToMissing(const int x, const int y)
{
  pPhaseCorrectedGrid.setToMissing(x, y);
}

//----------------------------------------------------------------
void GridsForPc::shiftFcst(const UvOutput &uv)
{
  // copy everything over.
  setNoPc();

  int nx, ny;
  pSmoothVerifGrid.getDim(nx, ny);
  double u, v;
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      if (!uv.getValuesOrZero(x,  y, u, v))
      {
	continue;
      }
      // the forecast at this point is moving away
      setPcToMissing(x, y);
    }
  }
  ILOG(DEBUG_VERBOSE, "shifting");

  // move forecast at from locations into to locations.
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      if (!uv.getValuesOrZero(x, y, u, v))
      {
	continue;
      }

      int fx = static_cast<int>(x + u);
      int fy = static_cast<int>(y + v);
      
      moveFcstForPc(x, y, fx, fy);
    }	  
  }
}

//----------------------------------------------------------------
void GridsForPc::shiftFcstAndSmooth(const UvOutput &uv,
				    const int gapFill,
				    const vector<int> &smooth)
{
  shiftFcst(uv);
  fillGapsInPc(gapFill);
  for (int i=0; i<static_cast<int>(smooth.size()); ++i)
  {
    smoothPc(smooth[i]);
  }
}

//----------------------------------------------------------------
void GridsForPc::moveFcstForPc(const int fromX, const int fromY,
			       const int toX, const int toY)
{
  if (pPhaseCorrectedGrid.inRange(toX, toY))
  {
    double v;
    if (pFcstGrid.getValue(fromX, fromY, v))
    {
      pPhaseCorrectedGrid.setv(toX, toY, v);
    }
    else
    {
      pPhaseCorrectedGrid.setToMissing(toX, toY);
    }
  }
}

//----------------------------------------------------------------
void GridsForPc::fillGapsInPc(const int n)
{
  ILOGF(DEBUG_VERBOSE, "(%d)", n);
  pPhaseCorrectedGrid.fillGaps(n, n);
}

//----------------------------------------------------------------
void GridsForPc::smoothPc(const int n)
{
  ILOGF(DEBUG_VERBOSE, "(%d)", n);
  pPhaseCorrectedGrid.fastsmooth(n, n);
}

//----------------------------------------------------------------
bool GridsForPc::dimensionsConsistent(const GridsForPc &g) const
{
  if (pVerifGrid.sizeEqual(pFcstGrid) && 
      pVerifGrid.sizeEqual(g.pVerifGrid) &&
      pVerifGrid.sizeEqual(g.pFcstGrid))
  {
    return true;
  }
  else
  {
    return false;
  }
}
    
//----------------------------------------------------------------
bool GridsForPc::hasEnoughInput(const double minPcnt) const
{
  bool verif = pVerifGrid.pcntGe(pParm.pThresh) >= minPcnt;
  bool fcst = pFcstGrid.pcntGe(pParm.pThresh) >= minPcnt;
  return (verif && fcst);
}

//----------------------------------------------------------------
bool GridsForPc::fractionalAreaTest(const int x0, const int y0,
				    const int *volSize,
				    const double minPcnt) const
{
  // change parameter to lower precision just in case, 1 percent resolution
  // (for integer comparisons below)
  int imin = static_cast<int>(minPcnt*100.0);
  double n=0.0, ng=0.0;
  for (int y=y0; y<y0+volSize[1]; ++y)
  {
    for (int x=x0; x<x0 + volSize[0]; ++x)
    {
      if (pVerifGrid.inRange(x, y))
      {
	n++;
	double v;
	if (pVerifGrid.getValue(x, y, v))
	{
	  if (v >= pParm.pFractionalAreaDataThresh)
	  {
	    ++ng;
	  }
	}
      }
    }
  }
  if (n == 0.0)
  {
    return false;
  }
  else
  {
    int ip = static_cast<int>((ng/n)*100.0);
    return ip >= imin;
  }
}

//----------------------------------------------------------------
bool GridsForPc::buildScore(const int x0, const int y0, const int xoff,
			    const int yoff, const int *volSize,
			    PcScore &pcs) const
{
  // initialize score object for return
  pcs.init(pParm.pVariance, pParm.pAlpha, pName);

  double dmean, num;

  // build up smooth verification data mean in this volume
  dmean = num = 0.0;
  for (int iy=y0; iy<y0+volSize[1]; iy++)
  {
    for (int ix=x0; ix<x0+volSize[0]; ix++)
    {
      double v;
      if (sGetVal(ix, iy, pSmoothVerifGrid, v))
      {
	dmean += v;
	num ++;
      }
    }
  }
  // no data at all is bad
  if (num == 0)
  {
    return false;
  }

  // mean too small is no different than mean = 0.0 THIS DOES DEPEND ON
  // DATA RANGE! 
  dmean = dmean/num;
  if (dmean < pParm.pThresh)
  {
    dmean = 0.0;
  }

  // build up smooth verification variance in this volume
  for (int iy=y0; iy<y0+volSize[1]; iy++)
  {
    for (int ix=x0; ix<x0+volSize[0]; ix++)
    {
      double ddata;
      if (sGetVal(ix, iy, pSmoothVerifGrid, ddata))
      {
	if (ddata < pParm.pThresh)
	{
	  ddata = 0.0;
	}
      
	// get offset (if motion is xoff, yoff, data comes from back this far)
	sPcScoreIncrement(ddata, dmean, ix-xoff, iy-yoff, pSmoothFcstGrid, 
			  pParm.pThresh, pcs);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
Grid GridsForPc::initializeGrid(const string &name, const string &units,
				const double initialValue) const
{
  Grid g(pSmoothVerifGrid);
  g.changeName(name);
  g.changeUnits(units);
  g.setAllToValue(initialValue);
  return g;
}
