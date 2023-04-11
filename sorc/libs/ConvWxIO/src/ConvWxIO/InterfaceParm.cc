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
 * @file InterfaceParm.cc
 */

//------------------------------------------------------------------
#include <algorithm>
#include <cstring>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/Trigger.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmPc.hh>
#include <ConvWx/ParmPcAlg.hh>
#include <ConvWx/ParmPcFcst.hh>
#include <ConvWx/ParmHtml.hh>
#include <ConvWx/ParmConv.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/ParmTiling.hh>

#include <ConvWxParams/ParamsHtmlLog.hh>
#include <ConvWxParams/ParamsConvTT.hh>
#include <ConvWxParams/ParamsDebug.hh>
#include <ConvWxParams/ParamsMain.hh>
#include <ConvWxParams/ParamsMainFile.hh>
#include <ConvWxParams/ParamsMainNone.hh>
#include <ConvWxParams/ParamsMainObsFcst.hh>
#include <ConvWxParams/ParamsMainObsFcstSubset.hh>
#include <ConvWxParams/ParamsMainObsSubset.hh>
#include <ConvWxParams/ParamsMainFcstSubset.hh>
#include <ConvWxParams/ParamsProjection.hh>
#include <ConvWxParams/ParamsFcst.hh>
#include <ConvWxParams/ParamsFcstObsInFcstOut.hh>
#include <ConvWxParams/ParamsFcstInOut.hh>
#include <ConvWxParams/ParamsObsInOut.hh>
#include <ConvWxParams/ParamsFcstIn.hh>
#include <ConvWxParams/ParamsObsIn.hh>
#include <ConvWxParams/ParamsPcTT.hh>
#include <ConvWxParams/ParamsPcAlg.hh>
#include <ConvWxParams/ParamsSetUV.hh>
#include <ConvWxParams/ParamsTiling.hh>

typedef enum { PARM_PRINT=0, PARM_LOAD=1, PARM_PRINT_AND_LOAD=2} Print_t;
static string sAppName;
static Print_t sParamType;
static string sParamPath;
static bool sParamPrint;
static bool sParamLoad;
static bool sParamArchive = false;
static time_t sParamArchiveT0;
static time_t sParamArchiveT1;

//------------------------------------------------------------------
static Print_t sSetTdrpFlag(int argc, char **argv)
{
  bool load=false, print=false;

  for (int i=0; i<argc; ++i)
  {
    if (strcmp(argv[i], "-params") == 0)
    {
      load = true;
    }
    else if (strcmp(argv[i], "-print_params") == 0)
    {
      print = true;
    }
  }

  if (load && print)
  {
    return PARM_PRINT_AND_LOAD;
  }
  else if (load && !print)
  {
    return PARM_LOAD;
  }
  else if ((!load) && print)
  {
    return PARM_PRINT;
  }
  else
  {
    return PARM_LOAD;
  }
}

