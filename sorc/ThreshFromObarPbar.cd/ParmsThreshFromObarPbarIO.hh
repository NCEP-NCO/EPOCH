
/**
 * @file ParmsThreshFromObarPbarIO.hh
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 * @class ParmsThreshFromObarPbarIO
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    ParmsThreshFromObarPbar_IO_hh
# define    ParmsThreshFromObarPbar_IO_hh

#include "ParmsThreshFromObarPbar.hh"

class ParmsThreshFromObarPbarIO : public ParmsThreshFromObarPbar
{

public:

  /**
   * empty
   */
  ParmsThreshFromObarPbarIO(void);
  
  /**
   * @param[in] argc  Number of command line arguments
   * @param[in] argv  Array of pointers to the strings which are the
   *                   command line arguments   *
   * The number of command line arguments is generally three for
   * real-time or six for archive mode.
   * 
   * Typical real-time and archive mode command lines are:
   * - 'ThreshFromObarPbar -params Parmfile.params' or
   * - 'ThreshFromObarPbar -params Parmfile.params -interval yyyymmddhhmmdss 
   *    yyyymmddhhmmdss'
   */
  ParmsThreshFromObarPbarIO(int argc, char **argv);
  
  /**
   *  Destructor
   */
  virtual ~ParmsThreshFromObarPbarIO(void);

protected:

private:
  /**
   * Constant indicating failure to initiate ParmsThreshFromObarPbarIO object
   */
  static const int failure;

  void _setTileLatLon(void);
};

# endif     // PARMS_ENSFCSTGEN_HH 
