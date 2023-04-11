/**
 * @file ParmsPrecipAccumCalcIO.cc
 * @brief Source code for user defined parameters class
 */ 

#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/Trigger.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <toolsa/LogStream.hh>
#include "Params.hh"
#include "ParmsPrecipAccumCalcIO.hh"

using std::vector;
using std::pair;
using std::string;



ParmsPrecipAccumCalcIO::ParmsPrecipAccumCalcIO()
{
}

ParmsPrecipAccumCalcIO::ParmsPrecipAccumCalcIO(int argc, char **argv)
{
  //
  // Params is a class of user defined parameters. Members of ParmsPrecipAccumCalcIO 
  // will be a superset of Params
  //
  Params params;

  if (!parmAppInit(params, argc, argv, Trigger::TRIGGER_NONE))
  {
    exit(PARMS_FAILURE);
  }

  //
  // Container for parameters for input dataset
  //
  vector<ParmFcst> fcstIn;

  //
  // Container for parameters for output dataset
  //
  vector<ParmFcst> fcstOut;

  //
  // Dataset parameters object
  // 
  ParmFcst a;

  a = ParmFcst("inputModel", "Forecast input", true);  
 
  fcstIn.push_back(a);

  a = ParmFcst("outputModel", "Forecast output", true);

  fcstOut.push_back(a);
  
  if (!InterfaceParm::loadConvWxParams(true, main, proj, fcstIn, fcstOut))
  {
    printf("Fatal..Problem with TDRP parameters\n");

    exit(PARMS_FAILURE);
  }
  
  parmAppFinish(params, main);

  //
  // Now assign class member values from tdrp params
  //
  for (int i = 0; i < params.ensembleMembers_n; i++)
  {
    //
    // Add ensemble member to inputs
    //
    ParmFcstIO ensMem(fcstIn[0]);

    ensMem.pUrl = ensMem.pUrl + "/" +
      params._ensembleMembers[i];

    modelInput.push_back(ensMem);

    modelUrls.push_back(ensMem.pUrl);
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


  modelOut = fcstOut[0];

  //
  // Three hour accumulation params
  //
  hr3AccumName = params.hr3AccumName;
  hr3AccumOutputDirTail =  params.hr3AccumOutputDirTail;

  //
  // Six hour accumulation params
  //
  hr6AccumName = params.hr6AccumName;

  if( params.outputDataType == Params::INT8)
  {
    outputDataType = INT8;
  }
  else if (params.outputDataType == Params::INT16)
  {
    outputDataType = INT16;
  }
  else
  {
    outputDataType = FLOAT32;
  }

  for (int i=0; i < params.extraDataFields_n; ++i)
  {
    extraDataFields.push_back(params._extraDataFields[i]);
  }
  

  hr3UlwrfName =  params.hr3UlwrfName;
  hr6UlwrfName  = params.hr6UlwrfName;
  
  forceAccumNonNegative = params.forceAccumNonNegative;

  numThreads = params.numThreads;

  for (int i=0; i<params.mdv_6hr_names_n; ++i)
  {
    pair<string,string> p(params._mdv_6hr_names[i].shortName,
			  params._mdv_6hr_names[i].longName);
    mdv6hrShortToLong.push_back(p);
  }

  for (int i=0; i<params.mdv_3hr_names_n; ++i)
  {
    pair<string,string> p(params._mdv_3hr_names[i].shortName,
			  params._mdv_3hr_names[i].longName);
    mdv3hrShortToLong.push_back(p);
  }

  string p = shortNameToLongName(6, hr6AccumName);
  if (p.empty())
  {
    LOG(ERROR) << "param file is bad";
    exit(-1);
  }
  longNames6hr.push_back(p);
  p = shortNameToLongName(6, hr6UlwrfName);
  if (p.empty())
  {
    LOG(ERROR) << "param file is bad";
    exit(-1);
  }
  longNames6hr.push_back(p);
  for (size_t i=0; i < extraDataFields.size(); i++)
  {
    p = shortNameToLongName(6, extraDataFields[i]);
    if (p.empty())
    {
      LOG(ERROR) << "param file is bad";
      exit(-1);
    }
    longNames6hr.push_back(p);
  }

  p = shortNameToLongName(3, hr3AccumName);
  if (p.empty())
  {
    LOG(ERROR) << "param file is bad";
    exit(-1);
  }
  longNames3hr.push_back(p);
  p = shortNameToLongName(3, hr3UlwrfName);
  if (p.empty())
  {
    LOG(ERROR) << "param file is bad";
    exit(-1);
  }
  longNames3hr.push_back(p);
  for (size_t i=0; i < extraDataFields.size(); i++)
  {
    p = shortNameToLongName(3, extraDataFields[i]);
    if (p.empty())
    {
      LOG(ERROR) << "param file is bad";
      exit(-1);
    }
    longNames3hr.push_back(p);
  }
}

ParmsPrecipAccumCalcIO::~ParmsPrecipAccumCalcIO()
{

}

