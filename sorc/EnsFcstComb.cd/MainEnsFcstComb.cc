 
/**
 * @mainpage EnsFcstComb
 *
 * This application combines grids of probabilities  that a given field 
 * has a value greater than a user specified threshold based on evaluation 
 * of ensemble forecasts. 
 */

/**
 * @file MainEnsFcstComb.cc
 */

#define _ENS_FCST_COMB_

#include <cstdlib>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include "EnsFcstCombMgr.hh"

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
static void cleanExit (int sig);

/**
 * New handler function
 */
static void outOfStore(void);

/**
 * Create algorithm manager and run algorithm 
 * @param[in] argc  Number of command line arguments is generally three for
 *                  real-time or six for archive mode.
 * @param[in] argv  Typical real-time and archive mode command lines are:
 *                  'EnsFcstComb -params EnsFcstComb.params' or
 *                  'EnsFcstComb -params EnsFcstComb.params -interval
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
  ParmsEnsFcstCombIO params(argc, argv);

  //
  // Create object that runs the algorithm               
  //
  EnsFcstCombMgr ensFcstCombMgr(params, cleanExit);


  InterfaceIO::startup(params.main.pProcessName, 
                       params.main.pInstance,
                       params.main.pRegistrationSeconds);
	
  
  //
  // Run the program
  //  
  int iret;
  if ( ensFcstCombMgr.run() == EnsFcstCombMgr::ENSFCSTCOMB_SUCCESS)
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

static void cleanExit (int sig)
{
  InterfaceLL::finish();

  exit(sig);
}

static void outOfStore()
{
  std::cerr << "FATAL ERROR - program EnsFcstComb " << std::endl;

  std::cerr << "Operator new failed - out of store" << std::endl;

  exit(failure);
}
