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
#include <unistd.h>

/**
 * @file Trigger.cc
 */

//------------------------------------------------------------------
#include <ConvWxIO/Trigger.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

/**
 * True to print out triggering to log output
 */
static bool sPrintTriggering=true;

/**
 * Triggering mode
 */
static Trigger::Trigger_t sTriggering=Trigger::TRIGGER_FCST;

/**
 * True if triggering observations, false for forecasts, used only
 * when sTriggering = TRIGGER_OBS_OR_FCST
 */
static bool sTriggeringObs = false;

/**
 *  This needs to be set to true when sTriggering = TRIGGER_OBS_OR_FCST
 */
static bool sObsOrFcstIsSpecified = false;

/**
 * True if subsets of all trigger events is allowed
 */
static bool sTriggeringAllowSubsets=false;

//------------------------------------------------------------------
bool Trigger::trigger(const ParmMain &parms, time_t &time)
{
  if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }

  if (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sObsOrFcstIsSpecified)
  {
    ILOG(ERROR, "Triggering TRIGGER_OBS_OR_FCST without specifying");
    return false;
  }

  bool stat = true;
  if (sTriggering == Trigger::TRIGGER_OBS ||
      (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::REALTIME)
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, parms.pGtSubsampleMin,
				     parms.pDebugTrigger, time);
    }
    else
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, parms.pGtSubsampleMin,
				     parms.pDebugTrigger, parms.pArchiveT0,
				     parms.pArchiveT1, time);
    }
  }
  else if (sTriggering == Trigger::TRIGGER_FCST ||
	   (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::REALTIME)
    {
      stat = InterfaceIO::triggerFcst(parms.pTriggerUrl, parms.pIsLtSubsample,
				      parms.pLt0, parms.pLt1, parms.pDlt,
				      parms.pGtSubsampleMin,
				      parms.pDebugTrigger, time);
    }
    else
    {
      stat = InterfaceIO::triggerFcst(parms.pTriggerUrl, parms.pIsLtSubsample,
				      parms.pLt0, parms.pLt1, parms.pDlt,
				      parms.pGtSubsampleMin,
				      parms.pDebugTrigger, parms.pArchiveT0,
				      parms.pArchiveT1, time);
    }
  }
  else
  {
    ILOGF(ERROR, "Triggering in wrong mode=%s",
	 sprintMode(sTriggering).c_str());
    return false;
  }
  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered %s----------",
	   ConvWxTime::stime(time).c_str());
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }
  return stat;
}
  
//------------------------------------------------------------------
bool Trigger::archiveRewind(const ParmMain &parms)
{
  if (sPrintTriggering)
  {
    ILOG(PRINT, "------ rewind------");
  }
  if (parms.pTriggerType != ParmMain::ARCHIVE)
  {
    ILOG(ERROR, "Mode not ARCHIVE");
    return false;
  }
  else
  {
    return InterfaceIO::triggerArchiveRewind();
  }
}
  
//------------------------------------------------------------------
bool Trigger::sequenceTrigger(const ParmMain &parms, time_t &gt, int &lt)
{
  if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }

  if (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sObsOrFcstIsSpecified)
  {
    ILOG(ERROR, "Triggering TRIGGER_OBS_OR_FCST without specifying");
    return false;
  }

  bool stat = true;
  if (sTriggering == Trigger::TRIGGER_OBS ||
      (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, 
				     parms.pGtSubsampleMin,
				     parms.pDebugTrigger, 
				     parms.pArchiveT0, 
				     parms.pArchiveT1,
				     gt);
    }
    else
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, parms.pGtSubsampleMin,
				     parms.pDebugTrigger, gt);
    }
    lt = 0;
  }
  else if (sTriggering == Trigger::TRIGGER_FCST ||
	   (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerFcstNextArchive(parms.pTriggerUrl,
						 parms.pIsLtSubsample,
						 parms.pLt0, parms.pLt1,
						 parms.pDlt,
						 parms.pGtSubsampleMin,
						 parms.pArchiveT0,
						 parms.pArchiveT1,
						 parms.pDebugTrigger, gt, lt);
    }
    else
    {
      stat = InterfaceIO::triggerFcstNext(parms.pTriggerUrl,
					  parms.pIsLtSubsample,
					  parms.pLt0, parms.pLt1, parms.pDlt,
					  parms.pGtSubsampleMin,
					  parms.pDebugTrigger, gt, lt);
    }
  }
  else
  {
    ILOGF(ERROR, "Triggering in wrong mode=%s",
	 sprintMode(sTriggering).c_str());
    return false;
  }

  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered %s+%d----------",
	   ConvWxTime::stime(gt).c_str(), lt);
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }
  return stat;
} 

