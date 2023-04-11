/**
 * @file ThreshComputeMgr.hh
 * @brief ThreshComputeMgr manages computation of thresholds
 *        from ensemble model forecast members and obar inputs
 * @class ThreshComputeMgr
 * @brief ThreshComputeMgr manages computation of thresholds
 *        from ensemble model forecast members and obar inputs
 */

#ifndef ThreshComputeMgr_HH
#define ThreshComputeMgr_HH

#include "ParmsThreshFromObarPbarIO.hh"
#include "ForecastState.hh"
#include "State.hh"
#include <Epoch/AveragingGrids.hh>
#include <Epoch/SpdbGenBasedThreshHandler.hh>
#include <Epoch/SpdbPbarHandler2.hh>
#include <dsdata/DsUrlTrigger.hh>
#include <toolsa/TaThreadDoubleQue.hh>

class DsEnsembleLeadTrigger;
class SpdbObsHandler;
class PbarVector;
class LeadtimeThreadData;

class ThreshFromObarPbarMgr
{
public:

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] cleanExit  Cleanup function to call at exit
   */
  ThreshFromObarPbarMgr(const ParmsThreshFromObarPbarIO &params,
		   void cleanExit(int));

  /**
   *  Destructor
   */
  ~ThreshFromObarPbarMgr(void);

  /**
   * Run the algorithm.
   * @return true for success, false for failure
   */ 
  bool run(void);
 
  /**
   * Method needed for threading
   * @param[in] i  Info pointer
   */
  static void compute(void *i);

protected:
private:

  /**
   * @class ThreshFromObarPbarThreads
   * @brief Instantiation to implement the clone() method
   */
  class ThreshFromObarPbarThreads : public TaThreadDoubleQue
  {
  public:
    inline ThreshFromObarPbarThreads(void) : TaThreadDoubleQue() {}
    inline virtual ~ThreshFromObarPbarThreads(void) {}
    TaThread *clone(int index);
  };


  /**
   *  User defined parameters
   */
   ParmsThreshFromObarPbarIO _params;

  /**
   * Triggering mechanism that waits for a gen time, SPDB
   */
  DsUrlTrigger *_strigger;

  time_t _latestGen;  /**< Latest gen time, to keep track of gen changes */
  State _state;       /**< Internal state, to keep track of complete gens */
  time_t _genTime;    /**< Current gen */

  
  SpdbPbarHandler2 _pbarSpdb;         /**< Pbar spdb */
  SpdbGenBasedThreshHandler _threshSpdb1;   /**< The Precip Thresh Database object written to */
  SpdbGenBasedThreshHandler _threshSpdb2;   /**< The CTH Thresh Database object written to */

  /**
   * Threading
   */
  ThreshFromObarPbarThreads _thread;  

  /**
   * True if a lead time modified the state, used during threading and checked after
   */
  bool _modified;
  

  void _processTilesAtObsThresh1(const time_t &genTime,
				const ForecastState::LeadStatus_t s,
				LeadtimeThreadData &ltData,
				SpdbObsHandler &obs1,
				int obsIndex1,
				vector<int> &pbarIndexAtTile);
  void _processTilesAtObsThresh2(const time_t &genTime,
				const ForecastState::LeadStatus_t s,
				 const vector<int> &pbarIndexAtTile,
				 LeadtimeThreadData &ltData,
				 SpdbObsHandler &obs2,
				 int obsIndex2);

  
  int  _setupAndRunAlg(LeadtimeThreadData &ltData,
		       int tileIndex, bool isMotherTile,
		       SpdbObsHandler &obs, int obsThreshIndex,
		       std::vector<double> thresholds);

  int _setupAndRunAlg2(LeadtimeThreadData &ltData,
		       int tileIndex, 
		       bool isMotherTile, 
		       SpdbObsHandler &obs,
		       int obsThreshIndex,
		       std::vector<double> thresholds,
		       int index1);

  bool _getObar(SpdbObsHandler &obs, int obarThreshIndex, int tileIndex,
		double &oBar);
  void _queryThreshSpdbAtGenTime(const time_t &gt);

  void _fillGaps(const time_t &genTime);
  void _processTrigger(const time_t &genTime, bool first);
  void _processGenTime(const ForecastState &state);
};

#endif
