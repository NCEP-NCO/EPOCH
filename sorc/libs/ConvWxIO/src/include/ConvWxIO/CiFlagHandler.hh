// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// Copyright, University Corporation for Atmospheric Research (UCAR) 2009-2017. 
// The Government's right to use this data and/or software is restricted per 
// the terms of Cooperative Agreement between UCAR and the National  Science 
// Foundation, to government use only which includes the nonexclusive, 
// nontransferable, irrevocable, royalty-free license to exercise or have 
// exercised for or on behalf of the U.S. Government throughout the world. 
// All other rights are reserved. 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>
/**
 * @file CiFlagHandler.hh 
 * @brief Convective Initiation Flag data, used by blending algorithms
 * @class CiFlagHandler
 * @brief Convective Initiation Flag data, used by blending algorithms
 */

#ifndef CI_FLAG_HANDLER_H
#define CI_FLAG_HANDLER_H

#include <string>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/DynamicWeightsIO.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/LtInterp.hh>
// class FcstGrid;

class Grid;
class ConvWxThreadMgr;



class CiFlagHandler
{
 public:

  /**
   * Constructor, empty
   */
  CiFlagHandler(void);
  
  /**
   * Constructor
   *
   * @param[in] proj  The projection parameters
   * @param[in] parms  The parameters specifying the input data
   * @param[in] maxSecondsBack  The maximum time (seconds) to look back
   *                            for data to use.
   */
  CiFlagHandler(const ParmProjection &proj,
		const ParmFcst &parms, int maxSecondsBack,
		int maxwaitseconds);


  /**
   * Destructor
   */
  virtual ~CiFlagHandler(void);


  /**
   * Start Up a new generation time, lead time not set
   *
   * @param[in] t  The Triggering generation time to use
   *
   */
  void startGenTime(const time_t &t);

  /**
   * Initialize for fixed gt/lt, sets both, but does not read in data
   *
   * @param[in] ciGt
   * @param[in] ciLt
   */
  void init(const time_t &ciGt, int ciLt);

  /**
   * Set local state for a particular lead time
   * @param[in] gt  Triggering generation time
   * @param[in] lt  Triggering lead time
   */
  void processInit(const time_t &gt, int lt, ConvWxThreadMgr *thread);

  /**
   * @return the local generation time
   */

  inline time_t getGenTime(void) const {return pGenTime;}

  /**
   * @return true if data grid is set
   */
  inline bool hasData(void) const {return pHasCiData;}

  /**
   * Load in data based on internal state (or not if internal state 
   * indicates you can't/shouldn't).  If threading is not null, lock threads
   * when reading data
   * @param[in] thread Pointer to threading, or NULL
   *
   * @return true if data has been loaded
   */
  bool loadData(ConvWxThreadMgr *thread);

  /**
   * Load in data based on internal state (or not if internal state 
   * indicates you can't/shouldn't). 
   *
   * @return true if data has been loaded
   */
  bool loadData(void);

  /**
   * @return XML string describing the state
   */
  std::string metaDataXml(void) const;

  /**
   * Construct a mask grid from local grids by  taking max if there are
   * two grids (interpolating lead times), or from the one grid if there
   * is one.
   *
   * @param[in] nptExpand  Expand the max values by this many pixels
   * @param[in] extrapGrid  Set mask grid to missing where this grid
   *                        has small values
   * @param[in] minValue  The extrapGrid min value, below that the returned
   *                      grid is set missing
   * @param[out] maskGrid  Returned grid
   * @return true if the grid was constructed
   */
  bool constructMaskGrid(int nptExpand,	 const Grid &extrapGrid,
			 double minValue, Grid &maskGrid) const;

  void extractGenLeadFromMetadata(bool hasGrid, const FcstGrid &grid,
				  const time_t &gridGt, int gridLt);

  /**
   * Parse XML data
   *
   * @param[in] xml  The string
   */
  bool parseXml(const std::string &xml);


protected:
private:

  bool pGood;            /**< True for good object */
  std::string pName;     /**< Debug name */
  ParmProjection pProj;  /**< Projection info */
  ParmFcstIO pParm;      /**< Input data info */
  int pMaxSecondsBack;   /**< Maximum seconds back to look for for data */
  int pMaxWaitSeconds;   /**< Maximum seconds to wait for data */

  /**
   * Generation time for initiation mask data grids
   */
  time_t pGenTime;

  /**
   * True if the gen time has been set
   */
  bool pGenTimeIsSet;

  /**
   * The lead time that is expected
   */
  int pLt;
  
  /**
   * True if pLt is set 
   */
  bool pLtIsSet;

  /**
   * True if lead time is out of range
   */
  bool pLtOutOfRange;

  /**
   * Ci Data
   */
  FcstGrid pCiData;

  /**
   * True if ci data has been read in for pGenTime, pLt
   */
  bool pHasCiData;


  bool pComputeWeightsLeadTime(int lt);
  bool pWaitForLeadTime(int lt, ConvWxThreadMgr *thread);
};


#endif
