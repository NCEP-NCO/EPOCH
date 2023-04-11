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
 * @file FcstThreshMetadata.hh
 * @brief metadata associated with forecast thresholds
 * @class FcstThreshMetadata
 * @brief metadata associated with forecast thresholds
 *
 * Includes threshold over various tiles
 */

# ifndef    FCST_THRESH_METADATA_HH
# define    FCST_THRESH_METADATA_HH

#include <vector>
#include <string>
#include <ConvWx/FcstThreshInfo.hh>

//----------------------------------------------------------------
class FcstThreshMetadata
{
public:

  /**
   * Constructor, only number of tiles is known
   * @param[in] numTiles  Total number of tiles
   */
  FcstThreshMetadata(int numTiles=1);

  /**
   * Constructor from parsing XML
   *
   * @param[in] xml  String to parse
   */
  FcstThreshMetadata(const std::string &xml);


  /**
   * Destructor
   */
  virtual ~FcstThreshMetadata(void);

  /**
   * @return number of tiles
   */
  inline int numTiles(void) const {return pNumTiles;}

  /**
   * Add input information to state
   * @param[in] gen  Gen time
   * @param[in] lead  Lead seconds
   * @param[in] fieldName  field
   * @param[in] threshold  Threshold for the field, one per tile
   */
  void add(const time_t &gen, int lead, const std::string &fieldName, 
	   const std::vector<double> &threshold);

  /**
   * Merge input object into local object 
   * @param[in] info
   */
  void merge(const FcstThreshMetadata &info);

  /**
   * @return XML representation of state
   */
  std::string getXml(void) const;

  /**
   * Erase all data for a particular gen time from state
   * @param[in] gt  Gen time
   */
  void erase(const time_t &gt);

  /**
   * @return a pair of string/double for each field, where the string is
   * the field name and the double is the average threshold value for
   * that field for the input tile, as computed using state
   *
   * @param[in] tileIndex  Specification of the tile
   */
  std::vector<std::pair<std::string,double> > nameThreshAverage(int tileIndex) const;

  /**
   * Debug print to stdout
   */
  void print(void) const;

  /**
   * @return The XML tag
   */
  inline static std::string fcstThreshMetadataTag(void) {return pTag;}

protected:
private:  

  int pNumTiles;  /**< Number of tiles, must remain consistent */

  /**
   * One set of forecast thresholds information per field, for some fields.
   * Each FcstThreshInfo object has thresholds for each tile
   */
  std::vector<FcstThreshInfo> pInfo;

  /** 
   * The fixed XML tag
   */
  static const std::string pTag;
};

# endif     // FcstThreshMetadata_HH
