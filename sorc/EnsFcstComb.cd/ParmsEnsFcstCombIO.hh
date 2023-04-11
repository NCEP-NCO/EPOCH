
/**
 * @file ParmsEnsFcstCombIO.hh
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 * @class ParmsEnsFcstCombIO
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    PARAMS_FSSGRID_IO_HH
# define    PARAMS_FSSGRID_IO_HH

#include "ParmsEnsFcstComb.hh"

class ParmsEnsFcstCombIO : public ParmsEnsFcstComb
{

public:

  /**
   * empty
   */
  ParmsEnsFcstCombIO(void);
  
  /**
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  Array of pointers to the strings which are the
   *                   command line arguments   *
   * The number of command line arguments is generally three for
   * real-time or six for archive mode.
   * 
   * Typical real-time and archive mode command lines are:
   * - 'EnsFcstComb -params EnsFcstComb.params' or
   * - 'EnsFcstComb -params EnsFcstComb.params -interval yyyymmddhhmmdss 
   *    yyyymmddhhmmdss'
   */
  ParmsEnsFcstCombIO(int argc, char **argv);
  
  /**
   *  Destructor
   */
  virtual ~ParmsEnsFcstCombIO(void);

protected:

private:
  /**
   * Constant indicating failure to initiate ParmsEnsFcstCombIO object
   */
  static const int failure;
};

# endif     // PARMS_ENSFCSTGEN_HH 
