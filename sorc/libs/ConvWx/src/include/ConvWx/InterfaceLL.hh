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
 * @file InterfaceLL.hh
 * @brief  Static functions that give Low Level interfaces to infrastructure
 * software 
 * @class InterfaceLL
 * @brief  Static functions that give Low Level interfaces to infrastructure
 * software
 *
 * This class has not state, only static methods
 */
# ifndef    INTERFACE_LL_HH
# define    INTERFACE_LL_HH

#include <string>
#include <vector>

class ParmProjection;
class Grid;
class GridData;
class FcstGrid;
class MultiFcstGrid;
class MultiGrid;
class MetaData;

//----------------------------------------------------------------
class InterfaceLL
{
public:

  /**
   * Inform any monitoring software of process activity, with a message
   *
   * @param[in] msg  The message sent to the monitoring software.
   */
  static void doRegister(const std::string &msg);


  /**
   * Cleanup performed at exit.
   */
  static void finish(void);

  /**
   * Sort input data such that data[index[]] is in ascending order, i.e.
   *  - data[index[i]] <= data[index[i+1]] for i=0,1,..,n-1
   *
   * @param[in] data  Array of data values
   * @param[in] n  Length of the array
   * @param[out] index  Index values to be set
   *
   * @note this method does not change data
   * @note the method assumes both data and index are arrays of length n
   */
  static void sort(double *data, int n, int *index);

  /**
   * Put contents of a file into a string.
   *
   * @param[in] fileName  The file to read
   * @param[out] str  The string that contains contents of file on exit
   *
   * @return True if entire file content has been copied to string.
   *
   * @note It helps if the file is ASCII data.
   */
  static bool loadFile(const std::string &fileName, std::string &str);

  /**
   * Make directories recursively. Parent directories are recursively created.
   *
   * @param[in] path  The path with a file as the last thing in the path.
   *
   * @return true if successful.
   *
   * examples:
   *  - path=/home/user/sub1/sub2/data.dat,
   *      - the directory /home/user/sub1/sub2 is created.
   *  - path=/home/user/sub1/sub2/sub3/sub4
   *      - the directory /home/user/sub1/sub2/sub3 is created.
   *
   * @note assumes last thing in path is the file itself 
   *
   * @note if the path already exists, nothing happens, and returned status is
   *       true
   */
  static bool makeDirRecurse(const std::string &path);

  /**
   * Parse a string into white-space delineated tokens. Look only up to 
   * a maximum number of characters, or until a 0 byte is found. Return the
   * tokens.
   *
   * @param[in] inputString  String to parse
   * @param[in] maxParseChars  Maximum number of characters in inputString to
   *                           parse
   * @param[in] maxTokenLen  Maximum allowed length of each token
   * @param[out] tokens  Vector of tokens
   *
   * @return  Number of tokens found.
   */
  static int stringParse(const char *inputString, const int maxParseChars,
			 const int maxTokenLen,
			 std::vector<std::string> &tokens);

  /**
   * Filter the input string down so that the result is a 'small'
   * subset of the full path, based on known format for such paths.
   * 
   * The paths are those typically seen for the Grid data storage used in
   * ConvWx, for which data is organized in a particular known way, and
   * file names have a known fixed format.
   *
   * @param[in,out] fullPath  The gridded data path that is filtered.
   * @param[in] time  Time value associated with this path, which may or
   *                  may not be used to do the filtering.
   *
   * @return True for success
   *
   * @note the 'known format' is the path hierarchy for internal gridded data.
   *
   * @note The results of this method become metadata that is written out.
   *       A filter that does nothing is therefore allowed in other 
   *       implementations. The effect is a change of metadata strings.
   *
   * @note see InterfaceLL::dataPathParse(), InterfaceLL::dataPathString()
   */
  static bool griddedDataPathFilter(std::string &fullPath, const time_t &time);

  /**
   * Parse a path of known format to pull out generation and lead time.
   *
   * The paths are those typically seen for the Grid data storage used in
   * ConvWx, for which data is organized in a particular known way, and
   * file names have a known fixed format, which includes generation and lead time.
   * 
   * @param[in] dataPath  Gridded data path to parse
   * @param[out] gt  Generation time returned
   * @param[out] lt  Lead seconds returned
   *
   * @return true if successful
   *
   * @note see InterfaceLL::griddedDataPathFilter(),
   *       InterfaceLL::dataPathString()
   */
  static bool dataPathParse(const std::string &dataPath, time_t &gt, int &lt);

  /**
   * Convert an input generation and lead time to a forecast data path string.
   *
   * For the Grid data storage used in ConvWx, this is the subset of the
   * full path that encodes the generation and lead time values.
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   *
   * @return  The string
   *
   * @note see InterfaceLL::griddedDataPathFilter(),
   *       InterfaceLL::dataPathParse()
   */
  static std::string dataPathString(const time_t &gt, const int &lt);

