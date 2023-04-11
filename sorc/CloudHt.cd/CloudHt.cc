/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright (c) 1995, UCAR
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/
/*********************************************************************
 * CloudHt.cc: CloudHt program object.
 *
 * RAP, NCAR, Boulder CO
 *
 * March 2002
 *
 * Gary Blackburn
 *
 *********************************************************************/

#include <iostream>
#include <string>
#include <cassert>
#include <signal.h>


#include <os_config.h>
#include <Mdv/DsMdvxInput.hh>
#include <Mdv/DsMdvx.hh>
#include <Mdv/MdvxField.hh>
#include <Mdv/MdvxProj.hh>

#include <toolsa/DateTime.hh>
#include <toolsa/Path.hh>
#include <toolsa/pmu.h>
#include <toolsa/str.h>
#include <toolsa/umisc.h>
#include <toolsa/utim.h>

#include "CloudHt.hh"
#include "Params.hh"

using namespace std;

// Global variables

CloudHt *CloudHt::_instance = (CloudHt *) NULL;

// Global constants

const int FOREVER = true;
const float MISSING_DATA_VAL = 99999999.;

// ************************************************************************
//  Standard Atmosphere Profile:
// ************************************************************************

//    Pressure (hPa)

static const fl32 _std_p[] = {1013.25, 954.57, 898.71, 845.50, 794.90,
                              746.75, 700.99, 657.53, 616.29, 577.15, 540.07, 504.90, 471.65,
                              440.20, 410.46, 382.35, 355.82, 330.81, 307.24, 285.07, 264.19,
                              244.58, 234.53, 226.19, 193.38, 165.33, 141.35, 120.86, 103.30,
                              88.34, 75.53, 64.57, 55.21};

static const int _num_std_p = 33;

//    Height (meters) 
static const fl32 _std_z[] = {0., 500., 1000., 1500., 2000., 2500., 3000., 3500., 4000., 4500., 5000., 5500., 6000.,
                              6500., 7000., 7500., 8000., 8500., 9000., 9500., 10000., 10500., 10769., 11000., 12000.,
                              13000., 14000., 15000., 16000., 17000., 18000., 19000., 20000.};

// ************************************************************************


/*********************************************************************
 * Constructor
 */

CloudHt::CloudHt(int argc, char **argv) {
  static const string routine_name = "Constructor";

  // Make sure the singleton wasn't already created.

  assert(_instance == (CloudHt *) NULL);

  // Initialize the okay flag.

  okay = true;

  // Set the singleton instance pointer

  _instance = this;

  // Set the program name.

  path_parts_t progname_parts;

  uparse_path(argv[0], &progname_parts);
  _progName = STRdup(progname_parts.base);

  // Display copyright message.

  ucopyright(_progName);

  // Get the command line arguments.

  _args = new Args(argc, argv, _progName);

  _params = new Params();
  char *params_path = (char *) "unknown";

  if (_params->loadFromArgs(argc, argv,
                            _args->override.list,
                            &params_path)) {
    fprintf(stderr,
            "ERROR: %s::%s\n", _className(), routine_name.c_str());
    fprintf(stderr,
            "Problem with TDRP parameters in file <%s>\n",
            params_path);

    okay = false;

    return;
  }
}


/*********************************************************************
 * Destructor
 */

CloudHt::~CloudHt() {
  // Unregister process
  PMU_auto_unregister();

  // Free contained objects
  delete _params;
  delete _args;
  delete _satTempInput;

  // Free included strings

  STRfree(_progName);
}


/*********************************************************************
 * Inst() - Retrieve the singleton instance of this class.
 */

CloudHt *CloudHt::Inst(int argc, char **argv) {
  if (_instance == (CloudHt *) NULL)
    new CloudHt(argc, argv);

  return (_instance);
}

CloudHt *CloudHt::Inst() {
  assert(_instance != (CloudHt *) NULL);

  return (_instance);
}


/*********************************************************************
 * init() - Initialize the local data.
 * 
 * Returns true if the initialization was successful, false otherwise.
 */

