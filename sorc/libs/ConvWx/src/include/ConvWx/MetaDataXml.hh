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
 * @file MetaDataXml.hh
 * @brief ConvWx specific metadata which can be represented as XML
 * @class MetaDataXml
 * @brief ConvWx specific metadata which can be represented as XML
 *
 * This is a fixed set of XML tagged variables, any subset of which
 * can be present. The class also allows 'free form' XML content of arbitrary
 * content
 *
 * The XML tags are the static const strings in the private section.
 * The content associated with each tag can be found nearby.
 */

# ifndef    METADATA_XML_HH
# define    METADATA_XML_HH

#include <string>
#include <ConvWx/PcType.hh>
#include <ConvWx/BlendingType.hh>
#include <ConvWx/UvUnits.hh>

//----------------------------------------------------------------
class MetaDataXml
{
public:

  /**
   * Empty constructor (no metadata)
   */
  MetaDataXml(void);

  /**
   * Constructor from a string to be parsed
   *
   * @param[in] xmlStr  String to parse, presumably valid XML
   *
   * @note this does not support the 'free form' XML, which must be added using
   * the method addFreeformXml()
   */
  MetaDataXml(const std::string &xmlStr);

  /**
   * Constructor for metadata that has phase correction type only.
   *
   * @param[in] phaseCorrectType Phase correction type
   */
  MetaDataXml(const PcType::Pc_t phaseCorrectType);

  /**
   * Destructor
   */
  virtual ~MetaDataXml(void);

  /**
   * Operator==
   *
   * @param[in] m  Object to compare to
   *
   * @return true for equality
   */
  bool operator==(const MetaDataXml &m) const;

  /**
   * 'Operator= lite'
   *
   * @param[in] m  Object to compare
   *
   * @return true for similarity
   */
  bool similar(const MetaDataXml &m) const;

  /**
   * Write the differences (if any) between the input and local object to a
   * string
   *
   * @param[in] m  Object to compare
   * 
   * @return the string with differences
   */
  std::string printDifferences(const MetaDataXml &m) const;

  /**
   * Print major differences (if any) between the input and local object to a
   * string
   *
   * @param[in] m Object to compare
   *
   * @return the string with differences
   *
   * @note Everything that causes the similar() method to fail is a major
   *       difference.
   */
  std::string printMajorDifferences(const MetaDataXml &m) const;

  /** 
   * @return Length in bytes of the object when represented as an XML string
   */
  inline int xmlSize(void) const {return static_cast<int>(pXmlContent.size());}

  /**
   * @return  True if there is any metadata
   */
  inline bool nonEmpty(void) const {return xmlSize() > 0;}

  /**
   * @return The C string representation of the metadata represented as XML.
   */
  inline const char *cStr(void) const {return pXmlContent.c_str();}

  /**
   * @return The string representation of the metadata represented as XML.
   */
  std::string xmlString(void) const;

  /**
   * Write the string XML representation of the metadata to stdout.
   */
  void printXmlString(void) const;

  /**
   * Return  A one line description (not XML) of the metadata
   */
  std::string oneLineString(void) const;

  /**
   * Return a debug non-XML representation of the metadata
   */
  std::string debugDescription(void) const;


  /**
   * Set metadata for input UV into the state, overwriting if need be with
   * a warning. Input UV metadata consists of a path and latency (Hours)
   * 
   * @param[in] path           Path for the UV input
   * @param[in] latencyHours   Latency hours
   */
  void addInputUv(const std::string &path, const double latencyHours);

  /**
   * Set metadata for Calibrate into the state, overwriting if need be
   * with a warning. Calibrate metadata consists of a forecast path and an
   * observation path
   *
   * @param[in] fcstPath  The forecast path
   * @param[in] obsPath   The observations path
   */
  void addCalibrate(const std::string &fcstPath, const std::string &obsPath);

  /**
   * Set metadata for input model data, overwriting if need be with a warning.
   * Input model metadata consists of a path and latency (Hours)
   * 
   * @param[in] path          Path 
   * @param[in] latencyHours  Latency hours
   */
  void addModelInput(const std::string &path, const double latencyHours);

