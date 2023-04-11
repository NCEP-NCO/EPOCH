/**
 * @file LeadtimeThreadData.hh
 * @brief 
 * @class LeadtimeThreadData
 * @brief 
 */

#ifndef LeadtimeThreadData_HH
#define LeadtimeThreadData_HH

#include "ParmsPbarComputeIO.hh"
#include "AdditionalInputs.hh"
#include <Epoch/AveragingGrids.hh>

class TaThreadDoubleQue;
class MultiGrid;
class Grid;
class PbarVector;

class LeadtimeThreadData
{
public:

  /**
   * Constructor
   * @param[in] params  The algorithm parameters
   * @param[in] gt  Gen time
   * @param[in] lt  Lead seconds
   */
  LeadtimeThreadData(const ParmsPbarCompute &params, const time_t &gt, int lt);


  /**
   *  Destructor
   */
  ~LeadtimeThreadData(void);

  inline int getLeadSeconds(void) const {return _leadTime;}
  inline bool agrid1WasModified(void) const {return _aGridModified1; }
  
  /**
   * Set up count/sum AveragingGrids for each threshold needed, and 
   * reset any additional inputs.
   * @param[in] egrid Template example grid for dimensions
   */
  void setupCountSums(const Grid &egrid);

  /**
   * Set pointers to the additional input grids in the input if there are any needed
   * @param[in] mInGrid  multiGrid with hopefully addtional input grid
   *
   * @return true if successful
   */
  bool setAdditionalGridPointers(const MultiGrid &mInGrid);

  /**
   * Set additional data value at a grid point, if configured to do so
   * @param[in] gridIndex  Index into data
   * @param[in] which  1 for first inputs additional values,
   *                   2 for second inputs additional values
   * @return true if add values that needed setting were set and are not
   *         missing 
   */
  bool setAdditionalValues(int gridIndex, int which);

  /**
   * Update count/sum averaging grids for inputs
   * @param[in] gridIndex  Index into data
   * @param[in] thresholdValue1  The value for data input 1
   * @param[in] hasValue2  True if data input 2 is not missing
   * @param[in] thresholdedValue2  The value for data input 2
   *
   * @note not called if data input 1 is missing
   * @note accesses additional input as needed at gridIndex for
   * the additional test
   */
  void updateCountSums(int gridIndex, double thresholdedValue1,
		       bool hasValue2, double thresholdedValue2);

  /**
   * Normalize all the count/sum averaging grids
   */
  void normalizeCountSums(void);

  /**
   * Run the algorithm for a particular tile and for data input 1 or 2
   * @return the pbar vector
   * @param[in] tileIndex which tile it is
   * @param[in] which 1 or 2
   */
  PbarVector runAlg(int tileIndex, int which);


  /**
   * @return true if tile is out of bounds and should use the below tile
   * @param[in] tileIndex
   * @param[out] belowTile
   */
  bool useTileBelow(int tileIndex, int &belowTile);


protected:
private:

  /**
   *  User defined parameters
   */
  ParmsPbarCompute _params;

  /**
   * Object to handle a fixed input
   */
  AdditionalInputs _additionalInputs;

  time_t _genTime;    /**< Current gen */
  int _leadTime;      /**< Current lead */

  bool _aGridSet1;  /**< True if _aGrid1 is set */
  bool _aGridModified1; /**< True if _aGrid1 is modified by an ensemble member */

  /**
   * for each configured threshold a pointer to AveragingGrids (1)
   */
  std::vector<AveragingGrids *> _aGrid1;

  bool _aGridSet2;  /**< True if _aGrid2 is set */
  bool _aGridModified2; /**< True if _aGrid2 is modified by an ensemble member */

  /**
   * for each configured threshold a pointer to AveragingGrids (2)
   */
  std::vector<AveragingGrids *> _aGrid2;


  bool _fcstComputePbarAtThresh(const TileRange &r, const Grid &fcst, 
				double &pBar) const;
};

#endif
