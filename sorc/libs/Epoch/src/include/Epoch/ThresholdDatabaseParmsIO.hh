
/**
 * @file ThresholdDatabaseParmsIO.hh
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 * @class ThresholdDatabaseParmsIO
 * @brief All user defined algorithm parameters, as read in through a
 *        constructor
 *
 * The parameters are the members of the base class
 */

# ifndef    ThresholdDatabaseParmsIO_hh
# define    ThresholdDatabaseParmsIO_hh

#include "ThresholdDatabaseParms.hh"

class ThresholdDatabaseParmsIO : public ThresholdDatabaseParms
{

public:

  /**
   * empty
   */
  ThresholdDatabaseParmsIO(void);
  
  /**
   * Initialize by reading in a paramfile assumed to be ThresholdDatabaseParams
   * @param[in] parmfile  File to read
   */
  ThresholdDatabaseParmsIO(const std::string &parmfile);
  
  /**
   *  Destructor
   */
  virtual ~ThresholdDatabaseParmsIO(void);

  /**
   * Print the params (tdrp)
   */
  void printParams(void) const;
  
  /**
   * @return true if object is good
   */
  inline bool isOk(void) const {return _ok;}

protected:
private:
  /**
   * Constant indicating failure to initiate ThresholdDatabaseParmsIO object
   */
  static const int failure;

  bool _ok;  /**< Status */
};

# endif     // PARMS_ENSFCSTGEN_HH 
