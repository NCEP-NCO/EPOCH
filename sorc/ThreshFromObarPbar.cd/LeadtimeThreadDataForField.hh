/**
 * @file LeadtimeThreadDataForField.hh
 * @brief Information that goes into each thread, for a lead time
 * @class LeadtimeThreadDataForField
 * @brief Information that goes into each thread, for a lead time
 */

#ifndef LeadtimeThreadDataForField_HH
#define LeadtimeThreadDataForField_HH

#include "ParmsThreshFromObarPbarIO.hh"
#include "ThresholdForEachTile.hh"
#include <Epoch/TileThreshInfoGenBased.hh>


class SpdbGenBasedThreshHandler;
class TaThreadDoubleQue;

class LeadtimeThreadDataForField
{
public:

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] gt  Gen time
   * @param[in] lt  Lead seconds
   */
  LeadtimeThreadDataForField(const ParmsThreshFromObarPbar &params, int which,
			     const time_t &genTime, int leadTime);


  /**
   *  Destructor
   */
  ~LeadtimeThreadDataForField(void);

  /**
   * Try to initialize using older data in the database, if unsuccessful use coldstart for all tiles
   * @param[in] obsThreshIndex  Index into obs thresholds
   * @param[in] genTime
   * @param[in] leadTime
   */
  void setInitialThresholds(int obsThreshIndex);


  bool usedTileBelow(int tileIndex, bool isMotherTile,
		     std::vector<double> &thresholds,
		     int &belowTile, int &threshIndex);
  int setToMotherOrColdstart(int tileIndex, bool isMotherTile);

  void handleMotherResults(int motherIndex, int pbarIndex);

  /**
   * @return the index for the chosen threshold
   */
  int setupAndRunObsZero(int tileIndex,
			  const std::vector<double> &pbar,
			  const std::vector<double> &thresh,
			  bool isMotherTile, int obsThreshIndex);
  /**
   * @return the index for the chosen threshold
   */
  int setupAndRunObsNonZero(double oBar, int tileIndex,
			     const std::vector<double> &pbar, 
			     const std::vector<double> &thresh, 
			     bool isMotherTile, int obsThreshIndex);

  void updateSpdbForAllTiles(SpdbGenBasedThreshHandler &spdb,
			     int index, TaThreadDoubleQue &threads);


protected:
private:

  /**
   *  User defined parameters
   */
  ParmsThreshFromObarPbar _params;
  time_t _genTime;
  int _leadTime;

  std::string _spdb;  /**< Database with thresholds */
  double _coldstartThresh;  /**< Cold start threshold */
  bool _greaterOrEqual;   /**< True if comparison is >= for better */
  std::vector<double> _targetBias;     /**< Configurable bias optimum, one per obar threshold*/
  
  bool _motherSet;                    /**< True if _motherInfo is set */
  TileThreshInfoGenBased _motherInfo; /**< Thresh info for the mother tile */
  bool _motherFail;                   /**< True if could not set _motherInfo */
  int _motherPbarIndex;
  std::vector<double> _bestOld;      /**< best available recent thresholds */
  ThresholdForEachTile _threshInfo;  /**< Storage for thresholds locally */
  double _currentThresh;     /**< The best recent thresholded thresh */

};

#endif
