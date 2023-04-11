#include <Epoch/ThresholdDatabaseParmsIO.hh>
#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWx/VerifThresh.hh>
#include "ParmsEnsLookupGenIO.hh"
#include "Params.hh"
#include "ConvWx/ConvWxConstants.hh"

using std::vector;
using std::string;

//
// failure return value
//
const int ParmsEnsLookupGenIO::failure = 1;

//-----------------------------------------------------------
ParmsEnsLookupGenIO::ParmsEnsLookupGenIO()
{
}

//-----------------------------------------------------------
ParmsEnsLookupGenIO::ParmsEnsLookupGenIO(int argc,
					 char **argv)
{
  //
  // Params is an automatically generated class of user defined parameters. 
  // Members of ParmsEnsLookupGenIO will be a superset of all user
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

  // Container for parameters for output dataset
  vector<ParmFcst> fcstOut;

  // Dataset parameters object
  ParmFcst a;

  a = ParmFcst("inputModel", "Forecast input", true);
  fcstIn.push_back(a);
  a = ParmFcst("outputModel", "Forecast output", true);
  fcstOut.push_back(a);
  
  // Load in main parameters, projection information, and inputs and output
  if (!InterfaceParm::loadConvWxParams(true, _main, _proj, fcstIn, fcstOut))
  {
    printf("Fatal problem loading parameters\n");

    exit(failure);
  }

  if (!InterfaceParm::loadProjectionParams(params.extendedProjectionFile,
					   _projExtended))
  {
    printf("Fatal problems loading parameters\n");
    exit(failure);
  }

  _fields = MultiThresholdDatabaseParms(params.threshParmFile_n,
					(const char **)params._threshParmFile);
  if (!_fields.isOk())
  {
    exit(failure);
  }
  if (!InterfaceParm::loadProjectionParams(params.extendedProjectionFile,
					   _projExtended))
  {
    printf("Fatal problems loading parameters, projection\n");
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
  
  _modelOut = fcstOut[0];


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
 
  _fieldNames = _fields.fieldNames();

  _maxAgeSeconds = static_cast<int>(params.maxAgeHours*3600);
  _triggerFeedbackSeconds =
    static_cast<int>(params.triggerFeedbackMinutes*60);
  _urlTimeoutSeconds = static_cast<int>(params.urlTimeoutMinutes*60);
  _urlDisableSeconds = static_cast<int>(params.urlDisableMinutes*60);

  _archiveSpdbBestOffsetSec = params.archiveSpdbBestOffsetHours*3600;
  _archiveSpdbMinOffsetSec = params._archiveSpdbOffsetHours[0]*3600;
  _archiveSpdbMaxOffsetSec = params._archiveSpdbOffsetHours[1]*3600;
  _realtimeSpdbMinOffsetSec = params._realtimeSpdbOffsetHours[0]*3600;
  _realtimeSpdbMaxOffsetSec = params._realtimeSpdbOffsetHours[1]*3600;

  if  (params.encodingType == Params::ENCODING_INT8)
  {
    _encodingType = Grid::ENCODING_INT8;
  }
  else if ( params.encodingType == Params::ENCODING_INT16)
  {
    _encodingType = Grid::ENCODING_INT16;
  }
  else if ( params.encodingType == Params::ENCODING_FLOAT32)
  {
    _encodingType = Grid::ENCODING_FLOAT32;
  }

  _nptSmoothTiledGrid = params.nptSmoothTiledGrid;
  _centerWeightTiledGrid = params.centerWeightTiledGrid; 
  _edgeWeightTiledGrid = params.edgeWeightTiledGrid;

  _debugSpdb = params.debugSpdb;
  _debugLat = params._debugLatlon[0];
  _debugLon = params._debugLatlon[1];
  if (_debugLat == 0 && _debugLon == 0)
  {
    _debugXY = false;
    _debugX = -1;
    _debugY = -1;
  }
  else
  {
    _debugXY = true;
    _proj.latlonToGridIndex(_debugLat, _debugLon, _debugX, _debugY);
  }    

  for (int i=0; i<params.debugTiles_n; ++i)
  {
    _debugTiles.push_back(params._debugTiles[i]);
  }

  _mapper = OutputToThreshProj(_proj, _projExtended);
  if (!_mapper.isOk())
  {
    printf("Fatal problems loading parameters\n");
    exit(failure);
  }    
  _maxNptInside = params.maximumNptInside;
  _numThreads = params.num_threads;
}

//-----------------------------------------------------------------
ParmsEnsLookupGenIO::~ParmsEnsLookupGenIO()
{
}