bool CloudHt::init() {
  static const string method_name = "CloudHt::init()";

  // Initialize the data trigger

  _satTempInput = new DsMdvxInput;

  switch (_params->mode) {
    case Params::REALTIME : {
      cout << "Initializing REALTIME trigger using url: " <<
           _params->sat_temperature_field_info.url << endl;

      if (_satTempInput->setRealtime(_params->sat_temperature_field_info.url,
                                     _params->sat_max_input_valid_secs,
                                     (DsMdvxInput::heartbeat_t) PMU_auto_register)) {
        cerr << "ERROR: " << method_name << endl;
        cerr << "Error initializing REALTIME trigger using url: " <<
             _params->sat_temperature_field_info.url << endl;
        return false;
      }
      break;
    }

    case Params::ARCHIVE : {
      cout << "Initializing ARCHIVE trigger using url: " <<
           _params->sat_temperature_field_info.url << endl;

      if (_satTempInput->setArchive(_params->sat_temperature_field_info.url,
                                    _args->getArchiveStartTime(),
                                    _args->getArchiveEndTime())) {
        cerr << "ERROR: " << method_name << endl;
        cerr << "  setup for archive mode failed." << endl;
        return false;
      }

      cout << "Using start and end times of:  "
           << DateTime::str(_args->getArchiveStartTime()) << " and "
           << DateTime::str(_args->getArchiveEndTime()) << endl;

      break;
    }

    case Params::FILELIST : {
      cerr << "Initializing FILELIST trigger using url: " <<
           _params->sat_temperature_field_info.url << endl;
      if (_satTempInput->setFilelist(_args->getSatFileList())) {
        cerr << "ERROR: " << method_name << endl;
        cerr << "  setup for file list mode failed." << endl;
        return false;
      }

      break;
    }

  } /* endswitch - _params->mode */

  // initialize process registration

  if (_params->mode == Params::REALTIME)
    PMU_auto_init(_progName, _params->instance,
                  PROCMAP_REGISTER_INTERVAL);

  return true;
}


/*********************************************************************
 * run()
 */

void CloudHt::run() {
  static const string method_name = "CloudHt::run()";

  // register with procmap
  PMU_auto_register("Running");

  while (!_satTempInput->endOfData()) {
    PMU_auto_register("In main loop");

    // Clear out any old data before starting

    _clearData();

    // Initialize the satellite temperature file

    if (!_readSatTempFile(_satTempMdvx)) {
      cerr << "ERROR: " << method_name << endl;
      cerr << "       Initializing Satellite Temperature file " << endl;

      continue;
    }

    // Initialize the model height field

    if (!_readModelField(_satTempInput->getDataTime(),
                         _params->height_field_info.url,
                         _params->height_field_info.field_name,
                         _params->height_field_info.field_num,
                         true,
                         _modelHeightMdvx)) {
      cerr << "ERROR: " << method_name << endl;
      cerr << "Error initializing model height field " << endl;

      continue;
    }

    MdvxProj modelProj(_modelHeightMdvx.getMasterHeader(),
                       _modelHeightMdvx.getField(0)->getFieldHeader());

    // Initialize the model tropopause height field

    if (_params->tropo_method == Params::TROPO_FROM_TROPO_HT_FIELD) {
      if (!_readModelField(_satTempInput->getDataTime(),
                           _params->tropo_height_field_info.url,
                           _params->tropo_height_field_info.field_name,
                           _params->tropo_height_field_info.field_num,
                           false,
                           _modelTropoHeightMdvx)) {
        cerr << "ERROR: " << method_name << endl;
        cerr << "Error initializing model tropopause height field " << endl;

        continue;
      }

      if (MdvxProj(_modelTropoHeightMdvx.getMasterHeader(),
                   _modelTropoHeightMdvx.getField(0)->getFieldHeader())
          != modelProj) {
        cerr << "ERROR: " << method_name << endl;
        cerr << "Model tropopause height field projection doesn't match "
             << "model height field projection." << endl;

        continue;
      }
    }

    // Initialize the model temperature field

    if (!_readModelField(_satTempInput->getDataTime(),
                         _params->temperature_field_info.url,
                         _params->temperature_field_info.field_name,
                         _params->temperature_field_info.field_num,
                         true,
                         _modelTempMdvx)) {
      cerr << "ERROR: " << method_name << endl;
      cerr << "   Initializing Model temperature field " << endl;

      continue;
    }

    if (MdvxProj(_modelTempMdvx.getMasterHeader(),
                 _modelTempMdvx.getField(0)->getFieldHeader())
        != modelProj) {
      cerr << "ERROR: " << method_name << endl;
      cerr << "Model temperature field projection doesn't match "
           << "model height field projection." << endl;

      continue;
    }

    // If we get here, we've read all of the input data and everything
    // looks okay, so go ahead and process it.

    if (!_processData()) {
      cerr << "ERROR: " << method_name << endl;
      cerr << "Error processing CloudHt data files " << endl;

      continue;
    }

  } /* endwhile - !_satTempInput->endOfData() */

}

/**********************************************************************
 *              Private Member Functions                              *
 **********************************************************************/

/*********************************************************************
 * _calcTropopauseIndices() - Calculate the tropopause indices using
 *                            the specified method.
 */

int *CloudHt::_calcTropopauseIndices() const {
  switch (_params->tropo_method) {
    case Params::TROPO_FROM_TROPO_HT_FIELD :
      return _calcTropoFromTropoHt();

    case Params::TROPO_FROM_3D_HT_AND_TEMP_FIELDS :
      return _calcTropoFrom3DHtTempFields();
  }

  // We should never get here

  return 0;
}


/*********************************************************************
 * _calcTropopauseFromTropoHt() - Calculate the tropopause indices using
 *                                the tropopause height field.
 */

