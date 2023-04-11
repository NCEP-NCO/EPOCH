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
 * @file MetaData.hh
 * @brief ConvWx specific metadata, typically to augment Grid data
 * @class MetaData
 * @brief ConvWx specific metadata, typically to augment Grid data
 *
 * MetaData has strings for 'name', 'info', and 'source', as well as XML
 * content, some of which is mapped to various classes, other which can
 * be free form.
 *
 * If the data is written/read as MDV, we set things as follows:
 *    - pName   master header :data_set_name
 *    - pinfo   master header :data_set_info
 *    - pSource master header :data_set_source
 *    - pXml    chunk CHUNK_COMMENT contents
 *
 * If the data is written/read as netCDF as follows:
 *    - pName   global attributes :title
 *    - pInfo   global attributes :history
 *    - pSource global attributes :source
 *    - pXml    global attributes :comment
 *
 *
 * If the data is written/read in some other way, these fields can be stored
 * and accessed in whatever way is appropriate for that data format
 *
 *
 * @note MDV and netCDF are not part of the ConvWx software.
 *
 * @note The knowledge of what is expected in the XML portion is
 *       contained in the MetaDataXml member object
 */

# ifndef    METADATA_HH
# define    METADATA_HH

#include <string>
#include <ConvWx/MetaDataXml.hh>

//----------------------------------------------------------------
class MetaData
{
public:

  /**
   * Empty constructor
   */
  MetaData(void);

  /**
   * Constructor with members input
   *
   * @param[in] name  Value for pName
   * @param[in] info  Value for pInfo
   * @param[in] source Value for pSource
   * @param[in] xml  MetaDataXml data to use
   */
  MetaData(const std::string &name, const std::string &info,
	   const std::string &source, const MetaDataXml &xml);

  /**
   * Constructor with members input
   *
   * @param[in] name  Value for pName
   * @param[in] info  Value for pInfo
   * @param[in] source Value for pSource
   * @param[in] xml  String to parse to create a MetaDataXml object
   */
  MetaData(const std::string &name, const std::string &info,
	   const std::string &source, const std::string &xml);

  /**
   * Destructor
   */
  virtual ~MetaData(void);

  /**
   * Operator==
   * @param[in] m  Object to compare to
   *
   * @return true for equality
   */
  bool operator==(const MetaData &m) const;

  /**
   * 'Operator= lite'
   *
   * @param[in] m  Object to compare
   *
   * @return true for similarity
   */
  bool similar(const MetaData &m) const;

  /**
   * Write the differences (if any) between the input and local object to a
   * string
   *
   * @param[in] m  Object to compare
   * 
   * @return the string with differences
   */
  std::string printDifferences(const MetaData &m) const;

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
  std::string printMajorDifferences(const MetaData &m) const;

  /**
   * @return true if pName is not empty
   */
  inline bool hasName(void) const {return !pName.empty();}

  /**
   * @return true if pInfo is not empty
   */
  inline bool hasInfo(void) const {return !pInfo.empty();}

  /**
   * @return true if pSource is not empty
   */
  inline bool hasSource(void) const {return !pSource.empty();}

  /**
   * @return true if the XML content is not empty
   */
  inline bool hasXml(void) const {return pXml.nonEmpty() != 0;}

  /**
   * @return C string for pName
   */
  inline const char *nameCStr(void) const {return pName.c_str();}

  /**
   * @return C string for pInfo
   */
  inline const char *infoCStr(void) const {return pInfo.c_str();}

  /**
   * @return C string for pSource
   */
  inline const char *sourceCStr(void) const {return pSource.c_str();}

  /**
   * @return C string for the XML representation of the pXml member
   */
  inline const char *xmlCStr(void) const {return pXml.cStr();}

  /**
   * set pName 
   * @param[in] s  The value to use
   */
  inline void setName(const std::string &s) { pName = s;}

  /**
   * set pInfo 
   * @param[in] s  The value to use
   */
  inline void setInfo(const std::string &s) { pInfo = s;}

  /**
   * set pSource
   * @param[in] s  The value to use
   */
  inline void setSource(const std::string &s) {pSource = s;}

  /**
   * set pXml by calling the MetaDataXml constructor
   * @param[in] xmlString  String with XML metadata to parse
   */
  inline void setXml(const std::string &xmlString)
  {
    pXml = MetaDataXml(xmlString);
  }

  /**
   * set pXml 
   * @param[in] xml  The object to use
   */
  inline void setXml(const MetaDataXml &xml) { pXml = xml;}

  /**
   * @return the XML string associated with pXml
   */
  inline std::string xmlString(void) const {return pXml.xmlString();}

  /**
   * Clear pXml to empty
   */
  inline void xmlClear(void) { pXml = MetaDataXml();}

  /**
   * @return the size of pXml, when it is represented as an XML string
   */
  inline int xmlSize(void) const {return pXml.xmlSize();}

  /**
   * @return a copy of pXml 
   */
  inline MetaDataXml xmlMetadata(void) const {return pXml;}

  /**
   * Append free form XML to pXml
   * @param[in] xmlString  Free form XML metadata to append
   */
  inline void xmlAddFreeform(const std::string &xmlString)
  {
    pXml.addFreeformXml(xmlString);
  }

  /**
   * @return a one line string representation of the local object 
   */
  std::string sprint(void) const;

  /**
   * @return a string representation of metadata that is easy to read (not XML)
   * (multiple lines)
   */
  std::string sprintDebug(void) const;

  /**
   * Log a string representation of metadata using ILogMsg
   *
   * @note The representation is that of sprint()
   */
  void logMetadata(void) const;

  /**
   * Create a MetaData object which is the merge of local and input objects
   * @param[in] mergeData   MetaData to merge with local object
   * @return merged object
   *
   * @note Concatenates the name, info and source strings, and merges the
   *       XML content using MetaDataXml::merge()
   */
  MetaData merge(const MetaData &mergeData) const;

  /**
   * Write the string from xmlString() to stdout
   */
  void printXmlString(void) const;

protected:
private:  

  /**
   * A name. It can be the empty string.
   */
  std::string pName;

  /**
   * An information string. It can be empty string.
   */
  std::string pInfo;

  /**
   * A source string. It can be the empty string.
   */
  std::string pSource;

  /**
   * The XML portion of the metadata
   */
  MetaDataXml pXml;
};

# endif     // MetaData_HH
