// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// � University Corporation for Atmospheric Research (UCAR) 2009-2010. 
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
 * @file ParmFcstIO.hh
 * @brief ParmFcst base class, with methods to handle I.O in the derived class.
 * @class ParmFcstIO
 * @brief ParmFcst base class, with methods to handle I.O in the derived class.
 *
 * This class has many methods that access data as a client with an assumed
 * server. The server is specified by a URL
 */

# ifndef    PARM_FCST_IO_HH
# define    PARM_FCST_IO_HH

#include <string>
#include <vector>
#include <cmath>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ParmFcst.hh>

class ParmProjection;
class FcstGrid;
class MultiGrid;
class MultiFcstGrid;
class Grid;
class UvOutput;
class MetaData;

//----------------------------------------------------------------
class ParmFcstIO : public ParmFcst
{
public:

  /**
   * Default constructor. values not set
   */
  ParmFcstIO(void);

  /**
   * Constructor from the base class
   * @param[in] parm  The base class parameters to use
   */
  ParmFcstIO(const ParmFcst &parm);

  /**
   * Destructor
   */
  virtual ~ParmFcstIO(void);

  /**
   * @return true if inputs are available
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   *
   * @note queries the server
   */
  bool hasData(const time_t &gt, const int lt) const;

  /**
   * @return true if any forecast at all is available at a generation time.
   * @param[in] gt  Generation time
   *
   * @note queries the server
   */
  bool hasData(const time_t &gt) const;

  /**
   * @return true if all configured forecasts are available at a generation time
   * @param[in] gt  Generation time
   *
   * @note queries the server
   */
  bool hasAllData(const time_t &gt) const;

  /**
   * Determine if for a particular generation time, there is a lead time <=
   * input lead time for which a forecast is available.
   * If some such forecast is available, set the lead time to the largest
   * such lead time for which this is true.
   *
   * @param[in] gt   Generation time
   * @param[in,out] lt  Bounding lead time on entry, available lead time on
   *                    exit
   *
   * @return true if conditions are satisfied.
   */
  bool matchingGentimeNewestLeadLessOrEqual(const time_t &gt, int &lt) const;


  /**
   * @return The number of forecasts at a generation time between a range of lead
   *         times that are available
   * @param[in] gt  Generation time
   * @param[in] lt0  Minimum lead times seconds
   * @param[in] lt1  Maximum lead times seconds
   */
  int numAvailable(const time_t &gt, const int lt0, const int lt1) const;

  /**
   * @return true if all configured forecasts at a generation time between a range
   *         of lead times are available
   * @param[in] gt  Generation time
   * @param[in] lt0  Minimum lead times seconds
   * @param[in] lt1  Maximum lead times seconds
   * @param[out] num  Number that are available in this range
   */
  bool allAvailable(const time_t &gt, const int lt0, const int lt1,
		    int &num) const;

  /**
   * @return all lead times (seconds) that are available at a particular
   * generation  time
   *
   * @param[in] gt  Generation time
   *
   * @note queries the server
   */
  std::vector<int> getAvailableLeadTimes(const time_t &gt) const;

  /**
   * @return pairs of available generation time/lead seconds that fall in a
   * range of generation times.  Use server to find which gen times that synch
   * to the ParmFcst configuration values are within the range specified. The
   * server is used to determine which lead times that synch to the config for
   * each such gen time are available.  
   * 
   * @param[in] t0  Earliest allowed generation time
   * @param[in] t1  Latest allowed generation time
   *
   * @note queries server
   */
  std::vector<std::pair<time_t,int> >
  getAvailableFcts(const time_t &t0,  const time_t &t1) const;

  /**
   * find smallest available generation time greater or equal to a
   * particular time, not after t1
   *
   * @param[in] t  The lower time that bounds the returned value
   * @para[in] t1  The upper time that bounds the returned value
   * @param[out] ret  The smallest gen time >= input time
   *
   * @return true if there was data for this day that fit the query
   * false if none
   */
  bool smallestAvailableGenGe(time_t t, time_t t1, time_t &ret) const;

  /**
   * find largest available generation time less or equal to a
   * particular time, not before t0
   *
   * @param[in] t  The upper time that bounds the returned value
   * @para[in] t0  The lower time that bounds the returned value
   * @param[out] ret  The largest gen time <= input time
   *
   * @return true if there was data for this day that fit the query
   * false if none
   */
  bool largestAvailableGenLe(time_t t, time_t t0, time_t &ret) const;

