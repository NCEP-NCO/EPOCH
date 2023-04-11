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
/**
 * @file ClimoHistRdr.hh
 * @brief  ClimoHistRdr reads climatology based distributions statistics 
 *         of forecast data and observations used to calibrate model at user 
 *         specified generation hour and forecast lead time resolutions.
 * @class ClimoHistRdr
 * @brief ClimoHistRdr reads climatology based distributions statistics 
 *        of forecast data and observations used to calibrate model at user 
 *        specified generation hour and forecast lead time resolutions.
 * @note 
 * @todo Input of forecast and matching observation data histograms will change 
 * with conversion of input to self-describing file format like netCDF.
 */

#ifndef CLIMO_HIST_RDR_HH
#define CLIMO_HIST_RDR_HH

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

class ClimoHistRdr{

public:

  /** 
   *   Constructor  
   */
  ClimoHistRdr(void);

  /** 
   *   Destructor  
   */
  virtual ~ClimoHistRdr(void);

  /**
   * Initialization routine creates containers to hold ratios of histogram
   * bin totals or another bin statistic of climatological distributions of 
   * forecast data and observations.Histogram bin total ratios are stored 
   * for each sub-grid and at generation time and lead time resolutions.
   * @param[in] numSubgrids  Number of sub-grids of domain
   * @param[in] genHourRes  Resolution of generation times of 
   *                        climatological forecast data distributions 
   * @param[in] leadHourRes  Resolution of lead times of of 
   *                         climatological forecast data distributions 
   * @param[in] maxLeadHour  Maximum expected generation hour 
   * @param[in] inputDir  Directory containing climatological data 
   *                      distributions
   * @param[in] obsBias  Vector of multipliers to apply to climatological 
   *                     histograms of observation data
   * @param[in] ratioMin  Lower bound on the ratio of histogram bin totals
   * @param[in] ratioMax  Upper bound on the ratio of histogram bin totals
   * @param[in] maxAge   Maximum age in days of climatological data that 
   *                     will be used in real-time 
   * @param[in] realtime  A flag indicating real-time or archive processing
   * @param[in] isQuantileData The histogram data statistic for each bin is 
   *                           either average histogram or bin totals(int) 
   *                           or quantile average value (float). This affects the
   *                           read and storage of the data in the file. 
   */
  void init(const int numSubgrids, const float genHourRes,
	    const float leadHourRes, const float maxLeadHour, 
	    const std::string inputDir, const std::vector <float > &obsBias, 
	    const float ratioMin, const float ratioMax, const int maxAge, 
	    const bool realtime,  const bool isQuantileData = false); 

  /**
   * Get ratios of observation to model histogram bin totals for specific
   * sub-grid, generation time, and lead hour.
   * @param[in] subgridNum  Sub-Grid identification number
   * @param[in] genHour  Generation hour
   * @param[in] leadHour  Lead time in hours
   *
   * @return The ratios
   */
  const std::vector <float> & getObsModelRatios(int subgridNum, float genHour, 
						float leadHour) const;

  /**
   * Get average observation histogram bin totals for specific
   * sub-grid, generation time, and lead hour.
   * @param[in] subgridNum  Sub-Grid identification number
   * @param[in] genHour  Generation hour
   * @param[in] leadHour  Lead time in hours
   *
   * @return The vector of average historgram bin totals 
   */
  const std::vector <float> & getObsAvg(int subgridNum, float genHour, 
					float leadHour) const;

  /**
   * Get average model forecast historgram bin totals for specific
   * sub-grid, generation time, and lead hour.
   * @param[in] subgridNum  Sub-Grid identification number
   * @param[in] genHour  Generation hour
   * @param[in] leadHour  Lead time in hours
   *
   * @return The vector of average model histogram bin totals 
   */
  const std::vector <float> & getFcstAvg(int subgridNum, float genHour,
                                         float leadHour) const;

  /**
   * Get differences of the averages of observation and model histogram bin totals for specific
   * sub-grid, generation time, and lead hour.
   * @param[in] subgridNum  Sub-Grid identification number
   * @param[in] genHour  Generation hour
   * @param[in] leadHour  Lead time in hours
   *
   * @return The vector of differences of average observation and model histogram bin totals 
   */
  const std::vector <float> & getObsModelAvgDiff(int subgridNum, float genHour, 
						 float leadHour) const;
 
  /**
   * Using ordered pairs of (x(i),y(i)) and (x(i+1), y(i+1))
   * where x(i) represents average forecast histogram bin total   
   * for bin i, and y(i) represents average observation histogram
   * bin total for bin i, Return the vector containing the slopes of
   * the lines joining points  (x(i),y(i)) and (x(i+1), y(i+1)) for specific
   * sub-grid, generation time, and lead hour.
   * @param[in] subgridNum  Sub-Grid identification number
   * @param[in] genHour  Generation hour
   * @param[in] leadHour  Lead time in hours
   *
   * @return The ratios
   */
  const std::vector <float> & getBin2BinSlopes(int subgridNum, float genHour, 
			float leadHour) const;
 
