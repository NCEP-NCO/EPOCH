// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// Copyright, University Corporation for Atmospheric Research (UCAR) 2009-2017. 
// The Government's right to use this data and/or software is restricted per 
// the terms of Cooperative Agreement between UCAR and the National  Science 
// Foundation, to government use only which includes the nonexclusive, 
// nontransferable, irrevocable, royalty-free license to exercise or have 
// exercised for or on behalf of the U.S. Government throughout the world. 
// All other rights are reserved. 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file ParmsCmorphAveragerIO.hh
 * @brief All the algorithm parameters for CmorphAverager, as read in through
 *        the constructor
 * @class ParmsCmorphAveragerIO
 * @brief All the algorithm parameters for CmorphAverager, as read in through
 *        the constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    ParmsCmorphAveragerIO_HH
# define    ParmsCmorphAveragerIO_HH

#include "ParmsCmorphAverager.hh"

//----------------------------------------------------------------
class ParmsCmorphAveragerIO : public ParmsCmorphAverager
{
public:

  /**
   * Default constructor (sets members to default values)
   */
  ParmsCmorphAveragerIO(void);

  /**
   * Default constructor, gives parameters values by reading in a parameter
   * file, using input command line arguments
   * 
   * @param[in] argc  Number of command line arguments is generally three for 
   *                  real-time or six for archive mode.
   * @param[in] argv  Typical realtime and archive mode command lines are: 
   *                   'CmorphAverager -params CmorphAverager.params' or 
   *                   'CmorphAverager -params CmorphAverager.params -interval 
   *                   yyyymmddhhmmdss yyyymmddhhmmdss'
   */
  ParmsCmorphAveragerIO(int argc, char **argv);
  
  /**
   * Destructor
   */
  virtual ~ParmsCmorphAveragerIO(void);

protected:
private:  

  
};

# endif
