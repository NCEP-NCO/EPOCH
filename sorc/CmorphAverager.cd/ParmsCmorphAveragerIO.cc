// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// Copyright, University Corporation for Atmospheric Research (UCAR) 2009-2017. 
// The Government's right to use this data and/or software is restricted per 
// the terms of Cooperative Agreement between UCAR and the National  Science 
// Foundation, to government use only which includes the nonexclusive, 
// nontransferable, irrevocable, royalty-free license to exercise or have 
// exercised for or on behalf of the U.S. Government throughout the world. 
// All other rights are reserved. 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/**
 * @file ParmsCmorphAveragerIO.cc
 * @brief Source code for setting parameters 
 */


#include "ParmsCmorphAveragerIO.hh"
#include "Params.hh"

using std::vector;
using std::string;

ParmsCmorphAveragerIO::ParmsCmorphAveragerIO(void)
{
}

ParmsCmorphAveragerIO::ParmsCmorphAveragerIO(int argc, char **argv)
{ 

  Params params;

  if (!parmAppInit(params, argc, argv, Trigger::TRIGGER_OBS, true))
  {
    exit(PARMS_FAILURE);
  }
  
  vector<ParmFcst> obsIn;
  
  vector<ParmFcst> obsOutput;
  
  ParmFcst a;

  a = ParmFcst("inputObs", "Observation input", false);
  
  obsIn.push_back(a);

  a = ParmFcst("outputObs", "Observation output", false);
  
  obsOutput.push_back(a);

 
  if (!InterfaceParm::loadConvWxParams(false, main, proj, obsIn, obsOutput))
  {
    printf("Fatal..Problem with TDRP parameters\n");
  
    exit(PARMS_FAILURE);
  }

  parmAppFinish(params, main);
  
  obs = obsIn[0];
  obsOut = ParmFcstIO(obsOutput[0]);

  unitsConvert = params.unitsConvert;

}

ParmsCmorphAveragerIO::~ParmsCmorphAveragerIO()
{
}
