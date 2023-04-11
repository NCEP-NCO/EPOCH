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
 * @file UvOutput.cc
 */

//----------------------------------------------------------------
#include <cstdlib>
#include <cmath>
#include <string>
#include <ConvWx/UvOutput.hh>
#include <ConvWx/GridTraverse2d.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxFuzzy.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/ParmPcAlg.hh>
#include <ConvWx/XyRun.hh>
#include <ConvWx/VectorMaxCounts.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/PcInput.hh>
#include <ConvWx/PhaseCorrect.hh>
#include <ConvWx/MetaData.hh>
using std::string;
using std::vector;

//----------------------------------------------------------------
/**
 * @return variance
 * @param[in] u  data value
 * @param[in] ubar  Mean
 * @param[in] v  data value
 * @param[in] vbar  Mean
 */
static double sComputeVariance(const double u, const double ubar,
			       const double v, const double vbar)
{
  return ((u-ubar)*(u-ubar)) + ((v-vbar)*(v-vbar));
}

//----------------------------------------------------------------
UvOutput::UvOutput() : MetaDataXml(), UvUnits(UvUnits::PURE_PC_GRIDPTS)
{
  pUvAllZero = true;
}

//----------------------------------------------------------------
UvOutput::UvOutput(const UvUnits &units) :
  MetaDataXml(),
  UvUnits(units),
  pUGrid(),
  pVGrid(),
  pUvAllZero(true)
{
}

//----------------------------------------------------------------
UvOutput::UvOutput(const string &uname, const string &vname,
		   const string &units, const int nx, const int ny,
		   const UvUnits &uvUnits) :
  MetaDataXml(),
  UvUnits(uvUnits),
  pUGrid(uname, units, nx, ny, convWx::UV_MISSING),
  pVGrid(vname, units, nx, ny, convWx::UV_MISSING),
  pUvAllZero(true)
{
  pUGrid.setAllToValue(0.0);
  pVGrid.setAllToValue(0.0);
}

//----------------------------------------------------------------
UvOutput::UvOutput(const Grid &templateGrid, const string &ufield,
		   const string &vfield, const string &units,
		   const string &defaultUfield, const string &defaultVfield,
		   const UvUnits &uvUnits) :
  MetaDataXml(),
  UvUnits(uvUnits),
  pUGrid(templateGrid, ufield, units),
  pVGrid(templateGrid, vfield, units),
  pUvAllZero(true)
{
  if (ufield.empty() || vfield.empty())
  {
    // if specific names, use them
    pUGrid.changeNameAndUnits(defaultUfield, units);
    pVGrid.changeNameAndUnits(defaultVfield, units);
  }

  // all missing
  pUGrid.setAllMissing();
  pVGrid.setAllMissing();
  pUGrid.changeMissing(convWx::UV_MISSING);
  pVGrid.changeMissing(convWx::UV_MISSING);
}

//----------------------------------------------------------------
UvOutput::UvOutput(const Grid &u, const Grid &v, const UvUnits &uvUnits,
		   const MetaDataXml &xml) :
  MetaDataXml(xml),
  UvUnits(uvUnits),
  pUGrid(u),
  pVGrid(v),
  pUvAllZero(false)
{

}

//----------------------------------------------------------------
UvOutput::~UvOutput()
{
}

//----------------------------------------------------------------
bool UvOutput::operator==(const UvOutput &u) const
{
  if (!MetaDataXml::operator==(u))
  {
    return false;
  }
  if (!UvUnits::operator==(u))
  {
    return false;
  }
  if (!(pUGrid == u.pUGrid))
  {
    return false;
  }
  if (!(pVGrid == u.pVGrid))
  {
    return false;
  }
  if (pUvAllZero != u.pUvAllZero)
  {
    return false;
  }
  return true;
}


//----------------------------------------------------------------
void UvOutput::pcCompute(const std::string &name, int lt, const Grid *verif, 
			 const Grid *fcst, const Grid *weight, 
			 const ParmSetUV &parms, const ParmProjection &proj,
			 MetaData &metadata, PcStats &stats,
			 ConvWxMultiThreadMgr *threads)
{
  // Set up for call to PhaseCorrect constructor
      
  PcInput p0(name, verif, fcst, parms.pFcst[0]);

  vector<PcInput> inp;
  inp.push_back(p0);
  UvUnits units(UvUnits::PURE_PC_GRIDPTS);

  ILOG(DEBUG_VERBOSE, "Begin computing phase correction vectors");

  // Create a PhaseCorrect object
  PhaseCorrect pc(inp, *weight, parms, units);

  // Do the phase correction computation
  pc.computePhaseCorrect(proj, lt, threads);

  // Get the phase correction results u,v, and some metadata
  metadata = MetaData();
  pc.getOutput(proj, pUGrid, pVGrid, metadata, stats);

  // The U,V units are at this point meters/second (external)
  MetaDataXml::operator=(metadata.xmlMetadata());
  UvUnits::operator=(UvUnits(UvUnits::METERS_PER_SECOND));
}