int *CloudHt::_calcTropoFromTropoHt() const {
  // Get pointers to the model height data

  MdvxField *model_height_field = _modelHeightMdvx.getFieldByNum(0);
  Mdvx::field_header_t model_height_field_hdr =
    model_height_field->getFieldHeader();

  model_height_field->setPlanePtrs();

  fl32 *modz_plane_ptr[model_height_field_hdr.nz];
  for (int i = 0; i < model_height_field_hdr.nz; i++)
    modz_plane_ptr[i] = (fl32 *) model_height_field->getPlane(i);

  // Get pointers to the model tropopause height data

  MdvxField *model_tropo_height_field = _modelTropoHeightMdvx.getFieldByNum(0);
  fl32 *model_tropo_height_data = (fl32 *) model_tropo_height_field->getVol();

  // Get the volume information

  int plane_size = model_height_field_hdr.nx * model_height_field_hdr.ny;
  int nz = model_height_field_hdr.nz;

  // Allocate space for the returned array

  int *trop_index = new int[plane_size];

  // Calculate the tropopause height.  We are assuming that the model
  // data is never missing. We probably need to add code to handle
  // missing data values.

  for (int i = 0; i < plane_size; ++i) {
    // Start at the top of the height field and look down until we
    // find a place where the tropopause height value is less than
    // the model height. Then take the level above this as the
    // tropopause level.

    bool trop_found = false;

    for (int z = nz - 1; z > 0; --z) {
      if (model_tropo_height_data[i] > modz_plane_ptr[z - 1][i]) {
        trop_index[i] = z;
        trop_found = true;

        break;
      }
    } /* endfor - z */

    // If we didn't find the tropopause, all of our data must be
    // above the tropopause.

    if (!trop_found)
      trop_index[i] = 0;

  } /* endfor - i */

  return trop_index;
}

/*********************************************************************
 * _calcTropoFrom3DHtTempFields() - Calculate the tropopause indices
 *                                  using the 3D height and temperature
 *                                  fields.
 */

int *CloudHt::_calcTropoFrom3DHtTempFields() const {
  // Get pointers to the model height data

  MdvxField *model_height_field = _modelHeightMdvx.getFieldByNum(0);
  Mdvx::field_header_t model_height_field_hdr =
    model_height_field->getFieldHeader();

  model_height_field->setPlanePtrs();

  fl32 *modz_plane_ptr[model_height_field_hdr.nz];
  for (int i = 0; i < model_height_field_hdr.nz; i++)
    modz_plane_ptr[i] = (fl32 *) model_height_field->getPlane(i);

  // Get pointers to the model temperature data

  MdvxField *model_temp_field = _modelTempMdvx.getFieldByNum(0);
  Mdvx::field_header_t model_temp_field_hdr =
    model_temp_field->getFieldHeader();

  model_temp_field->setPlanePtrs();

  fl32 *modt_plane_ptr[model_temp_field_hdr.nz];
  for (int i = 0; i < model_temp_field_hdr.nz; i++)
    modt_plane_ptr[i] = (fl32 *) model_temp_field->getPlane(i);

  // Allocate the other needed arrays

  int plane_size = model_height_field_hdr.nx * model_height_field_hdr.ny;
  int nz = model_height_field_hdr.nz;

  double dtdz[nz];
  double tmp_profile[nz];
  int trop_check[nz];

  // Allocate space for the returned array

  int *trop_index = new int[plane_size];

  // Calculate the tropopause height

  for (int ndex = 0; ndex < plane_size; ++ndex) {
    // Initialize the tropopause value to be at the top of the model volume

    trop_index[ndex] = nz - 1;

    // Initialize the dt/dz array

    for (int k = 0; k < nz; k++)
      dtdz[k] = 0.0;

    // Loop from the top of the atmosphere down, computing dT/dz

    for (int i = nz - 1; i > 0; i--) {
      if (i == (nz - 1)) {
        dtdz[i] = (10000.0 * (modt_plane_ptr[i][ndex] - modt_plane_ptr[i - 1][ndex])
                   / (modz_plane_ptr[i][ndex] - modz_plane_ptr[i - 1][ndex]));
      } else {
        dtdz[i] = (10000.0 * (modt_plane_ptr[i - 1][ndex] - modt_plane_ptr[i + 1][ndex])
                   / (modz_plane_ptr[i - 1][ndex] - modz_plane_ptr[i + 1][ndex]));
      }
    }

    // Initialize the tropopause check array. This array shows us levels
    // that we need to check to see if they are at the tropopause

    for (int i = 0; i < nz; i++)
      trop_check[i] = 0;

    // Look for onset of isothermal or positive lapse rate
    // ordered from the top of atmosphere down. (This loop could actually
    // go top to bottom or bottom to top.)

    for (int i = nz - 1; i > 0; i--) {
      if (dtdz[i] > -3.5)
        trop_check[i] = 1;
    }

    // Loop from surface upward looking for the tropopause.

    int trop_sum = 0;
    fl32 trop_mean = 0.0;

    for (int k = 2; k < nz; k++) {
      trop_sum = 0;
      for (int j = nz - 1; j >= k; j--)
        trop_sum += trop_check[j];

      trop_mean = (fl32) trop_sum / (nz - k);
      if (trop_check[k] == 1 && trop_mean >= 0.6) {
        trop_index[ndex] = k;
        break;
      }
    }

    // Look through the temperature profile for an inversion

    for (int kk = 0; kk < nz; kk++)
      tmp_profile[kk] = modt_plane_ptr[kk][ndex];

    if (trop_index[ndex] >= nz - 4) {
      int hgt = trop_index[ndex];
      for (int i = hgt; i > 1; i--) {
        if (tmp_profile[i] > tmp_profile[i - 1])
          hgt = i - 1;;

        if (hgt > 1 && (tmp_profile[i] < tmp_profile[i - 1])) {
          trop_index[ndex] = hgt;
          break;
        }
      }
    }

  } /* endfor - ndex */

  return trop_index;
}


