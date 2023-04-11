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
 * @file ThreshHistMgr.cc
 */

#include "ThreshHistMgr.hh"
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

//----------------------------------------------------------------
ThreshHistMgr::
ThreshHistMgr(const ParmsThreshHistIO &parms, void tidyAndExit(int)) :
  _parms(parms), _trigger(NULL), _reader(parms._inputSpdb),
  _writer(parms._outputSpdb)
{
  time_t t = time(0);
  LOG(DEBUG) << "Restarted at " << DateTime::strn(t);
  if (parms._mainParms.isRealtime())
  {
    _trigger = new DsUrlTrigger(parms._mainParms.pTriggerUrl,
				DsUrlTrigger::OBS, false);
  }
  else
  {
    _trigger = new DsUrlTrigger(parms._mainParms.pArchiveT0,
				parms._mainParms.pArchiveT1,
				parms._mainParms.pTriggerUrl,
				DsUrlTrigger::OBS, false, true);
  }

  // Standard initialization
  if (!InterfaceParm::driverInit(_parms._mainParms, tidyAndExit))
  {
    tidyAndExit(convWx::BAD_EXIT);
  }
}

//----------------------------------------------------------------
ThreshHistMgr::~ThreshHistMgr()
{
  if (_trigger != NULL)
  {
    delete _trigger;
    _trigger = NULL;
  }
}

//----------------------------------------------------------------
void ThreshHistMgr::run(void)
{
  // Infinite loop of triggering new data 
  time_t t;

  while (_trigger->nextTime(t))
  {
    _process(t);
  }
}

//----------------------------------------------------------------
void ThreshHistMgr::_process(const time_t &t)
{
  LOG(DEBUG) << "Processing " << DateTime::strn(t);
  if (!_reader.read(t))
  {
    LOG(DEBUG) << "Couldn't read at " << DateTime::strn(t);
  }
  if (_reader.average(t, _parms._daysBack, _parms._coldstartThresh,
		      _writer))
  {
    _writer.write();
  }
}

