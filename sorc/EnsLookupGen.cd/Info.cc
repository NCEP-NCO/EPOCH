/**
 * @file Info.cc
 */
#include "Info.hh"

//------------------------------------------------------------------
Info::Info(const time_t &t, int lt, EnsLookupGenMgr *alg) :
   _genTime(t), _lt(lt), _alg(alg)
{
}

//------------------------------------------------------------------
Info::~Info()
{
}

//------------------------------------------------------------------
