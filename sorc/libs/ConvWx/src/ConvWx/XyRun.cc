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
 * @file XyRun.cc
 */
#include <ConvWx/XyRun.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <string>

//----------------------------------------------------------------
XyRun::XyRun(const bool isX, const double convThresh,
	     const ConvWxFuzzy &nptToExpansion, const int nx, const int ny) :
  pIsX(isX),
  pConvThresh(convThresh),
  pNptToExpansion(nptToExpansion),
  pNx(nx),
  pNy(ny),
  pInside(false),
  pMin(0),
  pMax(0),
  pNum(0),
  pSum(0)
{
}

//----------------------------------------------------------------
XyRun::~XyRun()
{
}

//----------------------------------------------------------------
void XyRun::process(const int x, const int y, const double v, 
		    const Grid &u, Grid &g)
{
  if (v >= pConvThresh)
  {
    pExtendRun(x, y);
  }
  else
  {
    if (pInside)
    {
      endRun(x, y, u, g);
    }
  }
}

//----------------------------------------------------------------
void XyRun::endRun(const int x, const int y, const Grid &u, Grid &g)
{
  if (pInside)
  {
    pInside = false;
    if (pIsX)
    {
      pProcessRun(y, pNx, u, g);
    }
    else
    {
      pProcessRun(x, pNy, u, g);
    }
  }
}

//----------------------------------------------------------------
void XyRun::pExtendRun(const int x, const int y)
{
  int ind;
  if (pIsX)
  {
    ind = x;
  }
  else
  {
    ind = y;
  }

  if (pInside)
  {
    pMax = ind;
  }
  else
  {
    pInside = true;
    pMin = pMax = ind;
  }
}

//----------------------------------------------------------------
void XyRun::pProcessRun(const int otherInd, const int indMax, const Grid &data,
			Grid &g)
{
  int npt = pMax - pMin + 1;
  double expansion = pNptToExpansion.fuzzyF(static_cast<double>(npt));
  if (expansion <= 0.0)
  {
    // nothing to do
    return;
  }

  int filtWidth = static_cast<int>(static_cast<double>(npt)*expansion);
  int ipt0, ipt1, iptc;
  ipt0 = pMin - 2*filtWidth;
  iptc = pMin - filtWidth;
  ipt1 = pMin;
  
  // build up the first sum, from back up through pMin, centered halfway
  pNum = 0.0;
  pSum = 0.0;
  for (int ind=ipt0; ind<=ipt1; ++ind)
  {
    incSum(ind, indMax, otherInd, data);
  }

  // now do the algorithm. add in at leading end, subtract at tailing end
  for (; iptc <= pMax + filtWidth; ++iptc)
  {
    pRunStep(iptc, otherInd, indMax, data, g, ipt0, ipt1);
  }

  // now debug output
  pProcessRunDebugOutput(otherInd, data, g);
}


//----------------------------------------------------------------
void XyRun::pRunStep(const int iptc, const int otherInd, const int indMax,
		     const Grid &data, Grid &g, int &ipt0, int &ipt1)
{
  if (iptc >= 0 && iptc < indMax)
  {
    // set output
    if (pNum > 0)
    {
      double v;
      v = pSum/pNum;
      setValue(iptc, otherInd, v, g);
    }
    else
    {
      setToMissing(iptc, otherInd, g);
    }
  }
  if (ipt0 >= 0 && ipt0 < indMax)
  {
    decSum(ipt0, indMax, otherInd, data);
  }
  ++ipt0;
  ++ipt1;
  if (ipt1 >= 0 && ipt1 < indMax)
  {
    incSum(ipt1, indMax, otherInd, data);
  }
}

//----------------------------------------------------------------
void XyRun::pProcessRunDebugOutput(const int otherInd, const Grid &data,
				   Grid &g) const
{
  if (pIsX)
  {
    ILOGF(DEBUG_VERBOSE, "([%d,%d], %d)", pMin, pMax, otherInd);
  }
  else
  {
    ILOGF(DEBUG_VERBOSE, "(%d,[%d,%d])", otherInd, pMin, pMax);
  }
  std::string s;
  char buf[convWx::ARRAY_LEN_SHORT];
  int count = 0;
  for (int i=pMin; i<=pMax; ++i)
  {
    double v0, v1;
    getValue(i, otherInd, data, v0);
    getValue(i, otherInd, g, v1);
    sprintf(buf, "%4.2lf,%4.2lf ", v0, v1);
    s += buf;
    if (++count >= 8)
    {
      count = 0;
      ILOG(DEBUG_VERBOSE, s.c_str());
      s = "";
    }
  }
  if (count > 0)
  {
    ILOG(DEBUG_VERBOSE, s.c_str());
  }
}


//----------------------------------------------------------------
void XyRun::incSum(const int ind, const int indMax, const int otherInd,
		   const Grid &data)
{
  if (ind >= 0 && ind < indMax)
  {
    double v;
    if (getValue(ind, otherInd, data, v))
    {
      pNum += 1.0;
      pSum += v;
    }
  }
}

//----------------------------------------------------------------
void XyRun::decSum(const int ind, const int indMax, const int otherInd,
		   const Grid &data)
{
  if (ind >= 0 && ind < indMax)
  {
    double v;
    if (getValue(ind, otherInd, data, v))
    {
      pNum -= 1.0;
      pSum -= v;
    }
  }
}

//----------------------------------------------------------------
bool XyRun::getValue(const int runIndex, const int otherIndex, 
		     const Grid &data, double &v) const
{
  if (pIsX)
  {
    return data.getValue(runIndex, otherIndex, v);
  }
  else
  {
    return data.getValue(otherIndex, runIndex, v);
  }
}

//----------------------------------------------------------------
void XyRun::setValue(const int runIndex, const int otherIndex, const double v,
		     Grid &g) const
{
  if (pIsX)
  {
    g.setv(runIndex, otherIndex, v);
  }
  else
  {
    g.setv(otherIndex, runIndex, v);
  }
}

//----------------------------------------------------------------
void XyRun::setToMissing(const int runIndex, const int otherIndex,
			 Grid &g) const
{
  if (pIsX)
  {
    g.setToMissing(runIndex, otherIndex);
  }
  else
  {
    g.setToMissing(otherIndex, runIndex);
  }
}