//----------------------------------------------------------------
void UvOutput::setLocalCollapseZero(const ParmPcAlg &parm,
				    const ParmProjection &proj)
{
  gridExternalToLocal(proj.kmPerPixelX(), pUGrid);
  gridExternalToLocal(proj.kmPerPixelY(), pVGrid);
  UvUnits::operator=(UvUnits(UvUnits::PURE_PC_GRIDPTS));

  double zeroRes = externalToLocal(parm.pZeroResolution, proj.kmPerPixel());
  pUGrid.setDataInRangeToValue(-zeroRes, zeroRes, 0.0);
  pVGrid.setDataInRangeToValue(-zeroRes, zeroRes, 0.0);
  pUvAllZero = (pUGrid.allDataMissingOrEqual(0.0) && 
		pVGrid.allDataMissingOrEqual(0.0));
}

//----------------------------------------------------------------
void UvOutput::pcApply(const ParmPcAlg &parms, const Grid &data, Grid &pc)
{
  switch (MetaDataXml::getPcType())
  {
  case PcType::NO_PC_NO_UV_OR_ZERO_UV_DATA:
    // no need to do p.c.,  will have no effect
    pc = data;
    break;
  case PcType::NO_PC_LOW_SKILL:
    if (parms.pAlwaysPhaseCorrect)
    {
      // when outputting non-verified, want to see what happened anyway
      // so do the phase correction
      pcForceApply(parms, 1.0, data, pc);
    }
    else
    {
      // no need to do phase correction in this case
      pc = data;
    }
    break;
  default:
    // need to do phase correction in this case 
    pcForceApply(parms, 1.0, data, pc);
    break;
  }
}


//-------------------------------------------------------------------
void UvOutput::pcForceApply(const ParmPcAlg &parms, const double weight,
			    const Grid &data, Grid &pc)
{
  InterfaceLL::doRegister("PcApply::pPcDo");

  // phase correction now using uv.
  ILOG(DEBUG, "Doing phase correction now");
  pc = data;

  // IMPORTANT: assumed external format shows up in the units, checked
  // earlier. This UvOutput constructor converts the units to 'pure p.c.'
  // which is needed in the phaseCorrect method
  pPhaseCorrect(weight, parms.pGapfill, parms.pSmooth, parms.pUseHighSmooth,
		parms.pSmoothWhereCorrected, parms.pExcludeWhenSmoothing,
		parms.pSmoothExclude, pc);
  InterfaceLL::doRegister("Done with phase correction now");
}

//----------------------------------------------------------------
bool UvOutput::isZero(void) const
{
  return pUvAllZero;
}

//----------------------------------------------------------------
void UvOutput::setAllZero(void)
{
  pUvAllZero = true;
  pUGrid.setAllToValue(0.0);
  pVGrid.setAllToValue(0.0);
}

//----------------------------------------------------------------
bool UvOutput::getValuesOrZero(const int x,  const int y, double &u,
			       double &v) const
{
  bool stat = true;
  if (!pUGrid.getValue(x, y, u))
  {
    stat = false;
  }
  if (!pVGrid.getValue(x, y, v))
  {
    stat = false;
  }
  if (!stat)
  {
    u = v = 0.0;
  }
  return u != 0.0 || v != 0.0;
}

//----------------------------------------------------------------
void UvOutput::setValuesAtPoint(const int x, const int y, const double u,
				const double v)
{
  pUGrid.setv(x, y, u);
  pVGrid.setv(x, y, v);
}

//----------------------------------------------------------------
void UvOutput::setUGrid(const Grid &uGrid, const UvUnits &units)
{
  pUGrid = uGrid;
  UvUnits::operator=(units);
}

//----------------------------------------------------------------
void UvOutput::setVGrid(const Grid &vGrid, const UvUnits &units)
{
  pVGrid = vGrid;
  UvUnits::operator=(units);
}

//----------------------------------------------------------------
void UvOutput::setNames(const string &uName, const string &vName)
{
  pUGrid.changeName(uName);
  pVGrid.changeName(vName);
}

//----------------------------------------------------------------
void UvOutput::getRange(double &minVal, double &maxVal) const
{
  double minv2, maxv2;
  pUGrid.getRange(minVal, maxVal);
  pVGrid.getRange(minv2, maxv2);
  if (minv2 < minVal)
  {
    minVal = minv2;
  }
  if (maxv2 > maxVal)
  {
    maxVal = maxv2;
  }
}

//----------------------------------------------------------------
bool UvOutput::multiply(const Grid &multGrid)
{
  bool ret = true;
  if (!pUGrid.multiply(multGrid))
  {
    ret = false;
  }
  if (!pVGrid.multiply(multGrid))
  {
    ret = false;
  }
  return ret;
}