//------------------------------------------------------------------


bool Trigger::sequenceTriggerWLatency(const ParmMain &parms, time_t &gt, int &lt, int latency)
{
 if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }

  if (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sObsOrFcstIsSpecified)
  {
    ILOG(ERROR, "Triggering TRIGGER_OBS_OR_FCST without specifying");
    return false;
  }

  bool stat = true;
  if (sTriggering == Trigger::TRIGGER_OBS ||
      (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl,
                                     parms.pGtSubsampleMin,
                                     parms.pDebugTrigger,
                                     parms.pArchiveT0,
                                     parms.pArchiveT1,
                                     gt);
    }
    else
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, parms.pGtSubsampleMin,
                                     parms.pDebugTrigger, gt);
    }
    lt = 0;
  }
  else if (sTriggering == Trigger::TRIGGER_FCST ||
           (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerFcstNextArchive(parms.pTriggerUrl,
                                                 parms.pIsLtSubsample,
                                                 parms.pLt0, parms.pLt1,
                                                 parms.pDlt,
                                                 parms.pGtSubsampleMin,
                                                 parms.pArchiveT0,
                                                 parms.pArchiveT1,
                                                 parms.pDebugTrigger, gt, lt);
    }
    else
    {
      stat = InterfaceIO::triggerFcstNext(parms.pTriggerUrl,
                                          parms.pIsLtSubsample,
                                          parms.pLt0, parms.pLt1, parms.pDlt,
                                          parms.pGtSubsampleMin,
                                          parms.pDebugTrigger, gt, lt);

       time_t t = time(0);

       while( t - gt < latency)
       {
          sleep(1);
          InterfaceIO::doRegister("Sleeping");
          t = time(0);
       }
    }
  }
  else
  {
    ILOGF(ERROR, "Triggering in wrong mode=%s",
         sprintMode(sTriggering).c_str());
    return false;
  }

  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered %s+%d----------",
           ConvWxTime::stime(gt).c_str(), lt);
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }
  return stat;
}

//------------------------------------------------------------------


bool Trigger::sequenceTriggerWLatency(const ParmMain &parms, int latency,
				      const std::string &dataTimeUrl,
				      time_t &gt, int &lt)
				      
{
 if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }

  if (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sObsOrFcstIsSpecified)
  {
    ILOG(ERROR, "Triggering TRIGGER_OBS_OR_FCST without specifying");
    return false;
  }

  bool stat = true;
  if (sTriggering == Trigger::TRIGGER_OBS ||
      (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl,
                                     parms.pGtSubsampleMin,
                                     parms.pDebugTrigger,
                                     parms.pArchiveT0,
                                     parms.pArchiveT1,
                                     gt);
    }
    else
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, parms.pGtSubsampleMin,
                                     parms.pDebugTrigger, gt);
    }
    lt = 0;
  }
  else if (sTriggering == Trigger::TRIGGER_FCST ||
           (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerFcstNextArchive(parms.pTriggerUrl,
                                                 parms.pIsLtSubsample,
                                                 parms.pLt0, parms.pLt1,
                                                 parms.pDlt,
                                                 parms.pGtSubsampleMin,
                                                 parms.pArchiveT0,
                                                 parms.pArchiveT1,
                                                 parms.pDebugTrigger, gt, lt);
    }
    else
    {
      stat = InterfaceIO::triggerFcstNext(parms.pTriggerUrl,
                                          parms.pIsLtSubsample,
                                          parms.pLt0, parms.pLt1, parms.pDlt,
                                          parms.pGtSubsampleMin,
                                          parms.pDebugTrigger, gt, lt);
      if (stat)
      {
	time_t tlast = 0;
	while (true)
	{
	  time_t t;
	  if (InterfaceIO::newestObsTime(gt-24*3600, gt + latency + 24*3600,
					 dataTimeUrl, t))
	  {
	    if (t - gt < latency)
	    {
	      InterfaceIO::doRegister("Sleeping");
	      if (tlast != t)
	      {
		tlast = t;
		ILOGF(DEBUG,
		    "obs data %s not %d seconds newer than gen time %s--wait",
		    ConvWxTime::stime(t).c_str(), latency,
		    ConvWxTime::stime(gt).c_str());
	      }
	      sleep(1);
	    }
	    else
	    {
	      ILOGF(DEBUG,
		    "obs data %s at least %d seconds newer than gen time %s -- complete trigger now",
		    ConvWxTime::stime(t).c_str(), latency,
		    ConvWxTime::stime(gt).c_str());
	      break;
	    }
	  }
	  else
	  {
	    ILOGF(WARNING, "Waiting for any obs data within a day of triggering gen time %s",
		  ConvWxTime::stime(gt).c_str());
	    InterfaceIO::doRegister("Sleeping");
	    sleep(10);
	  }
	}
      }
    }
  }
  else
  {
    ILOGF(ERROR, "Triggering in wrong mode=%s",
         sprintMode(sTriggering).c_str());
    return false;
  }

  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered %s+%d----------",
           ConvWxTime::stime(gt).c_str(), lt);
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }
  return stat;
}

