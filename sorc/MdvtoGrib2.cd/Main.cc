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
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* RCS info
 *   $Author: mccabe $
 *   $Date: 2020/04/01 20:18:12 $
 *   $Revision: 1.4 $
 */
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/
/*********************************************************************
 * Main.cc: MdvtoGrib2 main routine
 *
 * RAP, NCAR, Boulder CO
 * Jason Craig
 * Sept 2006
 *
 *********************************************************************/

#include <stdio.h>

#include <toolsa/port.h>
#include <toolsa/umisc.h>

#include "MdvtoGrib2.hh"

using namespace std;

// Prototypes for static functions

static void tidy_and_exit(int sig);


// Global variables

MdvtoGrib2 *Prog = (MdvtoGrib2 *)NULL;
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
  tidy_and_exit(1);
}

/*********************************************************************
 * main()
 */

int main(int argc, char **argv)
{
  store_arguments(argc,argv);

  // Create program object.

  Prog = MdvtoGrib2::Inst(argc, argv);
  if (!Prog->okay)
    return -1;

  if (!Prog->init())
    return -1;
  
  // Register function to trap termination and interrupts.

  PORTsignal(SIGQUIT, tidy_and_exit);
  PORTsignal(SIGTERM, tidy_and_exit);
  PORTsignal(SIGINT, tidy_and_exit);
  PORTsignal(SIGSEGV, segv_handler);
  PORTsignal(SIGHUP, tidy_and_exit);
  PORTsignal(SIGPIPE, (PORTsigfunc)SIG_IGN);

  set_new_handler(no_memory);

  // Run the program.

  int retVal = Prog->run();

  // clean up

  tidy_and_exit(retVal);
  return retVal;
}

/*********************************************************************
 * tidy_and_exit()
 */

static void tidy_and_exit(int sig)
{
  // Delete the program object.

  if (Prog != (MdvtoGrib2 *)NULL)
    delete Prog;

  // Now exit the program.

  exit(sig);
}
