
#include "GmgsiNcf2Mdv.hh"

#include <Mdv/DsMdvx.hh>
#include <toolsa/pmu.h>

#include <cmath>
#include <math.h>
#include <Ncxx/Ncxx.hh>

using namespace std;

const float GmgsiNcf2Mdv::SAT_MISSING = -9999.0;

GmgsiNcf2Mdv::GmgsiNcf2Mdv(int argc, char **argv):
_progName("GmgsiNcf2Mdv"),
_args(_progName)
{
  isOK = true;

  //
  // set programe name
  //
  ucopyright((char *) _progName.c_str());

  //
  // get command line args
  //
  if (_args.parse(argc, argv)) 
    {
      cerr << "ERROR: " << _progName << endl;
      cerr << "Problem with command line args" << endl;
      isOK = FALSE;
      return;
    }

  //
  // get TDRP params
  //
  char *paramsPath = (char *) "unknown";
  if (_params.loadFromArgs(argc, argv, _args.override.list,
			   &paramsPath)) 
    {
      cerr << "ERROR: " << _progName << endl;
      cerr << "Problem with TDRP parameters" << endl;
      isOK = FALSE;
      return;
    }

  //
  // init process mapper registration
  //
  PMU_auto_init((char *) _progName.c_str(),
		_params.instance,
		300);

  //
  // Initialize and set file trigger
  //
  _fileTrigger = NULL;

  if ( _params.mode == Params::REALTIME ) 
  {
    if (_params.debug)
    {
      cerr << "FileInput::init: Initializing realtime input from " 
	   << _params.input_dir << endl;
    }

    _fileTrigger = new DsInputPath( _progName, _params.debug,
				    _params.input_dir,
				    _params.max_valid_realtime_age_min*60,
				    PMU_auto_register,
				    _params.ldata_info_avail,
				    false );
    //
    // Set wait for file to be written to disk before being served out.
    //
    _fileTrigger->setFileQuiescence( _params.file_quiescence_sec );
    
    //
    // Set time to wait before looking for new files.
    // 
    _fileTrigger->setDirScanSleep( _params.check_input_sec );  
  }
  else if ( _params.mode == Params::FILELIST ) 
  {    
    //
    // Archive mode.
    //
    const vector<string> &fileList =  _args.getInputFileList();
    
    if ( fileList.size() ) 
    {
      if (_params.debug)
      {
	cerr << "FileInput::init: Initializing archive FILELIST mode." << endl;
      }

      _fileTrigger = new DsInputPath( _progName, _params.debug , fileList );
    }
  }
  else if ( _params.mode == Params::TIME_INTERVAL ) 
  { 
    //
    // Set up the time interval
    //
    DateTime start( _params.start_time);
    
    DateTime end( _params.end_time);
    
    time_t time_begin = start.utime();
    
    time_t time_end = end.utime();
       
    if (_params.debug)
    {
      cerr << "FileInput::init: Initializing file input for time interval [" 
	   << time_begin << " , " << time_end << "]." << endl;  
    }
       
    _fileTrigger = new DsInputPath( _progName, _params.debug,
				   _params.input_dir,
				   time_begin,
				   time_end,false);
  }
  if(_fileTrigger == NULL) 
  {
    isOK = false;
  }
  
  //
  // Initialize data arrays, flags, and other variables
  //
  _latitudes = NULL;

  _longitudes = NULL;

  _satDataVals = NULL;

  _mdvDataVals = NULL;

  return;
}

GmgsiNcf2Mdv::~GmgsiNcf2Mdv()
{
  //
  // Free allocated memory
  //
  _clearData();

  //
  // unregister process
  //
  PMU_auto_unregister();  
}

void GmgsiNcf2Mdv::_clearData()
{
  if (_latitudes)
  {
    delete[] _latitudes;
  }
  _latitudes = NULL;

  if (_longitudes)
  {
    delete[] _longitudes;
  }
  _longitudes = NULL;

  if (_satDataVals)
  {
    delete[] _satDataVals;
  }
  _satDataVals = NULL;

 if (_mdvDataVals)
  {
    delete[] _mdvDataVals;
  }
  _mdvDataVals = NULL;
}

