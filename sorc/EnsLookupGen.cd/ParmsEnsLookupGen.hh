
/**
 * @file ParmsEnsLookupGen.hh
 * @brief All user defined algorithm parameters.
 * @class ParmsEnsLookupGen
 * @brief All user defined algorithm parameters
 */

# ifndef    ParmsEnsLookupGen_hh
# define    ParmsEnsLookupGen_hh

#include "OutputToThreshProj.hh"
#include <Epoch/ThresholdDatabaseParms.hh>
#include <Epoch/FieldThresh.hh>

#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWx/Grid.hh>

#include <string>
#include <vector>

class Grid2d;
class TileInfo;

class ParmsEnsLookupGen
{

public:

  /**
   * Create a threshold grid with constant threshold
   * everywhere, put obar threshold into the field name
   *
   * @param[in] fname Field name subset to use
   * @param[in] obarThresh Threshold to append to the name
   * @param[in] thresh  The fixed data threshold for the grid
   * @param[out] thresholdGrid  Returned grid
   */
  void createFixedTiledGrid(const std::string &fname,
			    double obarThresh, double thresh,
			    Grid2d &thresholdGrid) const;
  
  /**
   * @return the weight to give point inside the domain based on number of
   * gridpoints within the domain, using local member values to compute it.
   *
   * @param[in] nptIn  Number of gridpoints within the domain 
   */
  double insideWeight(int nptIn) const;

  /**
   * @return true if parms are set so this a debug point
   *
   * @param[in] x  Index
   * @param[in] y  Index
   */
  bool isDebugPoint(int x, int y) const;
  
  /**
   * @return true if database values passed in agree with local params
   *
   * @param[in] lt  The lead seconds from database
   * @param[in] tileInfo   from database
   */
  bool checkDb(const std::vector<int> &lt,
	       const TileInfo &tileInfo) const;

  /**
   * Output data encoding type
   */
   Grid::Encoding_t _encodingType;

  /**
   * Parameters to define message logging, process registration and process
   * triggering
   */
  ParmMain _main;

  /**
   * Parameters to defining map projection for threshold spdb data
   */
  ParmProjection _proj;

  /**
   * Parameters to defining projection of larger domain (extends north)
   */
  ParmProjection _projExtended;

  /**
   * Parameters that specify the model data input
   */
  std::vector<ParmFcstIO> _modelInput;

  /**
   * Url strings that specify the model data input
   */
  std::vector<std::string> _modelUrls;

  /**
   * Parameters for output model forecast dataset
   */
  ParmFcst _modelOut;

  /**
   * Container for forecast lead times in seconds
   */
  std::vector <int> _leadSeconds;

  /**
   * Informaton for each field, including Field names and comparison types, 
   * coldstart thresh and database information (including fixed fields
   * that do not have a database)
   */
  MultiThresholdDatabaseParms _fields;

  /**
   * Field names to read in, pulled from the _fields object
   */
  std::vector<std::string> _fieldNames;

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

  // int _outputObarFieldIndex; /**< Index into fields to the one that sets output 
  // 			      * for each obar configured for that field */

  int _archiveSpdbBestOffsetSec;  /**< preferred offset to SPDB data, archive*/
  int _archiveSpdbMinOffsetSec;   /**< minimum offset to SPDB data, archive */
  int _archiveSpdbMaxOffsetSec;   /**< maximum offset to SPDB data, archvie */
  int _realtimeSpdbMinOffsetSec;  /**< minimum offset to SPDB data, realtime*/
  int _realtimeSpdbMaxOffsetSec;  /**< maximum offset to SPDB data, realtime*/

  int _nptSmoothTiledGrid;        /**< Smoothing the stitched tiled grid */
  double _centerWeightTiledGrid;  /**< Weighting the stitched tiled grid */
  double _edgeWeightTiledGrid;    /**< Weighting the stitched tiled grid */

  bool _debugSpdb;                /**< True for extra SPDB debug output */
  double _debugLat;    /**< Debug point */
  double _debugLon;    /**< Debug point */
  bool _debugXY;       /**< True to debug at an index */
  int _debugX;         /**< Point to debug at, if debugXY=true */
  int _debugY;         /**< Point to debug at, if debugXY=true */
  std::vector<int> _debugTiles;  /**< Tiles to debug, empty for all */
  OutputToThreshProj _mapper; /**< Mapping between actual and extended domain*/
  int _maxNptInside;  /**< maximim number of point within the domain before
		       * the weights are set to be 100% the actual ones */
  int _numThreads;  /**< Number of threads */

protected:
private:

};

# endif     // PARMS_ENSFCSTGEN_HH 
