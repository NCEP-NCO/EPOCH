
/**
 * @file ParmsPrecipAccumCalcIO.hh
 * @brief User defined parameters for PrecipAccumCalc application, as read in
 *        through the constructor
 * @class ParmsPrecipAccumCalcIO
 * @brief User defined parameters for PrecipAccumCalc application, as read in 
 *        through the constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    PARMS_PRECIPACCUMCALC_IO_HH
# define    PARMS_PRECIPACCUMCALC_IO_HH

#include "ParmsPrecipAccumCalc.hh"

class ParmsPrecipAccumCalcIO : public ParmsPrecipAccumCalc
{
public:

  /**
   * Constructor
   */  
  ParmsPrecipAccumCalcIO(void);

  /**
   * Constructor
   * @param[in] argc  Number of command line arguments is generally three for
   *                  real-time or six for archive mode.
   * @param[in] argv  Typical real-time and archive mode command lines are:
   *                  'PrecipAccumCalc -params PrecipAccumCalc.params' or
   *                  'PrecipAccumCalc -params PrecipAccumCalc.params -interval
   *                  yyyymmddhhmmdss yyyymmddhhmmdss'
   */
  ParmsPrecipAccumCalcIO(int argc, char **argv);
  
  /**
   *  destructor
   */
  virtual ~ParmsPrecipAccumCalcIO(void);

protected:

private:

  
};

# endif    // ParmsPrecipAccumCalcIO.hh
