#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWx/ParmFcst.hh>
#include <Epoch/TileRange.hh>
#include "ParmsThreshFromObarPbarIO.hh"
#include "Params.hh"
#include "ConvWx/ConvWxConstants.hh"

using std::vector;
using std::string;

//
// failure return value
//
const int ParmsThreshFromObarPbarIO::failure = 1;

//-----------------------------------------------------------
ParmsThreshFromObarPbarIO::ParmsThreshFromObarPbarIO()
{
}

//-----------------------------------------------------------
ParmsThreshFromObarPbarIO::ParmsThreshFromObarPbarIO(int argc, char **argv)
{
  //
  // Params is an automatically generated class of user defined parameters. 
  // Members of ParmsThreshFromObarPbarIO will be a superset of all user
  // defined  parameters.
  // The source code for generating the Params class will not
  // be provided and should be replaced.
  //
  Params params;

  if (!parmAppInit(params, argc, argv, Trigger::TRIGGER_NONE))
  {
    exit(convWx::BAD_EXIT);
  }

  std::vector<ParmFcst> nofcst;
  if (!InterfaceParm::loadConvWxParams(false, _main, _proj, nofcst))
  {
    printf("Fatal problem loading parameters\n");
    exit(failure);
  }

  if (!InterfaceParm::loadTilingParams(params.tilingParamFile, _tiling))
  {
    printf("Fatal..Problem with TDRP parameters\n");
    exit(failure);
  }

  parmAppFinish(params, _main);
  
  _pbarSpdb = params.pbarSpdb;

  if (_tiling.pTiling)
  {
    _tileInfo = TileInfo(_tiling.pTileNptX,
			 _tiling.pTileNptY, _tiling.pTileNptOverlapX,
			 _tiling.pTileNptOverlapY, _tiling.pMotherIsSubset,
			 _tiling.pMotherLowerLeftX, _tiling.pMotherLowerLeftY,
			 _tiling.pMotherUpperRightX, _tiling.pMotherUpperRightY,
			 _proj.pNx, _proj.pNy);
    _setTileLatLon();
  }
  else
  {
    _tileInfo = TileInfo(_proj.pNx, _proj.pNy);
  }

  _obarSpdb1 = params.obarSpdb1;
  _thresholdsSpdb1 = params.thresholdsSpdb1;
  _thresholdedFieldColdstartThresh1 = params.threshFieldColdstartThreshold1;
  for (int i=0; i<params.obarThreshTargetBias1_n; ++i)
  {
    _obarThreshTargetBias1.push_back(pair<double,double>(params._obarThreshTargetBias1[i].obarThresh,
							 params._obarThreshTargetBias1[i].targetBias));
    _obarThresh1.push_back(params._obarThreshTargetBias1[i].obarThresh);
  }
  //_obarThresh1
  //_targetBias1 = params.target_bias1;

  _obarSpdb2 = params.obarSpdb2;
  _thresholdsSpdb2 = params.thresholdsSpdb2;
  _thresholdedFieldColdstartThresh2 = params.threshFieldColdstartThreshold2;
  for (int i=0; i<params.obarThreshTargetBias2_n; ++i)
  {
    _obarThreshTargetBias2.push_back(pair<double,double>(params._obarThreshTargetBias2[i].obarThresh,
							 params._obarThreshTargetBias2[i].targetBias));
    _obarThresh2.push_back(params._obarThreshTargetBias2[i].obarThresh);
  }
  //  _targetBias2 = params.target_bias2;

  // for (int i=0; i<params.obarThreshTargetBias2_n; ++i)
  // {
  //   _obarThreshTargetBias2.push_back(params._obarThreshTargetBias2_n[i]);
  //   _obarThresh2
  // }

  _maxIncompleteSeconds = (int)(params.maxIncompleteDays*24.0*3600.0);

  _backfillDaysBack = params.backfillDaysBack;
  _thresholdsMaxSecondsBack = params.thresholdsMaxDaysBack*24*3600;

  _numThreads = params.num_threads;
  _debugState = params.debug_state;
}

//-----------------------------------------------------------------
ParmsThreshFromObarPbarIO::~ParmsThreshFromObarPbarIO()
{
}

//----------------------------------------------------------------
void ParmsThreshFromObarPbarIO::_setTileLatLon(void)
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
