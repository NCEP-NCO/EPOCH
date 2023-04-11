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
 * @file InterfaceParm.hh
 * @brief  Static functions for parameter files and standard app actions.
 * @class InterfaceParm
 * @brief  Static functions for parameter files and standard app actions.
 *
 * @note The static methods in this class hide the implementation
 *
 * @note  The design obscures the actual parameter file reading software, which
 * is legacy software not part of the transferred software. This legacy software
 * cannot be brought to the FAA coding standards because the method names are
 * non-compliant, among other things, which would make any changes to that
 * software non-backwards compatible. The software is actively and widely
 * used in non FAA funded projects. Therefore, example parameter files can
 * be provided but the implementer must write or use their own parameter
 * reading software to set member variables in Parm classes that are
 * filled in by methods such as loadConvWxParams and similar methods.
 *
 */
#ifndef INTERFACE_PARM_HH
#define INTERFACE_PARM_HH
#include <vector>
#include <string>
class ParmProjection;
class ParmMain;
class ParmFcst;
class ParmPc;
class ParmPcAlg;
class ParmSetUV;
class ParmHtml;
class ParmConv;
class ParmTiling;

//------------------------------------------------------------------
class InterfaceParm
{
public:

  /**
   * Default driver initialization steps for apps.  This method is called
   * by all standard ConvWx apps.
   *
   * @param[in] mainParms  Parameters used to do the initialization
   * @param[in] cleanup  A cleanup function to call on various exit signals.
   * @param[in] threadSafe If true, writing of vertical levels to a static
   *                       state in InterfaceIO is disallowed.  If false,
   *                       vertical levels are written to that static state,
   *                       which is sometimes needed, but is not threadsafe.
   *
   * @return True if initialization was successful
   */
  static bool driverInit(const ParmMain &mainParms, void cleanup(int),
			 bool threadSafe=true);

  /**
   * Load ParmMain member values by reading a parameter file
   *
   * @param[out] mainParms  Object to set
   *
   * @return true for success
   */
  static bool loadMainParams(ParmMain &mainParms);

  /**
   * Load ParmConv member values by reading a parameter file
   *
   * @param[out] convParms  Convergence parameter object to set
   *
   * @return true for success
   */
  static bool loadConvParams(ParmConv &convParms);

