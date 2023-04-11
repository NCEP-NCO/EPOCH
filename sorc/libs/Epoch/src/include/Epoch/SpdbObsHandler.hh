/**
 * @file SpdbObsHandler.hh
 * @brief class to control access to SPDB database with 'obar' values
 * @class SpdbObsHandler
 * @brief class to control access to SPDB database with 'obar' values
 *
 * Has SpdbObsMetadata as a base class.
 */

# ifndef    SpdbObsHandler_hh
# define    SpdbObsHandler_hh

#include <Epoch/TileInfo.hh>
#include <string>
#include <vector>

#include <Epoch/SpdbObsMetadata.hh>

class DsSpdb;

//----------------------------------------------------------------
class SpdbObsHandler : public SpdbObsMetadata
{
public:
  /**
   * Constructor For an empty initial state
   * @param[in] spdb  The URL
   * @param[in] precipField Name of the precip field in data
   * @param[in] tiling  Tile information
   * @param[in] thresh  The obar thresholds 
   */
  SpdbObsHandler(const std::string &spdb, const std::string &precipField,
		 const TileInfo &tiling,
		 const std::vector<double> &thresh);

  /**
   * Constructor When you expect to read to fill in state
   * @param[in] spdb  The URL
   */
  SpdbObsHandler(const std::string &spdb);

  /**
   * Destructor
   */
  virtual ~SpdbObsHandler(void);
  
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
   * Read database into state at a time
   *
   * @param[in] obsTime
   * @return true for success
   */
  bool read(const time_t &obsTime);

  /**
   * @return the SPDB time written
   */
  inline time_t getChunkTimeWritten(void) const {return _chunkTimeWritten;}
  

protected:
private:  

  std::string _url;          /**< The SPDB url */
  time_t _chunkValidTime;    /**< The SPDB chunk valid time */
  time_t _chunkTimeWritten;  /**< The SPDB chunk time written */
  

  bool _load(DsSpdb &s);
		
};

# endif
