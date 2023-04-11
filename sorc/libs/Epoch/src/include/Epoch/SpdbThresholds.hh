/**
 * @file SpdbThresholds.hh
 * @brief Mapping from lead seconds to bias and to threshold for all gen times
 * @class SpdbThresholds
 * @brief Mapping from lead seconds to bias and to threshold for all gen times
 *
 * Stored to SPDB by gen time, contains XML mappings from lead seconds to
 * bias, and from lead seconds to threshold
 *
 * The base class is SpdbMetadata which corresponds to one SPDB chunk
 *
 * This class is the interface used  by EnsFcstGenMultiThresh
 */

# ifndef    SpdbThresholds_hh
# define    SpdbThresholds_hh

#include <Epoch/SpdbMetadata.hh>
class DsSpdb;
class FieldThresh;
class GenTimeAndOlder;

//----------------------------------------------------------------
class SpdbThresholds : public SpdbMetadata
{
public:

  /**
   * Empty
   */
  SpdbThresholds(void);

  /**
   * URL, but no other info (for reading only)
   *
   * @param[in] spdb  The url
   */
  SpdbThresholds(const std::string &spdb);

  /**
   * @param[in] spdb  The URL
   * @param[in] ltHours  The lead times
   * @param[in] fields  The fields with tiling
   * @param[in] fixedfields  The fields that are fixed, with thresholds
   * @param[in] doTile  True to break thresholds into tiles
   * @param[in] tiling  Information about tiles
   * @param[in] latLonsOptional  True if lat/lon information in the
   *                             data that is read is optional or not.
   *                             If not true, the latlon information is
   *                             expected.  This is put in for backwards
   *                             compatibility in reading older data.
   *
   * Thresholds not known
   */
  SpdbThresholds(const std::string &spdb,
			 const std::vector<double> &ltHours,
			 const std::vector<std::string> &fields,
			 const std::vector<FieldThresh> &fixedfields,
			 const TileInfo &tiling,
			 bool latLonsOptional=true);

  /**
   * Destructor
   */
  virtual ~SpdbThresholds(void);
  
  /**
   * Extract the thresholds from the database at current or earlier day
   * for some gen time, fill in local base class state with
   * the database information, and return the thresholds (one per tile), for
   * the input field and lead times.
   * If there is no data to read, set the thresholds to the default value
   * in all times.
   * @param[in] numTiles  Number of tiles
   * @param[in] tiledFieldName  Name of the field for which to get thresholds
   * @param[in] genTime  The gen time to align to
   * @param[in] leadTime  The lead time from which to get thresholds
   * @param[in] maxDaysBack  Maximum number of days back to look
   * @param[in] defaultThreshold  Threshold to use when cannot read data
   * @param[out] gt  The gen time SPDB information
   *
   * @return the thresholds, one per tile
   */
  std::vector<double>
  getTiledThresholdsOrSetToDefault(int numTiles,
				   const std::string &tiledFieldName,
				   const time_t &genTime, int leadTime,
				   int maxDaysBack, double defaultThreshold,
				   GenTimeAndOlder &gt);
  

  /**
   * Read the newest data whose time is less than input, back to a maximum,
   * and fill in local base class state with the data that was read in
   *
   * @param[in] t  Upper time limit
   * @param[in] maxSecondsBack  Look back [t-maxSecondsBack,t)
   * @param[in] maxSecondsBeforeColdstart  If data is read in, change that
   *                                       data to coldstart values if the
   *                                       generating time is older than
   *                                       this value compared to input t
   * @param[in]  coldstartThresh  The coldstart field/threshold pairs
   *
   * @return true if successful and state is filled in
   */
  bool readFirstBefore(const time_t &t, int maxSecondsBack,
		       int maxSecondsBeforeColdstart,
		       const std::vector<FieldThresh> &coldstartThresh);

  /**
   * Read the newest data whose time is in the range specified, and fill
   * in base class state with information that was read
   *
   * @param[in] t0  Lower time limit
   * @param[in] t1  Upper time limit
   *
   * @return true if successful and state is filled in
   */
  bool readNewestInRange(const time_t &t0, const time_t &t1);

  /**
   * Read the data within a range closest in time to a target time and
   * fill in base class state with information that was read
   *
   * @param[in] target  The target time
   * @param[in] t0  Lower time limit
   * @param[in] t1  Upper time limit
   *
   * @return true if successful and state is filled in
   */
  bool readClosestToTargetInRange(const time_t &target, const time_t &t0,
				  const time_t &t1);

  /**
   * Read the newest data that has the same hour/min/second as the input 
   * generation time either an exact match, or back some number of days
   * and fill in base class state with information that was read
   *
   * @param[in] gt  The generation time
   * @param[in] maxDaysBack
   * @param[out] gtSpdb  The actual generation time info that was used
   * @return true if successful and state is filled in
   */
  bool readExactPreviousOrCurrent(const time_t &gt, int maxDaysBack,
				  GenTimeAndOlder &gtSpdb);

  /**
   * Read the newest data that has the same hour/min/second as the input 
   * generation time back a minimum of 1 day and a maximum of maxDaysBack
   * and fill in base class state with information that was read
   *
   * @param[in] gt  The generation time
   * @param[in] maxDaysBack
   * @param[out] gtSpdb  The actual generation time that was used
   * @return true if successful and state is filled in
   */
  bool readExactPrevious(const time_t &gt, int maxDaysBack,
			 GenTimeAndOlder &gtSpdb);

  /**
   * Write internal state to SPDB at time t
   *
   * @param[in] t
   * @return true if successful
   */
  bool write(const time_t &t);

  /**
   * Write internal state to SPDB at time t to a new url
   *
   * @param[in] t
   * @param[in] url
   * @return true if successful
   */
  bool write(const time_t &t, const std::string &url);

  /**
   * @return the times that are in the range of inputs 
   *
   * @param[in] t0 Earliest time
   * @param[in] t1 Latest time
   */
  std::vector<time_t> timesInRange(const time_t &t0, const time_t &t1) const;
  
  /**
   * Read from SPDB into state, at a time
   *
   * @param[in] time
   * @return true if successful
   */
  bool read(const time_t &time);

  /**
   * @return the chunk valid time, which will be set with any successful
   * read
   */
  inline time_t getChunkValidTime(void) const {return _chunkValidTime;}

  /**
   * @return the chunk time written, which will be set with any successful
   * read
   */
  inline time_t getChunkTimeWritten(void) const {return _chunkTimeWritten;}

protected:
private:  

  /**
   * SPDB location
   */
  std::string _url;

  /**
   * Valid time of the chunk that was loaded, when  you read
   */
  time_t _chunkValidTime;

  /**
   * Time written of the chunk that was loaded, when  you read
   */
  time_t _chunkTimeWritten;

  /**
   * True for optional lat/lon information on read
   */
  bool _latlonsOptional;

  bool _load(DsSpdb &s, bool fieldLeadsTilesSet=true);
};

# endif