  /**
   * Load parameters for a standard ConvWx app which has main parameters,
   * projection information, and an arbitrary set of inputs and output,
   * by reading a parameter file and filling in multiple returned objects
   *
   * @param[out] mainParms  Main parameters 
   * @param[out] projParms  Projection parameters
   * @param[in,out] obsIn   Input observations specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   * @param[in,out] fcstIn  Input forecasts specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   * @param[in,out] obsOut   Output observations specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   * @param[in,out] fcstOut  Output forecasts specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   *
   * @return true if the parameter file was successfully parsed and all
   *         objects have valid values
   *
   *
   * @note 'names only' means the name and description are filled in,
   *       the other fields are not filled in with values.  See ParmFcst.
   *
   * @note The app typically calls the parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadConvWxParams(ParmMain &mainParms, ParmProjection &projParms,
			      std::vector<ParmFcst> &obsIn,
			      std::vector<ParmFcst> &fcstIn,
			      std::vector<ParmFcst> &obsOut,
			      std::vector<ParmFcst> &fcstOut);

  /**
   * Load parameters for a standard ConvWx app which has main parameters,
   * projection information, and an arbitrary set of inputs and output,
   * but with no observations outputs, by reading a parameter file and
   * filling in multiple returned objects
   *
   * @param[out] mainParms  Main parameters 
   * @param[out] projParms  Projection parameters
   * @param[in,out] obsIn   Input observations specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   * @param[in,out] fcstIn  Input forecasts specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   * @param[in,out] fcstOut  Output forecasts specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   *
   * @return true if the parameter file was successfully parsed and all
   *         objects have valid values
   *
   * @note 'names only' means the name and description are filled in,
   *       the other fields are not filled in with values.  See ParmFcst.
   *
   * @note The app typically calls the parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadConvWxParamsObsFcstFcst(ParmMain &mainParms,
					 ParmProjection &projParms,
					 std::vector<ParmFcst> &obsIn,
					 std::vector<ParmFcst> &fcstIn,
					 std::vector<ParmFcst> &fcstOut);

  /**
   * Load parameters for a standard ConvWx app which has main parameters,
   * projection information, and one set of inputs and one set of outputs
   * that are for the same kind of data, either observations or forecasts.
   * Reads a parameter file and fills in the multiple returned objects
   *
   * @param[in] isFcst  True if inputs and outputs are both forecast data,
   *                    false if both inputs and outputs are observations
   *                    data
   * @param[out] mainParms  Main parameters 
   * @param[out] projParms  Projection parameters
   * @param[in,out] inputs  Input specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   * @param[in,out] outputs  Output specification, zero or more,
   *                         on entry each is a 'names only' object, on exit
   *                         the remaining values are filled in.
   *
   * @return true if the parameter file was successfully parsed and all
   *         objects have valid values
   *
   * @note 'names only' means the name and description are filled in,
   *       the other fields are not filled in with values.  See ParmFcst.
   *
   * @note The app typically calls the parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadConvWxParams(const bool isFcst, ParmMain &mainParms,
			      ParmProjection &projParms,
			       std::vector<ParmFcst> &inputs,
			      std::vector<ParmFcst> &outputs);

  /**
   * Load parameters for a standard ConvWx app which has main parameters,
   * projection information, and one set of inputs that are for the same
   * kind of data, either observations or forecasts, and no output.
   * Reads a parameter file and fills in the multiple returned objects
   *
   * @param[in] isFcst  True if inputs are forecast data,
   *                    false if both inputs are observations data
   * @param[out] mainParms  Main parameters 
   * @param[out] projParms  Projection parameters
   * @param[in,out] inputs  Input specification, zero or more,
   *                        on entry each is a 'names only' object, on exit
   *                        the remaining values are filled in.
   *
   * @return true if the parameter file was successfully parsed and all
   *         objects have valid values
   *
   * @note The app typically calls the parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadConvWxParams(const bool isFcst, ParmMain &mainParms,
			      ParmProjection &projParms,
			       std::vector<ParmFcst> &inputs);

  /**
   * Load parameters for a standard ConvWx app which has main parameters,
   * projection information, an unknown set of inputs, and a known set of
   * outputs that are for the same kind of data, either observations or
   * forecasts. Reads a parameter file and fills in the multiple returned
   * objects.
   *
   * @param[in] isFcst  True if inputs and outputs are forecast data,
   *                    false if inputs and outputs are observations data
   * @param[out] mainParms  Main parameters 
   * @param[out] projParms  Projection parameters
   * @param[out] inputs   An empty vector that is filled in with any number
   *                      of objects by reading the parameter file
   * @param[in,out] outputs  Output specification, zero or more,
   *                         on entry each is a 'names only' object, on exit
   *                         the remaining values are filled in.
   *
   * @return true if the parameter file was successfully parsed and all
   *         objects have valid values
   *
   * @note 'names only' means the name and description are filled in,
   *       the other fields are not filled in with values.  See ParmFcst.
   *
   * @note The app typically calls the parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadVarinConvWxParams(const bool isFcst, ParmMain &mainParms,
				   ParmProjection &projParms,
				   std::vector<ParmFcst> &inputs,
				   std::vector<ParmFcst> &outputs);

  /**
   * Load ParmHtml member values by reading a parameter file.
   *
   * @param[out] htmlParms  ParmHtml object to be filled in.
   *
   * @return  true if the parameter file was successfully parsed and members
   *          are all set.
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadHtmlParams(ParmHtml &htmlParms);

  /**
   * Load ParmMain and ParmPc member values by reading a parameter file.
   * This is for the phase correction apply algorithm
   *
   * @param[out] mainParms  Main parameters
   * @param[out] pcParms  Phase Correction parameters
   *
   * @return  true if the parameter file was successfully parsed and members
   *          are all set.
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadPcParams(ParmMain &mainParms, ParmPc &pcParms);

  /**
   * Load ParmMain,ParmPc, and ParmFcst parameters where the ParmFcst
   * object is observations data, by reading a parameter file.
   * This is for the phase correction modify algorithm when observations
   * data is needed.
   *
   * @param[out] mainParms  Main parameters
   * @param[out] pcParms  Phase Correction parameters
   * @param[out] obsParms  Observations data params
   *
   * @return  true if the parameter file was successfully parsed and objects
   *          have all been set.
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadPcParams(ParmMain &mainParms, ParmPc &pcParms,
			   ParmFcst &obsParms);

  /**
   * Load ParmMain,ParmPc, and ParmFcst parameters where the ParmFcst
   * object is forecast data, by reading a parameter file.
   * This is for the phase correction modify algorithm when extrap (forecast)
   * data is needed.
   *
   * @param[out] mainParms  Main parameters
   * @param[out] pcParms  Phase Correction parameters
   * @param[out] fcstParms  Extrap forecast data params
   *
   * @return  true if the parameter file was successfully parsed and objects
   *          have all been set.
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadPcUsingFcstParams(ParmMain &mainParms, ParmPc &pcParms,
				    ParmFcst &fcstParms);

  /**
   * Load a ParmSetUV parameters by reading a parameter file. This is 
   * for the phase correction generation algorithm.
   *
   * @param[out] setUvParms  The object to fill
   *
   * @return  true if the parameter file was successfully parsed and members
   *          are all set.
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadSetUvParams(ParmSetUV &setUvParms);


  /**
   * Load ParmPcAlg parameters by reading a parameter file. This is 
   * for the phase correction application algorithm when only the algorithm
   * parameters are needed, not the full ParmPc derived class parameters.
   *
   * @param[out] pcParms  The object to fill
   *
   * @return  true if the parameter file was successfully parsed and members
   *          are all set.
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static bool loadPcAlgParams(ParmPcAlg &pcParms);


  /**
   * Load in projection parameters  by reading a parameter file
   *
   * @param[in] projFile  Name of the parameter file with projection
   *                      information in it
   * @param[out] projParms  Projection setting parameters to be filled in
   *
   * @return true if all members were successfully parsed
   */
  static bool loadProjectionParams(const std::string &projFile,
				   ParmProjection &projParms);

