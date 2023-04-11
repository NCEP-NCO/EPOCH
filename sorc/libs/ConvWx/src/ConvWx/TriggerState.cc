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
 * @file TriggerState.cc
 */
#include <string>
#include <vector>
#include <ConvWx/TriggerState.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::vector;

//----------------------------------------------------------------
TriggerState::TriggerState() :
  pUrl(""),
  pGentimeSubsample(),
  pIsLeadtimeSubsample(false),
  pLeadtimeSeconds0(0),
  pLeadtimeSeconds1(0),
  pLeadtimeSecondsDelta(0),
  pIsFcst(false),
  pArchiveTime0(convWx::BAD_TIME),
  pArchiveTime1(convWx::BAD_TIME),
  pIsArchive(false),
  pIsSequence(false)
{
}

//----------------------------------------------------------------
TriggerState::~TriggerState()
{
}

//----------------------------------------------------------------
void TriggerState::getArchiveInfo(time_t &archiveTime0, time_t &archiveTime1,
				  string &url, 
				  vector<int> &gentimeSubsample,
				  bool &isLeadtimeSubsample,
				  int &leadtimeSeconds0, int &leadtimeSeconds1,
				  int &leadtimeSecondsDelta,
				  bool &isFcst, bool &isSequence) const
{
  archiveTime0 = pArchiveTime0;
  archiveTime1 = pArchiveTime1;
  url = pUrl;
  gentimeSubsample = pGentimeSubsample;
  isLeadtimeSubsample = pIsLeadtimeSubsample;
  leadtimeSeconds0 = pLeadtimeSeconds0;
  leadtimeSeconds1 = pLeadtimeSeconds1;
  leadtimeSecondsDelta = pLeadtimeSecondsDelta;
  isFcst = pIsFcst;
  isSequence = pIsSequence;
}

//----------------------------------------------------------------
bool TriggerState::didChange(const string &url,
			     const vector<int> &gentimeSubsample) const
{
  if (pIsFcst || pIsArchive)
  {
    return true;
  }
  return (pUrl != url || gentimeSubsample != pGentimeSubsample);
}

//----------------------------------------------------------------
bool TriggerState::didChange(const string &url,
			     const vector<int> &gentimeSubsample,
			     const time_t &t0, const time_t &t1) const
{
  if (pIsFcst || !pIsArchive)
  {
    return true;
  }
  return (pUrl != url || gentimeSubsample != pGentimeSubsample ||
	  t0 != pArchiveTime0 || t1 != pArchiveTime1);
}

//----------------------------------------------------------------
bool TriggerState::didChange(const string &url, const bool isLeadtimeSubsample,
			     const int leadtimeSeconds0,
			     const int leadtimeSeconds1,
			     const int leadtimeSecondsDelta,
			     const vector<int> &gentimeSubsample, 
			     const bool sequence) const
{
  if (!pIsFcst || pIsArchive)
  {
    return true;
  }
  if (isLeadtimeSubsample != pIsLeadtimeSubsample)
  {
    return true;
  }
  if (isLeadtimeSubsample)
  {
    if (leadtimeSeconds0 != pLeadtimeSeconds0 ||
	leadtimeSeconds1 != pLeadtimeSeconds1 ||
	leadtimeSecondsDelta != pLeadtimeSecondsDelta)
    {
      return true;
    }
  }
  return (url != pUrl || gentimeSubsample != pGentimeSubsample || 
	  sequence != pIsSequence);
}

//----------------------------------------------------------------
bool TriggerState::didChange(const string &url, const bool isLtSubsample,
			     const int leadtimeSeconds0,
			     const int leadtimeSeconds1,
			     const int leadtimeSecondsDelta,
			     const vector<int> &gentimeSubsample, 
			     const bool sequence,
			     const time_t &t0, const time_t &t1) const
{
  if (!pIsFcst || !pIsArchive)
  {
    return true;
  }
  if (isLtSubsample != pIsLeadtimeSubsample)
  {
    return true;
  }
  if (isLtSubsample)
  {
    if (leadtimeSeconds0 != pLeadtimeSeconds0 ||
	leadtimeSeconds1 != pLeadtimeSeconds1 ||
	leadtimeSecondsDelta != pLeadtimeSecondsDelta)
    {
      return true;
    }
  }
  return (pUrl != url || gentimeSubsample != pGentimeSubsample || 
	  sequence != pIsSequence || t0 != pArchiveTime0 ||
	  t1 != pArchiveTime1);
}

//----------------------------------------------------------------
void TriggerState::setObsTriggering(const string &url,
				    const vector<int> &gentimeSubsample)
{
  pUrl = url;
  pGentimeSubsample = gentimeSubsample;
  pIsLeadtimeSubsample = false;
  pLeadtimeSeconds0 = 0;
  pLeadtimeSeconds1 = 0;
  pLeadtimeSecondsDelta = 0;
  pIsFcst = false;
  pArchiveTime0 = convWx::BAD_TIME;
  pArchiveTime1 = convWx::BAD_TIME;
  pIsArchive = false;
  pIsSequence = true;
}

//----------------------------------------------------------------
void TriggerState::setObsTriggering(const string &url,
				    const vector<int> &gentimeSubsample,
				    const time_t &t0, const time_t &t1)
{
  pUrl = url;
  pGentimeSubsample = gentimeSubsample;
  pIsLeadtimeSubsample = false;
  pLeadtimeSeconds0 = 0;
  pLeadtimeSeconds1 = 0;
  pLeadtimeSecondsDelta = 0;
  pIsFcst = false;
  pArchiveTime0 = t0;
  pArchiveTime1 = t1;
  pIsArchive = true;
  pIsSequence = true;
}

//----------------------------------------------------------------
void TriggerState::setFcstTriggering(const string &url,
				     const bool isLtSubsample,
				     const int leadtimeSeconds0,
				     const int leadtimeSeconds1,
				     const int leadtimeSecondsDelta,
				     const vector<int> &gentimeSubsample,
				     const bool sequence)
{
  pUrl = url;
  pGentimeSubsample = gentimeSubsample;
  pIsLeadtimeSubsample = isLtSubsample;
  pLeadtimeSeconds0 = leadtimeSeconds0;
  pLeadtimeSeconds1 = leadtimeSeconds1;
  pLeadtimeSecondsDelta = leadtimeSecondsDelta;
  pIsFcst = true;
  pArchiveTime0 = convWx::BAD_TIME;
  pArchiveTime1 = convWx::BAD_TIME;
  pIsArchive = false;
  pIsSequence = sequence;
}

//----------------------------------------------------------------
void TriggerState::setFcstTriggering(const string &url,
				     const bool isLtSubsample,
				     const int leadtimeSeconds0,
				     const int leadtimeSeconds1,
				     const int leadtimeSecondsDelta,
				     const vector<int> &gentimeSubsample,
				     const bool sequence,
				     const time_t &t0, const time_t &t1)
{
  pUrl = url;
  pGentimeSubsample = gentimeSubsample;
  pIsLeadtimeSubsample = isLtSubsample;
  pLeadtimeSeconds0 = leadtimeSeconds0;
  pLeadtimeSeconds1 = leadtimeSeconds1;
  pLeadtimeSecondsDelta = leadtimeSecondsDelta;
  pIsFcst = true;
  pArchiveTime0 = t0;
  pArchiveTime1 = t1;
  pIsArchive = true;
  pIsSequence = sequence;
}