//----------------------------------------------------------------
void UvOutput::multiply(const double scale)
{
  int nx, ny;
  pUGrid.getDim(nx, ny);
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      pUGrid.multiply(x, y, scale);
      pVGrid.multiply(x, y, scale);
    }
  }
}

//----------------------------------------------------------------
void UvOutput::multiply(const int x, const int y, const double scale)
{
  pUGrid.multiply(x, y, scale);
  pVGrid.multiply(x, y, scale);
}

//----------------------------------------------------------------
void UvOutput::multiply(const double uScale, const double vScale)
{
  int nx, ny;
  pUGrid.getDim(nx, ny);
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      pUGrid.multiply(x, y, uScale);
      pVGrid.multiply(x, y, vScale);
    }
  }
}

//----------------------------------------------------------------
bool UvOutput::normalize(const Grid &normGrid)
{
  int nx, ny;
  pUGrid.getDim(nx, ny);
  int nnx, nny;
  normGrid.getDim(nnx, nny);
  if (nnx != nx || nny != ny)
  {
    ILOGF(ERROR, "norm grid %d,%d  local:%d,%d", nnx, nny, nx, ny);
    return false;
  }
  
  double nuv;
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      if (normGrid.getValue(x, y, nuv))
      {
	if (nuv > 0)
	{
	  pUGrid.multiply(x, y, 1.0/nuv);
	  pVGrid.multiply(x, y, 1.0/nuv);
	}
	else
	{
	  pUGrid.setv(x, y, 0.0);
	  pVGrid.setv(x, y, 0.0);
	}
      }
      else
      {
	pUGrid.setv(x, y, 0.0);
	pVGrid.setv(x, y, 0.0);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
void UvOutput::smooth(const int sx, const int sy)
{
  pUGrid.fastsmooth(sx, sy);
  pVGrid.fastsmooth(sx, sy);
}

//----------------------------------------------------------------
void UvOutput::smooth(const int sx, const int sy, bool excludeZero)
{
  pUGrid.fastsmooth(sx, sy, excludeZero, 0.0, false);
  pVGrid.fastsmooth(sx, sy, excludeZero, 0.0, false);
}

//----------------------------------------------------------------
bool UvOutput::reduce(const int res)
{
  bool ret = true;
  if (!pUGrid.reduce(res))
  {
    ret = false;
  }
  if (!pVGrid.reduce(res))
  {
    ret = false;
  }
  return ret;
}


//----------------------------------------------------------------
Grid UvOutput::magnitude(void) const
{
  // note that we exclude points where either u or v is missing.
  Grid mag(pUGrid);
  mag.setAllMissing();
  int nx, ny;
  pUGrid.getDim(nx, ny);
  for (int iy=0; iy<ny; ++iy)
  {
    for (int ix=0; ix<nx; ++ix)
    {
      double u;
      if (pUGrid.getValue(ix, iy, u))
      {
	double v;
	if (pVGrid.getValue(ix, iy, v))
	{
	  double m = u*u + v*v;
	  mag.setv(ix, iy, m);
	}
      }
    }
  }
  return mag;
}

//----------------------------------------------------------------
const Grid &UvOutput::getU(void) const
{
  return pUGrid;
}

//----------------------------------------------------------------
const Grid &UvOutput::getV(void) const
{
  return pVGrid;
}

//----------------------------------------------------------------
bool UvOutput::interpolate(const UvOutput &lowresMotion, const int res)
{
  if (!pUGrid.interpolate(lowresMotion.pUGrid, res))
  {
    return false;
  }
  if (!pVGrid.interpolate(lowresMotion.pVGrid, res))
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------
void UvOutput::expand(const int boxSize)
{
  vectorMaxFast(boxSize, boxSize);
}

//----------------------------------------------------------------
void UvOutput::vectorMaxFast(const int sx, const int sy)
{
  // make a mag grid, since we look at those values multiple times
  // note that we exclude points where either u or v is missing.
  Grid mag = magnitude();

  int nx, ny;
  pUGrid.getDim(nx, ny);

  // make an object to loop through the grids
  GridTraverse2d traverse(nx, ny, sx, sy);

  // make temp objects
  UvOutput uv2 = *this;
  
  // build initial max state from this objects initial state
  UvMax_t stat;
  pVectorMaxSet(mag, traverse, stat, uv2);
    
  VectorMaxCounts v;

  // initialize counters
  v.init();

  // now loop through and try to do the minimum to find a new maximum as we go
  while (traverse.increment2d())
  {
    pVectorMax1(traverse, mag, stat, uv2, v);
  }

  v.log(LogStream::DEBUG_VERBOSE);
  *this = uv2;
}

//----------------------------------------------------------------
void UvOutput::convergence(Grid &conv, Grid &du, Grid &dv,
			   const ParmProjection &proj) const
{
  conv = Grid(pUGrid, "convergence", "m/s");
  du = Grid(pUGrid, "du", "m/s");
  dv = Grid(pUGrid, "dv", "m/s");
  conv.setAllMissing();
  du.setAllMissing();
  dv.setAllMissing();
  int nx, ny;
  pUGrid.getDim(nx, ny);
  for (int y0=0; y0<ny; ++y0)
  {
    for (int x0=0; x0<nx; ++x0)
    {
      double dui, dvi;
      if (pConvergence1(x0,  y0, nx, ny, dui, dvi))
      {
	double v = dui + dvi;
	v = -1*localToExternal(v, proj.kmPerPixel());
	conv.setv(x0, y0, v);
	dv.setv(x0, y0, -dvi);
 	du.setv(x0, y0, -dui);
      }
    }
  }
}

//----------------------------------------------------------------
bool UvOutput::convThreshFilter(const double convThresh,
				const ConvWxFuzzy &nptToExpansion,
				const Grid &du, const Grid &dv)
{
  int nx, ny;
  if (!pGetConsistentDimensions(du, dv, nx, ny))
  {
    return false;
  }

  XyRun runX(true, convThresh, nptToExpansion, nx, ny);
  Grid tmp(pUGrid);

  // now loop through looking for runs where convergence exceeds thresh
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      double v;
      if (du.getValue(x, y, v))
      {
	runX.process(x, y, v, pUGrid, tmp);
      }
      else
      {
	runX.endRun(x, y, pUGrid, tmp);
      }
    }
  }

  // replace pUGrid with tmp
  pUGrid = tmp;

  tmp = pVGrid;
  XyRun runY(false, convThresh, nptToExpansion, nx, ny);
  for (int x=0; x<nx; ++x)
  {
    for (int y=0; y<ny; ++y)
    {
      double v;
      if (dv.getValue(x, y, v))
      {
	runY.process(x, y, v, pVGrid, tmp);
      }
      else
      {
	runY.endRun(x, y, pVGrid, tmp);
      }
    }
  }

  // replace pVGrid with tmp
  pVGrid = tmp;
  return true;
}

//----------------------------------------------------------------
void UvOutput::replaceLeThreshOrMissing(const UvOutput &replacementUV,
					const double threshold)
{
  int n = pUGrid.getNdata();
  int n2 = replacementUV.pUGrid.getNdata();
  if (n != n2)
  {
    ILOGF(ERROR, "uneven dim %d %d", n, n2);
    return;
  }

  double tt = threshold*threshold;
  for (int i=0; i<n; ++i)
  {
    double ui, vi;
    bool replace = false;
    if (pUGrid.getValue(i, ui))
    {
      if (pVGrid.getValue(i, vi))
      {
	if (ui*ui + vi*vi <= tt)
	{
	  replace = true;
	}
      }
      else
      {
	replace = true;
      }
    }
    else
    {
      replace = true;
    }
    if (replace)
    {
      if (replacementUV.pUGrid.getValue(i, ui))
      {
	if (replacementUV.pVGrid.getValue(i, vi))
	{
	  pUGrid.setv(i, ui);
	  pVGrid.setv(i, vi);
	}
      }
    }
  }
}

//----------------------------------------------------------------
void UvOutput::timeTaper(const double weight)
{
  if (weight != 1.0)
  {
    pUGrid.multiply(weight);
    pVGrid.multiply(weight);
  }
}

//----------------------------------------------------------------
double UvOutput::maxMagnitudeInBox(const int x0, const int y0, const int nx, 
				   const int ny) const
{
  double wsmax = 0.0;
  int gridNx, gridNy;
  pUGrid.getDim(gridNx, gridNy);
  for (int iy=y0; iy<y0+ny; ++iy)
  {
    if (iy < 0 || iy >= gridNy)
    {
      continue;
    }
    for (int ix=x0; ix<x0+nx; ++ix)
    {
      if (ix < 0 || ix >= gridNx)
      {
	continue;
      }
      double u;
      if (pUGrid.getValue(ix, iy, u))
      {
	double v;
	if (pVGrid.getValue(ix, iy, v))
	{
	  double ws = u*u + v*v;
	  if (ws > wsmax)
	  {
	    wsmax = ws;
	  }
	}
      }
    }
  }
  return sqrt(wsmax);
}

//----------------------------------------------------------------
bool UvOutput::meanInBox(const int x0, const int y0, const int nx, 
			 const int ny, double &ubar, double &vbar) const
{
  ubar = 0.0;
  vbar = 0.0;
  double denom = 0.0;
  int gridNx, gridNy;
  pUGrid.getDim(gridNx, gridNy);
  for (int iy=y0; iy<y0+ny; ++iy)
  {
    if (iy < 0 || iy >= gridNy)
    {
      continue;
    }
    for (int ix=x0; ix<x0+nx; ++ix)
    {
      if (ix < 0 || ix >= gridNx)
      {
	continue;
      }
      double u;
      if (pUGrid.getValue(ix, iy, u))
      {
	double v;
	if (pVGrid.getValue(ix, iy, v))
	{
	  ubar += u;
	  vbar += v;
	  denom += 1.0;
	}
      }
    }
  }
  if (denom == 0.0)
  {
    return false;
  }
  else
  {
    ubar /= denom;
    vbar /= denom;
    return true;
  }
}

//----------------------------------------------------------------
double UvOutput::varianceInBox(const int x0, const int y0, const int nx, 
			       const int ny) const
{
  double ubar, vbar;
  if (meanInBox(x0,  y0, nx, ny, ubar, vbar))
  {
    return varianceInBox(x0, y0, nx, ny, ubar, vbar);
  }
  else
  {
    return 0.0;
  }
}

//----------------------------------------------------------------
double UvOutput::varianceInBox(const int x0, const int y0, const int nx, 
			       const int ny, const double &ubar, 
			       const double &vbar) const
{
  double num=0.0, denom=0.0;
  int gridNx, gridNy;
  pUGrid.getDim(gridNx, gridNy);
  for (int iy=y0; iy<y0+ny; ++iy)
  {
    if (iy < 0 || iy >= gridNy)
    {
      continue;
    }
    for (int ix=x0; ix<x0+nx; ++ix)
    {
      if (ix < 0 || ix >= gridNx)
      {
	continue;
      }
      double u;
      if (pUGrid.getValue(ix, iy, u))
      {
	double v;
	if (pVGrid.getValue(ix, iy, v))
	{
	  double vij = sComputeVariance(u, ubar, v, vbar);
	  num += vij;
	  denom += 1.0;
	}
      }
    }
  }
  if (denom > 0)
  {
    return num/denom;
  }
  else
  {
    return 0.0;
  }
}

//----------------------------------------------------------------
void UvOutput::copyBox(const UvOutput &inputUV, const int x0, const int y0,
		       const int nx, const int ny)
{
  int gridNx, gridNy;
  pUGrid.getDim(gridNx, gridNy);
  for (int iy=y0; iy<y0+ny; ++iy)
  {
    if (iy < 0 || iy >= gridNy)
    {
      continue;
    }
    for (int ix=x0; ix<x0+nx; ++ix)
    {
      if (ix < 0 || ix >= gridNx)
      {
	continue;
      }
      double u;
      if (inputUV.pUGrid.getValue(ix, iy, u))
      {
	double v;
	if (inputUV.pVGrid.getValue(ix, iy, v))
	{
	  pUGrid.setv(ix, iy, u);
	  pVGrid.setv(ix, iy, u);
	}
	else
	{
	  pUGrid.setToMissing(ix, iy);
	  pVGrid.setToMissing(ix, iy);
	}
      }
      else
      {
	pUGrid.setToMissing(ix, iy);
	pVGrid.setToMissing(ix, iy);
      }
    }
  }
}
  
//----------------------------------------------------------------
void UvOutput::normalizeInBox(const int x0, const int y0, const int nx, 
			      const int ny, const double scale)
{
  int gridNx, gridNy;
  pUGrid.getDim(gridNx, gridNy);
  for (int iy=y0; iy<y0+ny; ++iy)
  {
    if (iy < 0 || iy >= gridNy)
    {
      continue;
    }
    for (int ix=x0; ix<x0+nx; ++ix)
    {
      if (ix < 0 || ix >= gridNx)
      {
	continue;
      }
      pUGrid.multiply(ix, iy, scale);
      pVGrid.multiply(ix, iy, scale);
    }
  }
}

//----------------------------------------------------------------
void UvOutput::incrementValueAtPoint(const int x, const int y,
				     const int iu, const int iv)
{
  pUGrid.incrementValueAtPoint(x, y, static_cast<double>(iu));
  pVGrid.incrementValueAtPoint(x, y, static_cast<double>(iv));
}

//----------------------------------------------------------------
void UvOutput::appendOutput(const string &uname, const string &vname,
			    const double dxKm, const double dyKm,
			    MultiGrid &grids) const
{
  if (uname.empty() || vname.empty())
  {
    return;
  }
  Grid gu = Grid(pUGrid, uname, "m/s");
  Grid gv = Grid(pVGrid, uname, "m/s");
  pAppend(dxKm, dyKm, gu, gv, grids);
}
  
//----------------------------------------------------------------
void UvOutput::appendOutput(const double dxKm, const double dyKm,
			    MultiGrid &grids) const
{
  Grid gu(pUGrid);
  Grid gv(pVGrid);
  
  pAppend(dxKm, dyKm, gu, gv, grids);
}

//----------------------------------------------------------------
void UvOutput::getOutput(const double dxKm, const double dyKm,
			 Grid &uGrid, Grid &vGrid) const
{
  double scale;
  uGrid = pUGrid;
  vGrid = pVGrid;
  switch (pUnits)
  {
  case UvUnits::PURE_PC_GRIDPTS:
    // the original, u and v are phase corrections to be stored out.
    // convert to external units.
    gridLocalToExternal(dxKm, uGrid);
    gridLocalToExternal(dyKm, vGrid);
    break;
  case UvUnits::GRIDPTS_PER_DT_SECONDS:
    // the original, u and v are phase corrections to be stored out,
    // in units of grid points per pDtSeconds, convert to meters per second
    // here. m/s = meters per grid points / pDtSeconds
    scale = dxKm*convWx::METERS_PER_KM/static_cast<double>(pDtSeconds);
    uGrid.multiply(scale);
    scale = dyKm*convWx::METERS_PER_KM/static_cast<double>(pDtSeconds);
    vGrid.multiply(scale);
    break;
  case UvUnits::METERS_PER_SECOND:
    // nothing to do, meters per second is the external units
    break;
  default:
    ILOGF(ERROR, "bad local units %d", static_cast<int>(pUnits));
  }
}

//----------------------------------------------------------------
void UvOutput::debugAtPoint(const string &name, const int *x,
			    const int *y) const
{
  double v;
  if (pUGrid.getValue(x[0],  y[0], v))
  {
    printf("%s U:%.5lf\n", name.c_str(), v);
  }
  else
  {
    printf("%s U:missing\n", name.c_str());
  }
  if (pVGrid.getValue(x[0],  y[0], v))
  {
    printf("%s V:%.5lf\n", name.c_str(), v);
  }
  else
  {
    printf("%s V:missing\n", name.c_str());
  }
}

//----------------------------------------------------------------
void UvOutput::gridExternalToLocal(double kmPerGridpt, Grid &grid)
{
  double scale =
    static_cast<double>(pExternalSeconds())/(kmPerGridpt*convWx::METERS_PER_KM);
  grid.multiply(scale);
}

//----------------------------------------------------------------
void UvOutput::gridLocalToExternal(double kmPerGridpt, Grid &grid)
{
  double scale =
    kmPerGridpt*convWx::METERS_PER_KM/static_cast<double>(pExternalSeconds());
  grid.multiply(scale);
}

//----------------------------------------------------------------
double UvOutput::externalToLocal(const double value, const double kmPerGridpt)
{
  // this is from 'meters per second' into local units
  // 'meters per second' is for display purposes
  return value*
    static_cast<double>(pExternalSeconds())/(kmPerGridpt*convWx::METERS_PER_KM);
}

//----------------------------------------------------------------
double UvOutput::localToExternal(const double value, const double kmPerGridpt)
{
  // this is from local units into 'meters per second'
  // 'meters per second' is for display purposes
  return value*
    kmPerGridpt*convWx::METERS_PER_KM/static_cast<double>(pExternalSeconds());
}

//----------------------------------------------------------------
void UvOutput::pVectorMax1(const GridTraverse2d &gridTraverse,
			   const Grid &magGrid, 
			   UvMax_t &maxStatus, UvOutput &outUv,
			   VectorMaxCounts &counter)
{
  ++counter.pNumTotal;

  //adjust or set max
  bool adjust = pVectorMaxShouldAdjust(maxStatus, gridTraverse, counter);
  if (adjust)
  {
    pVectorMaxAdjust("adjust", magGrid, gridTraverse.newBox(), 
		     gridTraverse.hasNew(), gridTraverse, maxStatus, outUv);
  }
  else
  {
    pVectorMaxSet(magGrid, gridTraverse, maxStatus, outUv);
    ++counter.pNumWithRedo;
  }
}

//----------------------------------------------------------------
bool UvOutput::pVectorMaxShouldAdjust(const UvMax_t &maxStatus,
				      const GridTraverse2d &gridTraverse,
				      VectorMaxCounts &counter) const
{
  if (!maxStatus.valuesSet)
  {
    // look to new points for adjustments
    ++counter.pNumNoPreviousStat;
    return true;
  }

  if (maxStatus.uvMagnitude == 0.0)
  {
    // 0.0 is the lowest possible value for a vector magnitude.
    //
    // if previous max is our lowest possible value, check new stuff
    // only for something bigger than that. (rest of the box also not
    // greater than lowest possible value.)
    ++counter.pNumAtLowestValue;
    return true;
  }

  // here previous max is not our lowest possible value, if it is one
  // of the 'old' points, we need to start over, as we are throwing
  // it away. otherwise, we can just check new stuff.
  if (gridTraverse.isInOldBox(maxStatus.gridX, maxStatus.gridY))
  {
    // status x,y is in range of old..redo needed
    ++counter.pNumWherePreviousIsOldest;
    return false;
  }
  else
  {
    ++counter.pNumWherePreviousNotOldest;
    return true;
  }
}

//----------------------------------------------------------------
void UvOutput::pVectorMaxSet(const Grid &magGrid,
			     const GridTraverse2d &gridTraverse,
			     UvMax_t &maxStatus, UvOutput &outUv)
{
  maxStatus.valuesSet = false;
  pVectorMaxAdjust("set", magGrid, gridTraverse.currentBox(), true, 
		   gridTraverse, maxStatus, outUv);
}

//----------------------------------------------------------------
void UvOutput::pVectorMaxAdjust(const string &name, const Grid &magGrid, 
				const GridTraverse2d::Range_t &range,
				const bool hasRange,
				const GridTraverse &gridTraverse,
				UvMax_t &maxStatus, UvOutput &outUv)
{
  // get the max value in the magGrid field, and where it occured
  if (hasRange)
  {
    for (int y=range.miny; y<=range.maxy; ++y)
    {
      for (int x=range.minx; x<=range.maxx; ++x)
      {
	double v;
	if (magGrid.getValue(x, y, v))
	{
	  pVectorMaxAdjustStat(x, y, v, maxStatus);
	}
      }
    }
  }

  // get center point x, y
  int x, y;
  gridTraverse.getXy(x, y);
  if (!maxStatus.valuesSet)
  {
    outUv.pUGrid.setToMissing(x, y);
    outUv.pVGrid.setToMissing(x, y);
  }
  else
  {
    outUv.pUGrid.setv(x, y, maxStatus.uValue);
    outUv.pVGrid.setv(x, y, maxStatus.vValue);
  }
}

//----------------------------------------------------------------
void UvOutput::pVectorMaxAdjustStat(const int x, const int y, const double v,
				    UvMax_t &maxStatus)
{
  if (!maxStatus.valuesSet)
  {
    maxStatus.valuesSet = true;
    maxStatus.uvMagnitude = v;
    maxStatus.gridX = x;
    maxStatus.gridY = y;
    pUGrid.getValue(maxStatus.gridX, maxStatus.gridY, maxStatus.uValue);
    pVGrid.getValue(maxStatus.gridX, maxStatus.gridY, maxStatus.vValue);
  }
  else
  {
    if (v > maxStatus.uvMagnitude)
    {
      maxStatus.uvMagnitude = v;
      maxStatus.gridX = x;
      maxStatus.gridY = y;
      pUGrid.getValue(maxStatus.gridX, maxStatus.gridY, maxStatus.uValue);
      pVGrid.getValue(maxStatus.gridX, maxStatus.gridY, maxStatus.vValue);
    }
  }
}


//----------------------------------------------------------------
void UvOutput::pSetMaskandOutput(const int x, const int y,
				 const double uvWeight,
				 const double value,
				 Grid &maskGrid, Grid &outGrid) const
{
  double u, v;

  if (!pNonZeroUV(x, y, uvWeight, u, v))
  {
    return;
  }

  // u/v is non-zero at this point
  // the forecast at this point is moving away. Put in a special value to use later
  outGrid.setv(x, y, value);

  // indicate this point had something in the mask
  maskGrid.setv(x, y, value);
}

//----------------------------------------------------------------
void UvOutput::pPhaseCorrect(const double uvWeight, const int gapFill[2],
			     const int smooth[2], const bool useHighSmooth,
			     const bool smoothWhereCorrected,
			     const bool isExclude, const double excludeValue,
			     Grid &dataGrid) const
{
  if (uvWeight == 0.0)
  {
    return;
  }

  Grid out(dataGrid);
  Grid mask(dataGrid);
  mask.setAllToValue(0.0);

  int nx, ny;
  pUGrid.getDim(nx, ny);

  // this value is not expected in the data
  double specialValue = 1000.0;

  // set special value into output where moving 'away'
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      pSetMaskandOutput(x, y, uvWeight, specialValue, mask, out);
    }
  }

  // move forecast at from locations into to locations.
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      pPhaseCorrect1(x, y, uvWeight, dataGrid, specialValue, mask, out);
    }
  }

  // out now has special value at all points moved from and not later moved to.
  // replace those with something nearby
  out.replaceValueWithNearestMax(specialValue);

  // gap filling algorithm
  out.fillGaps(gapFill[0], gapFill[1]);

  if (useHighSmooth)
  {
    if (smoothWhereCorrected)
    {
      out.highSmoothInMask(mask, specialValue, smooth[0], smooth[1], isExclude,
			   excludeValue);
    }
    else
    {
      out.highSmooth(smooth[0], smooth[1], isExclude, excludeValue);
    }
  }
  else
  {
    if (smoothWhereCorrected)
    {
      out.smoothInMask(mask, specialValue, smooth[0], smooth[1], isExclude,
		       excludeValue);
    }
    else
    {
      out.smooth(smooth[0], smooth[1], isExclude, excludeValue);
    }
  }
  dataGrid = out;
}

