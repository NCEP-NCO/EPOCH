
/**
 * @file ParmsPbarComputeIO.hh
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 * @class ParmsPbarComputeIO
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    ParmsPbarCompute_IO_hh
# define    ParmsPbarCompute_IO_hh

#include "ParmsPbarCompute.hh"

class ParmsPbarComputeIO : public ParmsPbarCompute
{

public:

  /**
   * empty
   */
  ParmsPbarComputeIO(void);
  
  /**
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  Array of pointers to the strings which are the
   *                   command line arguments   *
   * The number of command line arguments is generally three for
   * real-time or six for archive mode.
   * 
   * Typical real-time and archive mode command lines are:
   * - 'PbarCompute -params Parmfile.params' or
   * - 'PbarCompute -params Parmfile.params -interval yyyymmddhhmmdss 
   *    yyyymmddhhmmdss'
   */
  ParmsPbarComputeIO(int argc, char **argv);
  
  /**
   *  Destructor
   */
  virtual ~ParmsPbarComputeIO(void);

protected:

private:
  /**
   * Constant indicating failure to initiate ParmsPbarComputeIO object
   */
  static const int failure;

  void _setTileLatLon(void);
};

# endif     // PARMS_ENSFCSTGEN_HH 
