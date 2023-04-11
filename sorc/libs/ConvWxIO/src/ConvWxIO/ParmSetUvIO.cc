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
 * @file ParmSetUvIO.cc
 * @brief Algorithm parameters all in one place
 */

//----------------------------------------------------------------
#include <ConvWxIO/ParmSetUvIO.hh>
#include <ConvWxIO/ParmPcFcstIO.hh>

using std::string;

//----------------------------------------------------------------
ParmSetUvIO::ParmSetUvIO(void)  :  ParmSetUV()
{
}

//----------------------------------------------------------------
ParmSetUvIO::ParmSetUvIO(const ParmSetUV &parm) : ParmSetUV(parm)
{
}

//----------------------------------------------------------------
ParmSetUvIO::~ParmSetUvIO()
{
}

//----------------------------------------------------------------
bool ParmSetUvIO::loadVerifObsData(const time_t &t, const string &name,
				   const ParmProjection &proj, Grid &g) const
{
  for (int i=0; i<static_cast<int>(pFcst.size()); ++i)
  {
    ParmPcFcstIO parm(pFcst[i]);
    if (parm.loadVerifObsData(t, name, proj, g))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------
bool ParmSetUvIO::loadVerifFcstData(const time_t &gt, const int lt,
				  const string &name, 
				  const ParmProjection &proj, 
				  FcstGrid &g) const
{
  for (int i=0; i<static_cast<int>(pFcst.size()); ++i)
  {
    ParmPcFcstIO parm(pFcst[i]);
    if (parm.loadVerifFcstData(gt, lt, name, proj, g))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------
bool ParmSetUvIO::loadFcstData(const time_t &gt, const int lt,
			     const string &name, const ParmProjection &proj,
			     FcstGrid &g) const
{
  for (int i=0; i<static_cast<int>(pFcst.size()); ++i)
  {
    ParmPcFcstIO parm(pFcst[i]);
    if (parm.loadFcstData(gt, lt, name, proj, g))
    {
      return true;
    }
  }
  return false;
}

