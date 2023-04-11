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
 * @file ParmPc.hh
 * @brief Parameters used in applying phase correction to data.
 * @class ParmPc
 * @brief Parameters used in applying phase correction to data.
 *
 * Any members that are hardwired are const, all are public by design.
 *
 * The base class ParmPcAlg has the algorithm parameters, this derived
 * class has parameters for the data that is phase corrected and the UV
 * phase correction vecctors
 */

# ifndef    PARM_PC_HH
# define    PARM_PC_HH

#include <ConvWx/ParmFcst.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmPcAlg.hh>
class Grid;

//----------------------------------------------------------------
class ParmPc : public ParmPcAlg
{
public:

  /**
   * Empty (invalid) constructor
   */
  ParmPc(void);

  /**
   * Constructor with all parameters passed in.
   * @param[in] modelParms  Parameters for input model data 
   * @param[in] uvParms  Input U/V phase correction vector data parameters
   * @param[in] vFieldname  Name of "V" data in input phase
   *                        correction data
   * @param[in] outputParms  Parameters for output phase corrected data
   * @param[in] proj  The data projection
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
   *
   * @note pDxdy is set outside of the constructor
   */
  ParmPc(const ParmFcst &modelParms, const ParmFcst &uvParms,
	 const char *vFieldname, const ParmFcst &outputParms,
	 const ParmProjection &proj, const int *smooth,
	 const bool useHighSmooth, const bool smoothWhereCorrected, 
	 const int *gapfill, const double zeroRes,
	 const bool alwaysPhaseCorrect=true,
	 const bool excludeWhenSmoothing=false,
	 const double smoothExclude=0.0);

  /**
   * Destructor
   */
  virtual ~ParmPc(void);

  /**
   * @return  Grid created for U or V using input field name and local
   *          parameters
   * @param[in] fieldname  Name to use
   *
   */
  Grid uvGrid(const std::string &fieldname) const;

  ParmFcst pModel;          /**< input model data */
  ParmFcst pIncomingUv;     /**< input U/V phase correction vector data */
  std::string pVFieldname;  /**< name of "V" data in input UV 
			     *  phase correction data  */
  ParmFcst pOutput;          /**< output data */
  ParmProjection pProj;      /**< the data projection */
  double pDxdy[2];           /**< Grid resolution from the projection (km) */

protected:
private:  
};

# endif
