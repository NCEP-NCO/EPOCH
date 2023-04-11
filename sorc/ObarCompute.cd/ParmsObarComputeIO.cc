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
 * @file ParmsObarComputeIO.cc
 */

//----------------------------------------------------------------
#include "ParmsObarComputeIO.hh"
#include "Params.hh"
#include <Epoch/TileRange.hh>
#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <algorithm>

//----------------------------------------------------------------
ParmsObarComputeIO::ParmsObarComputeIO(void)
{
}

//----------------------------------------------------------------
ParmsObarComputeIO::ParmsObarComputeIO(int argc, char **argv)
{
  Params p;

  // Triggers off of observation data
  if (!parmAppInit(p, argc, argv, Trigger::TRIGGER_OBS, true))
  {
    exit(convWx::BAD_EXIT);
  }

  vector<ParmFcst> obsIn, fcstIn;
  vector<ParmFcst> fcstOut, obsOut;

  ParmFcst P("obs input", "cmorph", true);
  obsIn.push_back(P);

  if (!InterfaceParm::loadConvWxParams(false, _main, _proj, obsIn))
  {
    printf("Fatal..Problem with TDRP parameters\n");
    exit(convWx::BAD_EXIT);
  }
  if (!InterfaceParm::loadTilingParams(p.tiling_param_file, _tiling))
  {
    printf("Fatal..Problem with TDRP parameters\n");
    exit(convWx::BAD_EXIT);
  }
  parmAppFinish(p, _main);

  if (_tiling.pTiling)
  {
    _tileInfo = TileInfo(_tiling.pTileNptX,
			 _tiling.pTileNptY, _tiling.pTileNptOverlapX,
			 _tiling.pTileNptOverlapY,_tiling.pMotherIsSubset,
			 _tiling.pMotherLowerLeftX, _tiling.pMotherLowerLeftY,
			 _tiling.pMotherUpperRightX, _tiling.pMotherUpperRightY,
			 _proj.pNx, _proj.pNy);
    _setTileLatLon();
  }
  else
  {
    _tileInfo = TileInfo(_proj.pNx, _proj.pNy);
  }

  // Set local state from what was returned.
  _obs = obsIn[0];
  _obarSpdb = p.obar_spdb;
  
  _inputField = p.input_field;
  for (int i=0; i<p.obs_threshold_n; ++i)
  {
    _obsThreshold.push_back(p._obs_threshold[i]);
  }
}

//----------------------------------------------------------------
ParmsObarComputeIO::~ParmsObarComputeIO()
{
}

//----------------------------------------------------------------
void ParmsObarComputeIO::_setTileLatLon(void)
{
  TileLatLon latlons;

  for (int i=0; i<_tileInfo.numTiles(); ++i)
  {
    TileRange range = _tileInfo.range(i);
    int x0 = range.getX0();
    int y0 = range.getY0();
    int x1 = x0 + range.getNx()-1;
    int y1 = y0 + range.getNy()-1;
    double lat, lon;
    _proj.gridIndexToLatlon((x0+x1)/2, (y0+y1)/2, lat, lon);
    latlons.add(i, lat, lon);
  }
  latlons.setOk();  // after adding
  _tileInfo.setLatLons(latlons);
}
