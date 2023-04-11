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
 * @file ParmPcAlg.hh
 * @brief Parameters used in applying phase correction to data
 * @class ParmPcAlg
 * @brief Parameters used in applying phase correction to data
 *
 * Any members that are hardwired are const, all are public by design
 */

# ifndef    PARM_PC_ALG_HH
# define    PARM_PC_ALG_HH

//----------------------------------------------------------------
class ParmPcAlg
{
public:

  /**
   * Empty (invalid) constructor
   */
  ParmPcAlg(void);

  /**
   * Constructor with all parameters passed in,
   *    
   * @param[in] smooth  Smoothing box size
   * @param[in] useHighSmooth  True to do 'high' smoothing
   * @param[in] smoothWhereCorrected  True to smooth only where phase
   *                                  corrected
   * @param[in] gapfill  Gap fill box size
   * @param[in] zeroRes  Values smaller than this are set to 0.0
   * @param[in] alwaysPhaseCorrect  True to do phase correction even when the
   *                                skill is low
   * @param[in] excludeWhenSmoothing True to exclude value when smoothing
   * @param[in] smoothExclude  Value to exclude when excludeWhenSmoothing=true
   */
  ParmPcAlg(const int *smooth, const bool useHighSmooth,
	    const bool smoothWhereCorrected, const int *gapfill,
	    const double zeroRes, const bool alwaysPhaseCorrect=true,
	    const bool excludeWhenSmoothing=false,
	    const double smoothExclude=0.0);

  /**
   * Destructor
   */
  virtual ~ParmPcAlg(void);

  int pSmooth[2];              /**< Smoothing box size */
  bool pUseHighSmooth;         /**< True to do 'high' smoothing */
  bool pSmoothWhereCorrected;  /**< True to smooth only where phase corrected */
  int pGapfill[2];             /**< Gap fill box size */
  double pZeroResolution;      /**< Values smaller than this are set to 0.0 */

  /**
   * True to compute phase correction even when skill is low
   */
  bool pAlwaysPhaseCorrect;

  /**
   * if true, a value is excluded from the smoothing steps done after phase
   * correction. 
   */
  bool pExcludeWhenSmoothing;  

  /**
   * A value to exclude from the smoothing steps done after phase correction,
   * when pExcludeWhenSmoothing=true.
   */
  double pSmoothExclude;

protected:
private:  
};

# endif
