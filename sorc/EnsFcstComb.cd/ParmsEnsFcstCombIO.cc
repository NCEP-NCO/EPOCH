#include "ParmsEnsFcstCombIO.hh"
#include "Params.hh"
#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWx/VerifThresh.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::vector;
using std::string;
using std::map;

//
// failure return value
//
const int ParmsEnsFcstCombIO::failure = 1;

ParmsEnsFcstCombIO::ParmsEnsFcstCombIO()
{
}

ParmsEnsFcstCombIO::ParmsEnsFcstCombIO(int argc, char **argv)
{
  //
  // Params is an automatically generated class of user defined parameters. 
  // Members of ParmsEnsFcstCombIO will be a superset of all user defined 
  // parameters. The source code for generating the Params class will not
  // be provided and should be replaced.
  //
  Params params;

  if (!parmAppInit(params, argc, argv, Trigger::TRIGGER_NONE))
  {
    exit(convWx::BAD_EXIT);
  }

  //
  // Container for parameters for variable number of input datasets
  //
  vector<ParmFcst> fcstIn;

  //
  // Container for parameters for output dataset
  //
  vector<ParmFcst> fcstOut;

  ParmFcst a;

  a = ParmFcst("outputModel", "Forecast output", true);

  fcstOut.push_back(a);
  
  //
  // Load in main parameters, projection information, and inputs and output
  //
  if (!InterfaceParm::loadVarinConvWxParams(true, main, proj, fcstIn, fcstOut))
  {
    printf("Fatal problem loading parameters\n");

    exit(failure);
  }
  
  parmAppFinish(params, main);
  
  for (int i = 0; i < (int) fcstIn.size(); i++)
  {
    modelInput.push_back(fcstIn[i]);
  }

  modelOut = fcstOut[0];
 
  //
  // Container for times in hours
  //
  vector <double> leadTimeHours = modelInput[0].getLtHours();

  for (int i = 0; i < static_cast<int>(leadTimeHours.size()); ++i)
  {
    //
    // Lead time seconds
    //
    int secs = static_cast<int>(leadTimeHours[i] * convWx::SECS_PER_HOUR);

    leadSeconds.push_back(secs);
  }

  for (int i=0; i<params.fieldNames_n; ++i)
  {
    fieldNames.push_back(params._fieldNames[i]);
  }
 
  for (int i=0; i<params.modelWeights_n;i++)
  {
    weights.push_back(params._modelWeights[i]);
  }

  maxAgeSeconds = static_cast<int>(params.maxAgeHours*3600);
  triggerFeedbackSeconds =
    static_cast<int>(params.triggerFeedbackMinutes*60);
  urlTimeoutSeconds = static_cast<int>(params.urlTimeoutMinutes*60);
  urlDisableSeconds = static_cast<int>(params.urlDisableMinutes*60);

  for (int i=0; i<params.genTimeProcessHours_n; i++)
  {
    genTimeProcessHours.push_back(params._genTimeProcessHours[i]);
  }

  if  (params.encodingType == Params::ENCODING_INT8)
  {
    outputEncodingType = Grid::ENCODING_INT8;
  }
  else if ( params.encodingType == Params::ENCODING_INT16)
  {
    outputEncodingType = Grid::ENCODING_INT16;
  }
  else if ( params.encodingType == Params::ENCODING_FLOAT32)
  {
    outputEncodingType = Grid::ENCODING_FLOAT32;
  }
}

ParmsEnsFcstCombIO::~ParmsEnsFcstCombIO()
{
}

