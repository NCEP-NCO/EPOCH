/**
 * @file PbarComputeMgr.hh
 * @brief PbarComputeMgr manages computation of pbar
 *        from ensemble model forecast members
 * @class PbarComputeMgr
 * @brief PbarComputeMgr manages computation of pbar
 *        from ensemble model forecast members
 */

#ifndef PbarComputeMgr_HH
#define PbarComputeMgr_HH

#include "ParmsPbarComputeIO.hh"
#include "ForecastState.hh"
#include <Epoch/AveragingGrids.hh>
#include <Epoch/SpdbPbarHandler2.hh>
#include <dsdata/DsUrlTrigger.hh>
#include <toolsa/TaThreadDoubleQue.hh>

class DsEnsembleLeadTrigger;
class LeadtimeThreadData;

class PbarComputeMgr
{
public:

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] cleanExit  Cleanup function to call at exit
   */
  PbarComputeMgr(const ParmsPbarComputeIO &params,
		 void cleanExit(int));

  /**
   *  Destructor
   */
  ~PbarComputeMgr(void);

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
   * @class PbarComputeThreads
   * @brief Instantiation to implement the clone() method
   */
  class PbarComputeThreads : public TaThreadDoubleQue
  {
  public:
    inline PbarComputeThreads(void) : TaThreadDoubleQue() {}
    inline virtual ~PbarComputeThreads(void) {}
    TaThread *clone(int index);
  };


  /**
   *  User defined parameters
   */
   ParmsPbarComputeIO _params;

  /**
   * Triggering mechanism that waits for a gen time, forecasts
   */
  DsEnsembleLeadTrigger *_trigger;

  time_t _latestGen;      /**< Latest gen time, to keep track of gen changes */
  ForecastState _state;   /**< Internal state, to keep track of complete gens */
  time_t _genTime;    /**< Current gen */
  SpdbPbarHandler2 _pbarSpdb;  /**< Pbar spdb for output */
  PbarComputeThreads _thread;    /**< Threading */

  /**
   * True if a lead time modified the state, used during threading and checked after
   */
  bool _modified;
  

  void _process(const time_t &genTime, int leadTime, size_t num, bool complete);
  void _processGenTime(const time_t &genTime);
  bool _processGenLead(const time_t &gt, const ForecastState::LeadStatus_t st,
		       LeadtimeThreadData &ltData);
  void _processEnsembleMember(const time_t &genTime, int leadTime,int ensembleIndex,
			      LeadtimeThreadData &ltData);
  bool _loadExampleInputData(const time_t &genTime, int leadTime, FcstGrid &grid) const;
  bool _processTiles(const time_t &genTime, const ForecastState::LeadStatus_t s,
		     LeadtimeThreadData &ltData);
  void _setupAndRunAlg(LeadtimeThreadData &ltData, int tileIndex);
};

#endif
