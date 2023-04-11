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
/**
 * @mainpage PrecipAccumCalc
 *
 * This application creates 3hr precip accum for each lead time from 
 * alternating 3hr and 6hr accumulations at sequential lead times.
 */

/**
 * @file MainPrecipAccumCalc.cc
 */

#define _PRECIPACCUMCALC_MAIN_

#include <cstdlib>
#include <iostream>
#include <ConvWx/InterfaceLL.hh>
#include "PrecipAccumCalcMgr.hh"
#include "ParmsPrecipAccumCalcIO.hh"

/**
 * Return value of program to indicate success
 */
const static int success = 0;

/**
 *  Return value of program to indicate failure
 */
const static int failure = 1;

/**
 * Exit program, return signal to operating system
 * @param[in] sig  Signal
 */
static void cleanExit(int sig);

/**
 * New handler function
 */
static void outOfStore(void);

/**
 * Pointer to algorithm
 */
static PrecipAccumCalcMgr *mgr = NULL;

/**
 * Create algorithm manager and run algorithm
 * @param[in] argc  Number of command line arguments is generally three for
 *                  real-time or six for archive mode.
 * @param[in] argv  Typical real-time and archive mode command lines are:
 *                  'PrecipAccumCalc -params PrecipAccumCalc.params' or
 *                  'PrecipAccumCalc -params PrecipAccumCalc.params -interval
 *                  yyyymmddhhmmdss yyyymmddhhmmdss'
 *
 * @return integer status
 */
int main(int argc, char **argv)
{

  //
  // set new() memory failure handler function
  //
  std::set_new_handler(outOfStore);

  //
  // Read in parameters
  //
  ParmsPrecipAccumCalcIO params(argc, argv);

  //
  // Create object that runs the algorithm
  //
  mgr = new PrecipAccumCalcMgr(params, cleanExit);

  //
  // Run the program
  //
  int iret;
  if ( mgr->run() == PrecipAccumCalcMgr::PRECIP_ACCUM_CALC_SUCCESS)
   {
    iret = success;
  }
  else
  {
    iret = failure;
  }

  cleanExit(iret);

  return iret;
}

static void cleanExit(int sig)

{    
  if (mgr != NULL)
  {
    delete mgr;
    mgr = NULL;
  }
  InterfaceLL::finish();
  
  exit(sig);
}

static void outOfStore()
{
  std::cerr << "FATAL ERROR - program PrecipAccumCalc " << std::endl;
  
  std::cerr << "  Operator new failed - out of store" << std::endl;
  
  if (mgr != NULL)
  {
    delete mgr;
    mgr = NULL;
  }
  exit(failure);
}
