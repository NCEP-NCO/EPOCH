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
 * @file BlendingInputData.cc
 */
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWxIO/BlendingInputData.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <string>
#include <ctime>
#include <unistd.h>
using std::string;

// set the tick seconds waiting value
const int BlendingInputData::pWaitTick = 5;

//-----------------------------------------------------------------
BlendingInputData::BlendingInputData(const string &name,
				     const ParmProjection &proj,
				     const ParmFcst &parms,
				     const int maxWaitSeconds) :
  pName(name),
  pProj(proj),
  pParm(parms),
  pMaxWaitSeconds(maxWaitSeconds)
{
}

//-----------------------------------------------------------------
BlendingInputData::~BlendingInputData()
{
}

//-----------------------------------------------------------------
bool BlendingInputData::fillFcstGridWithWait(const time_t &gt, int lt, 
					     int &totalWaitSeconds,
					     FcstGrid &outGrid,
					     bool &allMissing) const
{
  while (true)
  {
    if (pParm.hasData(gt, lt))
    {
      return pLoadData1(gt, lt, outGrid, allMissing);
    }
    else
    {
      InterfaceLL::doRegister("loadData");
      if (pWait(totalWaitSeconds))
      {
	ILOGF(DEBUG, "waiting %d for %s lt=%d", totalWaitSeconds,
	      pParm.pUrl.c_str(), lt);
      }
      else
      {
	// can't wait anymore
	ILOGF(WARNING, "no more waiting for %s lt=%d, output=all missing",
	      pParm.pUrl.c_str(), lt);
	outGrid.setAllMissing();
	allMissing = true;
	return false;
      }
    }
  }
}

//-----------------------------------------------------------------
bool BlendingInputData::fillFcstGridWithWait(const time_t &gt, int lt, 
					     int &totalWaitSeconds,
					     FcstGrid &outGrid,
					     bool &allMissing,
					     ConvWxThreadMgr &threads) const
{
  while (true)
  {
    threads.lock();
    if (pParm.hasData(gt, lt))
    {
      bool status = pLoadData1(gt, lt, outGrid, allMissing);
      threads.unlock();
      return status;
    }
    else
    {
      threads.unlock();
      InterfaceLL::doRegister("loadData");
      if (pWait(totalWaitSeconds))
      {
	ILOGF(DEBUG, "waiting %d for %s lt=%d", totalWaitSeconds,
	      pParm.pUrl.c_str(), lt);
      }
      else
      {
	// can't wait anymore
	ILOGF(WARNING, "no more waiting for %s lt=%d, output=all missing",
	      pParm.pUrl.c_str(), lt);
	outGrid.setAllMissing();
	allMissing = true;
	return false;
      }
    }
  }
}

//-----------------------------------------------------------------
bool BlendingInputData::pLoadData1(const time_t &gt, int lt, FcstGrid &outGrid,
				   bool &allMissing) const
{
  if (!pParm.loadFcstData(gt, lt, pProj, outGrid))
  {
    ILOGF(ERROR, "expected %s but none %d", pName.c_str(), lt);
    outGrid.setAllMissing();
    allMissing = true;
    return false;
  }
  else
  {
    allMissing = outGrid.allMissing();
    return true;
  }
}

//-----------------------------------------------------------------
bool BlendingInputData::pWait(int &totalWaitSeconds) const
{
  if (totalWaitSeconds < pMaxWaitSeconds)
  {
    InterfaceLL::doRegister("pWait");
    sleep(pWaitTick);
    totalWaitSeconds += pWaitTick;
    return true;
  }
  else
  {
    return false;
  }
}

