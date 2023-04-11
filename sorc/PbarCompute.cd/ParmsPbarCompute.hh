/**
 * @file ParmsPbarCompute.hh
 * @brief All user defined algorithm parameters.
 * @class ParmsPbarCompute
 * @brief All user defined algorithm parameters
 */

# ifndef    ParmsPbarCompute_hh
# define    ParmsPbarCompute_hh

#include "Params.hh"
#include <Epoch/TileInfo.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/ParmTiling.hh>
#include <ConvWx/Grid.hh>

#include <string>
#include <vector>

class ParmsPbarCompute   
{

public:

  /**
   * Parameters to define message logging, process registration and process
   * triggering
   */
  ParmMain _main;

  /**
   * Parameters to defining map projection
   */
  ParmProjection _proj;

  /**
   * Parameters that specify the model data input
   */
  std::vector<ParmFcstIO> _modelInput;

  /**
   * Url strings that specify the model data input
   */
  std::vector<std::string> _modelUrls;

  /**
   * Container for forecast lead times in seconds
   */
  std::vector <int> _leadSeconds;

  std::vector<double> _ltHours;  /**< lead times in hours */

  /**
   * All the input field names to read
   */
  std::vector<std::string> _inputFieldNames;

  /**
   * Tiling parameters
   */
  ParmTiling _tiling;

  /**
   * Triggering feedback interval (seconds)
   */
  int _triggerFeedbackSeconds;

  /**
   * URL timeout seconds
   */
  int _urlTimeoutSeconds;

  /**
   * URL disable seconds
   */
  int _urlDisableSeconds;

  /**
   * Max age in seconds of data to be processed
   */
  int _maxAgeSeconds;

  /**
   * Maximum number of days back to find SPDb data
   */
  int _maxDaysBack;

  /**
   * Tiling spec
   */
  TileInfo _tileInfo;


  std::string _pbarSpdb;  /**< SPDB URL for pbar (output) */
   /**
    * Maximum seconds to keep a gen time around compared to most recent before
    * going ahead and processing what you have
    */
  int _maxIncompleteSeconds;


  double _threshMin1;       /**< Min threshold for pbar computations */
  double _threshMax1;       /**< Max threshold for pbar computations */
  double _threshDelta1;     /**< Threshold resolution for pbar computations */
  int _numThresh1;          /**< Number of thresholds, field 1 */
  std::string _inputThresholdedField1;  /**< Name of the threshold field1 */
  double _thresholdedFieldColdstartThresh1; /**< Cold start threshold for field1 */
  Params::Comparison_t _thresholdedFieldComparison1; /**< Type of test to do, field 1 */
  bool _hasFixedField1; /**< True if we also include a fixed threshold field with 1 */
  std::string _inputFixedField1;  /**< The name of the fixed field1, if we have that */
  double _fixedFieldThresh1; /**< Fixed threshold value1, if we have that */
  Params::Comparison_t _fixedFieldComparison1; /**< Type of test for fixed field 1, if we have that */
  std::vector<double> _thresh1;  /**< ALl the thresholds, field 1 */


  double _threshMin2;       /**< Min threshold for pbar computations */
  double _threshMax2;       /**< Max threshold for pbar computations */
  double _threshDelta2;     /**< Threshold resolution for pbar computations */
  int _numThresh2;          /**< Number of thresholds, field 2 */
  std::string _inputThresholdedField2;  /**< Name of the threshold field2 */
  double _thresholdedFieldColdstartThresh2; /**< Cold start threshold for field2 */
  Params::Comparison_t _thresholdedFieldComparison2; /**< Type of test to do, field 2 */
  bool _hasFixedField2; /**< True if we also include a fixed threshold field with 2 */
  std::string _inputFixedField2;  /**< The name of the fixed field2, if we have that */
  double _fixedFieldThresh2; /**< Fixed threshold value2, if we have that */
  Params::Comparison_t _fixedFieldComparison2; /**< Type of test for fixed field 2, if we have that */
  std::vector<double> _thresh2;  /**< ALL the thresholds, field 2 */

  /**
   * Max seconds to look back into database for initial threshold values
   */
  int _thresholdsMaxSecondsBack; 

  /**
   * Number of threads
   */
  int _numThreads;
  
  /**
   * State debugging flag
   */
  bool _debugState;
  
  /**
   * @return true if value passed test relative to currentThresh 
   * @param[in] value
   * @param[in] currentThresh
   */
  bool passesTests1(double value, double currentThresh) const;

  /**
   * @return true if value passed test relative to currentThresh  pair
   * @param[in] value1
   * @param[in] value2
   * @param[in] currentThresh1
   * @param[in] currentThres2
   */
  bool passesTests2(double value1, double value2, double currentThresh1, double currentThresh2) const;

  /**
   * @return the actual threshold for the index'th threshold index
   * @param[in] index
   * @param[in] which 1 or 2
   */
  double thresholdFromIndex(int index, int which) const;

  /**
   * @return the threshold index for a threshold value
   * @param[in] thresh Threshold value
   * @param[in] which 1 or 2
   */
  int threshIndex(double thresh, int which) const;

  /**
   * @return the index into two dimensions as represented in a one dim array 
   * @param[in] index1 
   * @param[in] index2
   */
  int index2d(int index1, int index2) const;

  /**
   * @return true if the local comparison type is '>='
   * @param[in] which 1 or 2
   */
  inline bool isGreaterOrEqualTest(int which) const
  {
    if (which == 1)
    {
      return _thresholdedFieldComparison1 == Params::GREATER_THAN_OR_EQUAL;
    }
    else
    {
      return _thresholdedFieldComparison2 == Params::GREATER_THAN_OR_EQUAL;
    }
  }  


protected:
private:

};

# endif     // PARMS_ENSFCSTGEN_HH 
