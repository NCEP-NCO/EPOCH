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
 * @mainpage CmorphAverager 
 * @brief CmorphAverager converts observed VIL values to integer
 *        values in the range [0,253], filtering out values that have 
 *        particular VIL_Flag values
 *
 * @file MainCmorphAverager.cc
 * @brief Read in user defined parameters. Create algorithm manager to run
 *        algorithm
 */

#define _MDV_PRUNE_MAIN_

#include "CmorphAveragerMgr.hh"
#include "ParmsCmorphAveragerIO.hh"
#include <ConvWx/InterfaceLL.hh>
#include <ConvWxIO/ILogMsg.hh>

/**
 * Return value of program to indicate success
 */ 
const static int success = 0;

/**
 *  Return value of program to indicate failure
 */ 
const static int failure = 1;

/**
 * Exit program, free memory, inform monitoring processes that program is
 * terminating, return signal to operating system
 * @param[in] sig  Signal
 */
static void cleanExit (int sig);

/**
 * New handler function 
 */
static void outOfStore(void);

/** 
 * Create algorithm manager to run algorithm
 * @param[in] argc  Number of command line arguments is generally three for
 *                  real-time or six for archive mode.
 * @param[in] argv  Typical real-time and archive mode command lines are:
 *                  'CmorphAverager -params ObsVil2DigiatlVil.params' or
 *                  'CmorphAverager -params CmorphAverager.params 
 *                  -interval yyyymmddhhmmdss yyyymmddhhmmdss'
 *
 * @return integer status
 */
int main(int argc, char **argv)
{
  //
  // Set new() failure handler function
  //
  std::set_new_handler(outOfStore);
 
  //
  // Read in parameters
  //
  ParmsCmorphAveragerIO params(argc, argv);
 
  //
  // Create object that does the conversion of VIL to digitaVil
  //
  CmorphAveragerMgr pruner(params, cleanExit);

  //
  // Run the program
  // 
  int iret;
  if ( pruner.run())
  {
    iret = success;
  }
  else
  {
    iret = failure;
  }
  
  cleanExit(iret);
  
  return (iret); 
}

static void cleanExit (int sig)
{ 
  //
  // Free memory, inform monitoring processes that program is terminating
  //
  InterfaceLL::finish();
  
  exit(sig);
}

static void outOfStore() 
{
  ILOG(FATAL, " Operator new failed - out of store");
  exit(failure);
}
