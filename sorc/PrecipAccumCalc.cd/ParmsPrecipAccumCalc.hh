
/**
 * @file ParmsPrecipAccumCalc.hh
 * @brief User defined parameters for PrecipAccumCalc application.
 * @class ParmsPrecipAccumCalc
 * @brief User defined parameters for PrecipAccumCalc application.
 */

# ifndef    PARMS_PRECIPACCUM_HH
# define    PARMS_PRECIPACCUM_HH

#include <ConvWx/ParmFcst.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/ParmProjection.hh>

class MultiGrid;

class ParmsPrecipAccumCalc
{
public:

  /**
   *  Return value for ParmsCalbrate methods
   *  PARMS_SUCCESS indicates successful method execution
   *  PARMS_FAILURE indicates unsuccessful method execution
   */
  enum status {PARMS_SUCCESS, PARMS_FAILURE};

  enum outputDataType_t {INT8, INT16, FLOAT32};
 
  outputDataType_t outputDataType;

  /**
   *  Parameters to define message logging, process registration and triggering
   */
  ParmMain main;

  /**
   * Parameters to defining map projection
   */
  ParmProjection proj;
 
  /**
   * Parameters that specify the model data input
   */
  std::vector<ParmFcstIO> modelInput;

  /**
   * Url strings that specify the model data input
   */
  std::vector<std::string> modelUrls;
  
  /**
   * Parameters for output climatological dataset
   */
  ParmFcst modelOut;

  /**
   * Container for forecast lead times in seconds
   */
  std::vector <int> leadSeconds;

  /**
   * 3 and 6 hour precip accumulation field names
   */
  std::string hr3AccumName;
  std::string hr6AccumName;
 
  /**
   * 3 hour accumulation output directory tail
   */
  std::string hr3AccumOutputDirTail;

  /**
   * Flag to indicate subtraction of accumulation fields will be prevented
   * from generating negative numbers less than -1;
   */
  bool forceAccumNonNegative;
  
  /**
   * 3 hour average Upward Long-Wave Rad. Flux
   */
  std::string hr3UlwrfName;

  /**
   * 6 hour average Upward Long-Wave Rad. Flux
   */
  std::string hr6UlwrfName;
  
  /**
   * Names of additional fields to be passed through
   */ 
  std::vector<std::string> extraDataFields;

  /**
   * Number of threads processing the data at input data url
   */ 
  int numThreads;

  /**
   * All the long names, 3 hours
   */
  std::vector<std::string> longNames3hr;

  /**
   * All the long names, 6 hours
   */
  std::vector<std::string> longNames6hr;


  /**
   * Mapping for field names long/short, 6 hour
   */
  std::vector<std::pair<std::string,std::string> > mdv6hrShortToLong;
  
  /**
   * Mapping for field names long/short, 3 hour
   */
  std::vector<std::pair<std::string,std::string> > mdv3hrShortToLong;
  

  /**
   * Convert short to long name
   * @return long name
   * @param[in] hr  3 or 6
   * @param[in] shortName
   */
  std::string shortNameToLongName(int hr, const std::string &shortName) const;

  /**
   * Convert long to short name
   * @return short name
   * @param[in] hr  3 or 6
   * @param[in] longName
   */
  std::string longNameToShortName(int hr, const std::string &longName) const;

  /**
   * Convert field names from long to short using the converter methods
   * @param[in] hr  3 or 6
   * @param[in] grids  Grid data to change field names
   * @return true for success
   */
  bool longToShort(int hr, MultiGrid &grids) const;


protected:

private:

};

# endif    // PARMS_PRECIPACCUMCALC_HH 
