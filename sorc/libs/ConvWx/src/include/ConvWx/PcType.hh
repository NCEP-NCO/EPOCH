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
 * @file PcType.hh
 * @brief Enumerated types of phase correction and some static methods
 * @class PcType
 * @brief Enumerated types of phase correction and some static methods
 */

# ifndef    PC_TYPE_HH
# define    PC_TYPE_HH
#include <string>
#include <vector>

//----------------------------------------------------------------
class PcType
{
public:

  /**
   * @enum Pc_t 
   * @brief The types of phase correction
   */
  typedef enum
  {
    UNKNOWN=-3,                       /**< Unknown */
    NO_PC_NO_UV_OR_ZERO_UV_DATA =-2,  /**< no phase correction due to lack of
				       *   vectors */
    NO_PC_LOW_SKILL=-1,               /**< no phase correction due to no
				       *   improvement */
    NO_PC = 0,                        /**< no phase correction, reason not
				       *   given */
    PC_ARMOR_EULARIAN=1,              /**< Eularian ARMOR phase correction */
    PC_ARMOR_LAGRANGIAN=2,            /**< Lagrangian ARMOR phase correction */
    PC_NCAR=3,                        /**< Eularian NCAR phase correction */
    PC_NCAR_LAGRANGIAN=4,             /**< Lagrangian NCAR phase correction */
    PC_NCAR_EULARIAN_NO_ADVECT_UV=5   /**< NCAR Eularian phase correction,
				       * done without advection
				       * (uses extrapolation data) */
  } Pc_t;

  /**
   * @return A string describing all phase correction types, as XML.
   */
  static std::string typeDescrXml(void);

  /**
   * @return XML string for a particular phase correction type
   * @param[in] pcType  Phase correction type
   */
  static std::string xml(const Pc_t pcType);

  /**
   * Parse an XML string for phase correction type, when it is expected that
   * the string will have wanted XML content.
   *
   * @param[in] xmlStr  A string containing the tagged phase correction type
   * @return  The parsed PHASE CORRECTION type, UNKNOWN if some problem parsing
   */
  static Pc_t parseXml(const std::string &xmlStr);

  /**
   * Parse an XML string for phase correction type, when the XML for phase
   * correction type might not be in that string
   *
   * @param[in]  xmlStr  A string containing the tagged phase correction type
   *                     (or not)
   * @param[out] pcType  The parsed phase correction type if any
   * @return true  If the type was parsed
   */
  static bool parseXmlOptional(const std::string &xmlStr, Pc_t &pcType);

  /**
   * @return A string describing a particular phase correction type in a
   * non-XML format
   *
   * @param[in] pcType  The phase correction type to produce a string for
   */
  static std::string descriptor(const Pc_t pcType);

  /**
   * @return a vector of non XML string descriptions of all types, where
   * each item in the vector is a pair with first= descriptor, and 
   * second = meaning
   */
  static std::vector<std::pair<std::string,std::string> > helpDescriptors(void);

  /**
   * Replace the phase correction type in an XML string with the input type
   *
   * @param[in,out] xmlStr   The string to modify
   * @param[in] pcType  The type to use
   *
   * @return false if the string does not have content that allows this
   *         substitution
   */
  static bool replaceTypeXml(std::string &xmlStr, const Pc_t pcType);

  /**
   * Replace the type descriptor in an XML string with the type description
   * defined by PcType::typeDescrXml()
   *
   * @param[in,out] xmlStr the string to modify
   * @return false if the string does not have content that allows this
   *         substitution
   */
  static bool replaceTypeDescriptorXml(std::string &xmlStr);

  /**
   * @return A string with format 'correction=number'
   * @param[in] pcType  A phase correction type
   *
   * number is the integer enumerated value
   */
  static std::string correctionString(const Pc_t pcType);

  /**
   * @return A string with format 'correction=Number' when there is no
   * correction
   *
   * Number is the number that goes with enumerated value NONE
   */
  static std::string correctionStringNone(void);

  /**
   * @return true if the input phase correction type actually involves a
   * phase correction
   *
   * @param[in] pcType  Phase correction type
   */
  static bool doesPc(const Pc_t pcType);

  /**
   * @return the static XML tag for phase correction type
   */
  inline static std::string tag(void) {return pPcTag;}

protected:
private:  

  /** 
   * The static tag used in XML for phase correction type
   */
  static const std::string pPcTag;
};

# endif     // PcType_HH
