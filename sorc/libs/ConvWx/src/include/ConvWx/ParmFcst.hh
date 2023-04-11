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
 * @file ParmFcst.hh
 * @brief All parameters for one forecast data set
 * @class ParmFcst
 * @brief All parameters for one forecast data set
 *
 * Any members that are hardwired are const, all are public by design
 *
 */

# ifndef    PARM_FCST_HH
# define    PARM_FCST_HH

#include <string>
#include <vector>
#include <cmath>
#include <ConvWx/ConvWxConstants.hh>

class ParmProjection;
class FcstGrid;
class MultiGrid;
class MultiFcstGrid;
class Grid;
class UvOutput;
class MetaData;

//----------------------------------------------------------------
class ParmFcst
{
public:

  /**
   * Default constructor. values not set
   */
  ParmFcst(void);

  /**
   * Constructor with only names indicated, and whether the data is
   * required or not.
   * @param[in] name  Short name
   * @param[in] descr  Longer description
   * @param[in] isRequired  True if the data type is required
   */
  ParmFcst(const std::string &name, const std::string &descr,
	   const bool isRequired);

  /**
   * Constructor with all values set using inputs
   *
   * @param[in] name  Short name
   * @param[in] descr Longer description
   * @param[in] isRequired  True if the data type is required
   * @param[in] url   The Server location
   * @param[in] field  The data field name
   * @param[in] units  The data units
   * @param[in] hour0  The earliest lead time hour
   * @param[in] hour1  The latest lead time hour
   * @param[in] deltaHours  The increment between lead hours
   * @param[in] remap  True to re-map data upon read using a ParmProjection
   * @param[in] genDeltaMinutes  The number of minutes between generation times.
   */
  ParmFcst(const std::string &name, const std::string &descr,
	   const bool isRequired, const std::string &url,
	   const std::string &field, const std::string &units,
	   const double hour0, const double hour1, const double deltaHours,
	   const bool remap, const int genDeltaMinutes);
  
#ifdef NOTDEF
  /**
   * constructor with all values set using inputs, when the generation time
   * rate is unknown
   *
   * @param[in] name  Short name
   * @param[in] descr Longer description
   * @param[in] isRequired  True if the data type is required
   * @param[in] url   The Server location
   * @param[in] field  The data field name
   * @param[in] units  The data units
   * @param[in] hour0  The earliest lead time hour
   * @param[in] hour1  The latest lead time hour
   * @param[in] deltaHours  The increment between lead hours
   * @param[in] remap  True to re-map data upon read using a ParmProjection
   */
  ParmFcst(const std::string &name, const std::string &descr, 
	   const bool isRequired, const std::string &url,
	   const std::string &field, const std::string &units,
	   const double hour0, const double hour1, const double deltaHours,
	   const bool remap);
#endif

  
  /**
   * Destructor
   */
  virtual ~ParmFcst(void);

  /**
   * Fill in all member values except name, description and isRequired.
   *
   * @param[in] url   The Server location
   * @param[in] field  The data field name
   * @param[in] units  The data units
   * @param[in] hour0  The earliest lead time hour
   * @param[in] hour1  The latest lead time hour
   * @param[in] deltaHours  The increment between lead hours
   * @param[in] remap  True to re-map data upon read using a ParmProjection
   * @param[in] genDeltaMinutes  The number of minutes between generation times.
   */
  void setDetails(const std::string &url, const std::string &field,
		  const std::string &units, const double hour0,
		  const double hour1, const double deltaHours,
		  const bool remap, const int genDeltaMinutes);
  
#ifdef NOTDEF
  /**
   * Fill in all member values except name, description and isRequired, for
   * the case of not knowing generation time rate.
   *
   * @param[in] url   The Server location
   * @param[in] field  The data field name
   * @param[in] units  The data units
   * @param[in] hour0  The earliest lead time hour
   * @param[in] hour1  The latest lead time hour
   * @param[in] deltaHours  The increment between lead hours
   * @param[in] remap  True to re-map data upon read using a ParmProjection
   */
  void setDetails(const std::string &url, const std::string &field,
		  const std::string &units, const double hour0,
		  const double hour1, const double deltaHours,
		  const bool remap);
#endif
  

  /**
   * @return true if object is well formed
   */
  inline bool isOk(void) const
  {
    return pOk;
  }

  /**
   * @return true if the URL is set to a valid value
   */
  inline bool wasSet(void) const
  {
    return ((pUrl != "not_set") && (!pUrl.empty()));
  }

  /** 
   * @return true if the input lead seconds is the largest (last) lead time
   * @param[in] ltSeconds 
   */
  bool isLastLt(const int ltSeconds) const;

  /**
   * @return The number of lead times specified by parameters
   */
  inline int nlead(void) const 
  {
    return static_cast<int>(((pLtHour1-pLtHour0)/pLtDeltaHours)) + 1;
  }

  /**
   * @return the last lead time seconds
   */
  inline int lastLeadSeconds(void) const
  {
    return static_cast<int>(pLtHour1*convWx::DOUBLE_SECS_PER_HOUR);
  }
  