int GmgsiNcf2Mdv::Run()
{
  //
  // register with procmap
  //
  PMU_auto_register("Run");

  //
  // Basic header data doesnt change file to file so set it once
  // We will adjust time meta data in master header necessary
  //

  _setProjection();

  _setMasterHeader();
  
  _setVlevelHdr();
  
  _setFieldHdr();

  //
  // process data
  //
  char *inputPath;
  
  //
  // Process indefinitely in real-time or until there arent any files left in archive mode
  //
  while (true) 
  {
    if ((inputPath = _fileTrigger->next(false)) != NULL) 
    {
         PMU_auto_register("Run");

	 if (_processData(inputPath))
	 {
	    cerr << "GmsiNcf2Mdv::Run(): Errors in processing file." <<  inputPath << endl;
	 }
    }
    else 
    {
        if (_params.mode == Params::REALTIME)
        {
 	   PMU_auto_register("Sleep");
           if(_params.debug )
           {
             cerr << " GmgsiNcf2Mdv::Run(): No new files. Sleeping\n";
           }
           sleep(_params.sleep_secs);
        }
        else
        {
           if(_params.debug )
           {
             cerr << " GsgmiNcf2Mdv::Run(): No more archive files found. Exiting\n";
           }
           return 0;
        }
    }
  }// while
  
  delete _fileTrigger;
  
  return 0;
}

int GmgsiNcf2Mdv::_processData(char *inputPath)

{
  if (_params.debug)
  {
    cerr << "GmgsiNcf2Mdv::_processData: Processing file : " << inputPath << endl;
  }
  
  //
  // registration with procmap
  //
  PMU_force_register("Processing data");
  
  //
  // Read input NetCDF file
  //
  if (_readNetcdf(inputPath))
    return 1;

  _mapSatData2MdvArray();

  //
  // Configure time members of master header with latest data information
  //
  _masterHdrAdjustTime();
  
  //
  // Write the stored data to a file
  //
  _writeMdv();

  //
  // Clean up memory allocation needed for reading NetCDF file
  //
  _clearData();
 
  return 0;
}

int GmgsiNcf2Mdv::_writeMdv()
{
  DsMdvx mdv;
  
  //
  // Sync headers with projection, add master header, field header to Mdvx object
  //
  _outputProj.syncToHdrs(_master_hdr,_field_hdr);
  
  mdv.setMasterHeader(_master_hdr);
    
  MdvxField *satField = new MdvxField(_field_hdr, _vlevel_hdr, (void *)_mdvDataVals); 

  mdv.addField(satField);

  if (mdv.writeToDir(_params.output_url)) 
  {
    cerr << "GmgsiNcf2Mdv::_writeData(): Error. Cannot write mdv file" << endl;
    
    cerr << mdv.getErrStr() << endl;
    
    return 1;
  }
    
  if (_params.debug) 
  {
    cerr << "Gmgsi2Mdv::_writeData(): Wrote file: " << mdv.getPathInUse() << endl;
  }

  return 0;
}