/*********************************************************************
 * _clearData() - Clear out all of the MDV data.
 */

void CloudHt::_clearData() {
  _satTempMdvx.clear();
  _modelHeightMdvx.clear();
  _modelTropoHeightMdvx.clear();
}


/*********************************************************************
 * _processData() - Process the given satellite file.
 */

bool CloudHt::_processData() {
  static const string method_name = "CloudHt::_processData()";

  bool found = false;

  // Get pointers to the satellite data

  MdvxField *sat_bt_field = _satTempMdvx.getFieldByNum(0);
  Mdvx::master_header_t sat_bt_master_hdr = _satTempMdvx.getMasterHeader();
  Mdvx::field_header_t sat_bt_field_hdr = sat_bt_field->getFieldHeader();

  fl32 *satbt_ptr = (fl32 *) sat_bt_field->getVol();

  int plane_size = sat_bt_field_hdr.nx * sat_bt_field_hdr.ny;

  // Get pointers to the model height data.

  MdvxField *model_height_field = _modelHeightMdvx.getFieldByNum(0);
  Mdvx::field_header_t model_height_field_hdr =
    model_height_field->getFieldHeader();
  Mdvx::vlevel_header_t model_height_vert_hdr =
    model_height_field->getVlevelHeader();

  model_height_field->setPlanePtrs();

  fl32 *modz_plane_ptr[model_height_field_hdr.nz];
  for (int i = 0; i < model_height_field_hdr.nz; i++)
    modz_plane_ptr[i] = (fl32 *) model_height_field->getPlane(i);

  // Get pointers to the model temperature data.

  MdvxField *model_temp_field = _modelTempMdvx.getFieldByNum(0);
  Mdvx::master_header_t model_temp_master_hdr =
    _modelTempMdvx.getMasterHeader();
  Mdvx::field_header_t model_temp_field_hdr =
    model_temp_field->getFieldHeader();

  model_temp_field->setPlanePtrs();

  fl32 *modt_plane_ptr[model_temp_field_hdr.nz];

  for (int i = 0; i < model_temp_field_hdr.nz; i++)
    modt_plane_ptr[i] = (fl32 *) model_temp_field->getPlane(i);

  int nz = model_temp_field_hdr.nz;

  // create the projection objects. Base the model projection on the
  // temperature field since we already made sure that all of the model
  // field projections matched.

  MdvxProj satProj(sat_bt_master_hdr, sat_bt_field_hdr);
  MdvxProj modelProj(model_temp_master_hdr, model_temp_field_hdr);

  // Calculate the tropopause level indices

  int *trop_index = _calcTropopauseIndices();

  // Allocate needed arrays

  fl32 tmp_profile[nz];
  int trop_check[nz];

  // Allocate space for the cloud height field

  fl32 *flt_lev = new fl32[plane_size];

  PMU_auto_register("Cloud Height processing");

  // Calculate the cloud height for each grid square

  bool missing_value_message_displayed = false;

  int j = 0;
  for (int y = 0; y < sat_bt_field_hdr.ny; y++) {
    for (int x = 0; x < sat_bt_field_hdr.nx; x++, j++) {
      double lat;
      double lon;

      // If the satellite data is missing, we don't need to do anything

      if (satbt_ptr[j] == sat_bt_field_hdr.missing_data_value ||
          satbt_ptr[j] == sat_bt_field_hdr.bad_data_value) {
        if (_params->debug >= Params::DEBUG_VERBOSE &&
            !missing_value_message_displayed) {
          cout << "missing or bad data values found" << endl;
          missing_value_message_displayed = true;
        }

        flt_lev[j] = MISSING_DATA_VAL;

        continue;
      }

      // working units are degK
      double sat_temp_K;
      if (_params->sat_temperature_field_info.input_units == Params::degC){
        sat_temp_K = satbt_ptr[j] + 273.15;
      }
      else{
        sat_temp_K = satbt_ptr[j];
      }

      satProj.xyIndex2latlon(x, y, lat, lon);

      // If the current point is outside of the model grid, we
      // can't do anything

      int ndex;

      if (modelProj.latlon2arrayIndex(lat, lon, ndex) < 0) {
        if (_params->debug >= Params::DEBUG_VERBOSE) {
          cerr << "ERROR: " << method_name << endl;
          cerr << "Did not find lat - " << lat << " lon - " << lon
               << " in model grid" << endl;
          cerr << "corresponds to x  - " << x << " y - " << y
               << " in satellite grid" << endl;
        }

        flt_lev[j] = MISSING_DATA_VAL;

        continue;
      }

      found = false;

      // Starting at the bottom of atmosphere, find where satellite temp
      // falls in between model profile layers and interpolate to obtain
      // the equivalent geopotential height.

      // Get a temporary temperature profile for this point

      for (int kk = 0; kk < nz; kk++) {

        // working units degK
        double model_temp_K;
        if (_params->temperature_field_info.input_units == Params::degC) {
          model_temp_K = modt_plane_ptr[kk][ndex] + 273.15;
        }
        else {
          model_temp_K = modt_plane_ptr[kk][ndex];
        }

        tmp_profile[kk] = model_temp_K;
      }

      // From max height down to the surface, remove any minor inversions
      // from the profile by averaging the surrounding temperature values.

      for (int kk = trop_index[ndex] - 1; kk > 1; kk--) {
        if (tmp_profile[kk] < tmp_profile[kk + 1]) {
          tmp_profile[kk] = (tmp_profile[kk + 1] + tmp_profile[kk - 1]) / 2.0;
        }
      }

      // We need a spare level above where we are working to do the calculations,
      // so move the tropopause index down if needed.

      if (trop_index[ndex] >= nz - 1)
        trop_index[ndex] = nz - 2;

      // Now find the pressure at the cloud top

      double pressure_level = 0.0;

      for (int i = 0; i <= trop_index[ndex]; i++) {
        if ((sat_temp_K <= tmp_profile[i]) &&
            (sat_temp_K > tmp_profile[i + 1])) {
          double slope =
            (modz_plane_ptr[i + 1][ndex] - modz_plane_ptr[i][ndex]) /
            (tmp_profile[i + 1] - tmp_profile[i]);
          double intercept = modz_plane_ptr[i][ndex];
          double temp_diff = sat_temp_K - tmp_profile[i];
          double hgt_tmp = slope * temp_diff + intercept;

          // Determine model pressure corresponding to this height
          // via hypsometric equation interpolation

          pressure_level = _getPressure(sat_temp_K, tmp_profile[i],
                                        hgt_tmp, modz_plane_ptr[i][ndex],
                                        model_height_vert_hdr.level[i]);
          found = true;
          break;
        }
      }

      /* If we still haven't found a model height/pressure corresponding
	 to our satellite data, then the found flag will still be == 0
	 If the satellite temperature is colder than coldest model
	 temperature then truncate to the model height corresponding
	 to the coldest model temperature
      */
      /* Idea, comute moist adiabatic lapse rate (MALR)  at this level and 
       * use to interpolate to height and pressure corresponding to the satellite 
       * measured brightness temperature But at cold temperatures, 
       * MALR = DALR = -g/C_p_dry ~ -10 degrees K / km
       */

      if (!found && sat_temp_K > 0.0) {
        if (sat_temp_K <= tmp_profile[trop_index[ndex]] &&
            sat_temp_K <= 288.15) {
          // Set the slope corresponding to the DALR of -10 K /Km.
          // The units here are m/K

          double slope = -100.0;
          double intercept = modz_plane_ptr[trop_index[ndex]][ndex];
          double temp_diff = sat_temp_K - tmp_profile[trop_index[ndex]];
          double hgt_tmp = slope * temp_diff + intercept;

          pressure_level = _getPressure(sat_temp_K, tmp_profile[trop_index[ndex]],
                                        hgt_tmp,
                                        modz_plane_ptr[trop_index[ndex]][ndex],
                                        model_height_vert_hdr.level[trop_index[ndex]]);

          found = true;
        }
      }

      // If we couldn't find the pressure, set it to surface pressure of
      // the standard atmosphere

      if (pressure_level == 0.)
        pressure_level = 1013.25;

      // Now, convert flight levels from pressure (hPa) to height (Kft)
      // using a standard atmosphere assumption

      bool fltlev_flag = false;
      double flt_lev_m = 0.0;

      // Checking first for whether satellite measurement was
      // within acceptable range for processing

      if ((sat_temp_K < 288.15) && (sat_temp_K > 150.0)) {
        // Hypsometric interpolation using td_p and std_z profiles,
        //  looping from bottom of atmosphere upwards...
        //
        //       NOTE CONSTANTS and UNITS:
        //             R_dry = 287 (J/kg-K)

        //             g = 9.8 (m/s^2)
        //             R_dry/(2.0*g) = 14.6429 (m/K)
        //             Std Atms lapse rate = 0.0065 (K/m)
        //             Std Atms sfc temp = 15 (C) = 288.15 (K)
        //             Std Atms sfc pres = 1013.25 (hPa)
        //             Conversion from m to Kft: 3.28084(Kft)/1000.0(m)

        fl32 std_t;
        if (pressure_level > _std_p[0]) {
          // Assign to surface height and exit;
          flt_lev_m = 0.0;
          fltlev_flag = true;
        } else if (pressure_level < _std_p[_num_std_p - 1]) {
          // is the pressure was less than lowest Std Atmos Pres (55.21 mb)

          std_t = 218.15;
          double tmp_term = 14.6429 * log(_std_p[_num_std_p - 1] / pressure_level);
          flt_lev_m = _std_z[_num_std_p - 1] + tmp_term *
                                               (sat_temp_K + std_t);
          fltlev_flag = true;
        } else {
          // loop over standard atmosphere levels

          for (int p = 0; p < _num_std_p - 1; p++) {
            if (pressure_level <= _std_p[p] && pressure_level > _std_p[p + 1]) {
              if (p <= 22) {
                // Somewhere in the Troposphere
                std_t = 288.15 - _std_z[p] * 0.0065;
                double tmp_term = 14.6429 * log(_std_p[p] / pressure_level);

                flt_lev_m = _std_z[p] + tmp_term * (sat_temp_K + std_t) /
                                        (1.0 + 0.0065 * tmp_term);
                fltlev_flag = true;
                break;
              }
            } else {
              if (p > 22) {
                // Isothermal Region at/above Tropopause
                std_t = 288.15;
                double tmp_term = 14.6429 * log(_std_p[p] / pressure_level);

                flt_lev_m = _std_z[p] + tmp_term * (sat_temp_K + std_t);
                fltlev_flag = true;
                break;
              }
            }

          }
        }

        if (fltlev_flag == false) {
          flt_lev[j] = 0.;
          cout << "Didn't find flight level " << endl;
        } else {
          // convert result from meters to ft
          flt_lev[j] = flt_lev_m * 3.28084;
        }

      } // if ((sat_temp_K < 288.15)  && (sat_temp_K > 150.0)) 
      else {
        flt_lev[j] = 0.;
      }

    }      // for x=0
  }        // for y=0

  // create the output field reusing the satellite header

  if (_params->debug)
    cout << "Creating cldHt_field " << endl;

  MdvxField *cldHt_field;

  cldHt_field = _createField("CloudHeight", 11, "ft", MISSING_DATA_VAL, flt_lev,
                             sat_bt_field_hdr);

  if (cldHt_field == 0) {
    cerr << "ERROR: " << method_name << endl;
    cerr << "Error creating CAPE MDV field" << endl;

    delete[] flt_lev;
    delete[] trop_index;

    return false;
  }

  delete[] flt_lev;
  delete[] trop_index;

  // Create and write the output file

  DsMdvx output_file;

  _updateMasterHeader(output_file,
                      sat_bt_master_hdr.time_begin,
                      sat_bt_master_hdr.time_end,
                      sat_bt_master_hdr.time_centroid,
                      sat_bt_field_hdr);

  // Add the cloud top height field to output file

  cldHt_field->convertType(Mdvx::ENCODING_INT8,
                           Mdvx::COMPRESSION_RLE,
                           Mdvx::SCALING_DYNAMIC);

  output_file.addField(cldHt_field);

  output_file.setDataSetInfo("Generated by CloudHt");
  output_file.setDataSetName("CloudHt");
  output_file.setDataSetSource("CloudHt");


  output_file.setWriteLdataInfo();

  PMU_auto_register("writing data");

  cout << "Writing MDV file to dir " << _params->output_url << endl;

  if (_params->mode != Params::REALTIME)
    output_file.clearWriteLdataInfo();

  if (output_file.writeToDir(_params->output_url) != 0) {
    cerr << "ERROR: " << method_name << endl;
    cerr << "Error writing output MDV file to URL: " <<
         _params->output_url << endl;

    return false;
  }

  return true;
}