//----------------------------------------------------------------
void UvOutput::pPhaseCorrect1(const int x, const int y, 
			      const double  uvWeight, 
			      const Grid &dataGrid, const double value,
			      Grid &maskGrid, Grid &outGrid) const
{
  double u, v;
  if (!pNonZeroUV(x,  y, uvWeight, u, v))
  {
    return;
  }

  // u/v is non-zero at this point
  // get moved location fx,fy
  int fx = static_cast<int>(round(static_cast<double>(x) + u));
  int fy = static_cast<int>(round(static_cast<double>(y) + v));
  if (!outGrid.inRange(fx, fy))
  {
    return;
  }

  // set mask at 'to' location
  maskGrid.setv(fx, fy, value);

  // move data value at x,y into output at fx,fy
  if (dataGrid.getValue(x, y, v))
  {
    outGrid.setv(fx, fy, v);
  }
  else
  {
    outGrid.setToMissing(fx, fy);
  }
}

//----------------------------------------------------------------
bool UvOutput::pNonZeroUV(const int x, const int y, const double uvWeight, 
			  double &u, double &v) const
{
  // see if u/v is non-zero at this point
  if (!pUGrid.getValue(x, y, u))
  {
    u = 0;
  }
  if (!pVGrid.getValue(x, y, v))
  {
    v = 0;
  }
  u = u*uvWeight;
  v = v*uvWeight;
  return !(u == 0 && v == 0);
}