int GmgsiNcf2Mdv::_readNetcdf(char *filename)
{
  //
  // registration with procmap
  //
  PMU_force_register("Reading netCDF data");
 
  if (_params.debug) 
  {
    cerr << "GmgsiNcf2Mdv::_readNetcdf(): Reading file: " << filename << endl;
  }

  //
  // Open the file. 
  // 
  NcxxFile ncFile(filename, NcxxFile::read);

  //
  // Get time
  //
  NcxxVar dataTime = ncFile.getVar("time");

  if(dataTime.isNull())
  {
    cerr << "GmgsiNcf2Mdv::_readNetcdf(): Failure to get time variable. Exiting" << endl;

    return 1;
  }

  dataTime.getVal(&_dataTime);   

  if (_params.debug)
  {
    cerr << "GmgsiNcf2Mdv::_readNetcdf(): _dataTime: " << _dataTime << endl;
  }

  //
  // Get latitude data
  //
  NcxxVar latData =  ncFile.getVar(_params.lat_var);

  if(latData.isNull()) 
  {
    cerr << "GmgsiNcf2Mdv::_readNetcdf(): Failure to get latitude data. Exiting" << endl;

    return 1;
  }

  std::vector<NcxxDim> dataDims = latData.getDims();
  
  for (int i = 0; i < (int) dataDims.size(); i++)
  {
    string dimName = dataDims[i].getName();

    if (dimName == _params.x_dim)
       _nx = dataDims[i].getSize();
    else if (dimName == _params.y_dim)
       _ny = dataDims[i].getSize();
  }
  
  _latitudes = new float[_ny*_nx];
  
  latData.getVal(_latitudes);

  //
  // Get longitude data
  //
  NcxxVar lonData =  ncFile.getVar(_params.lon_var);
 
  if (lonData.isNull())
  {
    cerr << "GmgsiNcf2Mdv::_readNetcdf(): Failure to get sparse longitude data. Exiting" << endl;
   
    return 1;
  }

  _longitudes = new float [_ny *_nx];

  lonData.getVal(_longitudes);
  
  //
  // Get sat data field
  //
  NcxxVar satData = ncFile.getVar(_params.sat_data_field);
  
  if (satData.isNull())
  {
    cerr << "GmgsiNcf2Mdv::_readNetcdf(): Failure to get sat field. Exiting." << endl;
   
    return 1;
  }

  _satDataVals = new float[_nx*_ny];
   
  satData.getVal(_satDataVals);

  //
  // Convert to temp: is a bi-linear scale.
  // For pixels from 176 and up, T=418-brightness
  // Below brightness 176, T=330-B/2
  // Crossover at 242K
  // contact : clay.davenport@noaa.gov
  //   
  for (int i = 0;  i < _nx*_ny; i++)
  {
    if (_satDataVals[ i] >= 176) 
    {
        _satDataVals[ i]=418-_satDataVals[i];
    }
    else
    {
       _satDataVals[i]=330-(_satDataVals[i]/2); 
    }
  }
  return 0;
};


void GmgsiNcf2Mdv::_masterHdrAdjustTime()
{
  _master_hdr.time_gen = time(0);

  _master_hdr.time_begin = _dataTime;

  _master_hdr.time_end = _dataTime;
  
  _master_hdr.time_centroid = _dataTime;

  _master_hdr.time_expire = _dataTime;

}
void GmgsiNcf2Mdv::_setMasterHeader()
{
  // Fill out Master Header
  Mdvx::master_header_t _master_hdr;

  memset(&_master_hdr, 0, sizeof(_master_hdr));

  _master_hdr.data_collection_type = Mdvx::DATA_MEASURED;

  _master_hdr.grid_orientation = Mdvx::ORIENT_SN_WE;

  _master_hdr.data_ordering = Mdvx::ORDER_XYZ;

  _master_hdr.vlevel_included = 1;

  strncpy(_master_hdr.data_set_info,
          "GMGSI satellite data ingested by GmgsiNcf2Mdv", MDV_INFO_LEN);
  
  strncpy(_master_hdr.data_set_name, "GmgsiNcf2Mdv", MDV_NAME_LEN);
  
  strncpy(_master_hdr.data_set_source,
          "GMGSI NetCDF", MDV_NAME_LEN);
}

void GmgsiNcf2Mdv::_setVlevelHdr()
{
  //
  // fill out the vertical level header
  //
  memset(&_vlevel_hdr, 0, sizeof(_vlevel_hdr));

  _vlevel_hdr.type[0] = Mdvx::VERT_TYPE_SURFACE;

  _vlevel_hdr.level[0] = 0.0;

  return;
}