  /**
   * Set metadata for model data, overwriting if need be with a warning.
   * Model metadata consists of a path
   *
   * @param[in] path  Path
   */
  void addModel(const std::string &path);

  /**
   * Set metadata for extrapolation data, overwriting if need be with a warning.
   * Extrapolation metadata consists of a path
   *
   * @param[in] path  Path
   */
  void addExtrap(const std::string &path);

  /**
   * Set metadata for extrapolation data, overwriting if need, no warning.
   * Extrapolation metadata consists of a path
   *
   * @param[in] path  Path
   */
  void replaceExtrap(const std::string &path);

  /**
   * Set metadata for a blending type, overwriting if need be with warning.
   *
   * @param[in] blendingType  The blending type to use in the metadata
   */
  void addBlending(const BlendingType::Blend_t blendingType);

  /**
   * Set metadata for skill, overwriting if need be with warning.
   * Skill metadata consists of skill measurement values with and without
   * phase correction.
   *
   * @param[in] noPcSkill The skill without phase correction
   * @param[in] yesPcSkill The skill with phase correction
   */
  void addSkill(const double noPcSkill, const double yesPcSkill);

  /**
   * Set metadata for phase correction weight, overwriting if need be with
   * a warning.  Phase correction weight metadata consists of a weight value.
   *
   * @param[in] weight  Weight value
   */
  void addPcWeight(const double weight);

  /**
   * set metadata for climatology, overwriting if need be with a warning.
   * Climatology metadata consists of the number of days used.
   *
   * @param[in] daysInAve   Number of days used in the climatology averages 
   */
  void addDaysInClimo(const double daysInAve);

  /**
   * Replace or set metadata for model input.  
   *
   * @note This is same as the addModelInput() method without warning messages
   *
   * @param[in] modelPath   Path
   * @param[in] latencyHours  Latency hours
   */
  void replaceModelInput(const std::string &modelPath,
			 const double latencyHours);

  /**
   * Replace or set metadata for phase correction type.  Give no warning
   * message when an existing value is overwritten.
   *
   * @param[in] pcType  Phase correction type
   */
  void replacePcType(const PcType::Pc_t pcType);

  /**
   * @return  Phase correction type found in the metadata. If phase correction
   *          type is not set in metadata, returns PcType::UNKNOWN
   */
  PcType::Pc_t getPcType(void) const;

  /**
   * @return  String representation of the phase correction type (non XML)
   *          found in the metadata.  If phase correction type is not set
   *          in the metadata, return the string associated with PcType::UNKNOWN
   */
  std::string getPcDescr(void) const;

  /**
   * Get input model latency hours from the metadata
   *
   * @return true if the metadata contains input model latency 
   * @param[out] latency   Value to return if metadata does contain input model
   *                       latency
   */
  bool getModelLatencyHours(double &latency) const;

  /**
   * Get input UV latency hours from the metadata
   *
   * @return true if the metadata contains input UV latency 
   * @param[out] latency   Value to return if metadata does contain input UV
   *                        latency
   */
  bool getInputUvLatencyHours(double &latency) const;

  /**
   * Get skill values from the metadata
   *
   * @return true if the metadata contains skill
   * @param[out] noPcSkill  Skill without phase correction to return if metadata
   *                        does contain input UV
   * @param[out] yesPcSkill  Skill with phase correction to return if metadata
   *                         does contain input UV
   */
  bool getSkill(double &noPcSkill, double &yesPcSkill) const;

  /**
   * Get phase correction weight from the metadata
   *
   * @return true if the metadata contains phase correction weight
   * @param[out] weight  Weight to return if metadata does contain phase
   *                     correction weight
   */
  bool getPcWeight(double &weight) const;

  /**
   * Get number of days used in the climatology averages from the metadata
   *
   * @return true if the metadata contains climatology.
   * @param[out] daysInAve  Number of days used in the climatology averages 
   *                        to return if metadata does contain climatology
   */
  bool getClimoDays(double &daysInAve) const;

