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
 * @file  TiledFcstThreshInfo.hh
 * @brief  Information about a threshold associated with a forecast
 *         including generation/lead time, field name, and thresholds
 *         for each tile
 * @class  TiledFcstThreshInfo
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

# ifndef    TiledFcstThreshInfo_HH
# define    TiledFcstThreshInfo_HH

#include <Epoch/GenTimeAndOlder.hh>
#include <ctime>
#include <string>
#include <vector>

//----------------------------------------------------------------
class TiledFcstThreshInfo
{
public:

  /**
   * Empty constructor
   */
  TiledFcstThreshInfo(void);

  /**
   * Constructor that copies in all members
   *
   * @param[in] gt  Generation time information from SPDB
   * @param[in] lt  Lead seconds
   * @param[in] name  Field name
   * @param[in] thresh  Thresholds for each tile
   */
  TiledFcstThreshInfo(const GenTimeAndOlder &gt, 
		      const int lt, const std::string &name,
		      const std::vector<double> &thresh);

  /**
   * Constructor that interprets input string as XML which is parsed
   *
   * @param[in] xml  String to parse
   * @param[in] numTiles  Expected number of tiles
   */
  TiledFcstThreshInfo(const std::string &xml, int numTiles);

  /**
   * Destructor
   */
  virtual ~TiledFcstThreshInfo(void);

  /**
   * Operator < 
   * @param[in] f  Object to compare to
   * @return true if local object is < f
   */
  bool operator<(const TiledFcstThreshInfo &f) const;

  /**
   * Debug print to stdout
   * @param[in] verbose  Flag
   */
  void print(bool verbose=false) const;

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
  inline static std::string fcstThreshTag(void) {return _tag;}

  /**
   * @return true if input name matches local name
   * @param[in] name
   */
  inline bool nameMatch(const std::string &name) const
  {
    return _fieldName == name;
  }

  /**
   * @return the field name
   */
  inline std::string getName(void) const {return _fieldName;}

  /**
   * @return the gen time
   */
  inline time_t getGenTime(void) const {return _gentime.getGenTime();}

  /**
   * @return the thresholds, one per tile
   */
  inline std::vector<double> getThresh(void) const {return _thresh;}

  /**
   * @return the threshold for a particular tile
   * @param[in] tileIndex  The index for the tile to get threshold of
   * @note does not check the index to see if in bounds
   */
  inline double getTileThresh(int tileIndex) const {return _thresh[tileIndex];}


  /**
   * @return true if object is all right
   */
  inline bool isOk(void) const {return _ok;}

  /**
   * @return true if local gen time is older than input time
   * @param[in] t  Input time
   */
  inline bool olderThan(const time_t &t) const
  {
    return _gentime < t;
  }

protected:
private:  

  bool _ok;                    /**< True if object is good */
  GenTimeAndOlder _gentime;    /**< Gen time info from SPDB */
  int _leadtime;               /**< Lead time seconds */
  std::string _fieldName;      /**< Field name */
  std::vector<double> _thresh; /**< Thresholds, one per tile */

  /** 
   * The fixed XML tag
   */
  static const std::string _tag;
};

# endif     // TiledFcstThreshInfo HH
