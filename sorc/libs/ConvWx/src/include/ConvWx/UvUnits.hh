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
 * @file UvUnits.hh
 * @brief Units for phase correction vectors
 * @class UvUnits
 * @brief Units for phase correction vectors
 */

# ifndef    UV_UNITS_HH
# define    UV_UNITS_HH
#include <string>

//----------------------------------------------------------------
class UvUnits
{
public:

  /**
   * @enum UvUnits_t 
   * @brief The types of units
   */
  typedef enum
  {
    UNKNOWN=-3,      /**< unknown, i.e. not set */
    PURE_PC_GRIDPTS, /**< spatial phase correction number of grid points
		      * (no time)*/
    GRIDPTS_PER_DT_SECONDS, /**< Number of grid points over some interval of
			     * time */
    METERS_PER_SECOND       /**< 'external' units, for phase corrections
			     * passed between apps */
  } UvUnits_t;

  /**
   * Set units to unknown
   */
  UvUnits(void);

  /**
   * Constructor for GRIDPTS_PER_DT_SECONDS option
   * @param[in] dtSeconds  The number of seconds to use 
   */
  UvUnits(const int dtSeconds);

  /**
   * Constructor for PURE_PC_GRIDPTS and METERS_PER_SECOND options.
   * @param[in] type  Units object to use, expect PURE_PC_GRIDPTS or
   *               METERS_PER_SECOND.
   *
   * @note If type is not one of the two expected values, it is set to UNKNOWN
   */
  UvUnits(const UvUnits_t &type);

  /**
   * Destructor
   */
  virtual ~UvUnits(void);

  /**
   * operator== 
   * @param[in] u   Object to compare
   *
   * @return true for equality
   */
  bool operator==(const UvUnits &u) const;

  /**
   * Retrieve the units enumerated value that is local to this object.
   * @param[out] units  Units to return
   * @return true if u is not UNKNOWN
   */
  bool getUnits(UvUnits_t &units) const;

  /**
   * @return XML string for units from the internal state
   */
  std::string xml(void) const;

  /**
   * @return A string describing internal state, non-XML format
   */
  std::string briefDescr(void) const;

  /**
   * @return A string name for the enumerated units
   * @param[in] type  Enumerated units value
   */
  static std::string enumName(const UvUnits_t type);

  /**
   * @return Enumerated units parsed from a string name
   * @param[in] str  The name to parse
   */
  static UvUnits_t enumValue(const std::string &str);

  /**
   * @return An XML string for a UvUnits object
   *
   * @param[in] units  The object from which to produce the XML string
   */
  static std::string xml(const UvUnits &units);

  /**
   * Parse an XML string for UvUnits, when it is expected the string is XML
   * that has units content
   *
   * @param[in] xmlStr  String containing the XML
   *
   * @return The parsed units, UNKNOWN if failure
   */
  static UvUnits parseXml(const std::string &xmlStr);

  /**
   * Parse an XML string for UvUnits, which the string may or may not have 
   * units content
   * @param[in] xmlStr  String containing XML (or not)
   * @param[out] units  The parsed units (if any)
   *
   * @return true if found units XML and was able to parse it
   */
  static bool parseXmlOptional(const std::string &xmlStr, UvUnits &units);

  /**
   * @return A string describing a particular units type, non-XML format
   * @param[in] units  The units to produce a descriptor for
   */
  static std::string descriptor(const UvUnits &units);

  /**
   * @return The fixed XML tag for units
   */
  inline static std::string unitsTag(void) {return pUvUnitsTag;}

  /**
   * @return The fixed XML tag for "DtSeconds"
   */
  inline static std::string dtTag(void) {return pUvUnitsDtTag;}

protected:

  UvUnits_t pUnits; /**< The enumerated units */
  int pDtSeconds;   /**< The # of seconds */

private:  

  static const std::string pUvUnitsTag; /**< The tag used in XML for units */
  static const std::string pUvUnitsDtTag; /**< The tag used in XML for dt
					   * seconds */

};

# endif     // UvUnits_HH
