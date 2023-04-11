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
 * @file PcScore.hh
 * @brief Variables used for scoring phase correction quality
 * @class PcScore
 * @brief Variables used for scoring phase correction quality
 *
 * The variables used by the scoring algorithm are:
 *
 *  -  pDiffsq = sum of (fcst - obs)^2
 *  -  pNum = number of points summed in pDiffsq
 *  -  pDvar = currently a hardwired param, could be sum of (data - mean)^2
 *  -  pAlpha = algorithm parameter (see description of phase correction)
 */

# ifndef    PC_SCORE_HH
# define    PC_SCORE_HH

#include <string>

//----------------------------------------------------------------
class PcScore
{
public:

  /**
   * Default constructor, initialize the variables to 0
   */
  PcScore(void);
  
  /**
   * Destructor
   */
  virtual ~PcScore(void);

  /** 
   * Set hardwired local values to inputs
   *
   * @param[in] variance  Value to use for pDvar
   * @param[in] alpha   Value to use for pAlpha
   * @param[in] name  Name to give this object
   *
   * Set the other values to 0 (pDiffsq, pNum)
   */
  void init(const double variance, const double alpha, const std::string &name);

  /** 
   * Currently has no effect, because variance is hardwired
   * @param[in] data  A value
   * @param[in] mean  The mean data value
   */
  void incrementVariance(const double data, const double mean);

  /**
   * Increment pDiffsq by (fdata - ddata)^2, and increment pNum by 1
   * @param[in] fdata  Forecast data value
   * @param[in] ddata  Data value
   */
  void incrementDiffsq(const double fdata, const double ddata);

  /**
   * Adjust the arguments using internal score variables.
   *
   * @param[in,out] sumScore  Incremented by pAlpha*pNum on exit
   * @param[in,out] sumAlpha  Incremented by pAlpha*pDiffsq/(pDvar*pDvar)
   * @param[in,out] debugMsg  Append description of the action taken to this
   *                          string
   *    
   * @return true for success (when pNum > 0) false for no action
   *
   * This method allows more than one kind of data to go into the score for
   * phase correction, with a call to this method for each data type.
   */
  bool incrementScoreVars(double &sumScore, double &sumAlpha,
			  std::string &debugMsg) const;

protected:
private:  

  std::string pName; /**< name for this object */
  double pDiffsq;    /**< sum of (fcst - obs)^2 */
  double pNum;       /**< number of points in Diffsq */
  double pAlpha;     /**< algorithm adjustable parameter */
  double pDvar;      /**< could be sum of (data - mean)^2, but is hardwired */
};

# endif
