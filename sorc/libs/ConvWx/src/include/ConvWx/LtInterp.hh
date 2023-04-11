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
 * @file LtInterp.hh
 * @brief Stores information for interpolating grids between two lead times
 * @class LtInterp
 * @brief Stores information for interpolating grids between two lead times
 *
 * Contains two LtInterpPoint member objects.
 */

#ifndef LT_INTERP_HH
#define LT_INTERP_HH

#include <ConvWx/LtInterpPoint.hh>
#include <string>
#include <vector>

class LtInterp
{
 public:

  /**
   * Empty constructor, creates two empty LtInterpPoint objects
   */
  LtInterp(void);

  /**
   * Constructor for one point (no actual interpolation)
   * @param[in] lt  Lead time seconds
   * @param[in] weight   Weight value
   *
   * One LtInterpPoint is set using inputs, the other is set to empty.
   */
  LtInterp(int lt, double weight);

  /**
   * Constructor for two points
   *
   * @param[in] lt0  Lead time for one point (seconds)
   * @param[in] weight0  Weight for one point
   * @param[in] lt1  Lead time for the other point (seconds)
   * @param[in] weight1  Weight for the other point
   */
  LtInterp(int lt0, double weight0, int lt1, double weight1);

  /**
   * Constructor for two LtInterpPoints
   *
   * @param[in] lt0  Point 0
   * @param[in] lt1  Point 1
   */
  LtInterp(const LtInterpPoint &lt0, const LtInterpPoint &lt1);

  /**
   * Destructor
   */
  virtual ~LtInterp(void);

  /**
   * @return a diagnostic string for the interpolation object
   */
  std::string ltString(void) const;

  /**
   * @return a different diagnostic string for the interpolation object
   */
  std::string ltString2(void) const;

  /**
   * Parse a string that should look like ltSting2() output, converting it
   * into a LtInterp object which is returned.
   *
   * @return true for success
   * @param[in] s  String to parse
   * @param[out] lt
   */
  static bool parse2(const std::string &s, LtInterp &lt);

  /**
   * @return true if i'th data is part of the interpolation
   * @param[in] i   The value 0 or 1. 
   * 
   * @note this will return false for the empty constructor, or for
   * the one point constructor when i=1, and true in all other cases.
   */
  inline bool ok(int i) const {return pPoint[i].pOk;}

  /** 
   * @return true if there are no points to interpolate (as would happen
   * with the empty constructor)
   */
  inline bool isEmpty(void) const {return !pPoint[0].pOk && !pPoint[1].pOk;}

  /**
   * @return i'th lead seconds
   * @param[in] i  The value 0 or 1
   */
  inline int getLeadSeconds(int i) const { return pPoint[i].pLeadSeconds;}

  /**
   * @return i'th weight
   * @param[in] i  The value 0 or 1
   */
  inline double getWeight(int i) const { return pPoint[i].pWeight;}

  /**
   * @return vector of all the unique lead time seconds
   */
  inline std::vector<int> getAllLeadSeconds(void) const
  {
    std::vector<int> ret;
    ret.push_back(pPoint[0].pLeadSeconds);
    if (pPoint[0].pLeadSeconds != pPoint[1].pLeadSeconds)
    {
      ret.push_back(pPoint[1].pLeadSeconds);
    }
    return ret;
  }

protected:
private:

  /**
   * Interpolation information for the two points
   */
  LtInterpPoint pPoint[2];
};


#endif
