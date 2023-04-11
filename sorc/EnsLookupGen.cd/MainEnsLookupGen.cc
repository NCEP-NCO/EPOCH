 
/**
 * @mainpage EnsLookupGen 
 *
 * This application generates a grid of probabilities of that a given field 
 * has a value greater than a user specified threshold based on evaluation 
 * of ensemble forecasts. 
 */

/**
 * @file MainEnsLookupGen.cc
 */

#define _ENS_LOOKUP_GEN_

#include "EnsLookupGenMgr.hh"
#include <Epoch/ThresholdDatabaseParmsIO.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <cstdlib>
#include <iostream>

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

static EnsLookupGenMgr *_mgr = NULL;

static void _checkForSpecialArgs(int argc, char **argv)
{
  for (int i=0; i<argc; ++i)
  {
    string s = argv[i];
    if (s == "-printThreshParams")
    {
      ThresholdDatabaseParmsIO p;
      p.printParams();
      exit(0);
    }
  }
}

/**
 * Create algorithm manager and run algorithm 
 * @param[in] argc  Number of command line arguments is generally three for
 *                  real-time or six for archive mode.
 * @param[in] argv  Typical real-time and archive mode command lines are:
 *                  'EnsLookupGen -params Parmfile.params' or
 *                  'EnsLookupGen -params Parmfile.params -interval
 *                  yyyymmddhhmmdss yyyymmddhhmmdss'
 *
 * @return integer status
 */

int main(int argc, char **argv)
{
  // set new() memory failure handler function
  std::set_new_handler(outOfStore);

  // special initial test
  _checkForSpecialArgs(argc, argv);

  // Read in parameters
  ParmsEnsLookupGenIO params(argc, argv);

  // Create object that runs the algorithm               
  _mgr = new EnsLookupGenMgr(params, cleanExit);

  // standard initialization
  InterfaceIO::startup(params._main.pProcessName, 
                       params._main.pInstance,
                       params._main.pRegistrationSeconds);
	
  
  // Run the program
  int iret;
  if ( _mgr->run())
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
  printf("tidy and exit\n");
  if (_mgr != NULL)
  {
    delete _mgr;
    _mgr = NULL;
  }
  InterfaceLL::finish();
  exit(sig);
}

static void outOfStore()
{
  std::cerr << "FATAL ERROR - program EnsLookupGen " << std::endl;
  std::cerr << "Operator new failed - out of store" << std::endl;
  exit(failure);
}