  /**
   * Convert from a Grid x,y index to a latitude/longitude using a projection
   *
   * @param[in] p  Projection to use
   * @param[in] x  Index into a Grid in the projection
   * @param[in] y  Index into a Grid in the projection
   * @param[out] lat  Returned latitude
   * @param[out] lon  Returned longitude
   */
  static void xyIndexToLatlon(const ParmProjection &p, const int x,
			      const int y, double &lat, double &lon); 
  
  /**
   * Convert from latitude/longitude to Grid x,y index using a projection
   *
   * @return false if the latitude/longitude is out of range of the Grid
   *         limits
   *
   * @param[in] proj  Projection to use
   * @param[in] lat  Input latitude
   * @param[in] lon  Input longitude
   * @param[out] x  Index into Grid in the projection
   * @param[out] y  Index into Grid in the projection
   */
  static bool latlonToXyIndex(const ParmProjection &proj, const double lat,
			      const double lon, int &x, int &y);
  
  /**
   * Convert from latitude/longitude to Grid x,y index using a projectin.
   * Allow indices out of range of the grid.
   *
   * @param[in] proj  Projection to use
   * @param[in] lat  Input latitude
   * @param[in] lon  Input longitude
   * @param[out] x  Index into Grid in the projection
   * @param[out] y  Index into Grid in the projection
   */
  static void latlonToXyIndexAny(const ParmProjection &proj, const double lat,
				 const double lon, int &x, int &y);
  
  /**
   * Convert from latitude/longitude to Grid x,y double values using a 
   * projection
   *
   * @return false if the latitude/longitude is out of range of the Grid
   *         limits
   *
   * @param[in] proj  Projection to use
   * @param[in] lat  Input latitude
   * @param[in] lon  Input longitude
   * @param[out] x  Index into Grid in the projection (double)
   * @param[out] y  Index into Grid in the projection (double)
   *
   * @note the x,y values are returned correctly, even if the return status
   * is false
   */
  static bool latlonToXy(const ParmProjection &proj, const double lat,
			 const double lon, double &x, double &y);
  
  /**
   * Convert from a Grid x,y index to latitude/longitude using a projection,
   * with real valued x,y allowed.
   *
   * @param[in] proj  Projection to use
   * @param[in] x  Double valued index into Grid
   * @param[in] y  Double valued index into Grid
   * @param[out] lat  Returned latitude
   * @param[out] lon  Returned longitude
   */
  static void xyToLatlon(const ParmProjection &proj, const double x,
			 const double y, double &lat, double &lon);

  /**
   * Find the distance in kilometers between two points in a Grid
   * using a projection
   *
   * @param[in] proj  Projection to use
   * @param[in] arrayIndex1  One dimensional index into a Grid object
   * @param[in] arrayIndex2  One dimensional index into a Grid object
   * @param[out] dist  The distance between the two points (kilometers)
   */
  static void distanceKm( const ParmProjection &proj, const int arrayIndex1,
			  const int arrayIndex2, double &dist);

  /**
   * Find the distance in kilometers between two points in a Grid using
   * a projection
   *
   * @param[in] proj  Projection to use
   * @param[in] indexI1  X index into the Grid, first point
   * @param[in] indexJ1  Y index into the Grid, first point
   * @param[in] indexI2  X index into the Grid, second point
   * @param[in] indexJ2  Y index into the Grid, second point
   *                     
   * @param[out] dist  The distance between the two points (kilometers)
   */
  static void distanceKm( const ParmProjection &proj, const int indexI1,
			  const int indexJ1, const int indexI2,
			  const int indexJ2, double &dist);

  /**
   * Find the distance in kilometers between two points in a Grid using
   * a projection, where the points can be partially between grid indices
   *
   * @param[in] proj  Projection to use
   * @param[in] indexI1  X real valued index into the Grid, first point
   * @param[in] indexJ1  Y real valued index into the Grid, first point
   * @param[in] indexI2  X real valued index into the Grid, second point
   * @param[in] indexJ2  Y real valued index into the Grid, second point
   *                     
   * @param[out] dist  The distance between the two points (kilometers)
   */
  static void distanceKmD( const ParmProjection &proj, const double indexI1,
			   const double indexJ1, const double indexI2,
			   const double indexJ2, double &dist);

  /**
   * Write the interior of multiple polygons to a grid, if possible
   * @param[in] proj  The projection to use
   * @param[in] latlon  vector of polygon Latitude/longitude pairs
   * @param[in] value  The value to write at each interior grid point
   * @param[out] grid  The grid to write mask to
   * @param[out] edge  The grid to write edges to
   *
   * @return true if successful, false if not
   */
  static bool
  polygonsToGrid(const ParmProjection &proj, 
		 const std::vector<std::vector<std::pair<double,double> > > &latlon,
		 const double value, GridData &grid, GridData &edge);

