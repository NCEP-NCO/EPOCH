/**
 * @file Info.cc
 */
#include "Info.hh"
#include "PbarComputeMgr.hh"
#include "ParmsPbarCompute.hh"

//------------------------------------------------------------------
Info::Info(const time_t &genTime, ForecastState::LeadStatus_t state,
	   const ParmsPbarCompute &parms, PbarComputeMgr *alg) :
  _genTime(genTime), _leadTime(state.leadSeconds), _state(state),
  _ltData(parms, genTime, state.leadSeconds),
  _alg(alg)
{
}

//------------------------------------------------------------------
Info::~Info()
{
}

//------------------------------------------------------------------
