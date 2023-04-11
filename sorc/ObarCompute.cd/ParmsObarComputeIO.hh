// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
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
#include <toolsa/copyright.h>

/**
 * @file ParmsObarComputeIO.hh
 * @brief All the algorithm parameters for ObarCompute, as read in
 *        through the constructor
 * @class ParmsObarComputeIO
 * @brief All the algorithm parameters for ObarCompute, as read in
 *        through the constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    ParmsObarComputeIO_hh
# define    ParmsObarComputeIO_hh

#include "ParmsObarCompute.hh"

//----------------------------------------------------------------
class ParmsObarComputeIO : public ParmsObarCompute
{
public:

  /**
   * Default constructor (sets members to default values)
   */
  ParmsObarComputeIO(void);

  /**
   * Default constructor, gives parameters values by reading in a parameter
   * file, using input command line arguments
   * 
   * @param[in] argc  Number of command line arguments is generally three for 
   *                  real-time or six for archive mode.
   * @param[in] argv  Typical realtime and archive mode command lines are: 
   *                   'ObarCompute -params ObarCompute.parm'
   *                  or 
   *                   'ObarCompute -params ObarCompute.pars
   *                                 -interval yyyymmddhhmmdss yyyymmddhhmmdss'
   */
  ParmsObarComputeIO(int argc, char **argv);
  
  /**
   * Destructor
   */
  virtual ~ParmsObarComputeIO(void);


protected:
private:  

  void _setTileLatLon(void);
};

# endif     // ParmsObarComputeIO_HH
