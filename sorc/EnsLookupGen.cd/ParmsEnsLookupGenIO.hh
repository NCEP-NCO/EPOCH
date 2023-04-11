
/**
 * @file ParmsEnsLookupGenIO.hh
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 * @class ParmsEnsLookupGenIO
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    ParmsEnsLookupGenIO_hh
# define    ParmsEnsLookupGenIO_hh

#include "ParmsEnsLookupGen.hh"

class ParmsEnsLookupGenIO : public ParmsEnsLookupGen
{

public:

  /**
   * empty
   */
  ParmsEnsLookupGenIO(void);
  
  /**
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  Array of pointers to the strings which are the
   *                   command line arguments   *
   * The number of command line arguments is generally three for
   * real-time or six for archive mode.
   * 
   * Typical real-time and archive mode command lines are:
   * - 'EnsLookupGen -params Parmfile.params' or
   * - 'EnsLookupGen -params Parmfile.params -interval yyyymmddhhmmdss 
   *    yyyymmddhhmmdss'
   */
  ParmsEnsLookupGenIO(int argc, char **argv);
  
  /**
   *  Destructor
   */
  virtual ~ParmsEnsLookupGenIO(void);

protected:

private:
  /**
   * Constant indicating failure to initiate ParmsEnsLookupGenIO object
   */
  static const int failure;
};

# endif     // PARMS_ENSFCSTGEN_HH 
