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
 * @file ParmApp.hh
 * @brief Template functions to allow application parameters function calls
 * @class ParmApp
 * @brief Template functions to allow application parameters function calls
 *
 * These template functions are tied to the underlying non-ConvWx infrastructure
 * in several places.
 *
 * The consistent ConvWx app design:
 *
 *   Developer typically creates a "Parms" class for the app.
 *   The constructor source code has a "Params" object specific to the app.
 *   The class typically has a ParmMain object (common to all ConvWx apps)
 *   and other stuff.
 *
 *   The developer has code something like this:
 *   Parms::Parms(int argc, char **argv)
 *   {
 *     Params p;
 *     ParmMain main;
 *     bool allowSubsetTriggering;
 *     if (!parmAppInit(p, argc, argv, Trigger::TRIGGER_FCST,
 *                      allowSubsetTriggering))
 *       exit(0);
 *   
 *     InterfaceParm::load<xxx>(.., main, ..)
 *  
 *     parmAppFinish(p, main);
 *
 *     code to set/modify local members.
 *   }
 */

# ifndef    ParmApp_HH
# define    ParmApp_HH

#include <stdlib.h>
#include <string>
#include <tdrp/tdrp.h>  // Needed for PRINT_VERBOSE below
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/Trigger.hh>
using std::string;

/*----------------------------------------------------------------*/
/**
 * parmAppInit The app has a parameters class T which is the parameters
 *
 * @tparam T  Params class for app
 * @param[in,out] appParams  Parameters for the app (object of type T)
 * @param[in] argc  Command line number of args
 * @param[in] argv  Command line args
 * @param[in] triggering  The triggering type
 * @param[in] allowSubsetTriggering  true to show parameters for subsets of
 *            generation time and lead time in param file, false to hardwire to
 *            all generation/lead times for triggers.
 *
 * @note: appParams must have a method called load:
 *          bool load(const char *path, char **overrideList, bool expandEnv,
 *                    bool debug);
 */
template <class T>
bool parmAppInit(T &appParams, int argc, char **argv,
		 const Trigger::Trigger_t triggering = Trigger::TRIGGER_FCST,
		 const bool allowSubsetTriggering=false)
{
  // tell interface what the state is
  Trigger::setTriggeringFlags(triggering, allowSubsetTriggering);

  // ask interface if arguments are good for param loading, yes or no
  if (!InterfaceParm::parmInit(argc, argv))
  {
    return false;
  }

  // ask interface if settings are such that a parmfile should be loaded or not.
  if (InterfaceParm::isParmLoad())
  {
    string s = InterfaceParm::parmPath();
    // a param file should be loaded. Call the apps load method, using
    // the path from the interface and param printing status from the interface
    if (appParams.load(s.c_str(), NULL, !InterfaceParm::isParmPrint(), false))
    {
      printf("ERROR loading app params from file %s\n", s.c_str());
      return false;
    }
  }
  return true;
}

/*----------------------------------------------------------------*/
/**
 * parmAppFinish has an app parameters class object appParams and
 * a ParmMain object mainParams
 *
 * @tparam T  Parameter class for calling app
 * @tparam S  Main parameters 
 * @param[in,out] appParams      parameters for the app
 * @param[in]     mainParams     ParmMain object
 *
 * NOTE: appParams must have a method called print:
 *          void print(FILE *f, tdrp_print_mode_t mode);
 */
template <class T, class S>
void parmAppFinish(T &appParams, S &mainParams)
{
  // ask interface if command args specified printing of parameters
  if (InterfaceParm::isParmPrint())
  {
    // call the app print method
    appParams.print(stdout, PRINT_VERBOSE);
  }
  else
  {
    // make sure the input main parameters are o.k. by looking at its pOk member
    // this assumes the mainParams is a ParmMain object
    if (!mainParams.pOk)
    {
      printf("ERROR in main params %s\n", mainParams.pOkErr.c_str());
      exit(-1);
    }
  }
  // do the interface finish step.
  InterfaceParm::parmFinish();
}

#endif