void GmgsiNcf2Mdv::_setFieldHdr()
{
  //
  // Set the field header
  //
  memset(&_field_hdr, 0, sizeof(_field_hdr));
   
  _field_hdr.nz = 1;
  
  _field_hdr.nx = _params.output_proj.nx;
  
  _field_hdr.ny = _params.output_proj.ny;

  _field_hdr.grid_minx = _params.output_proj.minx;
  
  _field_hdr.grid_miny = _params.output_proj.miny;

  _field_hdr.grid_dx = _params.output_proj.dx;
  
  _field_hdr.grid_dy = _params.output_proj.dy;

  _field_hdr.proj_type = Mdvx::PROJ_LATLON;
  _field_hdr.proj_origin_lat =  _params.output_proj.origin_lat;
  _field_hdr.proj_origin_lon =  _params.output_proj.origin_lon;

  _field_hdr.encoding_type = Mdvx::ENCODING_FLOAT32;
  
  _field_hdr.data_element_nbytes = 4;
  
  _field_hdr.volume_size =
    _field_hdr.nx * _field_hdr.ny * _field_hdr.nz * _field_hdr.data_element_nbytes;
  
  _field_hdr.compression_type = Mdvx::COMPRESSION_NONE;
  
  _field_hdr.transform_type = Mdvx::DATA_TRANSFORM_NONE;
 
  _field_hdr.scaling_type = Mdvx::SCALING_DYNAMIC;
 
  _field_hdr.native_vlevel_type = Mdvx::VERT_TYPE_SURFACE;
  
  _field_hdr.vlevel_type = Mdvx::VERT_TYPE_SURFACE;
  
  _field_hdr.dz_constant = 1;
  
  _field_hdr.data_dimension = 2;
    
  _field_hdr.grid_dz = 0;
  
  _field_hdr.grid_minz = 0;
  
  _field_hdr.bad_data_value = SAT_MISSING;
  
  _field_hdr.missing_data_value = SAT_MISSING;
  
  strncpy(_field_hdr.field_name_long, _params.output_data_fieldname, MDV_LONG_FIELD_LEN);
  
  strncpy(_field_hdr.field_name, _params.output_data_fieldname, MDV_SHORT_FIELD_LEN);
  
  strncpy(_field_hdr.units,  _params.output_data_fieldname, MDV_UNITS_LEN); 
}
   
void GmgsiNcf2Mdv::_setProjection()
{ 
  //
  // Initialize the user defined projection
  //
  switch (_params.output_proj.proj_type)
  {
  case Params::PROJ_MERCATOR :
     _outputProj.initMercator(_params.output_proj.origin_lat,
                       _params.output_proj.origin_lon,
			_params.output_proj.nx,
                       _params.output_proj.ny,
                        1,
                       _params.output_proj.dx,
                       _params.output_proj.dy,
                        0.0,
                       _params.output_proj.minx,
                       _params.output_proj.miny,
                        0.0);
      break;
  case Params::PROJ_LATLON :
     _outputProj.initLatlon( _params.output_proj.nx,
                       _params.output_proj.ny,
                       1,
                       _params.output_proj.dx,
                       _params.output_proj.dy,
                       0.0,
                       _params.output_proj.minx,
                       _params.output_proj.miny,
                       0.0);
      break;

  default:
     //
     // Do something more graceful..., method should return value indicating success or failure
     //
     cerr << "GmgsiNcf2Mdv::_setProjection(): Projection not supported. Exiting." << endl;
     exit(1);

  } // end switch 
  return;
}

void GmgsiNcf2Mdv::_mapSatData2MdvArray()
{
   //
   // Create mdv data array
   //
   _mdvDataVals = new float[_params.output_proj.nx * _params.output_proj.ny];

   //
   // Initialize array
   //
   for (int i = 0; i < _params.output_proj.nx * _params.output_proj.ny; i++)
   {
     _mdvDataVals[i] = SAT_MISSING;
   }

  //
  // Map the satellite data to the mdv projection
  //
  for (int i = 0; i < _nx*_ny; i++)
  {
    int mdv_data_index;

    //
    // Map sat data location to mdv array postion
    //
    if (_outputProj.latlon2arrayIndex(_latitudes[i], _longitudes[i],
                                     mdv_data_index) < 0)
    {
      if (_params.debug == Params::DEBUG_VERBOSE)
      {
        cerr << "DEBUG: Gmgsi2Ncf2Mdv::_createMdv:" <<  endl;
        cerr << "Data point outside of output grid." << endl;
        cerr << "lat = " << _latitudes[i]
             << ", lon = " << _longitudes[i] << endl;
      } 
      continue;
   }
   //
   // Update the MDV data value
   //
   _mdvDataVals[mdv_data_index] = _satDataVals[i];
  }// end for
}