  /**
   * Return a generation time that satisfies the following:
   * -  Data exists for the generation time (query the server)
   * -  The generation time is within a range of times <= the input generation time
   * -  The generation time hour/minute/second = input generation time
   *    hour/minute/second or
   *    the generation time hour/minute/second has the minimum positive
   *    difference compared to the input generation time.
   * -  The generation time has all lead times available
   * -  The generation time is the newest generation time that satisfies all of the above.
   *    If no generation time satisfies all of the above, relax the last requirement
   *    that all leads are available.
   *
   * (hour/minute/second = hour*3600 + minute*60 + second )
   *
   * @param[in] gt  The generation time to compare against
   * @param[in] secondsBack  Number of seconds back from the input generation
   *                         time to look for 
   *                         generation times that satisfy the criteria
   *
   * @return the generation time, or BAD_TIME if none.
   */
  time_t similarOlderGenTime(const time_t &gt, const int secondsBack) const;
  
  /**
   * Load observations data into a Grid.
   * @param[in] t  Time
   * @param[in] proj  Projection information that may be used 
   * @param[out] outGrid  Grid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadObsData(const time_t t, const ParmProjection &proj,
		   Grid &outGrid, bool suppressWarnings=false) const;

  /**
   * Load observations data into a Grid.
   * @param[in] t  Time
   * @param[in] proj  Projection information that may be used
   * @param[out] outGrid  Grid filled with data on exit
   * @param[out] vertLevels  Vector filled with vertical levels 
   * @return true for success
   *
   * @note queries server
   */
 
  bool loadObsData(const time_t t, const ParmProjection &proj,
                   Grid &outGrid, std::vector<double> &vertLevels) const;

  /**
   * Load observations data into a Grid.
   * @param[in] t  Time
   * @param[in] proj  Projection information that may be used
   * @param[in] fName Name of field
   * @param[out] outGrid  Grid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadObsData(const time_t t, const ParmProjection &proj,
                   const std::string &fName, Grid &outGrid) const;

  /**
   * Load observations data into a FcstGrid.
   * @param[in] t  Time
   * @param[in] proj  Projection information that may be used 
   * @param[out] outGrid  FcstGrid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadObsData(const time_t t, const ParmProjection &proj,
		   FcstGrid &outGrid) const;

  /**
   * Load observations data into a FcstGrid.
   * @param[in] t  Time
   * @param[in] proj  Projection information that may be used
   * @param[in] fName Name of field
   * @param[out] outGrid  FcstGrid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadObsData(const time_t t, const ParmProjection &proj,
                   const std::string &fName, FcstGrid &outGrid) const;


  /**
   * Load forecast data into a FcstGrid
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] proj   Projection information that may be used 
   * @param[out] outGrid  FcstGrid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadFcstData(const time_t gt, const int lt, const ParmProjection &proj,
		    FcstGrid &outGrid) const;

  /**
   * Load forecast data into a FcstGrid
   *
   * @note same as the other method with the same name, except data request
   *       is for an input field instead of parameter field
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] proj   Projection information that may be used 
   * @param[in] fname  Field name
   * @param[out] outGrid  FcstGrid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadFcstData(const time_t gt, const int lt, const ParmProjection &proj,
		    const std::string &fname, FcstGrid &outGrid) const;

  /**
   * Load forecast data into one or more grids 
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] proj   Projection information that may be used 
   * @param[in] fname  Field names (one or more)
   * @param[out] outGrids  MultiFcstGrid filled with data on exit
   * @param[in] suppressErrorMessage  True to suppress errors when read fails
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadMultiFcstData(const time_t gt, const int lt,
			 const ParmProjection &proj,
			 const std::vector<std::string> &fname,
			 MultiFcstGrid &outGrids,
			 bool suppressErrorMessage=false) const;

  /**
   * Load forecasts for all configured lead times at a generation time into 
   * a vector of FcstGrid.
   * 
   * @param[in] gt  Generation time
   * @param[in] proj  Projection information that may be used 
   * @param[out] outGrids  Vector of FcstGrid filled with data on exit
   *
   * @return true for success, false if data for any lead time was not
   *         successfully retrieved
   *
   * @note queries server
   */
  bool loadAllFcstData(const time_t gt, const ParmProjection &proj,
		       std::vector<FcstGrid> &outGrids) const;