  /**
   * @return the number of lead times in a range of lead times
   * @param[in] lt0  Minimum lead times seconds to consider
   * @param[in] lt1  Maximum lead times seconds to consider
   */
  int nlead(const int lt0, const int lt1) const;

  /**
   * @return i'th lead hour
   * @param[in] i  Index of lead hour
   */
  inline double ithLeadHours(const int i) const 
  {
    return pLtHour0 + i*pLtDeltaHours;
  }

  /**
   * @return i'th lead seconds
   * @param[in] i  Index of lead seconds
   */
  inline int ithLeadSeconds(const int i) const 
  {
    return
      static_cast<int>(ithLeadHours(i)*convWx::DOUBLE_SECS_PER_HOUR);
  }

  /**
   * @return The index of the lead time that corresponds to input,
   *         BAD_INDEX for none
   *
   * @param[in] lt  Lead time seconds
   */
  int leadSecondsIndex(const int lt) const;

  /**
   * @return  All lead hours 
   */
  std::vector<double> getLtHours(void) const;

  /**
   * @return  All lead seconds
   */
  std::vector<int> getLtSeconds(void) const;

  /**
   * Return the generation/lead time pairs that are valid at the input time
   * based on configuration, in decreasing order of generation time (largest
   * generation time is first)
   *
   * @param[in] vt  Valid time
   *
   * @return  Vector of pairs where first=generation time, second=lead seconds
   */
  std::vector<std::pair<time_t,int> > fcstsValidAt(const time_t &vt) const;

  /**
   * @return All configured for generation times that satisfy the input
   * requirements
   *
   * @param[in] t0  An upper time bound
   * @param[in] latencyHours  The range of latency hours to use.
   *                          latencyHours[0] is the smallest possible latency.
   *                          latencyHours[1] is the largest possible latency.
   *                          All generation times must be at least this much older
   *                          than the input time t0
   * @param[in] oldest  The oldest allowed generation time, or BAD_TIME to
   *                    ignore
   *
   * This method will return all generation times that are configured for in the
   * range   [t0-latencyHours[1], t0-latencyHours[0]] that are not older
   * than oldest.
   */
  std::vector<time_t> genPossible(const time_t t0, const int latencyHours[2],
				  const time_t oldest) const;


  /**
   * @return i'th forecast hour, with index and bounds checking
   *
   * @param[in] i  Index
   *
   * If the index is out of range, it is truncated or rounded to
   * become in range
   */
  inline double ithFcstHour(const int i) const
  {
    if (i < 0)
    {
      return pLtHour0;
    }
    else
    {
      return fmin(pLtHour0 + i*pLtDeltaHours, pLtHour1);
    }
  }

  /**
   * @return  Smallest possible generation time greater or equal to a
   *          particular time, based on member settings
   *
   * @param[in] t  The time that bounds the returned value
   *
   */
  time_t smallestGenGe(time_t t) const;

  /**
   * @return  Largest possible generation time less or equal to a particular
   *          time, baesd on member settings.
   *
   * @param[in] t  The time that bounds the returned value
   *
   */
  time_t largestGenLe(time_t t) const;

  /**
   * @return the full set of possible hour/minute/second generation time
   *         values for the configuration. A returned value = 
   *         hour*3600 + minute*60 + second
   *
   */
  std::vector<int> hourMinuteSeconds(void) const;

  /**
   * @return lead time seconds nearest to input lead time seconds
   * @param[in] lt  Lead time seconds
   */
  int nearestLeadSeconds(int lt) const;

  /**
   * @return lead time seconds nearest to input valid time,
   * for the input generation time
   * @param[in] validTime
   * @param[in] generationTime
   */
  int nearestLeadSecondsValidTime(const time_t &validTime,
				  const time_t &generationTime) const;
  

  bool pOk;          /**< true if values were set false if not. */
  std::string pName; /**< short name description */
  std::string pDescription; /**< longer description */
  bool pIsRequired;  /**< true if this data input/output is required by app */
  std::string pUrl;  /**< where the forecast data is */
  std::string pField; /**< the field name */
  std::string pUnits; /**< the field units */
  double pLtHour0;    /**< earliest lead hour */
  double pLtHour1;    /**< latest lead hour */
  double pLtDeltaHours; /**< lead delta hours */
  bool   pRemap;      /**< true to re-map this data when read to projection 
		       * parameters */
  int pGenFrequencySeconds; /**< Seconds between generation times */

protected:

  /**
   * Adjust the value 'bestDiff' if the (hour/minute/second of the input
   * generation time - hourMinSec0) is less than input bestDiff and
   * non-negative.
   *
   * @param[in] gt  Input Generation time
   * @param[in] hms0  Hours,minutes/seconds to compare against
   * @param[in,out] bestDiff  Adjusted if a new minimum is found
   * @param[in,out] first  True if this is the first call to this method
   */
  void pAdjustHmsDiff(const time_t &gt, const double hms0, double &bestDiff,
		      bool &first) const;

private:  

  /**
   * @return true if the gen time sequence falls onto the same
   *         hour/minute/second values for all days.
   */
  bool pIsPeriodic(void) const;
};

# endif 
