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
 * @file  FixedFcstThreshInfo.hh
 * @brief  Information about a fixed threshold associated with a forecast
 *         which is field name and threshold.
 * @class  FixedFcstThreshInfo
 * @brief  Information about a fixed threshold associated with a forecast
 *         which is field name and threshold.
 *  
 * Contains:
 *  - fieldName  Name of the field
 *  - threshold  Threshold value
 */

# ifndef    FIXED_FCST_THRESH_INFO_HH
# define    FIXED_FCST_THRESH_INFO_HH

#include <ctime>
#include <string>
#include <vector>

//----------------------------------------------------------------
class FixedFcstThreshInfo
{
public:

  /**
   * Empty constructor
   */
  FixedFcstThreshInfo(void);

  /**
   * Constructor that copies in all members
   *
   * @param[in] name  Field name
   * @param[in] thresh  Threshold
   *
   */
  FixedFcstThreshInfo(const std::string &name, double thresh);

  /**
   * Constructor that interprets input string as XML which is parsed
   *
   * @param[in] xml  String to parse
   */
  FixedFcstThreshInfo(const std::string &xml);

  /**
   * Destructor
   */
  virtual ~FixedFcstThreshInfo(void);

  /**
   * Debug print to stdout
   */
  void print(void) const;

  /**
   * @return XML representation of content
   */
  std::string getXml(void) const;

  /**
   * Parse input string as XML to fill in state
   *
   * @param[in] xml  String to parse
   *
   * @return true for ok
   */
  bool parseXml(const std::string &xml);

  /**
   * @return The XML tag
   */
  inline static std::string fixedFcstThreshTag(void) {return _tag;}

  /**
   * Operator==
   * @param[in] f
   * @return true if the same
   */
  inline bool operator==(const FixedFcstThreshInfo &f) const
  {
    return f._field == _field && f._thresh == _thresh;
  }
  
  /**
   * @return true if names are same but thresholds are different
   * @param[in] f  Object to compare with
   */
  inline bool nameMatchThreshDoesntMatch(const FixedFcstThreshInfo &f) const
  {
    return f._field == _field && f._thresh != _thresh;
  }
  
  /**
   * @return true if input name matches local name
   * @param[in] name
   */
  inline bool nameMatch(const std::string &name) const
  {
    return _field == name;
  }

  /**
   * @return the field name
   */
  inline std::string getName(void) const {return _field;}

  /**
   * @return the threshold
   */
  inline double getThresh(void) const {return _thresh;}

  /**
   * @return true if object is all right
   */
  inline bool isOk(void) const {return _ok;}

protected:
private:  

  bool _ok;            /**< True if object is good */
  std::string _field;  /**< Field name */
  double _thresh;      /**< Threshold */

  /** 
   * The fixed XML tag
   */
  static const std::string _tag;
};

# endif     // FixedFcstThreshInfo HH
