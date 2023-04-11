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
 * @file  FcstThreshInfo.hh
 * @brief  Information about a threshold associated with a forecast
 *         including generation/lead time, field name, and thresholds
 *         for each tile
 * @class  FcstThreshInfo
 * @brief  Information about a threshold associated with a forecast
 *         including generation/lead time, field name, and thresholds
 *         for each tile
 *  
 * Contains:
 *  - generation time for a forecast
 *  - lead time(seconds) for a forecast
 *  - fieldName  Name of the field
 *  - threshold  Threshold values for each tile
 */

# ifndef    FCST_THRESH_INFO_HH
# define    FCST_THRESH_INFO_HH

#include <ctime>
#include <string>
#include <vector>

//----------------------------------------------------------------
class FcstThreshInfo
{
public:

  /**
   * Empty constructor
   */
  FcstThreshInfo(void);

  /**
   * Constructor that copies in all members
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] name  Field name
   * @param[in] thresh  Thresholds for each tile
   *
   */
  FcstThreshInfo(const time_t &gt, const int lt, const std::string &name,
		 const std::vector<double> &thresh);

  /**
   * Constructor that interprets input string as XML which is parsed
   *
   * @param[in] xml  String to parse
   * @param[in] numTiles  Expected number of tiles
   */
  FcstThreshInfo(const std::string &xml, int numTiles);

  /**
   * Destructor
   */
  virtual ~FcstThreshInfo(void);

  /**
   * Operator < 
   * @param[in] f  Object to compare to
   * @return true if local object is < f
   */
  bool operator<(const FcstThreshInfo &f) const;

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
   * @param[in] numTiles  Expected Number of tiles in XML
   *
   * @return true for ok
   */
  bool parseXml(const std::string &xml, int numTiles);

  /**
   * @return The XML tag
   */
  inline static std::string fcstThreshTag(void) {return pTag;}

  /**
   * @return true if input name matches local name
   * @param[in] name
   */
  inline bool nameMatch(const std::string &name) const
  {
    return pField == name;
  }

  /**
   * @return the field name
   */
  inline std::string getName(void) const {return pField;}

  /**
   * @return the gen time
   */
  inline time_t getGenTime(void) const {return pGentime;}

  /**
   * @return the thresholds, one per tile
   */
  inline std::vector<double> getThresh(void) const {return pThresh;}

  /**
   * @return the threshold for a particular tile
   * @param[in] tileIndex  The index for the tile to get threshold of
   * @note does not check the index to see if in bounds
   */
  inline double getTileThresh(int tileIndex) const {return pThresh[tileIndex];}


  /**
   * @return true if object is all right
   */
  inline bool isOk(void) const {return pOk;}

  /**
   * @return true if local gen time is older than input time
   * @param[in] t  Input time
   */
  inline bool olderThan(const time_t &t) const
  {
    return pGentime < t;
  }

protected:
private:  

  bool pOk;            /**< True if object is good */
  time_t pGentime;     /**<  Generation time */
  int pLeadtime;       /**< Lead time seconds */
  std::string pField;  /**< Field name */
  std::vector<double> pThresh;      /**< Threshold */

  /** 
   * The fixed XML tag
   */
  static const std::string pTag;
};

# endif     // FcstThreshInfo HH