  /**
   * Load forecasts for all configured lead times at a generation time into 
   * a vector of FcstGrid.
   * 
   * @note same as the other method with the same name, except data request
   *       is for an input field instead of parameter field
   *
   * @param[in] gt  Generation time
   * @param[in] proj  Projection information that may be used 
   * @param[in] fname  Field name
   * @param[out] outGrids  Vector of FcstGrid filled with data on exit
   *
   * @return true for success, false if data for any lead time was not
   *         successfully retrieved
   *
   * @note queries server
   */
  bool loadAllFcstData(const time_t gt, const ParmProjection &proj,
		       const std::string &fname,
		       std::vector<FcstGrid> &outGrids) const;

  /**
   * Load forecasts at all configured lead times for which data in a
   * range of generation times is valid at a particular time.
   * @param[in] vt  The valid time
   * @param[in] latencyHours  The range of latency hours to use.
   *                          latencyHours[0] is the smallest possible latency.
   *                          latencyHours[1] is the largest possible latency.
   *                          All generation times must be at least this much
   *                          older than the input valid time
   * @param[in] proj  Projection information that may be used 
   * @param[out] outGrids  Vector of FcstGrid filled with data on exit
   *
   * @return true for success
   *
   * @note queries server
   */
  bool loadValidFcstData(const time_t vt, const int latencyHours[2],
			 const ParmProjection &proj,
			 std::vector<FcstGrid> &outGrids) const;

  /**
   * return gen/lead pairs for all forecasts valid at a particular time.
   * 
   * @param[in] vt  The valid time
   *
   * @return gen/lead pairs
   *
   * @note queries server
   */
  std::vector<std::pair<time_t, int> >  genLeadVaidAt(const time_t vt);

  /**
   * Write one or more forecasts 
   *
   * @param[in] gt  Generation time of data
   * @param[in] lt  Lead seconds of data
   * @param[in] proj  Projection information that may be used 
   * @param[in] outGrids   The Grid data to write out (one or more)
   * @param[in] metadata  MetaData to attach to the output (empty for none)
   *
   * @note the local URL is used as the destination
   */
  void write(const time_t &gt, const int lt, const ParmProjection &proj,
	     const MultiGrid &outGrids, const MetaData &metadata) const;

  /**
   * Write one or more observations
   *
   * @param[in] gt  Time of data
   * @param[in] proj  Projection information that may be used 
   * @param[in] outGrids  Grids to write out
   * @param[in] metadata  MetaData to attach to the output (empty for none)
   */
  void writeObs(const time_t &gt, const ParmProjection &proj,
		const MultiGrid &outGrids, const MetaData &metadata) const;

  /**
   * Write one forecast 
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] proj  Projection information that may be used 
   * @param[in] outGrid  FcstGrid to write out, which contains MetaData
   */
  void write(const time_t &gt, const int lt, const ParmProjection &proj,
	     const FcstGrid &outGrid) const;

  /**
   * Write one observations output 
   *
   * @param[in] gt  Time
   * @param[in] proj  Projection information that may be used 
   * @param[in] outGrid  FcstGrid to write out, which contains MetaData
   */
  void writeObs(const time_t &gt, const ParmProjection &proj,
		const FcstGrid &outGrid) const;


protected:
private:  


  /**
   * If the hour/minute/second of the input generation time - hourMinSec0 is
   * equal to  'bestDiff' do the following:
   *   -  if first is true
   *       -  set 'bestGt' to gt (and set first=false).
   *   -  if the time has all lead times available (query server),
   *       -  set 'bestGt' to gt and return true.
   *
   * In all other cases return false.
   * 
   * @param[in] gt  Input Generation time
   * @param[in] hms0  Hours,minutes/seconds to compare against
   * @param[in,out] bestDiff  A specific Difference to look for
   * @param[in,out] bestGt   Adjusted in specific situations.
   * @param[in,out] first   True if this is the first call to this method
   *
   * @return true if bestDiff = current difference, and input time has all lead
   * times available.
   */
  bool pHmsDiffIsMinAndHasAllLeadTimes(const time_t &gt, const double hms0,
				       const double bestDiff,
				       time_t &bestGt, bool &first) const;
};

# endif 