//------------------------------------------------------------------
bool Trigger::sequenceTriggerNoWait(const ParmMain &parms,
				    const time_t &oldGt,
				    int oldLt,
				    time_t &gt, int &lt,
				    bool &timedOut)
{
  if (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sObsOrFcstIsSpecified)
  {
    ILOG(ERROR, "Triggering TRIGGER_OBS_OR_FCST without specifying");
    return false;
  }

  bool stat = true;
  if (sTriggering == Trigger::TRIGGER_OBS ||
      (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl,
                                     parms.pGtSubsampleMin,
                                     parms.pDebugTrigger,
                                     parms.pArchiveT0,
                                     parms.pArchiveT1,
                                     gt);
      timedOut = false;
    }
    else
    {
      stat = InterfaceIO::triggerObsNoWait(parms.pTriggerUrl,
					   parms.pGtSubsampleMin,
					   parms.pDebugTrigger, gt,
					   timedOut);
    }
    lt = 0;
  }
  else if (sTriggering == Trigger::TRIGGER_FCST ||
           (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerFcstNextArchive(parms.pTriggerUrl,
                                                 parms.pIsLtSubsample,
                                                 parms.pLt0, parms.pLt1,
                                                 parms.pDlt,
                                                 parms.pGtSubsampleMin,
                                                 parms.pArchiveT0,
                                                 parms.pArchiveT1,
                                                 parms.pDebugTrigger, gt, lt);
      timedOut = false;
    }
    else
    {
      stat = InterfaceIO::triggerFcstNextNoWait(parms.pTriggerUrl,
						parms.pIsLtSubsample,
						parms.pLt0, parms.pLt1,
						parms.pDlt,
						parms.pGtSubsampleMin,
						parms.pDebugTrigger,
						gt,lt, timedOut);
    }
  }
  else
  {
    ILOGF(ERROR, "Triggering in wrong mode=%s",
         sprintMode(sTriggering).c_str());
    return false;
  }

  if (!timedOut)
  {
      if (gt == oldGt && lt == oldLt && parms.pTriggerType != ParmMain::ARCHIVE)
      {
	ILOG(WARNING, "THE BAD SITUATION");
	ILOGF(WARNING, "--------RE-Triggered %s+%d----------",
	      ConvWxTime::stime(gt).c_str(), lt);
	timedOut = true;
	stat = false;
      }
  }
  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered %s+%d----------",
           ConvWxTime::stime(gt).c_str(), lt);
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      if (timedOut)
      {
	// ILOG(PRINT, "triggering time out");
      }
      else
      {
	ILOG(PRINT, "no more triggering");
      }
    }
  }
  return stat;
}

