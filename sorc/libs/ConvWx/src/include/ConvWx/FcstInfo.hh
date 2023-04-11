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
 * @file  FcstInfo.hh
 * @brief  Information about a forecast including generation/lead times, path,
 *         metadata, and time written
 * @class  FcstInfo
 * @brief  Information about a forecast including generation/lead times, path,
 *         metadata, and time written
 *  
 * Contains:
 *  - generation time for a forecast
 *  - lead time(seconds) for a forecast
 *  - path  Location of the forecast (depends on implementation)
 *  - metadata  Metadata associated with the forecast
 *
 *  - time written  The time the data was created. This is a more recent
 *                  addition to the class, and most methods ignore this member.
 *
 * @note see MetaData class
 */

# ifndef    FCST_INFO_HH
# define    FCST_INFO_HH

#include <string>
#include <ConvWx/MetaData.hh>

//----------------------------------------------------------------
class FcstInfo
{
public:

  /**
   * Empty constructor
   */
  FcstInfo(void);

  /**
   * Constructor that copies in all members except time written
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] path  Path value
   * @param[in] metadata  MetaData object to use
   *
   * @note Time written is set through a call to setTimeWritten()
   */
  FcstInfo(const time_t &gt, const int lt, const std::string &path,
	   const MetaData &metadata);

  /**
   * Constructor for just generation and lead time seconds, no metadata,
   * no path, no time written
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   *
   * @note Time written is set through a call to setTimeWritten()
   * @note no metadata or path, each set to empty default values.
   */
  FcstInfo(const time_t &gt, const int lt);
  
  /**
   * Destructor
   */
  virtual ~FcstInfo(void);

  /**
   * Operator==
   *
   * @param[in] f object to compare
   *
   * @return true for equality
   */
  bool operator==(const FcstInfo &f) const;

  /**
   * 'Operator== lite'
   *
   * @param[in] f  Object to compare
   *
   * @return true for similar
   */
  bool similar(const FcstInfo &f) const;

  /**
   * Write the differences (if any) between the input and local object to a
   * string
   *
   * @param[in] f  Object to compare
   * 
   * @return the string with differences
   */
  std::string printDifferences(const FcstInfo &f) const;

  /**
   * Print any major differences between input and local object to a string
   *
   * @param[in] f  Object to compare against
   *
   * @return  The string with differences
   *
   * @note This is tied to the 'similar' method
   */
  std::string printMajorDifferences(const FcstInfo &f) const;

  /**
   * Copy local state out to another object.
   *
   * @param[out] f   The object which is given local state
   */
  void copyOut(FcstInfo &f) const;

  /**
   * @return Generation time
   */
  inline time_t getGenTime(void) const {return pGentime;}

  /**
   * Set generation time
   *
   * @param[in] gt  Time to use as generation time
   */
  inline void setGenTime(const time_t &gt) { pGentime = gt;}

  /**
   * @return Time written
   */
  inline time_t getTimeWritten(void) const {return pTimeWritten;}

  /**
   * Set time written
   *
   * @param[in] t  Time to use as time written
   */
  inline void setTimeWritten(const time_t &t) { pTimeWritten = t;}

  /**
   * @return lead time
   */
  inline int getLeadSeconds(void) const {return pLeadtime;}

  /**
   * Set lead time
   *
   * @param[in] lt  Seconds to use as lead time
   */
  inline void setLeadSeconds(const int lt) { pLeadtime = lt;}

  /**
   * @return path
   */
  inline std::string getPath(void) const {return pPath;}

  /**
   * @return metadata 
   */
  inline MetaData getMetadata(void) const {return pMetadata;}

  /**
   * @return MetaDataXml associated with the metadata
   */
  inline MetaDataXml getMetadataXml(void) const
  {
    return pMetadata.xmlMetadata();
  }

  /**
   * Append arbitrary XML to the metadata.
   *
   * @param[in] xml Any metadata string
   *
   * @see MetaDataXml::xmlAddFreeform()
   */
  inline void addFreeformMetadataXml(const std::string &xml)
  {
    pMetadata.xmlAddFreeform(xml);
  }

  /**
   * Log 
   */
  void logInfo(void) const;

  /**
   * @return One line string description of state
   */
  std::string sprintInfo(void) const;

  /**
   * @return  Multiple line string description of state that is easy to read
   */
  std::string sprintInfoDebug(void) const;

  /**
   * @return  String description of state, minus the metadata
   */
  std::string sprintInfoNoMetadata(void) const;

  /**
   * Set the MetaData info field 
   * @param[in] info  Info string
   */
  inline void setMetadataInfo(const std::string &info)
  {
    pMetadata.setInfo(info);
  }

  /**
   * @return a subset of the full path.
   *
   * @note calls InterfaceLL::griddedDataPathFilter()
   */
  std::string getPathSubset(void) const;

protected:
private:  

  time_t pGentime;  /**<  Generation time */
  int pLeadtime;     /**< Lead time seconds */
  time_t pTimeWritten; /**< Time written */
  std::string pPath; /**< String description of where data is */
  MetaData pMetadata; /**<  The metadata associated with this forecast */
};

# endif     // FcstInfoHH
