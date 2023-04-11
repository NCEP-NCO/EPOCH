/**
 * @file Info.cc
 */
#include "Info.hh"
#include "ThreshFromObarPbarMgr.hh"
#include "ParmsThreshFromObarPbar.hh"

//------------------------------------------------------------------
Info::Info(const time_t &genTime, ForecastState::LeadStatus_t state,
	   const ParmsThreshFromObarPbar &parms, ThreshFromObarPbarMgr *alg) :
  _genTime(genTime), _leadTime(state.leadSeconds), _state(state),
  _ltData(parms, genTime, state.leadSeconds), _alg(alg)
{
}

//------------------------------------------------------------------
Info::~Info()
{
}

//------------------------------------------------------------------
