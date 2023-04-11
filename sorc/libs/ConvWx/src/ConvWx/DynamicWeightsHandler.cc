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
 * @file DynamicWeightsHandler.cc
 */

#include <ConvWx/DynamicWeightsHandler.hh>
#include <ConvWx/LtInterp.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//-----------------------------------------------------------------
DynamicWeightsHandler::DynamicWeightsHandler(const double *threshRange,
					     const double *weightRange) :
  pOk(true),
  pGt(convWx::BAD_TIME),
  pHas0(false),
  pLeadTime0(0),
  pWeight0(0.0),
  pHas1(false),
  pLeadTime1(0),
  pWeight1(0.0),
  pHasThresh(true),
  pThreshRangeMin(threshRange[0]),
  pThreshRangeMax(threshRange[1]),
  pWeightRangeMin(weightRange[0]),
  pWeightRangeMax(weightRange[1])
{
}

//-----------------------------------------------------------------
DynamicWeightsHandler::DynamicWeightsHandler(void) :
  pOk(false),
  pGt(convWx::BAD_TIME),
  pHas0(false),
  pLeadTime0(0),
  pWeight0(0.0),
  pHas1(false),
  pLeadTime1(0),
  pWeight1(0.0),
  pHasThresh(false),
  pThreshRangeMin(0),
  pThreshRangeMax(0),
  pWeightRangeMin(0),
  pWeightRangeMax(0)
{
}

//-----------------------------------------------------------------
DynamicWeightsHandler::~DynamicWeightsHandler()
{
}

//-------------------------------------------------------------------
void DynamicWeightsHandler::setState(const time_t &gt, const LtInterp &lt)
{
  pGt = gt;
  pHas0 = false;
  pLeadTime0 = lt.getLeadSeconds(0);
  pWeight0 = lt.getWeight(0);
  pHas1 = false;
  pLeadTime1 = lt.getLeadSeconds(1);
  pWeight1 = lt.getWeight(1);
}

//-------------------------------------------------------------------
bool DynamicWeightsHandler::getWeight(int ipos, double &weight) const
{
  if (pHas0 && pHas1)
  {
    return pInterp(ipos, weight);
  }
  else if (pHas0 && !pHas1)
  {
    return pOne(ipos, pWeight0, pDynamicWeight0, weight);
  }
  else if ((!pHas0) && pHas1)
  {
    return pOne(ipos, pWeight1, pDynamicWeight1, weight);
  }
  else
  {
    ILOGF(DEBUG_VERBOSE, "have no data ipos=%d", ipos);
    return false;
  }
}
  
//-------------------------------------------------------------------
bool DynamicWeightsHandler::getMaxWeight(int ipos, double &weight) const
{
  if (pHas0 && pHas1)
  {
    return pMax(ipos, weight);
  }
  else if (pHas0 && !pHas1)
  {
    return pOne(ipos, pWeight0, pDynamicWeight0, weight);
  }
  else if ((!pHas0) && pHas1)
  {
    return pOne(ipos, pWeight1, pDynamicWeight1, weight);
  }
  else
  {
    ILOGF(ERROR, "have no data ipos=%d", ipos);
    return false;
  }
}
  
//-------------------------------------------------------------------
bool DynamicWeightsHandler::getMinWeight(int ipos, double &weight) const
{
  if (pHas0 && pHas1)
  {
    return pMin(ipos, weight);
  }
  else
  {
    ILOGF(ERROR, "have no data ipos=%d", ipos);
    return false;
  }
}
  
//-------------------------------------------------------------------
bool DynamicWeightsHandler::getThresh(int ipos, double &thresh) const
{
  if (pHasThresh)
  {
    double weight;
    if (!getWeight(ipos, weight))
    {
      return false;
    }
    getThreshFromWeight(weight, thresh);
    return true;
  }
  else
  {
    ILOG(ERROR, "No thresholds allowed");
    return false;
  }
}

//-------------------------------------------------------------------
bool DynamicWeightsHandler::getThreshFromWeight(double weight,
						double &thresh) const
{
  if (pHasThresh)
  {
    if (weight < pWeightRangeMax)
    {
      double delta = pWeightRangeMax - pWeightRangeMin;
      double slope = 0.0;
      if(delta != 0.0)
      {
	slope = (pThreshRangeMin - pThreshRangeMax) / delta;
      }
      double intercept = pThreshRangeMin - slope * pWeightRangeMax;
      thresh = intercept + slope * weight;
    }
    else
    {
      thresh = pThreshRangeMin;
    }
    return true;
  }
  else
  {
    ILOG(ERROR, "No thresholds allowed");
    return false;
  }
}

