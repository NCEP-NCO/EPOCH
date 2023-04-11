
/**
 * @file ParmsEnsFcstGenIO.hh
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 * @class ParmsEnsFcstGenIO
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    PARAMS_FSSGRID_IO_HH
# define    PARAMS_FSSGRID_IO_HH

#include "ParmsEnsFcstGen.hh"

class ParmsEnsFcstGenIO : public ParmsEnsFcstGen
{

public:

  /**
   * empty
   */
  ParmsEnsFcstGenIO(void);
  
  /**
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  Array of pointers to the strings which are the
   *                   command line arguments
   *
   * The number of command line arguments is generally three for
   * real-time or six for archive mode.
   * 
   * Typical real-time and archive mode command lines are:
   * - 'EnsFcstGen -params EnsFcstGen.params' or
   * - 'EnsFcstGen -params EnsFcstGen.params -interval yyyymmddhhmmdss 
   *    yyyymmddhhmmdss'
   */
  ParmsEnsFcstGenIO(int argc, char **argv);
  
  /**
   *  Destructor
   */
  virtual ~ParmsEnsFcstGenIO(void);

protected:

private:
  /**
   * Constant indicating failure to initiate ParmsEnsFcstGenIO object
   */
  static const int failure;
};

# endif     // PARMS_FSSGRID_HH 
