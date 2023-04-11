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
 * Args.cc: class controlling the command line arguments for the
 *          program.
 *
 * RAP, NCAR, Boulder CO
 *
 * Dec 2003
 *
 * Gary Blackburn
 *
 *********************************************************************/

#include <iostream>
#include <string>

#include <string.h>

#include <os_config.h>
#include <tdrp/tdrp.h>
#include <toolsa/DateTime.hh>
#include <toolsa/str.h>
#include <toolsa/umisc.h>

#include "Args.hh"

/**********************************************************************
 * Constructor
 */

Args::Args (int argc, char **argv, char *prog_name) :
  _progName(prog_name)
{
  string tmp_str;

  string interval_str;
  DateTime start_time(DateTime::NEVER);
  DateTime end_time(DateTime::NEVER);
  string url;
  string mode;

  // Intialize

  bool okay = true;

  TDRP_init_override(&override);
  
  // search for command options
  
  for (int i =  1; i < argc; i++)
  {
    if (STRequal_exact(argv[i], "--") ||
	STRequal_exact(argv[i], "-help") ||
	STRequal_exact(argv[i], "-man"))
    {
      _usage(stdout);
      exit(0);
    }
    else if (STRequal_exact(argv[i], "-debug"))
    {
      _setOverride("debug = DEBUG_NORM;");
    } 
    else if (!strcmp(argv[i], "-verbose")) {
      _setOverride("debug = DEBUG_VERBOSE;");
    }
    else if (STRequal_exact(argv[i], "-start") ||
             STRequal_exact(argv[i], "-starttime"))
    {
      if (i < argc - 1)
      {
        if (start_time.set(argv[++i]) == DateTime::NEVER)
        {
          cerr << "*** Invalid start time string entered: " <<
            argv[i] << endl << endl;

          okay = false;
        }
        else {
          _archiveStartTime = start_time.utime();
          _setOverride( "mode = ARCHIVE;" );
        }
      }
      else
      {
        cerr << "User must provide time string with -start option" << endl;
        okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-end") ||
             STRequal_exact(argv[i], "-endtime"))
    {
      if (i < argc - 1)
      {
        if (end_time.set(argv[++i]) == DateTime::NEVER)
        {
          cerr << "*** Invalid end time string entered: " <<
            argv[i] << endl << endl;

          okay = false;
        }
        else
          _archiveEndTime = end_time.utime();
      }
      else
      {
        cerr << "User must provide time string with -end option" << endl;
        okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-mode"))
    {
      if (i < argc - 1)
      {
        ++i;

        if (STRequal_exact(argv[i], "REALTIME") ||
            STRequal_exact(argv[i], "ARCHIVE") ||
            STRequal_exact(argv[i], "FILELIST"))
        {
           _setOverride( "mode", argv[i] );
           if (STRequal_exact(argv[i], "FILELIST")) 
           {
             if (i < argc - 1) 
             {
               for (int j = i + 1; j < argc; j++) 
               {
                if (argv[j][0] == '-') 
                  break;
                else 
                  _satFileList.push_back(argv[j]);
               }
             }
             else
               cerr << "Invalid input file list" << endl;
          }
        }
        else
        {
          cerr << "*** Invalid mode entered: " << argv[i] << endl << endl;
          okay = false;
        }
      }
      else
      {
        cerr << "User must provide REALTIME, ARCHIVE or FILELIST with -mode option" << endl;

        okay = false;
      }
    }
  } /* i */

  if (!okay)
  {
    _usage(stderr);
    exit(-1);
  }
  
  // If a mode was entered on the command line, make sure that
  // the other appropriate information was also entered.

  if (mode == "ARCHIVE")
  {
  
    if (start_time == DateTime::NEVER)
    {
      cerr <<
        "*** Must include -start in command line when using TIME_LIST mode" <<
        endl << endl;
      _usage(stderr);
      exit(-1);
    }
  
    if (end_time == DateTime::NEVER)
    {
      cerr <<
        "*** Must include -end in command line when using TIME_LIST mode" <<
        endl << endl;

      _usage(stderr);
      exit(-1);
    }
  
  }
 
}


/**********************************************************************
 * Destructor
 */

Args::~Args(void)
{
  TDRP_free_override(&override);
}
  

/**********************************************************************
 *              Private Member Functions                              *
 **********************************************************************/

/**********************************************************************
 * _usage() - Print the usage for this program.
 */

void Args::_usage(FILE *stream)
{
  fprintf(stream, "%s%s%s",
	  "Cloud height data is created from satellite temperature and GFS model\n"
          "pressure and temperature fields.\n\n"
	  "Usage:\n\n", _progName.c_str(), " [options] as below:\n"
	  "\n"
	  "       [ --, -help, -man ] produce this list.\n"
	  "       [ -debug ] debugging on\n"
	  "       [ -verbose ] print verbose debug message.\n"
          "       [ -mode ?] ARCHIVE or REALTIME or FILELIST\n"

          "\n"
          "When specifying a mode on the command line, the following must also\n "
          "be specified:\n"
          "\n"
          "     -mode ARCHIVE:\n"
          "       -start, -end\n"
          "       [ -start yyyy/mm/dd_hh:mm:ss ]\n"
          "       [ -end yyyy/mm/dd_hh:mm:ss ]\n"
          "     -mode FILELIST:\n"
          "       [ input satellite file list ]\n\n"
          "       Corresponding GFS file times should be also available\n"
          "        via the URL defined in the parameter file\n"
	  "\n");


  TDRP_usage(stream);
}

//////////////////////////////////////////////////
// _setOverride
//
// this function sets TDRP overrides
//
void Args::_setOverride(const string& ov_str)
{
  TDRP_add_override(&override, (char *)ov_str.c_str());
}

void Args::_setOverride(const string& ov_str, const char* val_str)
{
  string tmp_str = ov_str + " = \"" + val_str + "\";";
  cout << "override string is " << tmp_str.c_str() << endl;
  TDRP_add_override(&override, (char *)tmp_str.c_str());
}


