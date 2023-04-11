/**
 * @file SpdbThreshHandler.hh
 * @brief class to control access to SPDB database with tiled thresholds
 * @class SpdbThreshHandler
 * @brief class to control access to SPDB database with tiled thresholds
 *
 * Has SpdbThresholds as a base class, which does most of the SPDB access.
 * This class is the interface used by ThreshChooser
 */

# ifndef    SpdbThreshHandler_hh
# define    SpdbThreshHandler_hh

#include <Epoch/SpdbThresholds.hh>

//----------------------------------------------------------------
class SpdbThreshHandler : public SpdbThresholds
{
public:
  /**
   * @param[in] spdb  The URL
   * @param[in] ltHours  The lead times
   * @param[in] fields  The field names with tiled thresholds
   * @param[in] fixedfields  The field names that are fixed,
   *                        with the fixed threshold
   * @param[in] genFrequencySeconds  How often a model run happens (seconds)
   * @param[in] tiling  Information about tiles
   * @param[in] maxSecondsBack  Look back [t-maxSecondsBack,t) into SPDB
   * @param[in] maxSecondsBeforeColdstart  If data is read in, change that
   *                                       data to coldstart values if the
   *                                       generating time is older than
   *                                       this value compared to input time
   * @param[in] coldstartThresh  The coldstart field/threshold pairs 
   * @param[in] latLonsOptional  True if lat/lon information in the
   *                             data that is read is optional or not.
   *                             If not true, the latlon information is
   *                             expected.  This is put in for backwards
   *                             compatibility in reading older data.
   *
   * Thresholds not known
   */
  SpdbThreshHandler(const std::string &spdb,
		     const std::vector<double> &ltHours,
		     const std::vector<std::string> &fields,
		     const std::vector<FieldThresh> &fixedfields,
		     int genFrequencySeconds,
		     const TileInfo &tiling, int maxSecondsBack,
		     int maxSecondsBeforeColdstart,
		     const std::vector<FieldThresh> &coldstartThresh,
		     bool latLonsOptional=true);

  /**
   * Destructor
   */
  virtual ~SpdbThreshHandler(void);
  
  /**
   * Read the newest data whose time is less than input time, back to 
   * maximum configured lookback. If nothing found, set things to 
   * configured coldstart values
   *
   * @param[in] t  Time to adjust to
   */
  void readFirstBeforeOrSetColdstart(const time_t &t);

  /**
   * Set a particular gen (hour,min,sec) /lead forecast to coldstart values
   *
   * @param[in] genTime  Local forecasts must match input gen hour/min/sec 
   *                     to be set to coldstart values
   * @param[in] leadTime Local forecasts must match input lead time
   *                     to be set to coldstart values
   */
  bool setForecastToColdstart(const time_t &genTime, int leadTime);

protected:
private:  

  int _genFrequencySeconds;  /**< The gen time frequency parameter (seconds)
			      * expect gentimes at h,m,s=0,0,0 and every
			      * _genFrequencySeconds*X throughout the day*/
  TileInfo _tiling;          /**< The tiling parameters */
  int _maxSecondsBack;       /**< How far back to look in SPDB for thresholds*/
  int _maxSecondsBeforeColdstart; /**< Maximum age of thresholds (obs time)
				   * before changing to coldstart values */
  std::vector<FieldThresh> _coldstartThresh;  /**< the cold start thresholds
					       * for each tile */
};

# endif
