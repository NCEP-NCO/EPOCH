// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
////////////////////////////////////////////////////////////////////
// Main entry point for Grib2toMdv application
//
// Converts Grib2 files into MDV format
// Tested GRIB2 Models:
//    gfs004    (gfs half degree resolution)  
//    dgex218   (Downscaled Gfs with Eta Extensions, 10km resolution)
//    eta218    (Eta/Nam 10km resolution)
//    NDFD      (National Digital Forecast Database CONUS operational fields)
//
// -Jason Craig-  Jun 2006
////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <signal.h>
#include <toolsa/port.h>

// for segv handling
#include <unistd.h>
#include <cstdio>
#include <ctime>
#include <string>

#include "Grib2toMdv.hh"
#include "Args.hh"
using namespace std;

static void dieGracefully( int signal );

// Global program object
Grib2toMdv *Prog = (Grib2toMdv *)NULL;
static std::string cmdline = "";

void store_arguments(int argc, char **argv){

        for (int ix = 0; ix < argc; ix++){
                cmdline += argv[ix];
                cmdline += " ";
        }
}

// based on blog at http://www.alexonlinux.com/how-to-handle-sigsegv-but-also-generate-core-dump
// NOTE:  that comments on the blog indicate the core file generated on red hat or on multi-threaded programs
//        might contain unhelpful information.
void segv_handler(int signum)
{
  time_t rawtime;
  struct tm * timeinfo;
  char timebuffer[80];
  char cwdbuffer[PATH_MAX+1];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(timebuffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
  getcwd(cwdbuffer,PATH_MAX+1);

  fprintf(stderr, "FATAL ERROR (SEGFAULT): Process %d got signal %d @ local time = %s\n", getpid(), signum, timebuffer);
  fprintf(stderr, "FATAL ERROR (SEGFAULT): Look for a core file in %s\n",cwdbuffer);
  fprintf(stderr, "FATAL ERROR (SEGFAULT): Process command line: %s\n",cmdline.c_str());
  signal(signum, SIG_DFL);
  kill(getpid(), signum);
}

void no_memory() {
  cerr << "Caught bad_alloc exception (Failed to allocate memory!)\n";
  dieGracefully(1);
}


//*********************************************************************
int main( int argc, char **argv )
{
   store_arguments(argc,argv);

   // Create program object by requesting a instance
   Prog = Grib2toMdv::Inst(argc, argv);
   if (!Prog->okay)
     return(-1);

   //
   // Trap signals for a clean exit
   //
   PORTsignal( SIGINT,  dieGracefully );
   PORTsignal( SIGTERM, dieGracefully );
   PORTsignal( SIGQUIT, dieGracefully );
   PORTsignal( SIGKILL, dieGracefully );
   PORTsignal( SIGPIPE, (PORTsigfunc)SIG_IGN);
   PORTsignal( SIGSEGV, segv_handler);
   set_new_handler(no_memory);

   Prog->run();

   delete Prog;

   return (0);
}


void dieGracefully( int signal )
{
  // Delete the program object.

  //if (Prog != (Grib2toMdv *)NULL)
  //delete Prog;

  exit( signal );
}
