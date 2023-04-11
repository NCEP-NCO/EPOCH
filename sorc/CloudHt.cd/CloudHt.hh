/***********************************************************
 ** Copyright (c) 1995, UCAR
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/************************************************************************
 * CloudHt.hh : header file for the CloudHt program class.
 *
 * RAP, NCAR, Boulder CO
 *
 * March 2002
 *
 * Gary Blackburn
 *
 ************************************************************************/

#ifndef CloudHt_HH
#define CloudHt_HH

#include <sys/time.h>
#include <vector>

#include <dsdata/DsIntervalTrigger.hh>
#include <dsdata/DsLdataTrigger.hh>
#include <dsdata/DsTimeListTrigger.hh>

#include <Mdv/DsMdvx.hh>
#include <Mdv/MdvxField.hh>

#include "Args.hh"
#include "Params.hh"

class DsMdvxInput;

class CloudHt
{
 public:

  /////////////////////////////// Public /////////////////////////////

  // Flag indicating whether the program status is currently okay.

  bool okay;

  /*********************************************************************
   * Destructor
   */

  ~CloudHt(void);
  
  /*********************************************************************
   * Inst() - Retrieve the singleton instance of this class.
   */

  static CloudHt *Inst(int argc, char **argv);
  static CloudHt *Inst();
  
  // Run the program.

  /*********************************************************************
   * init() - Initialize the local data.
   *
   * Returns true if the initialization was successful, false otherwise.
   */

  bool init();

  /*********************************************************************
   * run() - run the program.
   */

  void run();
  
  
 private:

  /////////////////////////////// Private /////////////////////////////

  // Singleton instance pointer

  static CloudHt *_instance;
  
  // Program parameters.

  char *_progName;
  Args *_args;
  Params *_params;
  
  DsMdvxInput *_satTempInput;

  DsMdvx _satTempMdvx;
  DsMdvx _modelHeightMdvx;
  DsMdvx _modelTropoHeightMdvx;
  DsMdvx _modelTempMdvx;
  
  void _clearData();
  
  int *_calcTropopauseIndices() const;
  int *_calcTropoFrom3DHtTempFields() const;
  int *_calcTropoFromTropoHt() const;
  
  /*********************************************************************
   * _readField() - Read the indicated field data.
   */

  bool _readSatTempFile(DsMdvx &input_file);

  bool _readModelField(const time_t data_time,
		       const string &url,
		       const string &field_name,
		       const int field_num,
		       const bool set_vertical_limits,
		       DsMdvx &input_file);

  static fl32 _getPressure(const fl32 satbt, const fl32 modt,
                           const fl32 hgt_tmp, const fl32 modz, 
                           const fl32 z);

  void _updateMasterHeader(DsMdvx &output_file,
			   const time_t time_begin,
			   const time_t time_end,
			   const time_t time_centroid,
			   const Mdvx::field_header_t &field_hdr);

  MdvxField *_createField(const string &field_name,
                                       const int field_code,
                                       const string &units,
                                       const fl32 missing_data_value,   
                                       fl32 *data,
                                       const Mdvx::field_header_t &sat_temp_hdr);

  // Constructor -- private because this is a singleton object

  CloudHt(int argc, char **argv);
  
  // Process the given file.

  bool _processData();
  
  vector <fl32 > _pressLvl;

  // Return the class name for error messages.

  static const char *_className(void)
  {
    return("CloudHt");
  }
  
};


#endif
