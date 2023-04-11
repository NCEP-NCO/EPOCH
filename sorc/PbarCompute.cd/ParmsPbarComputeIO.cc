#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWx/VerifThresh.hh>
#include <Epoch/TileRange.hh>
#include "ParmsPbarComputeIO.hh"
#include "Params.hh"
#include "ConvWx/ConvWxConstants.hh"
#include <algorithm>

using std::vector;
using std::string;

//
// failure return value
//
const int ParmsPbarComputeIO::failure = 1;

//-----------------------------------------------------------
ParmsPbarComputeIO::ParmsPbarComputeIO()
{
}

//-----------------------------------------------------------
ParmsPbarComputeIO::ParmsPbarComputeIO(int argc, char **argv)
{
  //
  // Params is an automatically generated class of user defined parameters. 
  // Members of ParmsPbarComputeIO will be a superset of all user
  // defined  parameters.
  // The source code for generating the Params class will not
  // be provided and should be replaced.
  //
  Params params;

  if (!parmAppInit(params, argc, argv, Trigger::TRIGGER_NONE))
  {
    exit(convWx::BAD_EXIT);
  }

  // Container for parameters for input dataset
  vector<ParmFcst> fcstIn;

  // Dataset parameters object
  ParmFcst a;

  a = ParmFcst("inputModel", "Forecast input", true);
  fcstIn.push_back(a);
  
  // Load in main parameters, projection information, and inputs and output
  if (!InterfaceParm::loadConvWxParams(true, _main, _proj, fcstIn))
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
  
  // Now assign class member values from tdrp params
  for (int i = 0; i < params.ensembleMembers_n; i++)
  {

    // Add ensemble member to inputs
    ParmFcstIO ensMem(fcstIn[0]);
    ensMem.pUrl = ensMem.pUrl + "/" +  params._ensembleMembers[i];
    _modelInput.push_back(ensMem);
    _modelUrls.push_back(ensMem.pUrl);
  }
  
  // Container for times in hours
  vector <double> leadTimeHours = _modelInput[0].getLtHours();

  for (int i = 0; i < static_cast<int>(leadTimeHours.size()); ++i)
  {
    //
    // Lead time seconds
    //
    int secs = static_cast<int>(leadTimeHours[i] * convWx::SECS_PER_HOUR);
    _leadSeconds.push_back(secs);
  }
  
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

  _maxAgeSeconds = static_cast<int>(params.maxAgeHours*3600);

  _triggerFeedbackSeconds =
    static_cast<int>(params.triggerFeedbackMinutes*60);
  _urlTimeoutSeconds = static_cast<int>(params.urlTimeoutMinutes*60);
  _urlDisableSeconds = static_cast<int>(params.urlDisableMinutes*60);

  _threshMin1 = params.threshMin1;
  _threshMax1 = params.threshMax1;
  _threshDelta1 = params.threshDelta1;
  _threshMin2 = params.threshMin2;
  _threshMax2 = params.threshMax2;
  _threshDelta2 = params.threshDelta2;
  _numThresh1 = 0;
  for (double x=_threshMin1; x<=_threshMax1; x += _threshDelta1)
  {
    ++_numThresh1;
  }
  _numThresh2 = 0;
  for (double x=_threshMin2; x<=_threshMax2; x += _threshDelta2)
  {
    ++_numThresh2;
  }
  
  _pbarSpdb = params.pbarSpdb;

  _inputThresholdedField1 = params.inputThreshField1;
  _thresholdedFieldColdstartThresh1 = params.threshFieldColdstartThreshold1;
  _thresholdedFieldComparison1 = params.thresholdedComparison1;

  _inputThresholdedField2 = params.inputThreshField2;
  _thresholdedFieldColdstartThresh2 = params.threshFieldColdstartThreshold2;
  _thresholdedFieldComparison2 = params.thresholdedComparison2;
  
  _hasFixedField1 = params.hasFixedField1;
  _inputFixedField1 = params.inputFixedField1;
  _fixedFieldThresh1 = params.fixedThreshold1;
  _fixedFieldComparison1 = params.fixedFieldComparison1;

  _hasFixedField2 = params.hasFixedField2;
  _inputFixedField2 = params.inputFixedField2;
  _fixedFieldThresh2 = params.fixedThreshold2;
  _fixedFieldComparison2 = params.fixedFieldComparison2;
    
  _inputFieldNames.push_back(_inputThresholdedField1);
  _inputFieldNames.push_back(_inputThresholdedField2);
  if (_hasFixedField1)
  {
    _inputFieldNames.push_back(_inputFixedField1);
  }
  if (_hasFixedField2)
  {
    if (find(_inputFieldNames.begin(), _inputFieldNames.end(),
	     _inputFixedField2) == _inputFieldNames.end())
    {
      _inputFieldNames.push_back(_inputFixedField2);
    }
  }

  _ltHours.clear();
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    _ltHours.push_back((double)_leadSeconds[i]/3600.0);
  }
  _thresh1.clear();
  for (double thr=_threshMin1; thr<= _threshMax1; thr+=_threshDelta1)
  {
    _thresh1.push_back(thr);
  }

  _thresh2.clear();
  for (double thr=_threshMin2; thr<= _threshMax2; thr+=_threshDelta2)
  {
    _thresh2.push_back(thr);
  }

  _numThreads = params.num_threads;
  _debugState = params.debug_state;
}

//-----------------------------------------------------------------
ParmsPbarComputeIO::~ParmsPbarComputeIO()
{
}

//----------------------------------------------------------------
void ParmsPbarComputeIO::_setTileLatLon(void)
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