  /**
   * Replace or set the metadata for UvUnits. Do not give a warning message
   * when overwriting and existing value.
   * @param[in] units  Units to use
   */
  void replaceUvUnits(const UvUnits &units);

  /**
   * @return The UvUnits object in the local metadata, if it is set.
   *         If it is not set in the metadata, return UvUnits::UNKNOWN.
   */
  UvUnits getUvUnits(void) const;

  /**
   * Merge input into the local state by taking a union.
   *
   * If both local and input share the same field, the local value is
   * overwritten with the input, and a warning message is printed.
   *
   * @param[in] mergeData  The object to merge into the local state
   */
  void merge(const MetaDataXml &mergeData);

  /**
   * Append the input to the 'free form' XML content.
   *
   * @param[in] xmlStr  The string to append, assumed to be valid XML.
   *
   * @note free form means any string that is XML, and should not be XML
   *       that contains any fixed known MetaDataXml key value
   */
  void addFreeformXml(const std::string &xmlStr);

  /**
   * Check content of incoming phase correction UV metadata. 
   * In this metadata a model latency is expected.
   *
   * @param[out] latencySeconds  Returned model latency (seconds) if it is found
   *
   * @return true if the metadata has input model from which the latency
   *         can be extracted.
   */
  bool checkInputUV(int &latencySeconds) const;

  /**
   * @return true If member pHasModelData is true,
   *  with the path metadata value in the returned string
   * 
   * @param[out] path
   */
  bool getModelDataPath(std::string &path) const;

  /**
   * @return true if member pHasModelData is true,
   * with the path metadata value parsed into the returned time values
   * @param[out] genTime  Generation time
   * @param[out] lt       Lead seconds
   */
  bool getModelGenLead(time_t &genTime, int &lt) const;

  /**
   * @return a copy of the free form XML string
   */
  inline std::string getFreeformXml(void) const {return pFreeformXml;}

protected:
private:  

  //---------------------------------------------------------------------
  /**
   * @name PhaseCorrection
   *
   * Metadata for phase correction type. 
   * @note the XML tag comes from PcType
   * @{
   */
  bool pHasPcType;       /**< true if metadata has phase correction type */
  PcType::Pc_t pPcType;  /**< The phase correction type, when pHasPcType=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name BlendingType
   *
   * Metadata for blending type. 
   * @note the XML tag comes from BlendingType
   * @{
   */
  bool pHasBlendType;               /**< true if metadata has blending type */
  BlendingType::Blend_t pBlendType; /**< The blending type, when
				     *    pHasBlendType=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name ModelInput
   *
   * Metadata for input model data.
   * @{
   */
  bool pHasModelInput;        /**< true if metadata has has model input */
  std::string pModelInput;    /**< model input name, when pHasModelInput=true */
  double pModelLatencyHours;  /**< model input latency hours, when
			       *   pHasModelInput=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name UvInput
   *
   * Metadata for input UV phase correction data.
   * @{
   */
  bool pHasUvInput;            /**< true if metadata has UV input */
  std::string pUvInput;        /**< UV input name, when pHasUvInput=true */
  double pUvInputLatencyHours; /**< UV input latency hours, when
				*   pHasUvInput=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name UvInput
   *
   * Metadata for input calibration
   * @{
   */
  bool pHasCalibrate;         /**< true if metadata has calibrate information */
  std::string pCalibrateFcst; /**< Calibrate forecast data name, when
			       *   pHasCalibrate=true */
  std::string pCalibrateObs;  /**< Calibrate observation data name, when
			       *   pHasCalibrate=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name Model
   *
   * Metadata for model data
   * @{
   */
  bool pHasModelData;     /**< true if metadata has model name */
  std::string pModelData; /**< model name, when pHasModelData=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name Extrapolation
   *
   * Metadata for extrapolation data
   * @{
   */
  bool pHasExtrapData;     /**< true if metadata has extrapolation name */
  std::string pExtrapData; /**< extrapolation name, when pHasExtrapData=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name Skill
   *
   * Metadata for skill
   * @{
   */
  bool pHasSkill;          /**< true if metadata has skill information */
  double pSkillWithPc;     /**< skill value with phase correction, when
			    *   pHasSkill=true */
  double pSkillWithoutPc;  /**< skill value without phase correction, when
			    *   pHasSkill=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name Phase correction weight
   *
   * Metadata for phase correction weight
   * @{
   */
  bool pHasPcWeight;  /**< true if metadata has phase correction weight */
  double pPcWeight;   /**< phase correction weight when pHasPcWeight=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name Climatology
   *
   * Metadata for climatology
   * @{
   */
  bool pHasClimo;      /**< true if metadata has Climatology information */
  double pDaysInClimo; /**< Number of days in the climatology,
			* when pHasClimo=true */
  /** @} */

