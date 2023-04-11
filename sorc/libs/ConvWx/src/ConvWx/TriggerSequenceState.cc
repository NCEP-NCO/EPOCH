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
 * @file TriggerSequenceState.cc
 */

#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/TriggerSequenceState.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ConvWxTime.hh>

//----------------------------------------------------------------
TriggerSequenceState::TriggerSequenceState(const ParmFcst &parms) :
  
  pGenTime(convWx::BAD_TIME),
  pGenTimeComplete(false),
  pIsInit(true),
  pParm(parms)
{
  std::vector<int> lts = pParm.getLtSeconds();
  for (size_t i=0; i<lts.size(); ++i)
  {
    pLeadTimeStatus.push_back(0);
  }
}

//----------------------------------------------------------------
TriggerSequenceState::~TriggerSequenceState()
{
}

//----------------------------------------------------------------
TriggerSequenceState::Update_t 
TriggerSequenceState::update(const time_t &gt, const int lt,
			     time_t &oldGt)
{
  if (gt == pGenTime)
  {
    if (pGenTimeComplete)
    {
      ILOGF(WARNING, "expected new gen time, but got same gt, lt=%d",
	    lt);
    }
    return TriggerSequenceState::SAME_GEN;
  }
  else
  {
    if (pIsInit)
    {
      // startup only.
      ILOG(DEBUG, "The startup situation");
      pGenTime = gt;
      pClearLeadStatus();
      pGenTimeComplete = false;
      return TriggerSequenceState::INIT;
    }
    else
    {
      Update_t ret;
      if (pGenTimeComplete)
      {
	ret = TriggerSequenceState::NEW_GEN_COMPLETE;
      }
      else
      {
	ret = TriggerSequenceState::NEW_GEN_INCOMPLETE;
	oldGt = pGenTime;
      }
      pGenTime = gt;
      pGenTimeComplete = false;
      pClearLeadStatus();
      return ret;
    }
  }
}

//----------------------------------------------------------------
bool TriggerSequenceState::finish(const int lt)
{
  int ltInd = pParm.leadSecondsIndex(lt);
  if (ltInd < 0)
  {
     return false;
  }
  pIsInit = false;
  pLeadTimeStatus[ltInd] = 1;
  pGenTimeComplete = pHasAllLeadsAtGen();
  if (pGenTimeComplete)
  {
    ILOGF(DEBUG, "All lead time processed at gen=%s",
	  ConvWxTime::stime(pGenTime).c_str());
  }
  return pGenTimeComplete;
}

//----------------------------------------------------------------
bool TriggerSequenceState::pHasAllLeadsAtGen(void) const
{
  for (size_t i=0; i<pLeadTimeStatus.size(); ++i)
  {
    if (pLeadTimeStatus[i] == 0)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------
void TriggerSequenceState::pClearLeadStatus(void)
{
  for (size_t i=0; i<pLeadTimeStatus.size(); ++i)
  {
    pLeadTimeStatus[i] = 0;
  }
}