//------------------------------------------------------------------
static void sSetOverrides(ParamsFcst &P, vector<ParmFcst> &obsIn,
			  vector<ParmFcst> &fcstIn,
			  vector<ParmFcst> &obsOut,
			  vector<ParmFcst> &fcstOut)
{
  if (!obsIn.empty())
  {
    P.arrayRealloc("obs_input", static_cast<int>(obsIn.size()));
  }
  if (!fcstIn.empty())
  {
    P.arrayRealloc("fcst_input", static_cast<int>(fcstIn.size()));
  }
  if (!obsOut.empty())
  {
    P.arrayRealloc("obs_output", static_cast<int>(obsOut.size()));
  }
  if (!fcstOut.empty())
  {
    P.arrayRealloc("fcst_output", static_cast<int>(fcstOut.size()));
  }
  for (int i=0; i<static_cast<int>(obsIn.size()); ++i)
  {
    TDRP_str_replace(&P._obs_input[i].name, obsIn[i].pName.c_str());
    TDRP_str_replace(&P._obs_input[i].description,
		     obsIn[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(fcstIn.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_input[i].name, fcstIn[i].pName.c_str());
    TDRP_str_replace(&P._fcst_input[i].description,
		     fcstIn[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(obsOut.size()); ++i)
  {
    TDRP_str_replace(&P._obs_output[i].name, obsOut[i].pName.c_str());
    TDRP_str_replace(&P._obs_output[i].description,
		     obsOut[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(fcstOut.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_output[i].name, fcstOut[i].pName.c_str());
    TDRP_str_replace(&P._fcst_output[i].description,
		     fcstOut[i].pDescription.c_str());
  }
  P.sync();
}

//------------------------------------------------------------------
static void sSetOverrides(ParamsObsInOut &P, vector<ParmFcst> &obsIn,
			  vector<ParmFcst> &obsOut)
{
  if (!obsIn.empty())
  {
    P.arrayRealloc("obs_input", static_cast<int>(obsIn.size()));
  }
  if (!obsOut.empty())
  {
    P.arrayRealloc("obs_output", static_cast<int>(obsOut.size()));
  }
  for (int i=0; i<static_cast<int>(obsIn.size()); ++i)
  {
    TDRP_str_replace(&P._obs_input[i].name, obsIn[i].pName.c_str());
    TDRP_str_replace(&P._obs_input[i].description,
		     obsIn[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(obsOut.size()); ++i)
  {
    TDRP_str_replace(&P._obs_output[i].name, obsOut[i].pName.c_str());
    TDRP_str_replace(&P._obs_output[i].description,
		     obsOut[i].pDescription.c_str());
  }
  P.sync();
}

//------------------------------------------------------------------
static void sSetOverrides(ParamsObsIn &P, vector<ParmFcst> &obsIn)
{
  if (!obsIn.empty())
  {
    P.arrayRealloc("obs_input", static_cast<int>(obsIn.size()));
  }
  for (int i=0; i<static_cast<int>(obsIn.size()); ++i)
  {
    TDRP_str_replace(&P._obs_input[i].name, obsIn[i].pName.c_str());
    TDRP_str_replace(&P._obs_input[i].description,
		     obsIn[i].pDescription.c_str());
  }
  P.sync();
}

//------------------------------------------------------------------
static void sSetOverrides(ParamsFcstInOut &P, 
			  vector<ParmFcst> &fcstIn,
			  vector<ParmFcst> &fcstOut)
{
  if (!fcstIn.empty())
  {
    P.arrayRealloc("fcst_input", static_cast<int>(fcstIn.size()));
  }
  if (!fcstOut.empty())
  {
    P.arrayRealloc("fcst_output", static_cast<int>(fcstOut.size()));
  }
  for (int i=0; i<static_cast<int>(fcstIn.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_input[i].name, fcstIn[i].pName.c_str());
    TDRP_str_replace(&P._fcst_input[i].description,
		     fcstIn[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(fcstOut.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_output[i].name, fcstOut[i].pName.c_str());
    TDRP_str_replace(&P._fcst_output[i].description,
		     fcstOut[i].pDescription.c_str());
  }
  P.sync();
}

//------------------------------------------------------------------
static void sSetOverrides(ParamsFcstIn &P, 
			  vector<ParmFcst> &fcstIn)
{
  if (!fcstIn.empty())
  {
    P.arrayRealloc("fcst_input", static_cast<int>(fcstIn.size()));
  }
  for (int i=0; i<static_cast<int>(fcstIn.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_input[i].name, fcstIn[i].pName.c_str());
    TDRP_str_replace(&P._fcst_input[i].description,
		     fcstIn[i].pDescription.c_str());
  }
  P.sync();
}

//------------------------------------------------------------------
static void sSetOverrides(ParamsFcstObsInFcstOut &P,
			  vector<ParmFcst> &obsIn,
			  vector<ParmFcst> &fcstIn,
			  vector<ParmFcst> &fcstOut)
{
  if (!obsIn.empty())
  {
    P.arrayRealloc("obs_input", static_cast<int>(obsIn.size()));
  }
  if (!fcstIn.empty())
  {
    P.arrayRealloc("fcst_input", static_cast<int>(fcstIn.size()));
  }
  if (!fcstOut.empty())
  {
    P.arrayRealloc("fcst_output", static_cast<int>(fcstOut.size()));
  }
  for (int i=0; i<static_cast<int>(obsIn.size()); ++i)
  {
    TDRP_str_replace(&P._obs_input[i].name, obsIn[i].pName.c_str());
    TDRP_str_replace(&P._obs_input[i].description,
		     obsIn[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(fcstIn.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_input[i].name, fcstIn[i].pName.c_str());
    TDRP_str_replace(&P._fcst_input[i].description,
		     fcstIn[i].pDescription.c_str());
  }
  for (int i=0; i<static_cast<int>(fcstOut.size()); ++i)
  {
    TDRP_str_replace(&P._fcst_output[i].name, fcstOut[i].pName.c_str());
    TDRP_str_replace(&P._fcst_output[i].description,
		     fcstOut[i].pDescription.c_str());
  }
  P.sync();
}

//------------------------------------------------------------------
static bool sMissingRet(const ParmFcst &f, const string &name)
{
  // it was not in the parameter file.
  if (f.pIsRequired)
  {
    ILOGF(ERROR, "%s:%s not present in param file but is required",
	  name.c_str(), f.pName.c_str());
    return false;
  }
  else
  {
    return true;
  }
}

//------------------------------------------------------------------
static void sCheckSizes(const vector<ParmFcst> &f, const int n,
			const std::string &name, bool &stat)
{
  if (static_cast<int>(f.size()) != n)
  {
    ILOGF(ERROR, "wanted %d %s got %d", static_cast<int>(f.size()), 
	   name.c_str(), n);
    stat = false;
  }
}

//------------------------------------------------------------------
static bool sSet(const int deltaMinutes, 
		 const char *url, const char *field,
		 const char *units, const double lth0, const double lth1, 
		 const double dlth, const bool remap, const string &name,
		 ParmFcst &f)
{
  f.setDetails(url, field, units, lth0, lth1, dlth, remap, deltaMinutes);
  return f.isOk();
}

//------------------------------------------------------------------
static bool sSetObs(const ParamsFcst::obs_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetObs");
  }
  else
  {
    return sSet(P.delta_minutes, P.url, P.field, P.units, 0, 0, 0,
		P.remap, "sSetObs", f);
  }
}

//------------------------------------------------------------------
static bool sSetObs(const ParamsObsIn::obs_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetObs");
  }
  else
  {
    return sSet(P.delta_minutes, P.url, P.field, P.units, 0, 0, 0,
		P.remap, "sSetObs", f);
  }
}

//------------------------------------------------------------------
static bool sSetObs(const ParamsObsInOut::obs_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetObs");
  }
  else
  {
    return sSet(P.delta_minutes, P.url, P.field, P.units, 0, 0, 0,
		P.remap, "sSetObs", f);
  }
}

//------------------------------------------------------------------
static bool sSetNewObs(const ParamsObsInOut::obs_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    ILOG(ERROR, "can't have missing input to variable inputs");
    return false;
  }
  else
  {
    f = ParmFcst("obs", "obs input", true);
    return sSet(P.delta_minutes, P.url, P.field, P.units, 0, 0, 0,
		P.remap, "sSetObs", f);
  }
}

//------------------------------------------------------------------
static bool sSetObs(const ParamsFcstObsInFcstOut::obs_data_t &P,
		    ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetObs");
  }
  else
  {
    return sSet(P.delta_minutes, P.url, P.field, P.units, 0, 0, 0,
		P.remap, "sSetObs", f);
  }
}

//------------------------------------------------------------------
static bool sSetFcst(const ParamsFcst::fcst_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetFcst");
  }
  else
  {
    return sSet(P.gt_delta_minutes, P.url, P.field, P.units,
		P.lt_hour0, P.lt_hour1, P.lt_delta_hours, P.remap,
		"sSetFcst", f);
  }
}

//------------------------------------------------------------------
static bool sSetFcst(const ParamsFcstInOut::fcst_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetFcst");
  }
  else
  {
    return sSet(P.gt_delta_minutes, P.url, P.field, P.units,
		P.lt_hour0, P.lt_hour1, P.lt_delta_hours, P.remap,
		"sSetFcst", f);
  }
}

//------------------------------------------------------------------
static bool sSetNewFcst(const ParamsFcstInOut::fcst_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    ILOG(ERROR, "can't have missing input to variable inputs");
    return false;
  }
  else
  {
    f = ParmFcst(P.name, P.description, true);
    return sSet(P.gt_delta_minutes, P.url, P.field, P.units,
		P.lt_hour0, P.lt_hour1, P.lt_delta_hours, P.remap,
		"sSetFcst", f);
  }
}

//------------------------------------------------------------------
static bool sSetFcst(const ParamsFcstIn::fcst_data_t &P, ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetFcst");
  }
  else
  {
    return sSet(P.gt_delta_minutes, P.url, P.field, P.units,
		P.lt_hour0, P.lt_hour1, P.lt_delta_hours, P.remap,
		"sSetFcst", f);
  }
}

//------------------------------------------------------------------
static bool sSetFcst(const ParamsFcstObsInFcstOut::fcst_data_t &P,
		     ParmFcst &f)
{
  if (strcmp(P.url, "urlNotSet") == 0 || strlen(P.url) == 0)
  {
    return sMissingRet(f, "sSetFcst");
  }
  else
  {
    return sSet(P.gt_delta_minutes, P.url, P.field, P.units,
		P.lt_hour0, P.lt_hour1, P.lt_delta_hours, P.remap,
		"sSetFcst", f);
  }
}

//------------------------------------------------------------------
static bool sLoadProjectionParamsActual(const string &parmfile,
					ParmProjection &p)
{
  ParamsProjection P;
  if (P.load(parmfile.c_str(), NULL, true, false) != 0)
  {
    ILOGF(DEBUG, "Loading file %s", parmfile.c_str());
    return false;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  parmfile.c_str());
    return false;
  }

  ParmProjection::Proj_t proj;
  switch (P.projection)
  {
  case ParamsProjection::REMAP_FLAT:
    proj = ParmProjection::FLAT;
    break;
  case ParamsProjection::REMAP_LATLON:
    proj = ParmProjection::LATLON;
    break;
  case ParamsProjection::REMAP_LAMBERT_CONFORMAL2:
    proj = ParmProjection::LAMBERT_CONFORMAL2;
    break;
  case ParamsProjection::REMAP_LAMBERT_AZIM_EQUAL_AREA:
    proj = ParmProjection::LAMBERT_AZIM_EQUAL_AREA;
    break;
  default:
    ILOGF(ERROR, "projection in file %s not in synch with ConvWx code",
	  parmfile.c_str());
    return false;
  }

  p = ParmProjection(proj, P.nx, P.ny, P.nz, P.x0, P.y0, P.z0, P.dx, P.dy,
		     P.dz, P.origin_lat, 
		     P.origin_lon, P.offset_origin_lat, P.offset_origin_lon,
		     P.lat1, P.lat2, P.rotation, P.earth_radius_km);
  return true;
}

//------------------------------------------------------------------
static bool sLoadProjectionParams(const string &parmfile,
				  ParmProjection &p)
{
  if (sParamType == PARM_PRINT || sParamType == PARM_PRINT_AND_LOAD)
  {
    // don't need to actually load
    return true;
  }
  else
  {
    return sLoadProjectionParamsActual(parmfile, p);
  }
}

//------------------------------------------------------------------
static bool sLoadFcstParams(vector<ParmFcst> &obsIn,
			    vector<ParmFcst> &fcstIn,
			    vector<ParmFcst> &obsOut,
			    vector<ParmFcst> &fcstOut,
			    ParmProjection &proj)
{
  ParamsFcst P;

  sSetOverrides(P, obsIn, fcstIn, obsOut, fcstOut);

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(obsIn, P.obs_input_n, "obs input", stat);
  sCheckSizes(fcstIn, P.fcst_input_n, "fcst input", stat);
  sCheckSizes(obsOut, P.obs_output_n, "obs output", stat);
  sCheckSizes(fcstOut, P.fcst_output_n, "fcst output", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<static_cast<int>(obsIn.size()); ++i)
  {
    if (!sSetObs(P._obs_input[i], obsIn[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(fcstIn.size()); ++i)
  {
    if (!sSetFcst(P._fcst_input[i], fcstIn[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(obsOut.size()); ++i)
  {
    if (!sSetObs(P._obs_output[i], obsOut[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(fcstOut.size()); ++i)
  {
    if (!sSetFcst(P._fcst_output[i], fcstOut[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadObsfcstFcstParams(vector<ParmFcst> &obsIn,
				   vector<ParmFcst> &fcstIn,
				   vector<ParmFcst> &fcstOut,
				   ParmProjection &proj)
{
  ParamsFcstObsInFcstOut P;

  sSetOverrides(P, obsIn, fcstIn, fcstOut);

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(obsIn, P.obs_input_n, "obs input", stat);
  sCheckSizes(fcstIn, P.fcst_input_n, "fcst input", stat);
  sCheckSizes(fcstOut, P.fcst_output_n, "fcst output", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<static_cast<int>(obsIn.size()); ++i)
  {
    if (!sSetObs(P._obs_input[i], obsIn[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(fcstIn.size()); ++i)
  {
    if (!sSetFcst(P._fcst_input[i], fcstIn[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(fcstOut.size()); ++i)
  {
    if (!sSetFcst(P._fcst_output[i], fcstOut[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadFcstParams(ParamsFcstInOut &P,
			    vector<ParmFcst> &in,
			    vector<ParmFcst> &out,
			    ParmProjection &proj)
{
  sSetOverrides(P, in, out);
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(in, P.fcst_input_n, "fcst input", stat);
  sCheckSizes(out, P.fcst_output_n, "fcst output", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<static_cast<int>(in.size()); ++i)
  {
    if (!sSetFcst(P._fcst_input[i], in[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(out.size()); ++i)
  {
    if (!sSetFcst(P._fcst_output[i], out[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadVarFcstParams(ParamsFcstInOut &P,
			       vector<ParmFcst> &in,
			       vector<ParmFcst> &out,
			       ParmProjection &proj)
{
  in.clear();
  sSetOverrides(P, in, out);
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(out, P.fcst_output_n, "fcst output", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<P.fcst_input_n; ++i)
  {
    ParmFcst f;
    if (!sSetNewFcst(P._fcst_input[i], f))
    {
      stat = false;
    }
    else
    {
      in.push_back(f);
    }
  }
  for (int i=0; i<static_cast<int>(out.size()); ++i)
  {
    if (!sSetFcst(P._fcst_output[i], out[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadObsParams(ParamsObsInOut &P,
			   vector<ParmFcst> &in,
			   vector<ParmFcst> &out,
			   ParmProjection &proj)
{
  sSetOverrides(P, in, out);
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(in, P.obs_input_n, "obs input", stat);
  sCheckSizes(out, P.obs_output_n, "obs output", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<static_cast<int>(in.size()); ++i)
  {
    if (!sSetObs(P._obs_input[i], in[i]))
    {
      stat = false;
    }
  }
  for (int i=0; i<static_cast<int>(out.size()); ++i)
  {
    if (!sSetObs(P._obs_output[i], out[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool  sLoadVarObsParams(ParamsObsInOut &P,
			       vector<ParmFcst> &in,
			       vector<ParmFcst> &out,
			       ParmProjection &proj)
{
  in.clear();
  sSetOverrides(P, in, out);
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(out, P.obs_output_n, "obs output", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<P.obs_input_n; ++i)
  {
    ParmFcst f;
    if (!sSetNewObs(P._obs_input[i], f))
    {
      stat = false;
    }
    else
    {
      in.push_back(f);
    }
  }
  for (int i=0; i<static_cast<int>(out.size()); ++i)
  {
    if (!sSetObs(P._obs_output[i], out[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadFcstParams(ParamsFcstIn &P,
			    vector<ParmFcst> &in,
			    ParmProjection &proj)
{
  sSetOverrides(P, in);
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(in, P.fcst_input_n, "fcst input", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<static_cast<int>(in.size()); ++i)
  {
    if (!sSetFcst(P._fcst_input[i], in[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadObsParams(ParamsObsIn &P,
			   vector<ParmFcst> &in,
			   ParmProjection &proj)
{
  sSetOverrides(P, in);
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }

  // should we override this every time (the descriptions?) or not.
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
  }

  // here either load or print and load..lets load
  bool stat = true;
  sCheckSizes(in, P.obs_input_n, "obs input", stat);
  if (stat == false)
  {
    return false;
  }

  for (int i=0; i<static_cast<int>(in.size()); ++i)
  {
    if (!sSetObs(P._obs_input[i], in[i]))
    {
      stat = false;
    }
  }
  if (!sLoadProjectionParams(P.projection_param_file, proj))
  {
    ILOGF(ERROR, "projection file %s not good",
	  P.projection_param_file);
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
static bool sLoadFcstParams(const bool isFcst,
			    vector<ParmFcst> &in,
			    vector<ParmFcst> &out,
			    ParmProjection &proj)
{
  ParamsFcstInOut Pf;
  ParamsObsInOut Po;

  if (isFcst)
  {
    return sLoadFcstParams(Pf, in, out, proj);
  }
  else
  {
    return sLoadObsParams(Po, in, out, proj);
  }
}

//------------------------------------------------------------------
static bool sLoadVarFcstParams(const bool isFcst,
			       vector<ParmFcst> &in,
			       vector<ParmFcst> &out,
			       ParmProjection &proj)
{
  ParamsFcstInOut Pf;
  ParamsObsInOut Po;

  if (isFcst)
  {
    return sLoadVarFcstParams(Pf, in, out, proj);
  }
  else
  {
    return sLoadVarObsParams(Po, in, out, proj);
  }
}

//------------------------------------------------------------------
static bool sLoadFcstParams(const bool isFcst,
			    vector<ParmFcst> &in,
			    ParmProjection &proj)
{
  ParamsFcstIn Pf;
  ParamsObsIn Po;

  if (isFcst)
  {
    return sLoadFcstParams(Pf, in, proj);
  }
  else
  {
    return sLoadObsParams(Po, in, proj);
  }
}

//------------------------------------------------------------------
static void sLoadPc(const ParamsPcTT &P, const ParamsPcAlg &A,
		    const ParmFcst &model, const ParmFcst &out,
		    const ParmFcst &other, const ParmProjection &proj,
		    bool &stat, ParmPc &p)
{
  if (!stat)
  {
    p = ParmPc();
    return;
  }

  if (!other.isOk() || strlen(P.input_pc_v_name) == 0 )
  {
    ILOG(ERROR, "No UV input as other, or no vName");
    stat = false;
  }

  if (!stat)
  {
    p = ParmPc();
  }
  else
  {
    p = ParmPc(model, other, P.input_pc_v_name, out, proj, 
	       A._smooth, A.use_high_smooth, A.smooth_where_corrected,
	       A._gap_fill, A.zero_resolution);
  }
}

//------------------------------------------------------------------
static void sSetParmconv(const ParamsConvTT &C, ParmConv &p)
{
  vector<pair<double,double> > f;
  for (int i=0; i<C.npt_to_expansion_n; ++i)
  {
    f.push_back(pair<double,double>(C._npt_to_expansion[i].npt,
				    C._npt_to_expansion[i].expansion));
  }
  ConvWxFuzzy F(f);
  p = ParmConv(C.conv_thresh, F);
}

//------------------------------------------------------------------
static bool sLoadMainParams(ParmMain &p)
{
  ParamsMain P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  if (sParamArchive)
  {
    ILOG(DEBUG, "detected archive mode");
  }

  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, 0, NULL, false, 0.0, 0.0, 0.0);
  }
  else
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, 0, NULL, false, 0.0, 0.0, 0.0,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}

//------------------------------------------------------------------
static bool sLoadMainFileParams(ParmMain &p)
{
  ParamsMainFile P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  if (sParamArchive)
  {
    ILOG(DEBUG, "detected archive mode");
  }

  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_path, 0, NULL, false, 0.0, 0.0, 0.0);
  }
  else
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_path, 0, NULL, false, 0.0, 0.0, 0.0,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}

//------------------------------------------------------------------
static bool sLoadMainNoneParams(ParmMain &p)
{
  ParamsMainNone P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  if (sParamArchive)
  {
    ILOG(DEBUG, "detected archive mode");
  }

  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 "", 0, NULL, false, 0.0, 0.0, 0.0);
  }
  else
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 "", 0, NULL, false, 0.0, 0.0, 0.0,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}

//------------------------------------------------------------------
static bool sLoadMainObsSubsetParams(ParmMain &p)
{
  ParamsMainObsSubset P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  if (sParamArchive)
  {
    ILOG(DEBUG, "detected archive mode");
  }

  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, P.trigger_gen_minutes_subset_n, 
		 P._trigger_gen_minutes_subset, false, 0.0, 0.0, 0.0);
  }
  else
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, P.trigger_gen_minutes_subset_n, 
		 P._trigger_gen_minutes_subset, false, 0.0, 0.0, 0.0,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}
  
//------------------------------------------------------------------
static bool sLoadMainFcstSubsetParams(ParmMain &p)
{
  ParamsMainFcstSubset P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  if (sParamArchive)
  {
    ILOG(DEBUG_VERBOSE, "detected archive mode");
  }

  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, P.trigger_gen_minutes_subset_n, 
		 P._trigger_gen_minutes_subset, P.lead_time_subsets,
		 P.trigger_lead_hour_0,	 P.trigger_lead_hour_1, 
		 P.trigger_lead_delta_hours);

  }
  else 
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, P.trigger_gen_minutes_subset_n, 
		 P._trigger_gen_minutes_subset, P.lead_time_subsets,
		 P.trigger_lead_hour_0,	 P.trigger_lead_hour_1, 
		 P.trigger_lead_delta_hours,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}

//------------------------------------------------------------------
static bool sLoadMainObsOrFcstSubsetParams(ParmMain &p)
{
  ParamsMainObsFcstSubset P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  Trigger::setTriggeringFcst(P.trigger_is_forecast);

  if (sParamArchive)
  {
    ILOG(DEBUG, "detected archive mode");
  }


  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, P.trigger_gen_minutes_subset_n, 
		 P._trigger_gen_minutes_subset, P.lead_time_subsets,
		 P.trigger_lead_hour_0,	 P.trigger_lead_hour_1, 
		 P.trigger_lead_delta_hours);
  }
  else
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, P.trigger_gen_minutes_subset_n, 
		 P._trigger_gen_minutes_subset, P.lead_time_subsets,
		 P.trigger_lead_hour_0,	 P.trigger_lead_hour_1, 
		 P.trigger_lead_delta_hours,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}

//------------------------------------------------------------------
static bool sLoadMainObsOrFcstParams(ParmMain &p)
{
  ParamsMainObsFcst P;

  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    return false;
  }

  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  
  ParamsDebug D;
  if (D.load(P.debug_file, NULL, true, false) != 0)
  {
    ILOGF(ERROR, "Loading debug file %s", P.debug_file);
    return false;
  }
  if (!D.checkAllSet(stdout))
  {
    ILOGF(ERROR, 
	  "debug file %s has missing parms. All params must be set",
	  P.debug_file);
    return false;
  }

  Trigger::setTriggeringFcst(P.trigger_is_forecast);

  if (sParamArchive)
  {
    ILOG(DEBUG, "detected archive mode");
  }

  if (!sParamArchive)
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, 0, NULL, false, 0.0, 0.0, 0.0);
  }
  else
  {
    p = ParmMain(sAppName, D.debug, D.debug_verbose, D.debug_triggering,
		 D.debug_show_realtime, D.debug_show_class_and_method,
		 P.instance, static_cast<int>(P.procmap_interval_minutes*60),
		 P.trigger_url, 0, NULL, false, 0.0, 0.0, 0.0,
		 sParamArchiveT0, sParamArchiveT1);
  }
  return true;
}

//------------------------------------------------------------------
bool InterfaceParm::driverInit(const ParmMain &p, void cleanup(int),
			       bool threadSafe)
{
  InterfaceIO::startup(p.pProcessName, p.pInstance, p.pRegistrationSeconds);
  InterfaceIO::setCleanExit(cleanup);
  InterfaceIO::setLogging(p.pDebug, p.pDebugVerbose, p.pDebugTrigger,
			  p.pDebugShowRealtime, p.pDebugShowClassAndMethod);
  if (threadSafe)
  {
    InterfaceIO::disallowStoringVerticalLevels();
  }

  return true;
}

//------------------------------------------------------------------
bool InterfaceParm::loadMainParams(ParmMain &p)
{
  bool ret;
  if (Trigger::allowSubsets())
  {
    switch (Trigger::getTriggering())
    {
    case Trigger::TRIGGER_OBS:
      ret = sLoadMainObsSubsetParams(p);
      break;
    case Trigger::TRIGGER_FCST:
      ret = sLoadMainFcstSubsetParams(p);
      break;
    case Trigger::TRIGGER_OBS_OR_FCST:
      ret = sLoadMainObsOrFcstSubsetParams(p);
      break;
    case Trigger::TRIGGER_FILE:
      ILOG(ERROR,
	   "No subset triggering allowed in mode = TRIGGER_FILE");
      ret = sLoadMainFileParams(p);
      break;
    case Trigger::TRIGGER_NONE:
    default:
      ILOG(ERROR,
	   "No subset triggering allowed in mode = TRIGGER_NONE");
      ret = sLoadMainNoneParams(p);
      break;
    }
  }
  else
  {
    switch (Trigger::getTriggering())
    {
    case Trigger::TRIGGER_OBS:
      ret = sLoadMainParams(p);
      break;
    case Trigger::TRIGGER_FCST:
      ret = sLoadMainParams(p);
      break;
    case Trigger::TRIGGER_OBS_OR_FCST:
      ret = sLoadMainObsOrFcstParams(p);
      break;
    case Trigger::TRIGGER_FILE:
      ret = sLoadMainFileParams(p);
      break;
    case Trigger::TRIGGER_NONE:
    default:
      ret = sLoadMainNoneParams(p);
      break;
    }    
  }
  return ret;
}

//------------------------------------------------------------------
bool InterfaceParm::loadConvWxParams(ParmMain &p, ParmProjection &proj,
				     vector<ParmFcst> &obsIn,
				     vector<ParmFcst> &fcstIn,
				     vector<ParmFcst> &obsOut,
				     vector<ParmFcst> &fcstOut)
{
  bool stat = true;

  if (!loadMainParams(p))
  {
    stat = false;
  }

  if (!sLoadFcstParams(obsIn, fcstIn, obsOut, fcstOut, proj))
  {
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
bool
InterfaceParm::loadConvWxParamsObsFcstFcst(ParmMain &p,
					   ParmProjection &proj,
					   vector<ParmFcst> &obsIn,
					   vector<ParmFcst> &fcstIn,
					   vector<ParmFcst> &fcstOut)
{
  bool stat = true;

  if (!loadMainParams(p))
  {
    stat = false;
  }

  if (!sLoadObsfcstFcstParams(obsIn, fcstIn, fcstOut, proj))
  {
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
bool
InterfaceParm::loadConvWxParams(const bool isFcst, ParmMain &p,
				ParmProjection &proj,
				vector<ParmFcst> &in,
				vector<ParmFcst> &out)
{
  bool stat = true;

  if (!loadMainParams(p))
  {
    stat = false;
  }

  if (!sLoadFcstParams(isFcst, in, out, proj))
  {
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
bool InterfaceParm::loadVarinConvWxParams(const bool isFcst,
					  ParmMain &p,
					  ParmProjection &proj,
					  vector<ParmFcst> &in,
					  vector<ParmFcst> &out)
{
  bool stat = true;

  if (!loadMainParams(p))
  {
    stat = false;
  }

  if (!sLoadVarFcstParams(isFcst, in, out, proj))
  {
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
bool
InterfaceParm::loadConvWxParams(const bool isFcst, ParmMain &p,
				ParmProjection &proj,
				vector<ParmFcst> &in)
{
  bool stat = true;

  if (!loadMainParams(p))
  {
    stat = false;
  }
  if (!sLoadFcstParams(isFcst, in, proj))
  {
    stat = false;
  }
  return stat;
}

//------------------------------------------------------------------
bool InterfaceParm::loadHtmlParams(ParmHtml &p)
{
  ParamsHtmlLog P;
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(DEBUG, "Loading file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  sParamPath.c_str());
    return false;
  }
  p = ParmHtml(P.html_path, P.html_subdir, P.html_hours_back);
  return true;
}

//------------------------------------------------------------------
bool InterfaceParm::loadPcParams(ParmMain &main, ParmPc &p)
{
  bool stat = true;

  ParmProjection proj;

  if (!loadMainParams(main))
  {
    stat = false;
  }

  vector<ParmFcst> in, out;
  ParmFcst Pi("model", "input model data", true);
  in.push_back(Pi);
  Pi = ParmFcst("UV", "input phase correction data", true);
  in.push_back(Pi);
  Pi = ParmFcst("PC", "output phase corrected model data", true);
  out.push_back(Pi);
  if (!sLoadFcstParams(true, in, out, proj))
  {
    stat = false;
  }

  ParamsPcTT P;
  ParamsPcAlg A;
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    A.print(stdout, PRINT_VERBOSE);
    return true;
  }

  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Pc params, file %s", sParamPath.c_str());
    stat = false;
  }
  if (A.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Pc Alg params, file %s", sParamPath.c_str());
    stat = false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    A.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing PC parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }
  if (!A.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing PCAlg parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }

  sLoadPc(P, A, in[0], out[0], in[1], proj, stat, p);
  return stat;
}

//------------------------------------------------------------------
bool InterfaceParm::loadPcParams(ParmMain &main, ParmPc &p, ParmFcst &obs)
{
  bool stat = true;

  ParmProjection proj;

  if (!loadMainParams(main))
  {
    stat = false;
  }

  vector<ParmFcst> obsIn, fcstIn, fcstOut;
  ParmFcst Pi("obs", "Obs input", true);
  obsIn.push_back(Pi);
  Pi = ParmFcst("model", "Model forecast input", true);
  fcstIn.push_back(Pi);
  Pi = ParmFcst("UV", "Phase correction vector inputs", true);
  fcstIn.push_back(Pi);
  Pi = ParmFcst("PC", "Phase correction vector outputs", true);
  fcstOut.push_back(Pi);

  if (!sLoadObsfcstFcstParams(obsIn, fcstIn, fcstOut, proj))
  {
    stat = false;
  }
  
  // for return
  obs = obsIn[0];

  ParamsPcTT P;
  ParamsPcAlg A;
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    A.print(stdout, PRINT_VERBOSE);
    return true;
  }

  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Pc params from %s", sParamPath.c_str());
    stat = false;
  }
  if (A.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Pc Alg params from %s", sParamPath.c_str());
    stat = false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    A.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing Pc parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }

  if (!A.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing PcAlg parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }

  sLoadPc(P, A, fcstIn[0], fcstOut[0], fcstIn[1], proj, stat, p);
  return stat;
}

//------------------------------------------------------------------
bool InterfaceParm::loadPcUsingFcstParams(ParmMain &main, ParmPc &p,
					  ParmFcst &fcst)
{
  bool stat = true;

  ParmProjection proj;

  if (!loadMainParams(main))
  {
    stat = false;
  }

  vector<ParmFcst> fcstIn, fcstOut;
  ParmFcst Pi("extrap", "Extrap input", true);
  fcstIn.push_back(Pi);
  Pi = ParmFcst("model", "Model forecast input", true);
  fcstIn.push_back(Pi);
  Pi = ParmFcst("UV", "Phase correction vector inputs", true);
  fcstIn.push_back(Pi);
  Pi = ParmFcst("PC", "Phase correction vector outputs", true);
  fcstOut.push_back(Pi);

  if (!sLoadFcstParams(true, fcstIn, fcstOut, proj))
  {
    stat = false;
  }
  
  // for return
  fcst = fcstIn[0];

  ParamsPcTT P;
  ParamsPcAlg A;
  if (sParamType == PARM_PRINT)
  {
    P.print(stdout, PRINT_VERBOSE);
    A.print(stdout, PRINT_VERBOSE);
    return true;
  }

  if (P.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Pc params from %s", sParamPath.c_str());
    stat = false;
  }
  if (A.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Pc Alg params from %s", sParamPath.c_str());
    stat = false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    P.print(stdout, PRINT_VERBOSE);
    A.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing Pc parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }

  if (!A.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing PcAlg parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }

  sLoadPc(P, A, fcstIn[1], fcstOut[0], fcstIn[2], proj, stat, p);
  return stat;
}

//------------------------------------------------------------------
bool InterfaceParm::loadConvParams(ParmConv &parm)
{
  bool stat = true;

  ParamsConvTT C;
  if (sParamType == PARM_PRINT)
  {
    C.print(stdout, PRINT_VERBOSE);
    return true;
  }

  if (C.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading file %s", sParamPath.c_str());
    stat = false;
  }
  if (sParamType == PARM_PRINT_AND_LOAD)
  {
    C.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!C.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing convergence parms. All params must be set",
	  sParamPath.c_str());
    stat = false;
  }
  sSetParmconv(C, parm);
  return stat;
}

//------------------------------------------------------------------
bool InterfaceParm::loadSetUvParams(ParmSetUV &p)
{
  ParamsSetUV Par;
  ParamsConvTT C;

  if (sParamType == PARM_PRINT)
  {
    Par.print(stdout, PRINT_VERBOSE);
    C.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (Par.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading SetUv params file %s", sParamPath.c_str());
    return false;
  }
  if (C.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading Conv params file %s", sParamPath.c_str());
    return false;
  }
  if (sParamType ==  PARM_PRINT_AND_LOAD)
  {
    Par.print(stdout, PRINT_VERBOSE);
    C.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!Par.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing parms. All setuv params must be set",
	  sParamPath.c_str());
    return false;
  }
  if (!C.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing parms. All Conv params must be set",
	  sParamPath.c_str());
    return false;
  }
  vector<ParmPcFcst> pcf;
  for (int i=0; i<Par.data_n; ++i)
  {
    ParmPcFcst PF(Par._data[i].name, Par._data[i].threshold, 
		  Par._data[i].fractional_area_data_threshold,
		  Par._data[i].alpha, Par._data[i].variance);
    pcf.push_back(PF);
  }

  vector<pair<double,double> > ff;
  ff.clear();
  for (int i=0; i<Par.fuzzy_grid_distance_n; ++i)
  {
    pair<double,double> d(Par._fuzzy_grid_distance[i].distance,
			  Par._fuzzy_grid_distance[i].value);
    ff.push_back(d);
  }

  p = ParmSetUV(pcf, 
		Par.forecast_input_smooth,
		Par.verif_input_smooth,
		Par.low_res_ngridpts,
		Par._volume_size,
		Par._volume_overlap,
		Par.phase_shift_res_npt,
		Par.max_phase_shift,
		Par.refine_phase_shift_res_npt,
		Par.refine_max_phase_shift,
		Par.low_res_motion_smooth_n,
		Par._low_res_motion_smooth,
		Par.high_res_motion_smooth_n,
		Par._high_res_motion_smooth,
		Par.high_res_gap_fill,
		Par.high_res_output_fcst_smooth_n,
		Par._high_res_output_fcst_smooth,
		Par.exclude_zero_from_smoothing,
		Par.high_res_max_expand_npt,
		Par.good_scaling,
		Par.good_dist_scaling,
		Par.fractional_area_min_pcnt,
		Par.grid_area_min_pcnt,
		ConvWxFuzzy(ff));
  sSetParmconv(C, p);
  return true;
}

//------------------------------------------------------------------
bool InterfaceParm::loadPcAlgParams(ParmPcAlg &pcParms)
{
  ParamsPcAlg Par;

  if (sParamType == PARM_PRINT)
  {
    Par.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (Par.load(sParamPath.c_str(), NULL, !sParamPrint, false) != 0)
  {
    ILOGF(ERROR, "Loading ParmPcAlg params file %s",
	  sParamPath.c_str());
    return false;
  }
  if (sParamType ==  PARM_PRINT_AND_LOAD)
  {
    Par.print(stdout, PRINT_VERBOSE);
    return true;
  }
  if (!Par.checkAllSet(stdout))
  {
    ILOGF(ERROR,
	  "file %s has missing parms. All ParmPcAlg params must be set",
	  sParamPath.c_str());
    return false;
  }

  pcParms = ParmPcAlg(Par._smooth, Par.use_high_smooth,
		      Par.smooth_where_corrected,
		      Par._gap_fill, Par.zero_resolution);
  return true;
}

//------------------------------------------------------------------
bool InterfaceParm::loadProjectionParams(const string &projFile,
					 ParmProjection &proj)
{
  return sLoadProjectionParamsActual(projFile, proj);
}

//------------------------------------------------------------------
bool InterfaceParm::loadTilingParams(const string &parmfile, ParmTiling &p)
{
  ParamsTiling P;
  if (P.load(parmfile.c_str(), NULL, true, false) != 0)
  {
    ILOGF(DEBUG, "Loading file %s", parmfile.c_str());
    return false;
  }
  if (!P.checkAllSet(stdout))
  {
    ILOGF(ERROR, "file %s has missing parms. All params must be set",
	  parmfile.c_str());
    return false;
  }

  p = ParmTiling(P.do_tiling, P._tile_npt, P._tile_overlap_npt,
		 P.mother_tile_is_subset, P._mother_tile_lower_left_index,
		 P._mother_tile_upper_right_index);
  return true;
}

//------------------------------------------------------------------
bool InterfaceParm::parmInit(int argc, char **argv)
{

  // turn off warning because parameter file has multiple uses.
  TDRP_warn_if_extra_params(FALSE);

  sAppName = argv[0];
  sParamType = sSetTdrpFlag(argc, argv);
  sParamPath = "";
  sParamPrint = false;
  sParamLoad = false;
  sParamArchive = InterfaceIO::getArchiveCmdargRange(argc, argv,
						     sParamArchiveT0,
						     sParamArchiveT1);
  for (int i=0; i<argc; ++i)
  {
    if (strcmp(argv[i], "-print_params") == 0)
    {
      sParamPrint = true;
    }
    else if (strcmp(argv[i], "-params") == 0)
    {
      if (i >= argc-1)
      {
	ILOG(ERROR, "-params was last arg");
	return false;
      }
      else
      {
	sParamPath = argv[i+1];
	sParamLoad = true;
      }
    }
  }
  return true;
}

//------------------------------------------------------------------
void InterfaceParm::parmFinish(void)
{
  // turn back on the warnings
  TDRP_warn_if_extra_params(TRUE);

  // if printParams is in there, exit now.
  if (sParamPrint)
  {
    exit(convWx::GOOD_EXIT);
  }
}

//------------------------------------------------------------------
const string InterfaceParm::parmPath(void)
{
  return sParamPath;
}

//------------------------------------------------------------------
bool InterfaceParm::isParmLoad(void)
{
  return sParamLoad;
}

//------------------------------------------------------------------
bool InterfaceParm::isParmPrint(void)
{
  return sParamPrint;
}

//------------------------------------------------------------------
time_t InterfaceParm::mostRecentTime(void)
{
  if (sParamArchive)
  {
    return sParamArchiveT1;
  }
  else
  {
    return time(0);
  }
}

//------------------------------------------------------------------
time_t InterfaceParm::oldestTime(void)
{
  if (sParamArchive)
  {
    return sParamArchiveT0;
  }
  else
  {
    return time(0);
  }
}

//------------------------------------------------------------------
bool InterfaceParm::paramTypeIsPrint(void)
{
  return sParamType == PARM_PRINT;
}

//------------------------------------------------------------------
bool InterfaceParm::paramTypeIsPrintAndLoad(void)
{
  return sParamType == PARM_PRINT_AND_LOAD;
}
