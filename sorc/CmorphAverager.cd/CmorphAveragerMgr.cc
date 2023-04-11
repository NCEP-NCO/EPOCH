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
/** 
 * @file CmorphAveragerMgr.cc
 * @brief Source for CmorphAveragerMgr class 
 */

#include "CmorphAveragerMgr.hh"

#include <ConvWx/MultiGrid.hh>
#include <ConvWx/MetaData.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <unistd.h>
#include <algorithm>
#include <vector>
using std::vector;
// using std::string;

CmorphAveragerMgr::CmorphAveragerMgr(const ParmsCmorphAverager &params, 
				     void cleanExit(int)):
  pParams(params)
{   
  time_t t = time(0);

  //
  // Set up real-time or archive data triggering
  //
  if (!InterfaceParm::driverInit(pParams.main, cleanExit))
  {
    cleanExit(1);
  }

  ILOGF(WARNING, "Restarted at %s", ConvWxTime::stime(t).c_str());
}

CmorphAveragerMgr::~CmorphAveragerMgr()
{
  time_t t = time(0);
  ILOGF(WARNING, "Terminated at %s", ConvWxTime::stime(t).c_str());
  
}

bool CmorphAveragerMgr::run() const
{ 
  //
  // Observation data time. 
  //
  time_t obsTime;
    
  int lastYear = -1;
  int lastMonth = -1;
  int lastDay = -1;
  int lastHour = -1;
  
  vector<time_t> hoursForProcessing;
    
  // if center averaging, look to do an average of older data
  // at hour 1, 4, 7, etc, with output time set to 0, 3, 6, etc.
  // the average will be at 7:  7, 6.5, 6, 5.5, 5, 4.5  output time = 6

  // for end time averaging, look to do an average of older data
  // at hour 0, 3, 6, etc. with output time same (0, 3, 6, etc).
  // the average at 6:  3.5, 4, 4.5, 5, 5.5, 6
  // if (pParams.centerAveraging)
  // {
  //   hoursForProcessing.push_back(1);
  //   hoursForProcessing.push_back(4);
  //   hoursForProcessing.push_back(7);
  //   hoursForProcessing.push_back(10);
  //   hoursForProcessing.push_back(13);
  //   hoursForProcessing.push_back(16);
  //   hoursForProcessing.push_back(19);
  //   hoursForProcessing.push_back(22);
  // }
  // else
  // {
    hoursForProcessing.push_back(0);
    hoursForProcessing.push_back(3);
    hoursForProcessing.push_back(6);
    hoursForProcessing.push_back(9);
    hoursForProcessing.push_back(12);
    hoursForProcessing.push_back(15);
    hoursForProcessing.push_back(18);
    hoursForProcessing.push_back(21);
  // }

  while (Trigger::sequenceTrigger(pParams.main, obsTime))
  {
    InterfaceLL::doRegister("Processing observation data");
    bool doWrite = false;
    int y, m, d, h, min, s;
    ConvWxTime::expandTime(obsTime, y, m, d, h, min, s);
    if (find(hoursForProcessing.begin(), hoursForProcessing.end(), h) == hoursForProcessing.end())
    {
      InterfaceLL::doRegister("Skipping observation data");
      continue;
    }
    if (min != 0)
    {
      InterfaceLL::doRegister("Skipping observation data");
      continue;
    }
    time_t t0, t1, outTime;
    // if (pParams.centerAveraging)
    // {
    //   t0 = obsTime-3.5*3600;
    //   t1 = obsTime;
    //   outTime = obsTime - 3600;
    // }
    // else
    // {
      t0 = obsTime - 3.5*3600;
      t1 = obsTime;
      outTime = obsTime;
    // }

    Grid data;
    Grid out;
    Grid counts;
    vector<time_t> times = InterfaceIO::availableObsTimesInARange(pParams.obs.pUrl, t0, t1);
    bool first = true;
    for (size_t i=0; i<times.size(); ++i)
    {
      if (pLoadObs(times[i], data))
      {
	if (first)
	{
	  first = false;
	  out = data;
	  out.setAllMissing();
	  counts = data;
	  counts.setAllToValue(0.0);
	}
	for (int i=0; i<data.getNdata(); ++i)
	{
	  double v;
	  if (data.getValue(i, v))
	  {
	    counts.incrementValueAtPoint(i, 1.0);
	    if (out.isMissingAt(i))
	    {
	      out.setv(i, v);
	    }
	    else
	    {
	      out.incrementValueAtPoint(i, v);
	    }
	  }
	}
      }
    }
    // normalize
    out.divide(counts);

    if (pParams.unitsConvert)
    {
      // multiply by 3 to change units from mm/hour to mm/3hr
      out.multiply(3.0);
      out.changeUnits("mm/3hr");
    }
    pWriteObs(outTime, out);
    InterfaceLL::doRegister("Wrote observation data");
  }  
  return true;
}

bool 
CmorphAveragerMgr::pLoadObs(const time_t obsTime, Grid &data) const
{
  InterfaceLL::doRegister("Loading observation data");

  if (!InterfaceIO::loadObs(obsTime, pParams.proj,
			    pParams.obs.pUrl, pParams.obs.pField,
			    pParams.obs.pRemap, data))
  {
    ILOGF(DEBUG, "Error loading observations data at %s", 
	  ConvWxTime::stime(obsTime).c_str());
    return false;
  } 
  else
  {    
    ILOGF(DEBUG, "Loaded observations data at %s.", ConvWxTime::stime(obsTime).c_str());
    return true;
  }
}

void CmorphAveragerMgr::pWriteObs(const time_t validTime, Grid &grid) const
{ 
 
  grid.setScalingDynamic();
  grid.setEncoding(Grid::ENCODING_INT16);
  //grid.changeName(pParams.obsOut.pField);

  ILOGF(DEBUG, "Writing data to %s. Valid time:%s\n",
	pParams.obsOut.pUrl.c_str(), ConvWxTime::stime(validTime).c_str());

  //
  // Empty meta data object
  //
  MetaData metaData;
   
  //
  // Grid container
  // 
  MultiGrid multiGrid;
  
  //
  // Add grid to the container
  //
  multiGrid.append(grid);

  //
  // Output data
  //
  pParams.obsOut.writeObs(validTime, pParams.proj, multiGrid, metaData);   
}

