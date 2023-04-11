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
 * @file  ConvWxFuzzy.hh
 * @brief  Fuzzy (linear) mapping from one variable to another.
 * @class  ConvWxFuzzy
 * @brief  Fuzzy (linear) mapping from one variable to another.
 */

# ifndef    CONVWX_FUZZYF_HH
# define    CONVWX_FUZZYF_HH

#include <string>
#include <vector>

//----------------------------------------------------------------
class ConvWxFuzzy
{
public:

  /**
   * Empty constructor
   */
  ConvWxFuzzy(void);

  /**
   * @brief Constructor using pairs of x,y points to define the mapping.
   *
   * @param[in] f  Pairs of values, where first = x, second = y in the
   *               function y = f(x)
   *
   * The fuzzy function then maps x values to y space.
   *
   * @note The x values should be strictly decreasing, or strictly
   *       increasing. If not, the constructor calls exit(1) (heavy handed,
   *       yes).
   */
  ConvWxFuzzy(const std::vector<std::pair<double,double> > &f);

  /**
   * Destructor
   */
  virtual ~ConvWxFuzzy(void);

  /**
   * Log contents 
   */
  void log(void);

  /**
   * Apply the fuzzy function to input value x.
   *
   * @param[in] x  An x value
   *
   * @return  Fuzzy value y corresponding to input value x
   *
   * @note  If x corresponds to one of the constructor pair x values,
   *        return the corresponding y value. Otherwise interpolate between
   *        x values defined by the constructor.
   *
   * @note  If x < smallest x in constructor, returns y corresponding
   *        to smallest x in constructor
   *
   * @note  If x > largest x in constructor, returns y corresponding
   *        to largest x in constructor
   */
  double fuzzyF(const double x) const;

  /**
   * @return Largest x value defined by the mapping
   */
  double maxX(void) const;

  /**
   * @return Minimum x value defined by the mapping
   */
  double minX(void) const;

  /**
   * @return Number of x,y pairs of points in the mapping
   */
  inline int num(void) const {return static_cast<int>(pMapping.size());}

  /**
   * Return an XML representation of the fuzzy function
   *
   * @param[in] name  Name to give this fuzzy function
   *
   * @return  A string with XML
   *
   * @note This is a way to store and the later retrieve a fuzzy function.
   */
  std::string xmlContent(const std::string &name) const;

  /**
   * Parse a string looking for the XML representation of a fuzzy function
   * with a particular name, setting the internal state accordingly
   *
   * @param[in] s  String to parse
   * @param[in] name  The name expected for the fuzzy function 
   *
   * @return  True if the input string was parsed as a fuzzy function with
   *          the input name
   *
   * @note  The string is expected to have content consistent with the
   *        output of ConvWxFuzzy::xmlContent(name)
   *
   * @note  This is a way to retrieve a previously stored fuzzy function.
   */
  bool readXml(const std::string &s, const std::string &name);

protected:
private:  

  /** 
   * The fuzzy bad value
   */
  static const double pBadValue;

  /**
   * Internal representation of the fuzzy function with x=first, y=second
   * y = f(x)
   */
  std::vector<std::pair<double,double> > pMapping;
};

# endif     // FUZZYF_HH
