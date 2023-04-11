/**
 * @file ClimoState.hh
 * @brief sum and count grids, and the gen times used, and metadata
 * @class ClimoState
 * @brief sum and count grids, and the gen times used, and metadata
 */

#ifndef CLIMO_STATE_HH
#define CLIMO_STATE_HH

#include <Epoch/MdvMetadata.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWxIO/ParmFcstIO.hh>

class ClimoState
{
public:

  /**
   * Empty-ish constructor
   * @param[in]  isCloudTop True for the cloud top state, false for not
   */
  ClimoState(bool isCloudTop);
  
  /**
   * Constructor in which a previous time's climo data has been read in,
   *  state is pulled from that.
   *
   * @param[in] inputGenTime  input data time
   * @param[in] inputLeadTime  input data lead time
   * @param[in] inputData  The input data, with sums and counts and averages
   *                       for all fields
   * @param[in] isCloudTop True for the 'right hand side' cloud top case,
   *                       false for non cloud top 'left hand side'
   *
   * @note  The data's metadata should have MdvMetadata XML.
   * This should have at least one gen time.  Iff isCloudTop=true, it should
   * also have tiled thresholds.
   */
  ClimoState(const time_t &inputGenTime, int inputLeadTime, 
	     const MultiFcstGrid &inputData, bool isCloudTop);

  /**
   *  Destructor
   */
  ~ClimoState(void);

  /**
   * @return those times that are in the state but are too old based
   * on number of days to keep
   *
   * @param[in] genTime  Current new gen time
   * @param[in] daysInClimo  Maximum number of days to keep
   */
  std::vector<time_t> tooOld(const time_t &genTime, int daysInClimo) const;

  /**
   * Purge the state for a particular forecst, removing it from sums and counts
   * and updating the metadata
   *
   * @param[in] genTime  The forecast time
   * @param[in] leadTime  The lead time
   * @param[in] data  The actual forecast data, all fields
   */
  void purgeForecast(const time_t &genTime, int leadTime,
		     const MultiFcstGrid &data);

  /**
   * Add a forecast to the state, updating counts and sums and metadata
   * @param[in] genTime  The forecast time being processed
   * @param[in] mdata  Data for all the fields (at actualGenTime)
   * @param[in] encoding  Encoding to give new grids
   */
  bool addToState(const time_t &genTime, const MultiFcstGrid &mdata,
		  Grid::Encoding_t encoding);
  
  /**
   * Normalize by dividing sums by counts internally, appending final grids
   * to the internal state (along with sums and counts).  Produce and return
   * the final metadata XML
   * @param[in] inputData  Data for all the fields, used to get nx,ny etc
   * @param[in] encoding  Encoding to give normalized grids
   *
   * @return  The XML
   */
  MetaDataXml normalizeAndSetXml(const MultiGrid &inputData,
				 Grid::Encoding_t encoding);

  /**
   * @return reference to the multigrid with sums/counts and averages
   */
  inline MultiFcstGrid & data(void) {return _data;}

  /**
   * @return reference to the multigrid with sums/counts and averages
   */
  inline const MultiFcstGrid & data(void) const {return _data;}

protected:
private:

  /**
   * Value of missing data for output grids.
   */ 
  static const double _climoMeanMissing; 
  

  bool _ok;                /**< True if set */
  bool _isCloudTop;        /**< True for cloud top case */
  MultiFcstGrid _data;     /**< Grids with sums/counts,
			    * and when done, averages */
  MdvMetadata _metadata;   /**< Metadata for forecast thresholds */

  /**
   * Remove particular data from state, decrementing counts and sums 
   *
   * @param[in] genTime  Time of data to remove
   * @param[in] data to remove
   */
  void _remove(const time_t &genTime, const MultiFcstGrid &data);


  /**
   * initialize when it is the first data to go in
   * @param[in] inputData  The data
   * @param[in] encoding
   */
  void _initNew(const MultiFcstGrid &inputData, Grid::Encoding_t encoding);

  /**
   * Add input data to the state by incrementing counts and sums
   * @param[in] name  Field name of input data
   * @param[in] data  The input data
   */
  bool _increment(const std::string &name, const Grid &data);
};

#endif