  //---------------------------------------------------------------------
  /**
   * @name UV Units
   *
   * Metadata for UV Units
   * @note the XML tag comes from UvUnits
   * @{
   */
  bool pHasUvUnits; /**< true if metadata has UV units */
  UvUnits pUvUnits; /**< UV Units, when pHasUvUnits=true */
  /** @} */

  /**
   * Any XML at all goes here.
   */
  std::string pFreeformXml;

  /**
   * Up to date string representation of state
   */
  std::string pXmlContent;

  //---------------------------------------------------------------------
  /**
   * @name Tags
   *
   * The fixed tags used to read/write metadata
   * @{
   */
  static const std::string pUvInputTag; /**< tag in XML for uv input */
  static const std::string pUvLatencyHoursTag; /**< tag in XML for UV latency */
  static const std::string pModelInputTag;  /**< XML tag for model input */
  static const std::string pModelLatencyHoursTag; /**< XML tag for model latency */

  static const std::string pCalibrateFcstTag; /**< tag in XML for calibration forecast */
  static const std::string pCalibrateObsTag;  /**< tag in XML for calibration
					       * observation */
  static const std::string pModelDataTag;  /**< tag in XML for model data */
  static const std::string pExtrapDataTag; /**< tag in XML for extrapolation data */
  static const std::string pSkillWithPcTag; /**< tag in XML for phase corrected skill */
  static const std::string pSkillWithoutPcTag; /**<tag in XML for non phase corrected skill*/
  static const std::string pPcWeightTag; /**< tag in XML for phase correction weight */
  static const std::string pDaysInClimoTag; /**< tag in XML for days in
					     * climatology */
  static const std::string pFreeformTag;  /**< tag in XML for free form XML */
  /** @} */

  /**
   * Merge free form XML from the input object into the local state
   * @param[in] mergeData MetaDataXml from which to get free form data to merge
   *                      in
   */
  void pMergeFreeform(const MetaDataXml &mergeData);

  /**
   * @return true if input data has phase correction type metadata the same as
   *         the local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pPcSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has blending type metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pBlendingSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has input model metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pModelInputSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has input UV metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pUvInputSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has calibration metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pCalibrateSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has model data metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pModelSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has extrapolation data metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pExtrapSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has skill metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pSkillSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has phase correction metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pPcWeightSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if input data has climatology metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   * @param[in] full  True to check whether days in climatology is the
   *                  same for both objects, false to check for the
   *                  existence of climatology metadata in both objects
   */
  bool pClimoSame(const MetaDataXml &compareData, std::string &ret,
		  const bool full) const;
  /**
   * @return true if input data has UV units metadata the same as the
   *         local object.
   *
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pUvUnitsSame(const MetaDataXml &compareData, std::string &ret) const;

  /**
   * @return true if free form metadata is the same for input and local
   * @param[in] compareData  MetaDataXml to compare against
   * @param[in,out] ret  String with any differences appended
   */
  bool pFreeformSame(const MetaDataXml &compareData, std::string &ret) const;
};

# endif     // MetaDataXml_HH