/*********************************************************************
 * _getPressure() - Read the indicated field data.
   Interpolation of model level pressure based on hypsometric equation:
     satbt   == the satellite brightness temperature
     modt    == the temperature at the *model level* just below tsat
     hgt_tmp == the satellite btemp-derived cloud height
     modz    == the height at the *model level* just below zsat
     z       == the model level index corresponding to modt and modz
               (used to index model pressure levels)
 
   FORMULA:
    P_SAT(hPa) = P_LVL * exp(-(HGT_TMP - MODZ) / (R_dry*<T>/g) )
    
   where: <T>  = (satbt+modt)/2.0 == layer average temperature
         R_dry = 287 (J/kg-K)
         g     = 9.8 (m/s^2)
         P_LVL = botp[z] (hPa)
  
   NOTE: we have consolidated (R_dry/(2.0*g))^-1 = 0.06829 (K/m)
    
 */
fl32 CloudHt::_getPressure(const fl32 satbt, const fl32 modt,
                           const fl32 hgt_tmp, const fl32 modz, const fl32 z) {
  return (z * exp(-0.06829 * (hgt_tmp - modz) / (satbt + modt)));

}

/*********************************************************************
 * _readSatTempFile() - Read the indicated field data.
 */

bool CloudHt::_readSatTempFile(DsMdvx &input_file) {
  static const string method_name = "CloudHt::_readSatTempFile()";

  // Set up the read request

  string fieldname = _params->sat_temperature_field_info.field_name;

  // do the setup

  input_file.setDebug(_params->debug >= Params::DEBUG_VERBOSE);

  // define which field is to be read

  if (!fieldname.empty()) {
    if (_params->debug >= Params::DEBUG_NORM)
      cout << "Using field name " << _params->sat_temperature_field_info.field_name << endl;
    input_file.addReadField(fieldname);
  } else {
    input_file.addReadField(_params->sat_temperature_field_info.field_num);
  }

  input_file.setReadEncodingType(Mdvx::ENCODING_FLOAT32);
  input_file.setReadCompressionType(Mdvx::COMPRESSION_NONE);
  input_file.setReadScalingType(Mdvx::SCALING_NONE);

  if (_params->debug >= Params::DEBUG_VERBOSE)
    input_file.printReadRequest(cerr);

  // Now read the volume

  if (_satTempInput->readVolumeNext(input_file)) {
    cerr << "ERROR: " << method_name << endl;
    cerr << "Error reading satellite data" << endl;
    cerr << input_file.getErrStr() << endl;

    return false;
  }

  // Make sure that we read the field successfully.

  if (input_file.getNFields() != 1) {
    cerr << "ERROR: " << method_name << endl;
    cerr << "Error reading ";
    if (fieldname.length() > 0)
      cerr << _params->sat_temperature_field_info.field_name;
    else
      cerr << "#" << _params->sat_temperature_field_info.field_num;
    cerr << " field from satellite URL." << endl;
    return false;
  }

  return true;
}

