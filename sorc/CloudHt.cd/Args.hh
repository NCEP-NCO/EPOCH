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

/* RCS info
 *   $Author: ocnd $
 *   $Locker:  $
 *   $Date: 2003/12/24 00:25:57 $
 *   $Id: Args.hh,v 1.2 2003/12/24 00:25:57 ocnd Exp $
 *   $Revision: 1.2 $
 *   $State: Exp $
 */
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/

/************************************************************************
 * Args.hh : header file for the Args class.
 *
 * RAP, NCAR, Boulder CO
 *
 * August 1999
 *
 * Nancy Rehak
 *
 ************************************************************************/

#ifndef Args_HH
#define Args_HH

/*
 **************************** includes **********************************
 */

#include <vector>
#include <stdio.h>
#include <time.h>

#include <tdrp/tdrp.h>

/*
 ******************************* defines ********************************
 */


/*
 ******************************* structures *****************************
 */

/*
 ************************* global variables *****************************
 */

/*
 ***************************** function prototypes **********************
 */

/*
 ************************* class definitions ****************************
 */

class Args
{
 public:

  // Constructor

  Args(int argc, char **argv, char *prog_name);
  
  // Destructor

  ~Args(void);
  
  // TDRP overrides specified in the command line arguments.

  tdrp_override_t override;
  
  ////////////////////
  // Access methods //
  ////////////////////

  const vector<string>& getSatFileList(void) const
  {
    return _satFileList;
  }

  const time_t& getArchiveStartTime(void) const
  {
    return _archiveStartTime;
  }

  const time_t& getArchiveEndTime(void) const
  {
    return _archiveEndTime;
  }

 private:

  /////////////
  // Members //
  /////////////

  // Filelist list of files
  vector<string> _satFileList;

  // Archive start and end times
  time_t _archiveStartTime;
  time_t _archiveEndTime;

  /////////////
  // Methods //
  /////////////

  string _progName;

  // TDRP override handling
  void _setOverride(const string&);
  void _setOverride(const string&, const char*);

  void _usage(FILE *stream);
  
};


#endif