  /**
   * Write the interior of a polygon to a grid, if possible
   * @param[in] proj  The projection to use
   * @param[in] latlon  Latitude/longitude pairs for polygon
   * @param[in] value  The value to write at each interior grid point
   * @param[in,out] grid  The grid to write to, not cleared prior to filling in
   * @param[in,out] edge  The grid to write to, not cleared prior to filling in
   *
   * @return true if succesful, false if the lat/lon's were outside the grid
   */
  static bool
  polygonToGrid(const ParmProjection &proj, 
		const std::vector<std::pair<double,double> > &latlon,
		const double value, GridData &grid, GridData &edge);


  /**
   * override the default earth radius in km
   * @param[in] proj  The projection to use, which should have a radius member
   */
  static void setEarthRadiusKm(const ParmProjection &proj);
  

  /**
   * Log a string to where ever output logging information is written
   *
   * @param[in] msg  The string to log
   *
   * @note The method assumes there is no '\n' at end of the input string.
   *
   * @note This hides the output logging detail
   */
  static void log(const std::string &msg);

  /**
   * Write FSSClimo output for grid
   * @param[in] outputDir  Output directory
   * @param[in] year  Year value (generation time)
   * @param[in] month  Month value (generation time)
   * @param[in] day  Day value (generation time)
   * @param[in] genHourBegin  Generation begin time hour  
   * @param[in] genHourEnd  Generation begin time hour 
   * @param[in] leadSecsBegin  Lead time begin of averaging 
   * @param[in] leadSecsEnd  Lead time end of averaging 
   * @param[in] threshScale   Vector of threshold scale strings corresponding to the fss values 
   * @param[in] fssVal  Vector of FSS values corresponding to the scales and thresholds
   * @param[in] last  True if this is the last item in the current file
   */ 
   static bool writeFssClimoAscii(const std::string outputDir,
                                  const int year,
                                  const int month,
                                  const int day, 
                                  const float genHourBegin,
                                  const float genHourEnd,
                                  const float leadSecsBegin,
                                  const float leadSecsEnd,
                                  const std::vector <std::string> threshScale,
                                  const std::vector <double> fssVal,
				  bool last);
 
  /**
   * Write observation and valid forecast histogram data for a subgrid   
   *
   * @param[in] outputDir  Output directory 
   * @param[in] year  Year value (generation time)
   * @param[in] month  Month value (generation time)
   * @param[in] day  Day value (generation time)
   * @param[in] genHour  Generation time hour
   * @param[in] leadSecs  Lead time (seconds)
   * @param[in] gridNum
   * @param[in] obsHist  Vector of histogram bin totals for observation data
   *                     on subgrid of forecast domain at generation time and
   *                     lead time
   * @param[in] fcstHist  Vector of histogram bin totals for forecast on 
   *                      subgrid of forecast domain at generation time and
   *                      lead time
   * @param[in] missingVal  Missing data histogram value to use when needed
   * @param[in] isAve  True to write out averages
   * @param[in] numInObsAve  Averages information to write when isAve
   * @param[in] numInFcstAve  Averages information to write when isAve
   *
   * @return true for success
   */
  static bool writeFcstObsHists(const std::string outputDir, 
				const int year,
				const int month,
				const int day,
				const int genHour,
				const int leadSecs,
				const int gridNum,
				const std::vector <int> &obsHist, 
				const std::vector <int> &fcstHist,
				const int missingVal,
				const bool isAve = false,
				const int numInObsAve = 1,
				const int numInFcstAve = 1);

 /**
   * Write observation and valid forecast histogram data for a subgrid
   * 
   * @param[in] outputDir  Output directory
   * @param[in] year  Year value (generation time)
   * @param[in] month  Month value (generation time)
   * @param[in] day  Day value (generation time)
   * @param[in] genHour  Generation time hour
   * @param[in] leadSecs  Lead time (seconds)
   * @param[in] gridNum
   * @param[in] obsHist  Vector of avg bin value for observation data
   *                     on subgrid of forecast domain at generation time and
   *                     lead time
   * @param[in] fcstHist  Vector of avg bin value for forecast on
   *                      subgrid of forecast domain at generation time and
   *                      lead time
   * @param[in] missingVal  Missing data histogram value to use when needed
   * @param[in] isAve  True to write out averages
   * @param[in] numInObsAve  Averages information to write when isAve
   * @param[in] numInFcstAve  Averages information to write when isAve
   * 
   * @return true for success
   */
  static bool writeFcstObsQuantileStats(const std::string outputDir,
                                const int year,
                                const int month,
                                const int day,
                                const int genHour,
                                const int leadSecs,
                                const int gridNum,
                                const std::vector <float> &obsHist,
                                const std::vector <float> &fcstHist,
                                const float missingVal,
                                const bool isAve = false,
                                const int numInObsAve = 1,
                                const int numInFcstAve = 1);
   
protected:
private:  
   
  /** 
   * The date for which the last ascii FSSClimo data was written. Used for closing and opening
   * ouput data files.
   */ 
  static std::string lastFSSClimoDate;
};

# endif
