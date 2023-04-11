/**
 * @file EnsLookupGenMgr.hh
 * @brief EnsLookupGenMgr manages computation of probablity field
 *        from ensemble model forecast members
 * @class EnsLookupGenMgr
 * @brief EnsLookupGenMgr manages computation of probablity field
 *        from ensemble model forecast members 
 */

#ifndef  EnsLookupGenMgr_hh
#define  EnsLookupGenMgr_hh

#include "ParmsEnsLookupGenIO.hh"
#include "MultiThreshInfo.hh"
#include <toolsa/TaThreadDoubleQue.hh>

class MultiFcstGrid;
class DsEnsembleLeadTrigger;
class Grid2d;
class GriddedThresh;

class EnsLookupGenMgr
{
public:

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] cleanExit  Cleanup function to call at exit
   */
  EnsLookupGenMgr(const ParmsEnsLookupGenIO &params,
		  void cleanExit(int));

  /**
   *  Destructor
   */
  ~EnsLookupGenMgr(void);

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
   * @class EnsLookupThreads
   * @brief Instantiation to implement the clone() method
   */
  class EnsLookupThreads : public TaThreadDoubleQue
  {
  public:
    inline EnsLookupThreads(void) : TaThreadDoubleQue() {}
    inline virtual ~EnsLookupThreads(void) {}
    TaThread *clone(int index);
  };


  /**
   *  User defined parameters
   */
   ParmsEnsLookupGenIO _params;

  /**
   * Triggering mechanism that waits for a gen time
   */
  DsEnsembleLeadTrigger *_trigger;

  time_t _genTime;  /**< Current gen time */

  /**
   * Multi thresholds object, one per data field
   */
  MultiThreshInfo _multiThreshInfo;

  /**
   * Threading
   */
  EnsLookupThreads _thread;  

  /**
   * Process 
   * @param[in] genTime  Forecast generation time  
   * @param[in] leadTime  Forecast lead time in seconds
   * @param[in] num  Number of urls
   * @param[in]  complete True if data gotten from every url
   */
  void _process(const time_t &genTime, int leadTime, size_t num, bool complete, int &index);

  /**
   * Process individual ensemble member
   * @param[in] ensIndex  Index into ensembles 0,1,...
   * @param[in] genTime  Forecast generation time  
   * @param[in] leadTime  Forecast lead time in seconds
   */
  void _processEnsembleMember(int ensIndex, const time_t &genTime, int leadTime,
			      Grid &ensembleCount,
			      GriddedThresh &gthresh);

  /**
   * Load in the fields for one ensemble member
   * @param[in] ensIndex  Index into ensembles 0,1,...
   * @param[in] genTime  Forecast generation time  
   * @param[in] leadTime  Forecast lead time in seconds
   * @param[out] mInGrid  The data grids, one per field
   * @param[out] grid  Pointers to each data grid within mInGrid,
   *                   done for efficienty
   *
   * @return true for success
   */
  bool _loadAllInputFieldsForEnsemble(int ensIndex, const time_t &genTime,
				      int leadTime,  MultiFcstGrid &mInGrid,
				      std::vector<const Grid *> &grid) const;
  /**
   * Load in the fields for one ensemble member,, normal forecast data
   *
   * @param[in] ensIndex  Index into ensembles 0,1,...
   * @param[in] genTime  Forecast generation time  
   * @param[in] leadTime  Forecast lead time in seconds
   * @param[out] inputMultGrid  The data grids, one per field
   *
   * @return true for success
   */ 
  bool _loadInputData(int ensIndex, const time_t &genTime, 
		      int leadTime, MultiFcstGrid &inputMultGrid) const;

  
  /**
   * Output the results
   *
   * @param[in] genTime  Forecast generation time   
   * @param[in] leadTime  Forecast lead time in seconds
   */ 
  void _outputData(const time_t &genTime, int leadTime, GriddedThresh &gthresh);

  /**
   * Normalize the ensemble sum by dividing by counts
   */
  void _normalize(const Grid &ensembleCount,
		  GriddedThresh &gthresh) const;

};

#endif // ENSFCSTGEN_HH
