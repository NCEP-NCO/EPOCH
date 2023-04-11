/**
 * @file EnsFcstCombMgr.hh
 * @brief EnsFcstCombMgr manages computation of probablity field
 *        from ensemble model forecast members
 * @class EnsFcstCombMgr
 * @brief EnsFcstCombMgr manages computation of probablity field
 *        from ensemble model forecast members 
 */

#ifndef ENSFCSTGENMGR_HH
#define ENSFCSTGENMGR_HH

#include <ConvWx/FcstGrid.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include "ParmsEnsFcstCombIO.hh"
#include <dsdata/DsEnsembleLeadTrigger.hh>
#include <iostream>
class EnsFcstCombMgr {

public:

  /**
   *  Return EnsFcstCombMgr success/failure indicators
   */
  enum Status_t {ENSFCSTCOMB_SUCCESS, ENSFCSTCOMB_FAILURE};

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] cleanExit  Cleanup function to call at exit
   */
  EnsFcstCombMgr(const ParmsEnsFcstCombIO &params, void cleanExit(int));

  /**
   *  Destructor
   */
  ~EnsFcstCombMgr(void);

  /**
   * Run the algorithm.
   * @return FSSMGR_SUCCESS or  FSSMGR_FAILURE
   */ 
  Status_t run(void);
 
protected:
   
private:

  /**
   * Constants
   */ 

  /**
   * Missing data value 
   */
  const static double ensFcstMissing;


  /**
   *  User defined parameters
   */
   ParmsEnsFcstCombIO pParams;


  /**
   * Triggering mechanism that waits for a gen time
   */
   DsEnsembleLeadTrigger *pTrigger;


  /**
   * A fake MultiGrid to use when there is a missing model input
   */
  MultiGrid pMissingDataGrid;
  
  /**
   * Process a new triggering event
   * @param[in] genTime  Forecast generation time
   * @param[in] leadTime  Forecast generation time
   * @param[in] url  Subset of URLs which triggered at gen/lead time
   * @param[in] complete  Flag indicates all URLs triggered or not
   */
  void pProcessTrigger(const time_t &genTime, int leadTime,
		       const std::vector<std::string> &url,  bool complete);

  /**
   * Set pairs of generation lead times to process
   * @param[in] genTime  Forecast generation time
   * @param[in] leadTime  Forecast generation time
   * @param[in] url  Subset of URLs which triggered at gen/lead time
   * @param[in] complete  Flag indicates all URLs triggered or not
   *
   * @return vector of tuples first being gen time, second being lead time,
   *         third being a flag set true if data exists
   */
  std::vector<std::tuple<time_t,int,bool> >
  pSetTimePairs(const time_t &genTime, int leadTime,
		const std::vector<std::string> &url, bool complete) const;

  /**
   * Process 
   * @param[in] genTimeLeadTimePairs  Generation and lead time pairs for
   *                                  processing input model data. Note all
   *                                  pairs should have the
   *                                  same valid time == genTime + leadTime
   *                                  3rd thing is flag true if data exists
   * @param[in] genTime  Forecast generation time  
   * @param[in] leadTime  Forecast lead time in seconds
   */
  void
  pProcess( const
	    std::vector<std::tuple<time_t,int,bool> > &genTimeLeadTimePairs,
	    const time_t &genTime, const int &leadTime);

  
  /**
   * Load
   * @param[in] ensembleMem  Integer indicator of ensemble member
   * @param[in] genTime  Forecast generation time  
   * @param[in] leadTime  Forecast lead time in seconds
   * @param[in] inputMultGrid   
   */ 
  Status_t pLoadInputData(const int ensembleMem, const time_t &genTime, 
			  const int &leadTime, MultiGrid &inputMultGrid);
  
  /**
   * Process inputs for one field
   * @param[in] inputGrids  One Grid per input mode
   * @param[in] outputFieldName  The name to assign to the output grid
   * @param[out] combProbGrid  The final combined output grid
   *
   * @return true for success false for failure
   */
  bool  pProcessInputField(const std::vector<Grid> &inputGrids,
			   const std::string &outputFieldName,
			  Grid &combProbGrid);
  
  /**
   * Write multiple grids
   * @param[in] genTime  Forecast generation time   
   * @param[in] leadTime  Forecast lead time in seconds
   * @param[out] mOutGrid  Object containing gridded data.
   */ 
  void
  pOutputData(const time_t &genTime, const int &leadTime,
	      const
	      std::vector<std::tuple<time_t,int,bool> > &genTimeLeadTimePairs,
	      MultiGrid &mOutGrid);

};

#endif // ENSFCSTGEN_HH
