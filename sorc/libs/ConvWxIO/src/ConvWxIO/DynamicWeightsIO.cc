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
 * @file DynamicWeightsIO.cc
 */

#include <ConvWxIO/DynamicWeightsIO.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/LtInterp.hh>

//-----------------------------------------------------------------
DynamicWeightsIO::DynamicWeightsIO(const double *threshRange,
				   const double *weightRange) :
  DynamicWeightsHandler(threshRange, weightRange)
{
}

//-----------------------------------------------------------------
DynamicWeightsIO::DynamicWeightsIO(void) : DynamicWeightsHandler()
{
}

//-----------------------------------------------------------------
DynamicWeightsIO::DynamicWeightsIO(const DynamicWeightsHandler &dw,
				   const time_t &gt, const LtInterp &lt,
				   const ParmProjection &proj,
				   const ParmFcstIO &parms,
				   bool setMissingToZero, bool debug,
				   ConvWxThreadMgr *threads) :
  DynamicWeightsHandler(dw)
{
  pOk = update(gt, lt, proj, parms, setMissingToZero, debug, threads);
}

//-----------------------------------------------------------------
DynamicWeightsIO::~DynamicWeightsIO()
{
}

//-----------------------------------------------------------------
bool DynamicWeightsIO::update(const time_t &gt, const LtInterp &lt,
			      const ParmProjection &proj,
			      const ParmFcstIO &parms, bool setMissingToZero,
			      bool debug, ConvWxThreadMgr *threads)
{
  if (lt.isEmpty() || gt == convWx::BAD_TIME)
  {
    return false;
  }

  // set base class state
  DynamicWeightsHandler::setState(gt, lt);

  // for now this is a simple technique that reloads all the forecasts
  // every time called
  if (lt.ok(0))
  {
    pHas0 = pLoadData(gt, pLeadTime0, proj, parms, pDynamicWeight0,
		      setMissingToZero, debug, threads);
  }
  else 
  {
    pHas0 = false;
  }
  if (lt.ok(1))
  {
    pHas1 = pLoadData(gt, pLeadTime1, proj, parms, pDynamicWeight1,
		      setMissingToZero, debug, threads);
  }
  else
  {
    pHas1 = false;
  }
  return pHas0 && pHas1;
}      

//-----------------------------------------------------------------
bool DynamicWeightsIO::pLoadData(const time_t &gt, int lt,
				 const ParmProjection &proj,
				 const ParmFcstIO &parms,
				 FcstGrid &grid, bool setMissingToZero,
				 bool debug,
				 ConvWxThreadMgr *threads) const
{
  bool hasData=true;
  if (threads != NULL)
  {
    threads->lock();
    hasData = parms.hasData(gt, lt);
    threads->unlock();
  }
  else
  {
    hasData = parms.hasData(gt, lt);
  }    
  if (!hasData)
  {
    ILOGF(WARNING, "no data at %s time=%s+%d",
	  parms.pUrl.c_str(), ConvWxTime::stime(gt).c_str(), lt);
    return false;
  }

  if (debug)
  {
    ILOGF(DEBUG_VERBOSE, "data does exist %s, %s+%d",
	  parms.pUrl.c_str(), ConvWxTime::stime(gt).c_str(), lt);
    ILOGF(DEBUG, "Loading data now %s  %s+%d",
	  parms.pUrl.c_str(), ConvWxTime::stime(gt).c_str(), lt);
  }

  if (threads != NULL)
  {
    threads->lock();
    hasData = parms.loadFcstData(gt, lt, proj, grid);
    threads->unlock();
  }
  else
  {
    hasData = parms.loadFcstData(gt, lt, proj, grid);
  }    
  if (!hasData)
  {
    ILOGF(ERROR, "expected data but none %s at %s+%d",
	  parms.pUrl.c_str(), ConvWxTime::stime(gt).c_str(), lt);
    return false;
  }
  if (debug)
  {
    ILOGF(DEBUG, "Did load data %s  %s+%d",
	  parms.pUrl.c_str(), ConvWxTime::stime(gt).c_str(), lt);
  }
  if (setMissingToZero)
  {
    if (grid.getMissing() == 0.0)
    {
      grid.changeMissing(-99);
    }
    for (int i=0; i<grid.getNdata(); ++i)
    {
      if (grid.isMissingAt(i))
      {
	grid.setv(i, 0.0);
      }
    }
  }

  return true;
}