//----------------------------------------------------------------
bool UvOutput::pGetConsistentDimensions(const Grid &duGrid, const Grid &dvGrid,
					int &nx, int &ny) const
{
  int cnx, cny;
  pUGrid.getDim(nx, ny);
  duGrid.getDim(cnx, cny);
  if (cnx != nx || cny != ny)
  {
    ILOGF(ERROR, "dims unequal %d,%d  %d,%d", cnx, cny, nx ,ny);
    return false;
  
  }
  dvGrid.getDim(cnx, cny);
  if (cnx != nx || cny != ny)
  {
    ILOGF(ERROR, "dims unequal %d,%d  %d,%d", cnx, cny, nx ,ny);
    return false;
  }
  return true;
}

//----------------------------------------------------------------
bool UvOutput::pConvergence1(const int x0, const int y0,
			     const int nx, const int ny,
			     double &du, double &dv) const
{
  if (y0 < 1 || y0 >= ny-2 || x0 < 1 || x0 >= nx-2)
  {
    return false;
  }
  double u0, u1, v0, v1;
  if (!pUGrid.getValue(x0-1, y0, u0))
  {
    return false;
  }
  if (!pUGrid.getValue(x0+1, y0, u1))
  {
    return false;
  }
  if (!pVGrid.getValue(x0, y0-1, v0))
  {
    return false;
  }
  if (!pVGrid.getValue(x0, y0+1, v1))
  {
    return false;
  }
  dv = (v1-v0)/2.0;
  du = (u1-u0)/2.0;
  return true;
}

