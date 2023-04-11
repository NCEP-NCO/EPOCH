/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright (c) 1995, UCAR
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*********************************************************************
 * Main.cc: CloudHt main routine
 *
 * RAP, NCAR, Boulder CO
 *
 * March 2002
 *
 * Gary Blackburn
 *
 *********************************************************************/

#include <stdio.h>

#include <toolsa/port.h>

#include "CloudHt.hh"


// Prototypes for static functions

static void tidy_and_exit(int sig);


// Global variables

CloudHt *Prog = (CloudHt *)NULL;


/*********************************************************************
 * main()
 */

int main(int argc, char **argv)
{
  // Create program object.

  Prog = CloudHt::Inst(argc, argv);
  if (!Prog->okay)
    return(-1);

  if (Prog->init()==false)
    return -1;

  // Register function to trap termination and interrupts.

  PORTsignal(SIGQUIT, tidy_and_exit);
  PORTsignal(SIGTERM, tidy_and_exit);
  PORTsignal(SIGINT, tidy_and_exit);

  // Run the program.

  Prog->run();

  // clean up

  tidy_and_exit(0);
  return(0);
  
}

/*********************************************************************
 * tidy_and_exit()
 */

static void tidy_and_exit(int sig)
{
  // Delete the program object.

  if (Prog != (CloudHt *)NULL)
    delete Prog;

  // Now exit the program.

  exit(sig);
}
