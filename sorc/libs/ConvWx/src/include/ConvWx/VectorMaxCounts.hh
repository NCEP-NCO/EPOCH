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
 * @file VectorMaxCounts.hh
 * @brief simple counts of various things that happen in the VectorMax
 * algorithm used in UvOutput
 * @class VectorMaxCounts
 * @brief simple counts of various things that happen in the VectorMax
 * algorithm used in UvOutput
 *
 * Members are public because its a struct-like class
 */

# ifndef    VECTOR_MAX_COUNTS_HH
# define    VECTOR_MAX_COUNTS_HH
#include <string>
#include <ConvWxIO/ILogMsg.hh>

//----------------------------------------------------------------
class VectorMaxCounts
{
public:

  /**
   * Empty constructor
   */
  VectorMaxCounts(void);

  /**
   * Destructor
   */
  virtual ~VectorMaxCounts(void);

  /**
   * Initialize counters after the first step
   */
  void init(void);

  /**
   * Debug log output of the various counts
   * @param[in] severity  ILogMsg severity
   */
  void log(LogStream::Log_t severity) const;

  double pNumNoPreviousStat; /**< Count of points with no previous stats */
  double pNumAtLowestValue;  /**< Count of points where lowest value is found */
  double pNumWherePreviousIsOldest; /**< Count of points where previous = oldest (redo) */
  double pNumWherePreviousNotOldest; /**< Count of points where previous is not oldest */
  double pNumTotal;       /**< Total count of points */
  double pNumWithRedo;   /**< Total count of points where redo happened */

protected:
private:  

};

# endif     // VectorMaxCounts

