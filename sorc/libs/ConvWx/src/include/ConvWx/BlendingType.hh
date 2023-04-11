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
 * @file BlendingType.hh
 * @brief  Allowed blending types
 * @class BlendingType
 * @brief  Allowed blending types
 *
 * This is a class with static members only (no internal state).
 *
 */

# ifndef    BLENDING_TYPE_HH
# define    BLENDING_TYPE_HH
#include <string>
#include <vector>

//----------------------------------------------------------------
class BlendingType
{
public:

  /**
   * @enum Blend_t
   * @brief The types of blending
   */
  typedef enum
  {
    UNKNOWN=-1,      /**< Type is not known */
    MISSING=0,       /**< Data is all missing */
    BLENDED=1,       /**< Data is a blend of extrapolation and model data */
    EXTRAP_ONLY=2,   /**< Data is extrapolation data only */
    MODEL_ONLY=3     /**< Data is model data only */
  } Blend_t;

  /**
   * @return  XML string describing all the blending types:
   *          "<forecast_descr>#=meaning,..,#=meaning</forecast_descr>"
   */
  static std::string typeDescrXml(void);

  /**
   * @return  XML string for a particular blending type
   *
   * @param[in] blendType  The type in question
   *
   * The integer value for the type is given the 
   * pBlendingTypeTag
   */
  static std::string xml(const Blend_t blendType);

  /**
   * Parse an XML string for a blending type, expecting it to be present, with
   * the pBlendingTypeTag.
   *
   * @param[in] xmlString  The XML string to parse
   * @return The blending type found in the string
   */
  static Blend_t parseXml(const std::string &xmlString);

  /**
   * Parse an XML string for a blending type, with the pBlendingTypeTag,
   * when it might not be present.
   *
   * @param[in] xmlString  The XML string to parse
   * @param[out] blendType  The blending type found in the string, if any
   *
   * @return  True if found a blending type tag and got a value
   */
  static bool parseXmlOptional(const std::string &xmlString,
			       Blend_t &blendType);

  /**
   * @return  A string describing a particular blending type, non-XML format
   *
   * @param[in] blendType  The type in question
   */
  static std::string descriptor(const Blend_t blendType);

  /**
   * @return pairs of string descriptors/meaning for all blending types, with
   *         the first thing in each pair the descriptor, the second thing
   *         in each pair the meaning
   */
  static std::vector<std::pair<std::string,std::string> > helpDescriptors(void);

  /**
   * Return a string describing the integer value for a particular blending type
   *
   * @param[in] blendType  A blending type
   *
   * @return  String of format 'blending=#'
   */
  static std::string blendingDescriptionString(const Blend_t blendType);

  /** 
   * Returns a string describing BlendingType::UNKNOWN
   *
   * @return  "blending=-1"
   */
  static std::string blendingDescriptionStringNone(void);

  /**
   * @return  The XML tag used for blending type
   */
  inline static std::string blendingTypeTag(void) {return pBlendingTypeTag;}

protected:
private:  

  /** 
   * The fixed XML tag
   */
  static const std::string pBlendingTypeTag;
};

# endif     // BlendingType_HH
