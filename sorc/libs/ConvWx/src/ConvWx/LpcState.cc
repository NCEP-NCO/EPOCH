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
 * @file LpcState.cc
 */
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <ConvWx/LpcState.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::vector;
using std::pair;
using std::string;

//----------------------------------------------------------------
LpcState::LpcState(void) :
  pModelGt(convWx::BAD_TIME),
  pModel(),
  pOutputGt(convWx::BAD_TIME),
  pOutput()
{
}

//----------------------------------------------------------------
LpcState::LpcState(const ParmFcst &inputModelParms,
		   const ParmFcst &outputModelParms,
		   const time_t &modelGt, const time_t &outputGt,
		   const std::vector<int> &alt) :
  pModelGt(modelGt),
  pModel(),
  pOutputGt(outputGt),
  pOutput()
{  
  for (int i=0; i<inputModelParms.nlead(); ++i)
  {
    int lt = inputModelParms.ithLeadSeconds(i);
    bool available = find(alt.begin(), alt.end(), lt) != alt.end();
    LpcModelState ms(lt, available);
    pModel.push_back(pair<int, LpcModelState>(lt, ms));
  }

  for (int i=0; i<outputModelParms.nlead(); ++i)
  {
    int lt = outputModelParms.ithLeadSeconds(i);
    LpcOutputState os(lt);
    pOutput.push_back(pair<int, LpcOutputState>(lt, os));
  }
}

//----------------------------------------------------------------
LpcState::~LpcState()
{
}

//----------------------------------------------------------------
void LpcState::reInitialize(const ParmFcst &inputModelParms,
			    const ParmFcst &outputModelParms,
			    const time_t &modelGt, const time_t &outputGt,
			    const std::vector<int> &alt)
{  
  pModelGt = modelGt;
  pModel.clear();
  pOutputGt = outputGt;
  pOutput.clear();
  for (int i=0; i<inputModelParms.nlead(); ++i)
  {
    int lt = inputModelParms.ithLeadSeconds(i);
    bool available = find(alt.begin(), alt.end(), lt) != alt.end();
    LpcModelState ms(lt, available);
    pModel.push_back(pair<int, LpcModelState>(lt, ms));
  }

  for (int i=0; i<outputModelParms.nlead(); ++i)
  {
    int lt = outputModelParms.ithLeadSeconds(i);
    LpcOutputState os(lt);
    pOutput.push_back(pair<int, LpcOutputState>(lt, os));
  }
}

//----------------------------------------------------------------
void LpcState::print(void) const
{
  printf("Model gen:%s    Output gen:%s      latency:%.2lf\n",
	 ConvWxTime::stime(pModelGt).c_str(),
	 ConvWxTime::stime(pOutputGt).c_str(),
	 static_cast<double>(pOutputGt - pModelGt)/convWx::DOUBLE_SECS_PER_HOUR);
  for (int i=0; i<static_cast<int>(pModel.size()); ++i)
  {
    printf("%s\n", pModel[i].second.sprint().c_str());
  }
  for (int i=0; i<static_cast<int>(pOutput.size()); ++i)
  {
    printf("%s\n", pOutput[i].second.sprint().c_str());
  }
}

//----------------------------------------------------------------
void LpcState::setModelAvailability(const int lt, const bool available)
{
  for (int i=0; i<static_cast<int>(pModel.size()); ++i)
  {
    if (pModel[i].first == lt)
    {
      pModel[i].second.setAvailability(available);
      return;
    }
  }
}

//----------------------------------------------------------------
void LpcState::setOutputMissing(const int lt)
{
  for (int i=0; i<static_cast<int>(pOutput.size()); ++i)
  {
    if (pOutput[i].first == lt)
    {
      pOutput[i].second.setMissing();
    }
  }  
}

//----------------------------------------------------------------
void LpcState::setOutputProcessed(const int lt)
{
  for (int i=0; i<static_cast<int>(pOutput.size()); ++i)
  {
    if (pOutput[i].first == lt)
    {
      pOutput[i].second.setProcessed();
    }
  }  
}

//----------------------------------------------------------------
bool LpcState::outputComplete(void) const
{
  for (int i=0; i<static_cast<int>(pOutput.size()); ++i)
  {
    if (!pOutput[i].second.isProcessed())
    {
      return false;
    }
  }  
  return true;
}

//----------------------------------------------------------------
void LpcState::getLogInfo(const bool isRestart,
			  string &modelGt, string &latency,
			  string &missingModelLeadTimes, string &outGt,
			  string &missingOutputLeadTimes) const
{
  char buf[convWx::ARRAY_LEN_SHORT];

  // create return modelGt and outGt strings "hh:mm"
  int y, m, d, h, min, sec;
  ConvWxTime::expandTime(pModelGt, y, m, d, h, min, sec);
  sprintf(buf, "%02d:%02d", h, min);
  modelGt = buf;

  ConvWxTime::expandTime(pOutputGt, y, m, d, h,min,sec);
  sprintf(buf, "%02d:%02d", h, min);
  outGt = buf;

  // create return latency hours string
  int latencySec = pOutputGt - pModelGt;
  double latencyHours =
    static_cast<double>(latencySec)/convWx::DOUBLE_SECS_PER_HOUR;
  sprintf(buf, "%.2lf", latencyHours);
  latency = buf;

  // create the list of missing model lead hours
  missingModelLeadTimes = "";
  for (int i=0; i<static_cast<int>(pModel.size()); ++i)
  {
    if (!pModel[i].second.available())
    {
      double d = 
	static_cast<double>(pModel[i].first)/convWx::DOUBLE_SECS_PER_HOUR;
      sprintf(buf, "%5.2lf ", d);
      missingModelLeadTimes += buf;
    }
  }
  if (missingModelLeadTimes.empty())
  {
    missingModelLeadTimes = "None";
  }

  if (isRestart)
  {
    // create the list of processed output lead hours
    missingOutputLeadTimes = "";
    for (int i=0; i<static_cast<int>(pOutput.size()); ++i)
    {
      if (!pOutput[i].second.allMissing())
      {
	double d =
	  static_cast<double>(pOutput[i].first)/convWx::DOUBLE_SECS_PER_HOUR;
	sprintf(buf, "%5.2lf ", d);
	missingOutputLeadTimes += buf;
      }
    }
    if (missingOutputLeadTimes.empty())
    {
      missingOutputLeadTimes = "Restart,processed:None";
    }
    else
    {
      missingOutputLeadTimes = "Restart,processed:" + missingOutputLeadTimes;
    }
  }
  else
  {
    // create the list of missing output lead hours
    missingOutputLeadTimes = "";
    for (int i=0; i<static_cast<int>(pOutput.size()); ++i)
    {
      if (pOutput[i].second.allMissing())
      {
	double d =
	  static_cast<double>(pOutput[i].first)/convWx::DOUBLE_SECS_PER_HOUR;
	sprintf(buf, "%5.2lf ", d);
	missingOutputLeadTimes += buf;
      }
    }
    if (missingOutputLeadTimes.empty())
    {
      missingOutputLeadTimes = "None";
    }
  }
}
