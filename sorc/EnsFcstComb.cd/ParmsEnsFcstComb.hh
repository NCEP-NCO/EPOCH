
/**
 * @file ParmsEnsFcstComb.hh
 * @brief All user defined algorithm parameters.
 * @class ParmsEnsFcstComb
 * @brief All user defined algorithm parameters
 */

# ifndef    PARAMS_ENSFCSTCOMB_HH
# define    PARAMS_ENSFCSTCOMB_HH

#include <string>
#include <map>
#include <vector>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/Grid.hh>

class ParmsEnsFcstComb
{

public:

  /**
   * Output data encoding type 
   */
   Grid::Encoding_t outputEncodingType;

  /**
   * Parameters to define message logging, process registration and process
   * triggering
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
   * Parameters for output model forecast dataset
   */
  ParmFcst modelOut;

  /**
   * Container for forecast lead times in seconds
   */
  std::vector <int> leadSeconds;

  /**
   * Any number of field names shared by input and output
   */
  std::vector<std::string> fieldNames;

  /**
   * Derived set of all lead times shared by all ensemble members
   */
  std::vector<double> weights;

  /**
   * Triggering feedback interval (seconds)
   */
  int triggerFeedbackSeconds;

  /**
   * URL timeout seconds
   */
  int urlTimeoutSeconds;

  /**
   * URL disable seconds
   */
  int urlDisableSeconds;

  /**
   * Max age in seconds of data to be processed
   */
  int maxAgeSeconds;

  /**
   * Gen time hours that will be proceessed in [0,23]
   */
  std::vector< int > genTimeProcessHours;

protected:

private:

};

# endif     // PARMS_ENSFCSTCOMB_HH 
