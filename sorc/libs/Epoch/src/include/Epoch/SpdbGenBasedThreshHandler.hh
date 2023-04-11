/**
 * @file SpdbGenBasedThreshHandler.hh
 * @brief class to control access to SPDB database with tiled thresholds
 * @class SpdbGenBasedThreshHandler
 * @brief class to control access to SPDB database with tiled thresholds
 *
 * Has SpdbGenBasedMetadata as a base class.
 * This class is the interface used by ThreshCompute
 */

# ifndef    SpdbGenBasedThreshHandler_hh
# define    SpdbGenBasedThreshHandler_hh

#include <Epoch/SpdbGenBasedMetadata.hh>
#include <Epoch/TileInfo.hh>
#include <string>
#include <vector>

class DsSpdb;
class TileThreshInfoGenBased;

//----------------------------------------------------------------
class SpdbGenBasedThreshHandler : public SpdbGenBasedMetadata
{
public:
  /**
   * Constructor - the base class metadata set to input
   *
   * @param[in] spdb  The URL
   * @param[in] md  The metadata to use for base class
   */
  SpdbGenBasedThreshHandler(const std::string &spdb,
			    const SpdbGenBasedMetadata &md);

  /**
   * Constructor - The base class metadata set to empty
   *
   * @param[in] spdb  The URL
   */
  SpdbGenBasedThreshHandler(const std::string &spdb);

  /**
   * Destructor
   */
  virtual ~SpdbGenBasedThreshHandler(void);
  
  /**
   * Read database at a particular chunk (gen) time
   *
   * @param[in] genTime
   * @return true for success
   */
  bool read(const time_t &genTime);

  /**
   * Read database at newest chunk (gen) time prior to a max
   *
   * @param[in] genTime  Max gentime to use is this time minus 1 second
   * @param[in] maxSecondsBack  Min gentime to use is genTime-maxSecondsBack
   *
   * @return true for success
   */
  bool readBestOlder(const time_t &genTime, int maxSecondsBack);

  /**
   * Real time initialization at a gen time
   * Read in the newest gen time in the range given
   * @param[in] genTime0
   * @param[in] genTime1
   *
   * @return true for success
   */
  bool readNewestInRange(const time_t &genTime0, const time_t &genTime1);


  /**
   * Archive initialization over a range of gen times
   * @param[in] best
   * @param[in] min
   * @param[in] max
   *
   * Read in the gen that is closest to best in the range min to max
   *
   * @return true for success
   */
  bool readClosestToTargetInRange(const time_t &best,
				  const time_t &min,
				  const time_t &max);

  /**
   * Write internal state to SPDB
   *
   * @return true if successful
   */
  bool write(void);

  /**
   * Write internal state to SPDB to a new url
   *
   * @param[in] url
   *
   * @return true if successful
   */
  bool write(const std::string &url);

  /**
   * Take average of ALL appropriate thresholds in a range and put results into
   * a returned object.  This is data from all database entries whose hour
   * matches that of the recentTime
   *
   * @param[in] recentTime  Maximum time to put into the average
   * @param[in] daysBack  Number of days back in which to accumulate averages
   *                      that have the same hour as recentTime
   * @param[in] coldstartThresh  Threshold to use when there is no data to
   *                             average
   * @param[out] out  The object that is created
   *
   * @return true for success
   */
  bool average(const time_t &recentTime, int daysBack, double coldstartThresh,
	       SpdbGenBasedMetadata &out);

  /**
   * @return the gen times that are in the range of inputs 
   *
   * @param[in] t0 Earliest time
   * @param[in] t1 Latest time
   */
  std::vector<time_t> timesInRange(const time_t &t0, const time_t &t1) const;

  /**
   * @return the gen times that are in the range of inputs, whos hour is the
   * same as t1
   *
   * @param[in] t0 Earliest time
   * @param[in] t1 Latest time
   */
  std::vector<time_t> timesInRangeWithMatchingHour(const time_t &t0,
						   const time_t &t1) const;

  /**
   * @return the SPDB valid time for the chunk
   */
  inline time_t getChunkValidTime(void) const {return _chunkValidTime;}

  /**
   * @return the SPDB time written for the chunk
   */
  inline time_t getChunkTimeWritten(void) const {return _chunkTimeWritten;}

protected:
private:  

  std::string _url;            /**< URL for data */
  time_t _chunkValidTime;      /**< SPDB chunk valid time */
  time_t _chunkTimeWritten;    /**< SPDB chunk time written */
  
  bool _readExisting(const time_t &genTime, const std::string &description);
  bool _load(DsSpdb &s, const std::string &description);
		
};

# endif
