
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// � University Corporation for Atmospheric Research (UCAR) 2009-2020.
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
 * @file GmgsiNcf2Mdv.hh
 * @brief Converts GMGSI NetCDF file of known format to Mdv.This is not
 *         a general netCDF file converter (assumptions about the file contents
 *         are made)
 *        
 * @class  GmgsiNcf2Mdv
 * @brief  Converts GMGSI NetCDF file of known format to Mdv. This is not
 *         a general netCDF file converter.
 *     
 */

#ifndef GmgsiNcf2Mdv_H
#define GmgsiNcf2Mdv_H


#include <string>
#include <didss/DsInputPath.hh>
#include <didss/LdataInfo.hh>
#include <dsdata/TriggerInfo.hh>
#include <Mdv/DsMdvx.hh>
#include <Mdv/MdvxPjg.hh>
#include <Mdv/MdvxField.hh>
#include <toolsa/DateTime.hh>
#include "Args.hh"
#include "Params.hh"

using namespace std;

class GmgsiNcf2Mdv {
 

public:
/**
   * Constructor 
   * Initialize data members, set up archive or real-time triggering based on
   * user defined parameters. Exit if driver does not initialize properly.
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  Typical real-time and archive mode command lines are:
   *                  'GmgsiNcf2Mdv -params GmgsiNcf2Mdv.params' or
   *                  'GmgsiNcf2Mdv -params GmgsiNcf2Mdv.params -f [satFilePath]'
   */
  GmgsiNcf2Mdv (int argc, char **argv);

  /**
   * Destructor  Free memory, unregister with the process mapper
   */
  ~GmgsiNcf2Mdv();

  /**
   * For each data trigger process input data
   */
  int Run();

  
  /**
   * Flag to indicate object initialized properly
   */
  bool isOK;

protected:
  
private:
 
  /**
   *  Missing or bad data for satellite data field
   */
  const static float SAT_MISSING;

  /**
   * Used in debug messages
   */
  string _progName;
 
  /**
   * Object to parse command line arguments
   */
  Args _args;
  
  /**
   * Object containing user defined parameters
   */
  Params _params;
  
  /**
   * File trigger. Note that in real-time mode this trigger requires input files to be 
   * named yyyymmdd/hhmmss.nc
   */
  DsInputPath *_fileTrigger;
  
  /**
   * Header for Mdv object
   */ 
  Mdvx::master_header_t _master_hdr;
  
  /**
   * Header for vertical level component of Mdv object
   */ 
  Mdvx::vlevel_header_t _vlevel_hdr;

  /**
   * Header for Mdv field data
   */ 
  Mdvx::field_header_t _field_hdr;

  /**
   * Output data projection object handles mapping satellite data to user specified
   * projection
   */
  MdvxPjg _outputProj;

  /**
   * Array holds latitudes of every data point in the granule. These are calculated
   * from the sparse geo location information contained in the file.
   */
  float *_latitudes;
  
  /**
   * Array holds longitudes of every data point in the granule.These are calculated
   * from the sparse geo location information contained in the file.
   */
  float *_longitudes;
  
  /**
   * Pointer to array of satellite data field values at resolution in netCDF data file
   */
  float * _satDataVals;

  /**
   * Pointer to array of satellite data field values at according to user specified projection
   */
  float *_mdvDataVals;
 
  int  _dataTime;

  int _nx;

  int _ny;
 
  /**
   * Free allocated memory 
   */ 
  void _clearData();

  /**
   * Call methods to read the netCDF, create the Mdv object, and write the output file
   * @param[in] inputPath  path of input netCDF file returned by the data trigger.
   * @return  0 if successful, 1 otherwise
   */
  int _processData(char *inputPath); 

  /**
   * Read the netCDF file. The application expects nx3 sparse latitude and longitude 
   * arrays stored as floats, one satellite data field stored as shorts with scale and
   * bias. The names of these fields, and the ground track and cross track granule 
   * data dimensions are entered as user defined parameters.
   * @param[in] filename  Name or path the netCDF file
   * @return   0 if successful, 1 otherwise
   */
  int _readNetcdf(char *filename);

  /**
   * Create a master header, field header, vlevel header, and convert granule data to 
   * user specified projection.  Add all parts to the Mdvx object
   * @param[in]  mdv
   */
  void _createMdv(DsMdvx &mdv);
  
  /**
   * Convert Mdv data ( need to add user specified storage type, apply compression), and write
   * to user specified directory. 
   * @param[in]  mdv
   * @return  0 if successful, 1 otherwise
   */
  int _writeMdv();
  
  /**
   *  Set members of header for Mdv object.
   */
  void _setMasterHeader();

  /**
   *  Adjust the start, end, and centroid times in Mdv header.
   */
  void _masterHdrAdjustTime();

  /**
   * Set members of vertical level object which is part of Mdv object. 
   */
  void _setVlevelHdr();

  /**
   * Set members of Mdv field header.  
   */
  void _setFieldHdr();
  
  /**
   * Set output data projection object based on user parameters
   */
  void _setProjection();

  /**
   * Loop through satellite data values and attempt to map the mdv data array according
   * to user specified projection
   */
  void _mapSatData2MdvArray();

};

#endif

