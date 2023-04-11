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
 * @file Stat.hh 
 * @brief One statistic result
 * @class Stat
 * @brief One statistic result
 * 
 * Class used to compare skill of phase corrected and non phase corrected
 * forecasts
 */

#ifndef STAT_H
#define STAT_H
#include <string>

//----------------------------------------------------------------
class Stat 
{
public:
  /**
   * @enum Stat_t
   * @brief Types of statistics that can be done
   */
  typedef enum
  { 
    UNKNOWN=-1,    /**< Error or not yet set */
    CSI=0,         /**< Critical Success Index */
    CORRELATION=1  /**< Correlation */
  } Stat_t;

  /**
   * Constructor
   *
   * @param[in] threshold  Threshold used to build the statistic
   * @param[in] value  The statistical result
   * @param[in] weight  The weight to give this result when summing
   * @param[in] isPc  True if the result is for a phase corrected forecast
   * @param[in] statType  The type of statistic that was computed
   */
  Stat(const double threshold, const double value, const double weight,
       const bool isPc, const Stat_t statType);

  /**
   * Destructor
   */
  virtual ~Stat(void);

  /**
   * @return True if it is a statistic for a phase corrected forecast
   */
  inline bool isPc(void) const {return pIsPc;}

  /**
   * @return The weighted score, which is value*weight
   */
  inline double weightedScore(void) const {return pValue*pWeight; }

  /**
   * @return  Metadata XML string for this statistic
   */
  std::string metadata(void) const;

  /**
   * Return a string for the statistics type
   *
   * @param[in] statType The type
   *
   * @return  A string description for the input type
   */
  static std::string typeDescr(const Stat_t statType);

protected:
private:

  double pThresh; /**< threshold used to compute statistics */
  double pValue;  /**< statistical value */
  double pWeight; /**< weight associated with the value */
  bool pIsPc;     /**< True if phase correction was done */
  Stat_t pType;   /**< type of statistic */
};

#endif