  /**
   * Load in tiling parameters  by reading a parameter file
   *
   * @param[in] tilingFile  Name of the parameter file with tiling
   *                      information in it
   * @param[out] projTiling  Tiling setting parameters to be filled in
   *
   * @return true if all members were successfully parsed
   */
  static bool loadTilingParams(const std::string &tilingFile,
			       ParmTiling &projTiling);

  /**
   * Set internal state regarding parameter files from command line arguments.
   *
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  The arguments
   *
   * @return true if these arguments agree with expected ConvWx values
   *         specifically:  -params parmfile   and/or  -printParams
   *
   * @note This is called from within the parmAppInit template function
   */
  static bool parmInit(int argc, char **argv);

  /**
   * Perform all needed actions regarding parameter file reading
   * after loading and parsing all parameters
   *
   * @note called from within the parmAppFinish template function
   */
  static void parmFinish(void);

  /**
   * @return The name of the path for the parameter file that is specified
   * by command line arguments
   *
   * This would be the parmfile in '-params parmfile'
   *
   * @note The app typically calls parmAppInit() template function
   * (see ParmApp.hh) prior to calling this.
   */
  static const std::string parmPath(void);

  /**
   * @return  true if a parameter file should be loaded (read and parsed)
   *          based on the command line arguments
   *
   * This would be if the command line arguments include '-params parmfile'
   *
   * @note app typically calls parmAppInit() template function (see ParmApp.hh)
   * prior to calling this.
   */
  static bool isParmLoad(void);

  /**
   * @return  true if parameter values should be printed based on the 
   *          command line arguments
   *
   * This would be if the command line arguments include '-printParams'
   *
   * @note app typically calls parmAppInit() template function (see ParmApp.hh)
   * prior to calling this.
   */
  static bool isParmPrint(void);

  /**
   * @return  the most recent time. If it is ParmMain::REALTIME mode, return the
   * current real time, otherwise return the upper limit of the archive time
   * range.
   */
  static time_t mostRecentTime(void);

  /**
   * @return  the oldest time. If it is ParmMain::REALTIME mode, return the
   * current real time, otherwise return the lower limit of the archive time
   * range.
   */
  static time_t oldestTime(void);

  /**
   * @return True if the command line arguments indicated parameters should be
   *         printed but a file should not be read.
   *
   * This is when the arguments have '-printParams' but do not have
   * '-params parmfile'
   *
   * @note app typically calls parmAppInit() template function (see ParmApp.hh)
   * prior to calling this.
   */
  static bool paramTypeIsPrint(void);

  /**
   * @return  True if the command line arguments indicated parameters should be
   *          printed and a file s should be read.
   *
   * This is when the arguments have both '-printParams' and '-params parmfile'
   *
   * @note app typically calls parmAppInit() template function (see ParmApp.hh)
   * prior to calling this.
   */
  static bool paramTypeIsPrintAndLoad(void);

protected:
private:  

};

# endif