//------------------------------------------------------------------
bool Trigger::sequenceTrigger(const ParmMain &parms, time_t &time)
{
  if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }
 
  if (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sObsOrFcstIsSpecified)
  {
    ILOG(ERROR, "Triggering TRIGGER_OBS_OR_FCST without specifying");
    return false;
  }

  bool stat = true;
  if (sTriggering == Trigger::TRIGGER_OBS ||
      (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && sTriggeringObs))
  {
    if (parms.pTriggerType == ParmMain::ARCHIVE)
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl,
				     parms.pGtSubsampleMin,
				     parms.pDebugTrigger,
				     parms.pArchiveT0, 
				     parms.pArchiveT1,
				     time);
    }
    else
    {
      stat = InterfaceIO::triggerObs(parms.pTriggerUrl, parms.pGtSubsampleMin,
				     parms.pDebugTrigger, time);
    }
  }
  else if (sTriggering == Trigger::TRIGGER_FCST ||
	   (sTriggering == Trigger::TRIGGER_OBS_OR_FCST && !sTriggeringObs))
  {
    ILOG(ERROR, 
	"wrong method.. does not return lead time with forecast triggering");
    stat = false;
  }
  else
  {
    ILOGF(ERROR, "Triggering in wrong mode=%s",
	 sprintMode(sTriggering).c_str());
    return false;
  }

  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered %s----------",
	   ConvWxTime::stime(time).c_str());
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }
  return stat;
}

//------------------------------------------------------------------
bool Trigger::filepathTrigger(const ParmMain &parms, 
			      const int daysData, 
			      string &filepath)
{
  if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }
  bool stat = true;

  time_t t0;
  time_t t1;

  if (sTriggering != Trigger::TRIGGER_FILE)
  {
    ILOGF(ERROR, "Triggering in wrong mode = %s",
	sprintMode(sTriggering).c_str());
    return false;
  }

  if (parms.pTriggerType == ParmMain::ARCHIVE)
  {

    t0 = parms.pArchiveT0;
    t1 = parms.pArchiveT1;
    stat = InterfaceIO::triggerFile(parms.pTriggerUrl,
                                    parms.pDebugTrigger,
                                    t0,
                                    t1,
                                    filepath);

   
  }
  else
  {
    t1 = time(0);
    t0 = t1 - daysData * convWx::SECS_PER_DAY;
    stat = InterfaceIO::triggerFile(parms.pTriggerUrl,
                                    parms.pDebugTrigger,
                                    t0,
                                    t1,
                                    filepath);

  }
  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered interval [%s, %s]----------",
	   ConvWxTime::stime(t0).c_str(), ConvWxTime::stime(t1).c_str());
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }

  return stat;

}

//------------------------------------------------------------------
bool Trigger::filepathTriggerRealtime(const ParmMain &parms, 
				      const int daysData, 
				      const time_t &currentT,
				      string &filePath)
{
  if (parms.pTriggerType == ParmMain::ARCHIVE)
  {
    return filepathTrigger(parms, daysData, filePath);
  }
  if (sTriggering != Trigger::TRIGGER_FILE)
  {
    ILOGF(ERROR, "Triggering in wrong mode = %s",
	  sprintMode(sTriggering).c_str());
    return false;
  }

  //
  // at this point it is real time mode, which is first use of currentT
  // otherwise currentT not used
  //

  if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }

  bool stat = true;

  time_t t0 = currentT - daysData * convWx::SECS_PER_DAY;
  stat = InterfaceIO::triggerFile(parms.pTriggerUrl,
				  parms.pDebugTrigger,
				  t0, currentT, filePath);
  if (stat)
  {
    if (sPrintTriggering)
    {
      ILOGF(PRINT, "--------Triggered interval [%s, %s]----------",
	   ConvWxTime::stime(t0).c_str(), ConvWxTime::stime(currentT).c_str());
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }

  return stat;

}

//------------------------------------------------------------------

bool Trigger::filepathTriggerTwoUrls(const ParmMain &parms, 
                                     const std::string triggerUrl1,
                                     std::string &filepath1,
                                     const std::string triggerUrl2,
                                     std::string &filepath2,
                                     const int daysData)

