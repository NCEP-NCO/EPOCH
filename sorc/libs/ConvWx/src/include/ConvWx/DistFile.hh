/**
 * @file DistFile.hh
 * @brief  DistFile parses or writes files containing observation and valid 
 *         forecast histogram statistics (bin totals, averge values etc).  
 * @class DistFile
 * @brief  DistFile parses or writes files containing observation and valid 
 *         forecast histogram statistics (bin totals, averge values etc).  
 */

#ifndef DISTFILE_HH
#define DISTFILE_HH

#include <string>
#include <iostream>
#include <vector>
#include <ConvWx/DistStats.hh>

class DistFile {

public:

  /**
   * Constructor
   */ 
  DistFile(void);
  
  /**
   * Destructor
   */
  virtual ~DistFile(void);

  /**
   * Parse ASCII file containing observation and forecast distribution data
   * @param[in] inputPath  Character array representing input file
   * @param[out] distStats  Container to hold the observation and forecast
   *                       distribution statistics
   * @param[in] isAve  Flag to indicate that the input file contains 
   *                   distribution averages. (The file format for averaged 
   *                   data differs by a line from files without averaged data 
   * @param[in] isInt  Flag to indicate integer data contained in file, Otherwise
   *                   floating point data is assumed.
   *
   * @return true for success
   */
  bool parse(const std::string inputPath, std::vector<DistStats*>  &distStats,
	     const bool isAve = false, const bool isInt = true);
  
  /**
   * Write average distributions for all subgrids at each generation and 
   * lead time. 
   *  @param[in] pAveStats  Container holds average histogram bin totals for 
   *                        generation and lead time and each subgrid of the 
   *                        forecast domain
   *  @param[in] maxTime  Output data will be assigned the date from this time.
   *  @param[in] outputDir  Output directory of data
   *  @param[in] isInt  Flag to indicate integer data contained in file, otherwise
   *                    floating point data is assumed
   */
  void writeAve(std::vector<std::vector<std::vector<DistStats*> > > &pAveStats,
		const time_t maxTime, const std::string &outputDir, const bool isInt = true);

protected:
private:

  /**
   * Method used in reading input file
   * @param[in] fptr   pointer to FILE
   * @param[out] line  non empty line from input file.
   *
   * @return true if a line was returned
   */
  bool pGetNextLine( FILE *fptr, char *line);

  /**
   * Get histogram data from file line.
   * @param[in] line  Character array assumed to contain histogram bin totals
   *                  data
   * @param[out] histData  Vector of integer bin totals 
   */
  void pGetHist(char *line, std::vector <int> &histData);

  /**
   * Get histogram data from file line.
   * @param[in] line  Character array assumed to contain histogram bin totals
   *                  data
   * @param[out] histData  Vector of integer bin totals 
   */
  void pGetHist(char *line, std::vector <float> &histData);

  
};

#endif
