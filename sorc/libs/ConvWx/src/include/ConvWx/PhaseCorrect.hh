// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>
/**
 * @file PhaseCorrect.hh
 * @brief Algorithm for Phase correction 
 * @class PhaseCorrect
 * @brief Algorithm for Phase correction 
 *
 * Given a set of data types, with verifying observation data and forecasts,
 * for each type, we can do phase correction
 * - Reduce to a lower resolution for computations
 * - if (more than 1% of the data (observation or forecast) of any type
 *   is above a threshold)
 *     - compute the motion at the lower resolution
 *        - for each volume (volX by volY moving dx,dy)
 *          - create a Box object and compute best phase correction
 *          - for Grid: take average over overlapping boxes at each point
 *            as the phase correction
 *          - smooth that grid
 *          - do the ConvThresh convergence thresholding motion reduction
 *            algorithm to get final motion
 *      - interpolate back to high resolution
 *      - multiply by a weight grid
 *      - smooth the results
 *      - expand by some number of points
 *      - phase shift the forecasts 
 *      - smooth the results
 *      - set phase correction type to PC_NCAR
 * - else
 *     - no phase correction
 */

# ifndef    PHASE_CORRECT_HH
# define    PHASE_CORRECT_HH

#include <vector>
#include <ConvWxIO/ConvWxMultiThreadMgr.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include <ConvWx/MultiGridsForPc.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/ConvThresh.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/PcMotion.hh>
#include <ConvWx/PcStats.hh>

class ParmProjection;
class PcInput;
class PcVolume;
class UvUnits;

//----------------------------------------------------------------
class PhaseCorrect
{
public:

  /**
   * Default constructor
   * @param[in] pcInputs  The input grids, one for each data type
   * @param[in] weightGrid  A weight grid computed externally
   * @param[in] parms  Parameters for setting phase correction
   * @param[in] units  Units of the phase correction
   */
  PhaseCorrect(const std::vector<PcInput> &pcInputs, const Grid &weightGrid,
	       const ParmSetUV &parms, const UvUnits &units);


  /**
   * Create a copy of the input object at a lower resolution.
   *
   * @param[in] original  The object to copy
   * @param[in] reduction  Resolution reduction 1 = none, 2 = twice, etc.
   */
  PhaseCorrect(const PhaseCorrect &original, const int reduction);
  
  /**
   * Destructor
   */
  virtual ~PhaseCorrect(void);
  
  /**
   * Compute the phase correction.
   *
   * @param[in] proj  Projection information needed to scale motions
   *
   * Computes phase correction using the local state as set in the constructor
   * When done, phase correction is in pInitialMotion and pFinalMotion grids,
   * and pGrids have been modified. 
   */
  void computePhaseCorrect(const ParmProjection &proj,
			   int lt,
			   ConvWxMultiThreadMgr *threads=NULL);

  /**
   * Set pInitialMotion and pFinalMotion grids to all zero (no phase correction)
   */
  void setNoMotion(void);

  /**
   * Interpolate from local (low res) PhaseCorrect object to a higher
   * resolution object, which is returned.
   *
   * @param[out] highresData  High res returned data
   * @param[in] resolution  How much higher the resolution is 1=same,
   *                        2 = twice, ...
   *
   * @note Fatal error exit if input is not as expected.
   */
  void interpolate(PhaseCorrect &highresData, int resolution) const;

  /**
   * Return the state of phase correction, to be used for output
   *
   * @param[in] proj  Projection used to change units in some cases
   * @param[out] uGrid  The U component of phase correction 
   * @param[out] vGrid  The V component of phase correction 
   * @param[out] metadata  The metadata associated with this phase correction
   */
  void getOutput(const ParmProjection &proj, Grid &uGrid, Grid &vGrid, 
		 MetaData &metadata, PcStats &stats) const;

  /**
   * @return a copy of the final motion vectors
   */
  UvOutput getFinalMotion(void) const;

  /**
   * Instantiation of the compute method used by ConvWxThreadMgr
   *
   * @param[in] info  Pointer to a PcComputeInfo object, cast to void
   *
   * Invokes the PcCompute runAlg method and writes out results
   *
   * @note as specified in the ConvWxThreadMgr documentation, this method
   *       deletes the ti pointer when the computations are completed
   */
  static void compute(void *info);

  static std::string leadThreadKey(int lt);

protected:
private:  

  /**
   * The grids uses to do the phase correction, created from inputs
   */
  MultiGridsForPc pGrids;

  /**
   * A weight grid.
   */
  Grid pWeightGrid;

  /**
   * Convergence thresholding algorithm to reduce local PcMotion object
   */
  ConvThresh pConvThresh;

  /**
   * Phase correction setting parameters
   */
  ParmSetUV pParm;

  /**
   * Phase correction vectors without convergence thresholding
   */
  PcMotion pInitialMotion;

  /**
   * Phase correction vectors with convergence thresholding
   */
  PcMotion pFinalMotion;


  /**
   * Stats for phase correct computation
   */
  PcStats pStats;

  /**
   * Compute pInitialMotion and pFinalMotion grids from the local state.
   *
   * @param[in] proj  Projection information needed to scale motions
   */
  void pComputeMotion(const ParmProjection &proj);
  void pComputeMotionThreaded(const ParmProjection &proj,
			      int lt, 
			      ConvWxMultiThreadMgr *threads);

  /**
   * @class PhaseCorrectInfo
   * @brief info class used for threading the phase correction of each box
   */
  class PhaseCorrectInfo
  {
  public:
    /**
     * Constructor
     * @param[in] box  Pointer to this box
     * @param[in] lt  Lead time seconds
     * @param[in] alg  Pointer to the algorithm class
     */
    inline PhaseCorrectInfo(PcVolume *box, int lt, const PhaseCorrect *alg) :
      pBox(box), pLt(lt), pAlg(alg) {}

    /**
     * Destructor
     */
    inline ~PhaseCorrectInfo(void) {}

    PcVolume *pBox;              /**< The box */
    int pLt;
    const PhaseCorrect *pAlg;    /**< The algorithm */
  protected:
  private:
  };

};

# endif