{
  if (sPrintTriggering)
  {
    ILOG(PRINT, "------before trigger------");
  }
  bool stat = true;


  time_t t0;
  time_t t1;

  if (sTriggering != Trigger::TRIGGER_FILE)
  {
    ILOGF(ERROR, "Triggering in wrong mode = %s",
        sprintMode(sTriggering).c_str());
    return false;
  }
 
  const bool isGenLeadForecastFormat = false;
 
  bool isRealtime;
 
  if (parms.pTriggerType == ParmMain::ARCHIVE)
  {
    isRealtime = false;
    t0 = parms.pArchiveT0;
    t1 = parms.pArchiveT1;
    stat = InterfaceIO::triggerFiles(triggerUrl1,
                                     filepath1,
                                     triggerUrl2,
                                     filepath2,
                                     parms.pDebugTrigger,
                                     t0, t1, isGenLeadForecastFormat);
    

  }
  else
  {
    isRealtime = true;
    int realtimeFileLookBack = 86400;
    stat = InterfaceIO::triggerFilesRealtime(triggerUrl1,
                                             filepath1,
                                             triggerUrl2,
                                             filepath2,
                                             realtimeFileLookBack,
                                             parms.pDebugTrigger, 
                                             isGenLeadForecastFormat);
  }
  if (stat)
  {
    if( sPrintTriggering)
    {
       if (isRealtime)
       {
         time_t t1 = time(0);
         ILOGF(PRINT, "--------Triggered interval [%s, %s]----------",
              ConvWxTime::stime(t1 - 86400).c_str(), ConvWxTime::stime(t1).c_str());
       }
       else 
       {
         ILOGF(PRINT, "--------Triggered interval [%s, %s]----------",
              ConvWxTime::stime(t0).c_str(), ConvWxTime::stime(t1).c_str());
       }
    }
  }
  else
  {
    if (sPrintTriggering)
    {
      ILOG(PRINT, "no more triggering");
    }
  }

  return stat;

}


//------------------------------------------------------------------
void Trigger::setTriggeringFcst(const bool isFcst)
{
  if (sTriggering != TRIGGER_OBS_OR_FCST)
  {
    ILOGF(WARNING, "specifiying mode with wrong trigger type %s",
	 sprintMode(sTriggering).c_str());
  }
  sTriggeringObs = !isFcst;
  sObsOrFcstIsSpecified = true;
}

//------------------------------------------------------------------
bool Trigger::allowSubsets(void)
{
  return sTriggeringAllowSubsets;
}

//------------------------------------------------------------------
Trigger::Trigger_t Trigger::getTriggering(void)
{
  return sTriggering;
}

//------------------------------------------------------------------
bool Trigger::isForecastTrigger(void)
{
  if (sTriggering != TRIGGER_OBS_OR_FCST)
  {
    ILOGF(WARNING, "asking for mode from wrong trigger type %s",
	 sprintMode(sTriggering).c_str());
    return false;
  }
  else
  {
    if (!sObsOrFcstIsSpecified)
    {
      ILOG(WARNING,
	  "Asked for mode without specifying mode, TRIGGERING_OBS_OR_FCST");
    return false;
    }      
    else
    {
      return !sTriggeringObs;
    }
  }
}

//------------------------------------------------------------------
void
Trigger::setTriggeringFlags(const Trigger::Trigger_t triggerType,
			    const bool allowSubsetTriggering)
{
  sTriggering = triggerType;
  sTriggeringObs = false;
  sObsOrFcstIsSpecified = false;
  sTriggeringAllowSubsets = allowSubsetTriggering;
  if ((triggerType == TRIGGER_FILE || triggerType == TRIGGER_NONE) &&
      allowSubsetTriggering)
  {
    ILOGF(WARNING, "subset triggering in mode %s not allowed, ignore",
	 sprintMode(triggerType).c_str());
    sTriggeringAllowSubsets = false;
  }
}

//------------------------------------------------------------------
void Trigger::setTriggeringPrint(const bool printTriggering)
{
  sPrintTriggering = printTriggering;
}


//------------------------------------------------------------------
std::string Trigger::sprintMode(const Trigger::Trigger_t &mode)
{
  std::string ret = "";

  switch (mode)
  {
  case TRIGGER_OBS:
    ret = "TRIGGER_OBS";
    break;
  case TRIGGER_FCST:
    ret = "TRIGGER_FCST";
    break;
  case TRIGGER_OBS_OR_FCST:
    ret = "TRIGGER_OBS_OR_FCST";
    break;
  case TRIGGER_FILE:
    ret = "TRIGGER_FILE";
    break;
  case TRIGGER_NONE:
    ret = "TRIGGER_NONE";
    break;
  default:
    ret = "TRIGGER_UNKNOWN";
    break;
  }
  return ret;
}