/*********************************************************************
 * _readModelField() - Read the indicated field data.
 */

bool CloudHt::_readModelField(const time_t sat_temp_time,
                              const string &url,
                              const string &field_name,
                              const int field_num,
                              const bool set_vertical_limits,
                              DsMdvx &input_file) {
  static const string method_name = "CloudHt::_readModelField()";

  // Set up the read request

  input_file.setDebug(_params->debug >= Params::DEBUG_VERBOSE);

  input_file.setReadTime(Mdvx::READ_BEST_FORECAST, url,
                         _params->forecast_search_margin, sat_temp_time, 0);


  // define which field is to be read
  input_file.clearReadFields();
  if (field_name.length() > 0)
    input_file.addReadField(field_name);
  else
    input_file.addReadField(field_num);

  // Match the horizontal extents of the satellite data. This will also
  // normalize the longitudes in the model data to match the longitudes
  // in the satellite data.

  MdvxProj sat_proj(_satTempMdvx.getMasterHeader(),
                    _satTempMdvx.getField(0)->getFieldHeader());
  Mdvx::coord_t coord = sat_proj.getCoord();

  input_file.setReadHorizLimits(coord.miny,
                                coord.minx,
                                coord.miny + (coord.ny * coord.dy),
                                coord.minx + (coord.nx * coord.dx));

  // Match levels expected by the algorithm

  if (set_vertical_limits)
    input_file.setReadVlevelLimits(_params->height_info.min,
                                   _params->height_info.max);

  input_file.setReadEncodingType(Mdvx::ENCODING_FLOAT32);
  input_file.setReadCompressionType(Mdvx::COMPRESSION_NONE);
  input_file.setReadScalingType(Mdvx::SCALING_NONE);

  if (_params->debug >= Params::DEBUG_VERBOSE)
    input_file.printReadRequest(cerr);

  // Now read the volume

  if (input_file.readVolume() != 0) {
    cerr << "ERROR: " << method_name << endl;
    cerr << "Error model reading volume:" << endl;
    cerr << "   url: " << url << endl;
    if (field_name.length() > 0)
      cerr << "   field: \"" << field_name << "\"" << endl;
    else
      cerr << "   field: #" << field_num << endl;
    cerr << input_file.getErrStr() << endl;

    return false;
  }

  // Make sure that we read the field successfully.

  if (input_file.getNFields() != 1) {
    cerr << "ERROR: " << method_name << endl;
    cerr << "Error reading ";
    if (field_name.length() > 0)
      cerr << field_name;
    else
      cerr << "#" << field_num;
    cerr << " field from model URL: " << url << endl;
    return false;
  }

  if (_params->debug >= Params::DEBUG_NORM) {
    if (field_name.length() > 0)
      cerr << "Model field " << field_name << " successfully read:" << endl;
    else
      cerr << "Model field #" << field_num << " successfully read:" << endl;
    cerr << "     URL: " << url << endl;
    cerr << "     File valid time: "
         << DateTime::str(input_file.getValidTime()) << endl;
    cerr << "     File forecast time: "
         << DateTime::str(input_file.getForecastTime()) << endl;
    cerr << "     File forecast lead secs: "
         << input_file.getForecastLeadSecs() << endl;
  }

  return true;
}


