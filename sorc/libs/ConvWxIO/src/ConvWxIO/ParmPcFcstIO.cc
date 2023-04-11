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
 * @file ParmPcFcstIO.cc
 */

//----------------------------------------------------------------
#include <string>
#include <ConvWxIO/ParmPcFcstIO.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/InterfaceIO.hh>
using std::string;

//----------------------------------------------------------------
ParmPcFcstIO::ParmPcFcstIO(void) : ParmPcFcst()
{
}

//----------------------------------------------------------------
ParmPcFcstIO::ParmPcFcstIO(const ParmPcFcst &parm) :  ParmPcFcst(parm)
{
}

//----------------------------------------------------------------
ParmPcFcstIO::~ParmPcFcstIO()
{
}

//----------------------------------------------------------------
bool ParmPcFcstIO::loadVerifObsData(const time_t ot, const string &name,
				    const ParmProjection &proj,
				    Grid &outGrid) const
{
  if (!pVerifAndFcstSet)
  {
    return false;
  }
  if (!pVerifIsObs)
  {
    return false;
  }
  if (name != pName)
  {
    return false;
  }
  return InterfaceIO::loadObs(ot, proj, pVerif.pUrl, pVerif.pField,
			      pVerif.pRemap, outGrid);
}

//----------------------------------------------------------------
bool ParmPcFcstIO::loadVerifFcstData(const time_t gt, const int lt,
				     const string &name,
				     const ParmProjection &proj,
				     FcstGrid &outGrid) const
{
  if (!pVerifAndFcstSet)
  {
    return false;
  }
  if (pVerifIsObs)
  {
    return false;
  }
  if (name != pName)
  {
    return false;
  }
  ParmFcstIO parm(pVerif);
  return parm.loadFcstData(gt, lt, proj, outGrid);
}

//----------------------------------------------------------------
bool ParmPcFcstIO::loadFcstData(const time_t gt, const int lt,
				const string &name,
				const ParmProjection &proj,
				FcstGrid &outGrid) const
{
  if (name != pName)
  {
    return false;
  }
  ParmFcstIO parm(pFcst);
  return parm.loadFcstData(gt, lt, proj, outGrid);
}
