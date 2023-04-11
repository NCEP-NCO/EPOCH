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
 * @mainpage ThreshHist
 *
 * @file MainThreshHist.cc
 */

#include <cstdlib>
#include "ThreshHistMgr.hh"
#include "ParmsThreshHistIO.hh"
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxConstants.hh>

/**
 * Cleanup function
 * @param[in] i  Signal
 */
static void tidyAndExit(int i);

//----------------------------------------------------------------
/** 
 * Create algorithm manager to run algorithm
 * @param[in] argc  Number of command line arguments is generally three for
 *                  real-time or six for archive mode.
 * @param[in] argv  Typical real-time and archive mode command lines are:
 *                  'ThreshHist -params ThreshHist.params' or
 *                  'ThreshHist -params ThreshHist.params 
 *                  -interval yyyymmddhhmmdss yyyymmddhhmmdss'
 *
 * @return integer status
 */
int main(int argc, char **argv)
{
  // Read in parameters to set ParmThreshHist object state
  ParmsThreshHistIO p(argc, argv);

  // Create object that manages things
  ThreshHistMgr m(p, tidyAndExit);

  // Run (infinite loop)
  m.run();

  // Exit
  tidyAndExit(convWx::GOOD_EXIT);
}

//----------------------------------------------------------------
static void tidyAndExit(int i)
{
  InterfaceLL::finish();
  exit(i);
}