//-----------------------------------------------------------------
bool DynamicWeightsHandler::maxGrid(Grid &ret) const
{
  if (pHas0 && !pHas1)
  {
      ret = pDynamicWeight0;
      return true;
  }
  else if (pHas1 && !pHas0)
  {
    ret = pDynamicWeight1;
    return true;
  }
  else if (pHas1 && pHas0)
  {
    ret = pDynamicWeight0;
    ret.maximum(pDynamicWeight1);
    return true;
  }
  else
  {
    ILOG(ERROR, "No grids to take max of");
    return false;
  }
}

//-----------------------------------------------------------------
std::string DynamicWeightsHandler::xml(const std::string &dataType) const
{
  string tag = dataType + "_genTime";
  string ret = Xml::writeTime(tag, 0, pGt);
  tag = dataType + "_lead_weight";
  char buf[convWx::ARRAY_LEN_LONG];
  sprintf(buf, "[%d,%05.3lf],[%d,%05.3lf]", pLeadTime0, pWeight0,
	  pLeadTime1, pWeight1);
  ret = ret + Xml::writeString(tag, 0, buf);
  return ret;
}

//-----------------------------------------------------------------
bool DynamicWeightsHandler::pOne(int ipos, double interpWeight,
				 const FcstGrid &grid, double &weight) const
{
  if (interpWeight == 0.0)
  {
    ILOG(WARNING,"The only available data has interpolating weight=0");
    weight = 0.0;
    return false;
  }
  else
  {
    return grid.getValue(ipos, weight);
  }
}

//-----------------------------------------------------------------
bool DynamicWeightsHandler::pInterp(int ipos, double &weight) const
{
  double v0, v1;
  bool ok0, ok1;
  ok0 = pDynamicWeight0.getValue(ipos, v0);
  ok1 = pDynamicWeight1.getValue(ipos, v1);
  bool stat = true;
  if (ok0 && ok1)
  {
    weight = v0*pWeight0 + v1*pWeight1;
  }
  else if (ok0 && !ok1)
  {
    weight = v0;
  }
  else if (ok1 && !ok0)
  {
    weight = v1;
  }
  else
  {
    ILOGF(DEBUG_VERBOSE, "have no data ipos=%d", ipos);
    weight = 0.0;
    stat = false;
  }
  return stat;
}

//-----------------------------------------------------------------
bool DynamicWeightsHandler::pMax(int ipos, double &weight) const
{
  double v0, v1;
  bool ok0, ok1;
  ok0 = pDynamicWeight0.getValue(ipos, v0);
  ok1 = pDynamicWeight1.getValue(ipos, v1);
  bool stat = true;
  if (ok0 && ok1)
  {
    weight = v0;
    if (v1 > v0)
    {
      weight = v1;
    }
  }
  else if (ok0 && !ok1)
  {
    weight = v0;
  }
  else if (ok1 && !ok0)
  {
    weight = v1;
  }
  else
  {
    ILOGF(ERROR, "have no data ipos=%d", ipos);
    weight = 0.0;
    stat = false;
  }
  return stat;
}

//-----------------------------------------------------------------
bool DynamicWeightsHandler::pMin(int ipos, double &weight) const
{
  double v0, v1;
  bool ok0, ok1;
  ok0 = pDynamicWeight0.getValue(ipos, v0);
  ok1 = pDynamicWeight1.getValue(ipos, v1);
  bool stat = true;
  if (ok0 && ok1)
  {
    weight = v0;
    if (v1 < v0)
    {
      weight = v1;
    }
  }
  else if (ok0 && !ok1)
  {
    ILOGF(ERROR, "Not both data values at ipos=%d", ipos);
    weight = 0;
    stat = false;
  }
  else if (ok1 && !ok0)
  {
    ILOGF(ERROR, "Not both data values at ipos=%d", ipos);
    weight = 0;
    stat = false;
  }
  else
  {
    ILOGF(ERROR, "have no data ipos=%d", ipos);
    weight = 0.0;
    stat = false;
  }
  return stat;
}