/*********************************************************************
 * _createField() - Create the given field.
 */

MdvxField *CloudHt::_createField(const string &field_name,
                                 const int field_code,
                                 const string &units,
                                 const fl32 missing_data_value,
                                 fl32 *data,
                                 const Mdvx::field_header_t &header) {
  Mdvx::field_header_t field_hdr;

  memset(&field_hdr, 0, sizeof(field_hdr));

  field_hdr.field_code = field_code;
  field_hdr.nx = header.nx;
  field_hdr.ny = header.ny;
  field_hdr.nz = header.nz;
  field_hdr.proj_type = header.proj_type;
  field_hdr.encoding_type = Mdvx::ENCODING_FLOAT32;
  field_hdr.data_element_nbytes = 4;
  field_hdr.volume_size = field_hdr.nx * field_hdr.ny * field_hdr.nz * field_hdr.data_element_nbytes;
  field_hdr.compression_type = Mdvx::COMPRESSION_NONE;
  field_hdr.transform_type = Mdvx::DATA_TRANSFORM_NONE;
  field_hdr.scaling_type = Mdvx::SCALING_NONE;
  field_hdr.native_vlevel_type = 0;
  field_hdr.vlevel_type = 0;
  field_hdr.dz_constant = header.dz_constant;
  field_hdr.proj_origin_lat = header.proj_origin_lat;
  field_hdr.proj_origin_lon = header.proj_origin_lon;
  for (int i = 0; i < MDV_MAX_PROJ_PARAMS; ++i) {
    field_hdr.proj_param[i] = header.proj_param[i];
  }
  field_hdr.grid_dx = header.grid_dx;
  field_hdr.grid_dy = header.grid_dy;
  field_hdr.grid_dz = header.grid_dz;
  field_hdr.grid_minx = header.grid_minx;
  field_hdr.grid_miny = header.grid_miny;
  field_hdr.grid_minz = header.grid_minz;;
  field_hdr.scale = header.scale;
  field_hdr.bias = header.bias;
  field_hdr.bad_data_value = missing_data_value;
  field_hdr.missing_data_value = missing_data_value;
  STRcopy(field_hdr.field_name_long, field_name.c_str(), MDV_LONG_FIELD_LEN);
  STRcopy(field_hdr.field_name, field_name.c_str(), MDV_SHORT_FIELD_LEN);
  STRcopy(field_hdr.units, units.c_str(), MDV_UNITS_LEN);


  // two dimensional field
  Mdvx::vlevel_header_t vlevel_hdr;

  memset(&vlevel_hdr, 0, sizeof(vlevel_hdr));

  return new MdvxField(field_hdr, vlevel_hdr, (void *) data);
}