  /**
   * 
   * Using ordered pairs of (x(i),y(i)) and (x(i+1), y(i+1))
   * where x(i) represents average forecast histogram bin total
   * for bin i, and y(i) represents average observation histogram
   * bin total for bin i, Return the vector containing the y intercepts
   * the lines joining points  (x(i),y(i)) and (x(i+1), y(i+1)) for specific
   * sub-grid, generation time, and lead hour.
   *  
   * @param[in] subgridNum  Sub-Grid identification number
   * @param[in] genHour  Generation hour
   * @param[in] leadHour  Lead time in hours
   *
   * @return The ratios
   */
  const std::vector <float> & getBin2BinIntercepts(int subgridNum, 
						   float genHour, 
						   float leadHour) const;

protected:

private:
 
  /**
   * Observation/Model ratios at each histogram bin (for each sub-grid,at
   * each generation time, and each lead time)
   */
  std::vector < std::vector < std::vector < std::vector <float> > > >  
  pRatiosObsModel;

  /**
   * Observation/Model differences at each histogram bin (for each sub-grid,at
   * each generation time, and each lead time)
   */
  std::vector < std::vector < std::vector < std::vector <float> > > >  
  pObsModelAvgDiff;

  /**
   * Observation averages at each histogram bin (for each sub-grid,at
   * each generation time, and each lead time)
   */
  std::vector < std::vector < std::vector < std::vector <float> > > >  
  pObsAvg;

  /**
   * Forecast averages at each histogram bin (for each sub-grid,at
   * each generation time, and each lead time)
   */
  std::vector < std::vector < std::vector < std::vector <float> > > >
  pFcstAvg;

  /**
   * Slopes between histogram bins (for each sub-grid,at
   * each generation time, and each lead time)
   */
  std::vector < std::vector < std::vector < std::vector <float> > > >  
  pSlopes;

  /**
   * Y interpcepts of lines formed with point slope equation of a line using points (x,y) where x = avg model bin val and y = avg obs bin val.  For all bins (for each sub-grid,at
   * each generation time, and each lead time)
   */
  std::vector < std::vector < std::vector < std::vector <float> > > >  
  pIntercepts;
  
  
  /**
   * Directory containing climatological data 
   */
  std::string pClimoHistDir;

  /**
   * Resolution of generation times of climatological forecast data 
   * distributions 
   */
  float pGenHourRes;
  
  /**
   * Resolution of lead times of climatological forecast data distributions 
   */
  float pLeadHourRes;
  
  /**
   * Number of generation times 
   */
  int pNumGenHours;
  
  /**
   * Number of lead times
   */
  int pNumLeads;

  /**
   * Vector of multipliers to apply to climatological histograms of 
   * observation data
   */
  std::vector <float> pObsBias;

  /**
   * Lower bound on the ratio of histogram bin totals
   */
  float pRatioMin;

  /**
   * Upper bound on the ratio of histogram bin totals
   */
  float pRatioMax;

  /**
   * Maximum age in days of climatological data that will be used in real-time 
   */  
  int pMaxAge;

  /**
   * Flag indicating real-time or archive processing
   */
  bool pRealtime;

  /**
   * Required number of histogram bins in input data histograms
   */
  int pRequiredBinNum;

  /**
   * The data histogram statistic for each bin is either average histogram 
   * or bin totals(int) or quantile average value (float). This affects the
   * read and storage of the data in the file.
   */
  bool pIsQuantileData;
  
  /**
   * Private methods
   */ 
  
  /**
   * Load climatolgical data distributions for each sub-grid at specified
   * generation and lead time resolution
   */
  void pLoad(void);

  /**
   * Clear container holding ratios of observation to forecast histogram bin
   * totals
   */
  void pClear(void);

  /**
   * Apply observation bias to observation histogram bin totals. Find ratios of 
   * observation to model histogram bin totals.
   * @param[in] fcstHist  Vector of containing forecast histogram bin totals
   * @param[in] obsHist  Vector of containing observation histogram bin totals
   * @param[out] ratio  Vector containing ratios of observation to forecast 
   *                     histogram bin totals
   */
  void pComputeRatios(const std::vector <int> &fcstHist, 
		      const std::vector <int> &obsHist,
		      std::vector <float>  &ratio);

  /**
   * Construct climatological histogram data file path in real-time mode. 
   * Files are expected to have the format:
   * pClimoHistDir/yyyymmdd/g_hhmmss/f_ssssssss.ascii.
   * Most recent available file will be opened within a 
   * @param[in] numGen  Integer id of generation hour
   * @param[in] numLead  Integer id of lead time
   * @param[out] filepath  Path of data file.
   *
   * @return true for success
   */
  bool pGetFileRealtime(const int numGen, const int numLead, 
			std::string &filepath);

  /**
   * Construct  climatological histogram data file path and open file. Files 
   * are expected to have the format: pClimoHistDir/g_hhmmss/f_ssssssss.ascii. 
   *(Note that the user specifies the date to be used in parameter file).
   * @param[in] numGen  Integer id of generation hour
   * @param[in] numLead  Integer id of lead hours
   * @param[out] filepath  Path of data file.
   *
   * @return true for success
   */
  bool pGetFileArchive(const int numGen, const int numLead, 
		       std::string &filepath);
  
  /**
   * Determine if a file exists
   * @param[in] filepath  Path of file for which to check existence
   * @return true if file exists, false if the file does not.
   */
  bool pfileExists(const std::string filepath);

  void pComputeLinearFunction(const std::vector <float> &fcstHist, 
			      const std::vector <float> &obsHist,
			      const int subgridNum, const int genNum, const int leadNum);
};

#endif
