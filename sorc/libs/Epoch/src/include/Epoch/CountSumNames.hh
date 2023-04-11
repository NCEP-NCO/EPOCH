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
 * @file  CountSumNames.hh
 * @brief 
 * @class  CountSumNames
 * @brief 
 */

#ifndef    CountSumNames_HH
#define    CountSumNames_HH

#include <ctime>
#include <string>
#include <vector>

class ParmFcst;
class ParmProjection;
class MultiFcstGrid;

//----------------------------------------------------------------
class CountSumNames
{
public:

  /**
   * Read in ALL fields from location specified by parms at forecast time,
   * except for fields that have the 'counts' or 'sums' suffix
   * @param[in] genTime
   * @param[in] leadTime
   * @param[in] proj 
   * @param[in] parms
   * @param[out] fcsts  What is returned
   *
   * @return true for success
   */
  static bool loadNonSumCountFcsts(const time_t &genTime, int leadTime,
				   const ParmProjection &proj,
				   const ParmFcst &parms,
				   MultiFcstGrid &fcsts);
  
  /**
   * @return field name for counts data in state
   *
   * @param[in] name  Name of the underlying field
   */
  static std::string countsFieldName(const std::string &name);

  /**
   * @return field name for sums data in state
   *
   * @param[in] name  Name of the underlying field
   */
  static std::string sumsFieldName(const std::string &name);

  /**
   * @return the count/sum field names associated with a set of field namse
   *
   * @param[in] fieldNames the field names
   */
  static std::vector<std::string>
  countSumNames(const std::vector<std::string> &fieldNames);

protected:
private:  

  static const std::string _count;  /**< Suffix string for counts */
  static const std::string _sum;    /**< Suffix string for sums */

  /**
   *  vector with the two suffixes
   */
  static const std::vector<std::string> _countSum;
  
};

# endif     // CountSumNames HH
