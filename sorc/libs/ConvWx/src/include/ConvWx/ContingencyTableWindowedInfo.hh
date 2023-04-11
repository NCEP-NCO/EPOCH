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
 * @file ContingencyTableWindowedInfo.hh
 * @brief  simple struct like information for windowed contingency tables
 * @class ContingencyTableWindowedInfo
 * @brief  simple struct like information for windowed contingency tables
 */

# ifndef    CONTINGENCY_TABLE_WINDOWED_INFO_HH
# define    CONTINGENCY_TABLE_WINDOWED_INFO_HH

#include <string>

//----------------------------------------------------------------
class ContingencyTableWindowedInfo
{
public:
  
  /**
   * Constructor, sets all members
   * @param[in] obsThresh  Observations threshold 
   * @param[in] fcstThresh  Forecast data threshold
   * @param[in] secondsBefore  Number of seconds before valid time in window
   *                           used for both obs and forecasts
   * @param[in] secondsAfter  Number of seconds after valid time in window
   *                          used for both obs and forecasts
   * @param[in] name   Name to give this particular window
   */
  inline ContingencyTableWindowedInfo(double obsThresh, double fcstThresh,
				      int secondsBefore, int secondsAfter,
				      std::string &name) :
    pObsThresh(obsThresh), pFcstThresh(fcstThresh),
    pObsSecondsBefore(secondsBefore), pObsSecondsAfter(secondsAfter),
    pFcstSecondsBefore(secondsBefore), pFcstSecondsAfter(secondsAfter),
    pName(name)
  {
  }

  /**
   * Constructor, sets all members
   * @param[in] obsThresh  Observations threshold 
   * @param[in] fcstThresh  Forecast data threshold
   * @param[in] obsSecondsBefore  Number of seconds before valid time in window,
   *                              oservations
   * @param[in] obsSecondsAfter  Number of seconds after valid time in window,
   *                              oservations
   * @param[in] fcstSecondsBefore  Number of seconds before valid time in
   *                               window, forecasts
   * @param[in] fcstSecondsAfter   Number of seconds after valid time in
   *                               window, forecasts
   * @param[in] name   Name to give this particular window
   */
  inline ContingencyTableWindowedInfo(double obsThresh, double fcstThresh,
				      int obsSecondsBefore, int obsSecondsAfter,
				      int fcstSecondsBefore,
				      int fcstSecondsAfter, std::string &name) :
    pObsThresh(obsThresh), pFcstThresh(fcstThresh),
    pObsSecondsBefore(obsSecondsBefore), pObsSecondsAfter(obsSecondsAfter),
    pFcstSecondsBefore(fcstSecondsBefore), pFcstSecondsAfter(fcstSecondsAfter),
    pName(name)
  {
  }


  /**
   * Destructor
   */
  inline virtual ~ContingencyTableWindowedInfo(void) {}

  /**
   * @return name of 'hit' field associated with local state
   */
  inline std::string hitName(void) const {return "H_" + pName;}
  /**
   * @return name of 'miss' field associated with local state
   */
  inline std::string missName(void) const {return "M_" + pName;}
  /**
   * @return name of 'false positive' field associated with local state
   */
  inline std::string falsepName(void) const {return "F_" + pName;}
  /**
   * @return name of 'none' field associated with local state
   */
  inline std::string noneName(void) const {return "N_" + pName;}

  /**
   * @return name of 'Bias' field associated with local state
   */
  inline std::string biasName(void) const {return "BIAS_" + pName;}
  /**
   * @return name of 'CSI' field associated with local state
   */
  inline std::string csiName(void) const {return "CSI_" + pName;}
  /**
   * @return name of 'HSS' field associated with local state
   */
  inline std::string hssName(void) const {return "HSS_" + pName;}
  /**
   * @return name of 'FAR' field associated with local state
   */
  inline std::string farName(void) const {return "FAR_" + pName;}
  /**
   * @return name of 'POD' field associated with local state
   */
  inline std::string podName(void) const {return "POD_" + pName;}

  /**
   * @return name of 'PODno' field associated with local state
   */
  inline std::string podNoName(void) const {return "PODNO_" + pName;}

  /**
   * @return name of 'ObsFreq' field associated with local state
   */
  inline std::string obsFreqName(void) const {return "OBSFREQ_" + pName;}
  /**
   * @return name of 'FcstFreq' field associated with local state
   */
  inline std::string fcstFreqName(void) const {return "FCSTFREQ_" + pName;}

  double pObsThresh;  /**< Observations threshold */
  double pFcstThresh; /**< Forecast threshold */
  int pObsSecondsBefore; /**< Seconds before valid time for window, obs */
  int pObsSecondsAfter;  /**< Seconds after valid time for window, obs */
  int pFcstSecondsBefore; /**< Seconds before valid time for window, forecast */
  int pFcstSecondsAfter;  /**< Seconds after valid time for window, forecast */
  std::string pName;  /**< Name */

protected:
private:  

};

# endif     // ContingencyTableWindowedInfo_HH
