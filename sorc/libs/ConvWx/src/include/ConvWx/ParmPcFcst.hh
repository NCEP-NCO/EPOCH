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
 * @file ParmPcFcst.hh
 * @brief Parameters for forecast data used in Phase Correction
 * @class ParmPcFcst
 * @brief Parameters for forecast data used in Phase Correction
 *
 * Several data types are allowed in phase correction, one of these
 * sets of parameters must be defined for each such type.
 *
 * Any members that are hardwired are const, all are public by design
 */

# ifndef    PARM_PC_FCST_HH
# define    PARM_PC_FCST_HH
# include <ConvWx/ParmFcst.hh>

//----------------------------------------------------------------
class ParmPcFcst
{
public:

  /**
   * Constructor empty 
   */
  ParmPcFcst(void);

  /**
   * Constructor which passes in values for all public members except for
   * pVerif and pFcst which are set later.
   * @param[in] name  Informative name of this type of data
   * @param[in] thresh Threshold used in phase correction
   * @param[in] fareaDthresh  See external documentation for meaning
   * @param[in] alpha   See external documentation for meaning
   * @param[in] variance   See external documentation for meaning
   */
  ParmPcFcst(const char *name, const double thresh, const double fareaDthresh,
	     const double alpha, const double variance);

  /**
   * Destructor
   */
  virtual ~ParmPcFcst(void);

  /**
   * Set the pVerif and pFcst parameters, as well as pVerifIsObs
   *
   * @param[in] verifParms  Values to put into pVerif
   * @param[in] fcstParms  Values to put into pFcst
   * @param[in] verifIsObs   Value to use for pVerifIsObs
   */
  void setVerifAndFcst(const ParmFcst &verifParms, const ParmFcst &fcstParms,
		       const bool verifIsObs);


  std::string pName;          /**< informative name of this type of data */
  double pThresh;             /**< threshold used in phase correction */
  ParmFcst pVerif;            /**< verifying (observation) data parameters */
  ParmFcst pFcst;             /**< forecast data parameters */
  double pFractionalAreaDataThresh; /**< see external documentation */
  double pAlpha;              /**< see external documentation */
  double pVariance;           /**< see external documentation */

protected:

  bool pVerifAndFcstSet; /**< true if the verifying and forecast parameters
			  *    are set */
  bool pVerifIsObs;      /**< true if the verifying data is observations data */

private:  

};

# endif