/*********************************************************************
 * _updateMasterheader() - Update the master header for the output file.
 */

void CloudHt::_updateMasterHeader(DsMdvx &output_file,
                                  const time_t time_begin,
                                  const time_t time_end,
                                  const time_t time_centroid,
                                  const Mdvx::field_header_t &field_hdr) {
  Mdvx::master_header_t master_hdr;

  memset(&master_hdr, 0, sizeof(master_hdr));

  master_hdr.time_gen = time(0);
  master_hdr.time_begin = time_begin;
  master_hdr.time_end = time_end;
  master_hdr.time_centroid = time_centroid;
  master_hdr.time_expire = time_end;

  master_hdr.data_dimension = 2;
  master_hdr.data_collection_type = Mdvx::DATA_SYNTHESIS;
  master_hdr.native_vlevel_type = field_hdr.vlevel_type;
  master_hdr.vlevel_type = field_hdr.vlevel_type;
  master_hdr.vlevel_included = 1;
  master_hdr.grid_orientation = Mdvx::ORIENT_SN_WE;
  master_hdr.data_ordering = Mdvx::ORDER_XYZ;

  output_file.setDataSetInfo("Generated by CloudHt");
  output_file.setDataSetName("CloudHt");
  output_file.setDataSetSource("CloudHt");

  output_file.setMasterHeader(master_hdr);
}

