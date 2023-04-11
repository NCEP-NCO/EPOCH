/**
 * @file LeadtimeThreadData.hh
 * @brief Information that goes into each thread, for a lead time
 * @class LeadtimeThreadData
 * @brief Information that goes into each thread, for a lead time
 */

#ifndef LeadtimeThreadData_HH
#define LeadtimeThreadData_HH

#include "ParmsThreshFromObarPbarIO.hh"
#include "LeadtimeThreadDataForField.hh"
#include "ThresholdForEachTile.hh"

class LeadtimeThreadData
{
public:

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] gt  Gen time
   * @param[in] lt  Lead seconds
   */
  LeadtimeThreadData(const ParmsThreshFromObarPbar &params, const time_t &gt, int lt);


  /**
   *  Destructor
   */
  ~LeadtimeThreadData(void);

  /**
   * Try to initialize using older data in the database, field1, if unsuccessful use coldstart for all tiles
   * @param[in] obsThreshIndex1  Index into obs thresholds
   */
  void setInitialThresholds(int which, int obsThreshIndex);

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

  inline int getLeadSeconds(void) const {return _leadTime;}

  void updateSpdbForAllTiles(SpdbGenBasedThreshHandler &spdb,
			     int index, int which, TaThreadDoubleQue &threads);


protected:
private:

  /**
   *  User defined parameters
   */
   ParmsThreshFromObarPbar _params;

  time_t _genTime;
  int _leadTime;      /**< Current lead */

  bool _is1;
  LeadtimeThreadDataForField _field1;
  LeadtimeThreadDataForField _field2;
  
};

#endif