//----------------------------------------------------------------
void UvOutput::pAppend(const double dxKm, const double dyKm,
		       Grid &uGrid, Grid &vGrid, MultiGrid &outGrids) const
{
  double scale;

  switch (pUnits)
  {
  case UvUnits::PURE_PC_GRIDPTS:
    // the original, u and v are phase corrections to be stored out.
    // convert to external units.
    gridLocalToExternal(dxKm, uGrid);
    outGrids.append(uGrid);
    gridLocalToExternal(dyKm, vGrid);
    outGrids.append(vGrid);
    break;
  case UvUnits::GRIDPTS_PER_DT_SECONDS:
    // the original, u and v are phase corrections to be stored out,
    // in units of grid points per pDtSeconds, convert to meters per second
    // here. m/s = meters per gridpoints / pDtSeconds
    scale = dxKm*convWx::METERS_PER_KM/static_cast<double>(pDtSeconds);
    uGrid.multiply(scale);
    outGrids.append(uGrid);
    scale = dyKm*convWx::METERS_PER_KM/static_cast<double>(pDtSeconds);
    vGrid.multiply(scale);
    outGrids.append(vGrid);
    break;
  case UvUnits::METERS_PER_SECOND:
    // append as is, meters per second is the external units
    outGrids.append(uGrid);
    outGrids.append(vGrid);
    break;
  default:
    ILOGF(ERROR, "bad local units %d", static_cast<int>(pUnits));
  }
}

//----------------------------------------------------------------
int UvOutput::pExternalSeconds(void)
{
  return convWx::SECS_PER_HOUR;
}

