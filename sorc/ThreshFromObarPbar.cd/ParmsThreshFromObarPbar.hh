/**
 * @file ParmsThreshFromObarPbar.hh
 * @brief All user defined algorithm parameters.
 * @class ParmsThreshFromObarPbar
 * @brief All user defined algorithm parameters
 */

# ifndef    ParmsThreshFromObarPbar_hh
# define    ParmsThreshFromObarPbar_hh

#include "Params.hh"
#include <Epoch/TileInfo.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/ParmTiling.hh>

#include <string>
#include <vector>

class ParmsThreshFromObarPbar
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
   * Triggering url, with pbars
   */
  std::string _pbarSpdb;

  /**
   * Tiling parameters
   */
  ParmTiling _tiling;

  /**
   * Tiling spec
   */
  TileInfo _tileInfo;

  std::string _obarSpdb1;        /**< SPDB URL for Obar (precip input) */
  std::string _thresholdsSpdb1;  /**< SPDB URL for precip thresholds (input/output) */

   /**
    * Cold start precip threshold
    */
  double _thresholdedFieldColdstartThresh1;

  /**
   * The obar precip thresholds that should match the obar database, and target bias values
   */
  std::vector<std::pair<double,double> > _obarThreshTargetBias1;
  std::vector<double> _obarThresh1;

  // /**
  //  * Bias correction for precip
  //  */
  // double _targetBias1;


  std::string _obarSpdb2;        /**< SPDB URL for Obar (CTH input) */
  std::string _thresholdsSpdb2;  /**< SPDB URL for CTH thresholds (input/output) */

   /**
    * Cold start CTH threshold
    */
  double _thresholdedFieldColdstartThresh2;

  /**
   * The obar CTH thresholds that should match the obar database, and target bias values
   */
  std::vector<std::pair<double,double> > _obarThreshTargetBias2;
  std::vector<double> _obarThresh2;

  // /**
  //  * Bias correction for CTH
  //  */
  // double _targetBias2;


   /**
    * Maximum seconds to keep a gen time around compared to most recent before
    * going ahead and processing what you have
    */
  int _maxIncompleteSeconds;

  /**
   * Max seconds to look back into database for initial threshold values
   */
  int _thresholdsMaxSecondsBack; 

  /**
   * Days back to fill in on startup
   */
  int _backfillDaysBack;

  /**
   * Number of threads
   */
  int _numThreads;
  
  /**
   * State debugging flag
   */
  bool _debugState;
  
protected:
private:

};

# endif     // PARMS_ENSFCSTGEN_HH 
