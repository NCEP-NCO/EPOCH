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
///////////////////////////////////////////////////////////////
//
// main for MdvMerge2
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// April 2004
//
///////////////////////////////////////////////////////////////
//
// MdvMerge2 merges multiple MDV files into a mosaic
//
////////////////////////////////////////////////////////////////

#include "MdvMerge2.hh"
#include <toolsa/str.h>
#include <toolsa/port.h>
#include <csignal>
#include <new>
#include <cstdlib>


using namespace std;

// file scope

static void tidy_and_exit (int sig);
static void out_of_store();
static MdvMerge2 *_prog;
static int _argc;
static char **_argv;
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

// main

int main(int argc, char **argv)

{
  store_arguments(argc,argv);

  _argc = argc;
  _argv = argv;

  // create program object

  _prog = new MdvMerge2(argc, argv);
  if (!_prog->isOK) {
    return(-1);
  }

  // set signal handling
  
  PORTsignal(SIGINT, tidy_and_exit);
  PORTsignal(SIGHUP, tidy_and_exit);
  PORTsignal(SIGTERM, tidy_and_exit);
  PORTsignal(SIGPIPE, (PORTsigfunc)SIG_IGN);
  PORTsignal(SIGSEGV, segv_handler);

  // set new() memory failure handler function

  set_new_handler(out_of_store);

  // run it

  int iret = _prog->Run();

  // clean up

  tidy_and_exit(iret);
  return (iret);
  
}

///////////////////
// tidy up on exit

static void tidy_and_exit (int sig)

{

  // delete(_prog);
  exit(sig);

}
////////////////////////////////////
// out_of_store()
//
// Handle out-of-memory conditions
//

static void out_of_store()

{

  fprintf(stderr, "FATAL ERROR - program MdvMerge2\n");
  fprintf(stderr, "  Operator new failed - out of store\n");
  exit(-1);

}

