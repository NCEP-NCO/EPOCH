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
 * @file UvHtml.cc
 */

//----------------------------------------------------------------
#include <cstdio>
#include <string>
#include <vector>
#include <ConvWx/UvHtml.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::vector;
using std::pair;

//----------------------------------------------------------------
UvHtml::UvHtml(void) :
  pHasHtml(false),
  pGenTime(convWx::BAD_TIME),
  pLeadSeconds(0),
  pLatencyHours(0),
  pPcType(PcType::UNKNOWN)
{
}

//----------------------------------------------------------------
UvHtml::UvHtml(const time_t &gt, const int lt, const double latencyHours,
	       const PcType::Pc_t pcType) :
  pHasHtml(true),
  pGenTime(gt),
  pLeadSeconds(lt),
  pLatencyHours(latencyHours),
  pPcType(pcType)
{
}

//----------------------------------------------------------------
UvHtml::~UvHtml()
{
}

//----------------------------------------------------------------
string UvHtml::getHtmlCorrectionString(void) const
{
  if (pHasHtml)
  {
    return PcType::correctionString(pPcType);
  }
  else
  {
    return PcType::correctionStringNone();
  }
}

//----------------------------------------------------------------
double UvHtml::getHtmlLatencyHours(void) const
{
  if (pHasHtml)
  {
    return pLatencyHours;
  }
  else
  {
    return convWx::BAD_LATENCY_HOURS;
  }
}

//----------------------------------------------------------------
void UvHtml::getHtmlInfo(string &gentimeHourMin, string &leadHours,
			 string &latencyHours, string &pcType) const
{
  if (pHasHtml)
  {
    char buf[convWx::ARRAY_LEN_SHORT];
    int y, m, d, h, min, sec;
    ConvWxTime::expandTime(pGenTime, y, m, d, h, min, sec);
    sprintf(buf, "%02d:%02d", h, min);
    gentimeHourMin = buf;

    sprintf(buf, "%.2lf",
	    static_cast<double>(pLeadSeconds)/convWx::DOUBLE_SECS_PER_HOUR);
    leadHours = buf;

    sprintf(buf, "%.2lf", pLatencyHours);
    latencyHours = buf;
    
    pcType = PcType::descriptor(pPcType);
  }
  else
  {
    gentimeHourMin = "NONE";
    leadHours = "NONE";
    latencyHours = "NONE";
    pcType = "NONE";
  }
}

//----------------------------------------------------------------
vector<pair<string,string> > UvHtml::getHelpDescriptors(void)
{
  return PcType::helpDescriptors();
}
