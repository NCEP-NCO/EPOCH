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
 * @file ClimoHistRdr.cc
 */

#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ClimoHistRdr.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/DistFile.hh>
#include <ConvWx/DistStats.hh>
#include <sys/stat.h>
#include <cstdio>
#include <cmath>

using std::vector;
using std::string;

ClimoHistRdr::ClimoHistRdr() :
  pRatiosObsModel(),
  pClimoHistDir(),
  pGenHourRes(0),
  pLeadHourRes(0),
  pNumGenHours(0),
  pNumLeads(0),
  pObsBias(),
  pRatioMin(0),
  pRatioMax(0),
  pMaxAge(0),
  pRealtime(0),
  pRequiredBinNum(0)
{
}

ClimoHistRdr::~ClimoHistRdr()
{
}

void ClimoHistRdr::init(const int numSubgrids, const float genHourRes,
			const float leadHourRes, const float maxLeadHour, 
			const string inputDir, const vector <float> &obsBias, 
			const float ratioMin, const float ratioMax, 
			const int maxAge, const bool realtime, const bool isQuantileData)
{
  //
  // Clear containers of any previous climatological data
  //
  pClear();

  //
  // Initialize class members
  //
  pRatiosObsModel.resize(numSubgrids);

  pObsModelAvgDiff.resize(numSubgrids);

  pObsAvg.resize(numSubgrids);  

  pSlopes.resize(numSubgrids);  

  pIntercepts.resize(numSubgrids); 
  
  pGenHourRes = genHourRes;
  
  pLeadHourRes = leadHourRes;
  
  pNumGenHours =
    static_cast<int>(static_cast<float>(convWx::HOURS_PER_DAY)/genHourRes);
  
  pNumLeads = static_cast<int>(maxLeadHour/leadHourRes) + 1;
  
  //
  // Resize containers to hold data based on number of sub-grids, generation 
  // hours and lead times.
  //
  for (int i = 0; i < numSubgrids ; i ++)
  {    
    pRatiosObsModel[i].resize(pNumGenHours);

    pObsModelAvgDiff[i].resize(pNumGenHours);

    pObsAvg[i].resize(pNumGenHours);

    pSlopes[i].resize(pNumGenHours);  

    pIntercepts[i].resize(pNumGenHours); 
    
    for (int j = 0; j < pNumGenHours; j++)
    { 
      pRatiosObsModel[i][j].resize(pNumLeads);

      pObsModelAvgDiff[i][j].resize(pNumLeads);

      pObsAvg[i][j].resize(pNumLeads);

      pSlopes[i][j].resize(pNumLeads);

      pIntercepts[i][j].resize(pNumLeads);      
    }
  }
  
  //
  // Initialize more members
  //
  pClimoHistDir = inputDir;
  
  pObsBias =  obsBias;

  pRatioMin = ratioMin;
  
  pRatioMax = ratioMax;
  
  pMaxAge = maxAge;
  
  pRealtime = realtime;

  pRequiredBinNum = static_cast<int>(obsBias.size()); 

  pIsQuantileData = isQuantileData;

  //
  // Load climatological data, calculate ratios of observations to model data
  // for each histogram bin and store that data.
  // 
  pLoad();
}

void ClimoHistRdr::pClear()
{
  //
  // Clear container for ratios of histogram bin totals
  //
  pRatiosObsModel.clear();
  
  pObsModelAvgDiff.clear();
  
  pObsAvg.clear();
  
  pSlopes.clear();
  
  pIntercepts.clear();
}

void ClimoHistRdr::pLoad()
{ 
  //
  // For each generation and lead time, load climatological data 
  // histograms from appropriate file.
  //
  for (int i = 0; i < pNumGenHours; i++)
  {
    for (int j = 0; j < pNumLeads; j++)
    {
      //
      // Path of file containing climatological data histograms for 
      // forecast and observations
      //
      string filepath;

      //
      // Flag to indicate that a file exists for generation hour and lead time
      // specified by iterators
      //
      bool gotFile;
     
      if (pRealtime)
      {
	gotFile = pGetFileRealtime(i, j, filepath);
      }
      else
      {
	gotFile = pGetFileArchive(i, j, filepath);
      }

      //
      // If we have a file, extract all available forecast and observation 
      // histogram information.  
      //
      if (gotFile)
      {
	//
	// Container to hold forecast and observation histogram information.
	//
	std::vector<DistStats*>  distStats;
	
	//
	// Object for reading and writing files containing distribution 
	// information 
	//
	DistFile distFile;

	//
	// Extract the data from the file, argument 'true' indicates that this
	// is a file containing averaged data( the format differs slightly)
	//

	if(!pIsQuantileData)
	{
	  distFile.parse(filepath, distStats, true);  
	}
	else
	{
	  distFile.parse(filepath, distStats, true, false);
	}
	
	for(int k = 0; k < static_cast<int>(distStats.size()); k++)
	{
	  int subgridNum = distStats[k]->getSubgridNum();
	  
	  if(!pIsQuantileData)
	  {
	    vector < int > obsHist = distStats[k]->getObsHist();
	    
	    vector < int > fcstHist = distStats[k]->getFcstHist();
	    	    
	    if( static_cast<int>(obsHist.size()) != pRequiredBinNum ||
		static_cast<int>(fcstHist.size())  != pRequiredBinNum)
	    {
	      ILOGF(ERROR, 
		    "Histograms from %s for subgrid %d do not have the "
		    "required number of histogram bins. Required is %d bins."
		    "Observation histogram has %d and forecast histogram has "
		    "%d bins", filepath.c_str(), subgridNum, pRequiredBinNum,
		    static_cast<int>(obsHist.size()),
		    static_cast<int>(fcstHist.size()));
	      
	      continue;
	    }
	  
	    //
	    // Compute and record the ratio of observation/model for each histogram  bin
	    //  
	    vector <float> ratio;
	    
	    pComputeRatios(fcstHist, obsHist, ratio);
	    
	    pRatiosObsModel[subgridNum][i][j] = ratio;
	  }
	  else
	  {
	    vector < float > obsHistStats = distStats[k]->getObsHistStats();
	    
	    vector < float > fcstHistStats = distStats[k]->getFcstHistStats();
	    
	    
	    if( static_cast<int>(obsHistStats.size()) != pRequiredBinNum ||
		static_cast<int>(fcstHistStats.size())  != pRequiredBinNum)
	    {
	      ILOGF(ERROR, 
		    "Histograms from %s for subgrid %d do not have the "
		    "required number of histogram bins. Required is %d bins."
		    "Observation histogram has %d and forecast histogram has "
		    "%d bins", filepath.c_str(), subgridNum, pRequiredBinNum,
		    static_cast<int>(obsHistStats.size()),
		    static_cast<int>(fcstHistStats.size()));
	      
	      continue;
	    }
	  
	    //
	    // Compute and record the ratio of observation/model for each histogram  bin
	    //  
	    vector <float> obsModelAvgDiff;
	    
	    for(int l = 0; l < static_cast<int>(obsHistStats.size()) ; l++)
	    {
	      float diff;
              if (obsHistStats[l] != convWx::BAD_HIST_BIN_STAT && 
		  fcstHistStats[l] !=  convWx::BAD_HIST_BIN_STAT)
	      {
		diff = obsHistStats[l] - fcstHistStats[l];
	      }
	      else
	      {
		diff =  convWx::BAD_HIST_BIN_STAT;
	      }
	      obsModelAvgDiff.push_back(diff);
	    }

	    pObsModelAvgDiff[subgridNum][i][j] = obsModelAvgDiff;
	    
	    pObsAvg[subgridNum][i][j] =  obsHistStats;

	    pComputeLinearFunction(fcstHistStats, obsHistStats, subgridNum, i,j);
	  }	  
	}//End for k 
        //clean up DistStats vector
        for (int iter = 0; iter < static_cast<int>(distStats.size()); iter++)
	{
            delete (distStats[iter]);
	}
      }// End if gotFile
    }// End for j 
  }//End for i 
}


void ClimoHistRdr::pComputeRatios(const vector <int> &fcstHist, 
				  const vector <int> &obsHist, 
				  vector <float> &ratio)
{
  
  //
  // Number of histogram bins
  //
  int numBins = static_cast<int>(fcstHist.size());
  
  //
  // Create a vector to hold observation histogram totals with bias applied
  //
  vector <int> obsB;
  
  obsB.resize(numBins);
  
  //
  // Apply user defined bias to the observation histogram 
  //
  if ( static_cast<int>(obsHist.size()) != pRequiredBinNum)
  {
    ILOG(DEBUG_VERBOSE, 
	 "check size of climo shift bias array in param file, bias not applied!");
  }
  else
  {
    for (int i = 0; i < pRequiredBinNum; i++)
    {
      obsB[i] = static_cast<int>(static_cast<float>(obsHist[i]) * pObsBias[i]);
    }
  }
  
  //
  // Calculate and store ratios of observation bin totals to model bin totals
  //       
  ratio.resize(numBins);
  
  //
  // Find ratio of observation to model for each bin and record
  //
  for ( int i = 0; i < numBins; i++)
  { 
    //
    // Ratio of observation bin total to forecast bin total
    //
    float rat;
    
    if (fcstHist[i] > 0)
    {
      rat = static_cast<float>(obsB[i])/fcstHist[i];
    }
    else
    {
      rat = pRatioMax;
    }
    
    //
    // Make sure ratio is in the interval [ratioMin, ratioMax]. Reassign the 
    // to the maximum or minimum value if out of bounds.
    //
    if (rat < pRatioMin)
    {
      ratio[i] = pRatioMin;
    }
    else if (rat > pRatioMax)
    {
      ratio[i] = pRatioMax;
    }
    else
    {
      ratio[i] = rat;
    }
  }
}

const vector <float> &ClimoHistRdr::getObsModelRatios(int subgridNum, 
						      float genHour, 
						      float leadHour) const
{
  //
  // Integer representing the generation hour 
  //
  int genNum = static_cast<int>(genHour/pGenHourRes);
  
  //
  // Integer representing the lead time 
  //
  int leadNum = static_cast<int>(leadHour/pLeadHourRes);
  
  ILOGF(DEBUG_VERBOSE, 
	"Using subg %d genHour %f leadHour %f genNum %d leadNum %d "
	"fracShifts size %d", subgridNum, genNum * pGenHourRes , 
	leadNum * pLeadHourRes, genNum, leadNum, 
	static_cast<int>(pRatiosObsModel[subgridNum][genNum][leadNum].size()));
  
  //
  // Return the vector containing ratios of histogram observation bin totals
  // to model bin totals
  // 
  return pRatiosObsModel[subgridNum][genNum][leadNum];
}

const std::vector <float> &ClimoHistRdr::getObsAvg(int subgridNum, 
						   float genHour, 
						   float leadHour) const
{
  //
  // Integer representing the generation hour 
  //
  int genNum = static_cast<int>(genHour/pGenHourRes);
  
  //
  // Integer representing the lead time 
  //
  int leadNum = static_cast<int>(leadHour/pLeadHourRes);
  
  ILOGF(DEBUG_VERBOSE, 
	"Using subg %d genHour %f leadHour %f genNum %d leadNum %d "
	"fracShifts size %d", subgridNum, genNum * pGenHourRes , 
	leadNum * pLeadHourRes, genNum, leadNum, 
	static_cast<int>(pRatiosObsModel[subgridNum][genNum][leadNum].size()));
  
  //
  // Return the vector containing ratios of histogram observation bin totals
  // to model bin totals
  // 
  return pObsAvg[subgridNum][genNum][leadNum];
}

const std::vector <float> &  ClimoHistRdr::getObsModelAvgDiff(int subgridNum, 
							      float genHour, 
							      float leadHour) const
{
  //
  // Integer representing the generation hour 
  //
  int genNum = static_cast<int>(genHour/pGenHourRes);
  
  //
  // Integer representing the lead time 
  //
  int leadNum = static_cast<int>(leadHour/pLeadHourRes);
  
  ILOGF(DEBUG_VERBOSE, 
	"Using subg %d genHour %f leadHour %f genNum %d leadNum %d "
	"fracShifts size %d", subgridNum, genNum * pGenHourRes , 
	leadNum * pLeadHourRes, genNum, leadNum, 
	static_cast<int>(pRatiosObsModel[subgridNum][genNum][leadNum].size()));
  
  //
  // Return the vector containing ratios of histogram observation bin totals
  // to model bin totals
  // 
  return pObsModelAvgDiff[subgridNum][genNum][leadNum];
}

const std::vector <float> &  ClimoHistRdr::getBin2BinSlopes(int subgridNum, 
							    float genHour, 
							    float leadHour) const
{
  //
  // Integer representing the generation hour 
  //
  int genNum = static_cast<int>(genHour/pGenHourRes);
  
  //
  // Integer representing the lead time 
  //
  int leadNum = static_cast<int>(leadHour/pLeadHourRes);
  
  ILOGF(DEBUG_VERBOSE, 
	"Using subg %d genHour %f leadHour %f genNum %d leadNum %d "
	"slopes size %d", subgridNum, genNum * pGenHourRes , 
	leadNum * pLeadHourRes, genNum, leadNum, 
	static_cast<int>(pSlopes[subgridNum][genNum][leadNum].size()));
  
  //
  // Return the vector containing ratios of histogram observation bin totals
  // to model bin totals
  // 
  return pSlopes[subgridNum][genNum][leadNum];
}

const std::vector <float> &  ClimoHistRdr::getBin2BinIntercepts(int subgridNum, 
								float genHour, 
								float leadHour) const
{
  //
  // Integer representing the generation hour 
  //
  int genNum = static_cast<int>(genHour/pGenHourRes);
  
  //
  // Integer representing the lead time 
  //
  int leadNum = static_cast<int>(leadHour/pLeadHourRes);
  
  ILOGF(DEBUG_VERBOSE, 
	"Using subg %d genHour %f leadHour %f genNum %d leadNum %d "
	"intercepts size %d", subgridNum, genNum * pGenHourRes , 
	leadNum * pLeadHourRes, genNum, leadNum, 
	static_cast<int>(pSlopes[subgridNum][genNum][leadNum].size()));
  
  //
  // Return the vector containing ratios of histogram observation bin totals
  // to model bin totals
  // 
  return pIntercepts[subgridNum][genNum][leadNum];
}


bool ClimoHistRdr::pGetFileRealtime(const int numGen, const int numLead, 
				    string &filepath)
{
  //
  // Realtime files have the format:
  // pClimoHistDir/yyyymmdd/g_hhmmss/f_ssssssss.ascii
  // where generation time follows 'g_' and forecast
  // lead in seconds follows 'f_'
  //

  //
  // Calculate the hour and minutes after the hour of the generation time from 
  // numGen and generation hour resolution
  //
  int genHour = static_cast<int>( numGen*pGenHourRes);
  
  float fracHour = numGen*pGenHourRes - genHour;
  
  int genMins = 
    static_cast<int>(fracHour * static_cast<float>(convWx::MINS_PER_HOUR)); 
  
  //
  // Initialize variables. 
  //
  int lookback = 0;
  
  //
  // Time from which date will be parsed and used to search for climatological 
  // data 
  //
  time_t climoDay = time(0);

  //
  // Look for climatology files for numGen and numLead but dates must be 
  // within pMaxAge seconds of the  current time.
  //
  while ( lookback < pMaxAge)
  {
    //
    // Get the date for the file
    //
    tm *tmPtr = gmtime( &climoDay);
      
    int year = tmPtr->tm_year + 1900;
    
    int month = tmPtr->tm_mon + 1;
    
    int day = tmPtr->tm_mday; 
    
    //
    // Create file path
    //
    char tail[convWx::ARRAY_LEN_SHORT];
    
    //
    // Realtime files have the format:
    // pClimoHistDir/yyyymmdd/g_hhmmss/f_ssssssss.ascii
    //
    sprintf(tail, "/%04d%02d%02d/g_%02d%02d00/f_%08d.ascii",
	    year,month,day, genHour, genMins, 
	    static_cast<int>((numLead * pLeadHourRes * convWx::SECS_PER_HOUR)));
  
    string tailStr(tail);
 
    filepath = pClimoHistDir + tailStr;

    //
    // See if file exists
    //
    if (pfileExists(filepath))
    {
      //
      // We have file
      //
      return true;
    }
    else
    {
      //
      // We do not have file. Look back in time one more day.
      //
      lookback = lookback + 1;
      
      climoDay = climoDay - convWx::SECS_PER_DAY;
    }
  }

  //
  // We did not find a file within look back time interval
  //
  return false;
}

bool ClimoHistRdr::pGetFileArchive(const int numGen, const int numLead, 
				   string &filepath)
{
  //
  // Files in archive mode have the format:
  // 'pClimoHistDir/g_hhmmss/f_ssssssss.ascii' where generation time 
  // follows 'g_' and forecast lead in seconds follows 'f_'
  //

  //
  // Calculate the hour and minutes after the hour of the generation time from 
  // numGen and generation hour resolution
  //
  int genHour = static_cast<int>( numGen * pGenHourRes);
	
  float fracHour = numGen*pGenHourRes - genHour;
	
  int genMins =
    static_cast<int>(fracHour * static_cast<float>(convWx::MINS_PER_HOUR)); 
	
  char tail[convWx::ARRAY_LEN_SHORT];
	
  sprintf(tail, "/g_%02d%02d00/f_%08d.ascii", genHour, genMins, 
	  static_cast<int>((numLead * pLeadHourRes * convWx::SECS_PER_HOUR)));

  string tailStr(tail);
	
  filepath = pClimoHistDir + tailStr;

  //
  // See if file exists
  //
  if (pfileExists(filepath))
  {
    //
    // We have file
    //
    return true;
  }
  else
  {
    //
    // No file exists
    //
    return false;
  }
}

bool ClimoHistRdr::pfileExists( const string filepath)
{
  //
  // Data structure containing detailed file information 
  // 
  struct stat info;
 
  //
  //stat the file
  //
  int ret = stat(filepath.c_str(), &info);

  if(ret == 0) 
  {
    //
    // File exists, stat was successful
    //
    return true;
  } 
  else 
  {
      
    //
    // File does not exist, stat was unsuccessful
    //
    return false;
  }
}

void ClimoHistRdr:: pComputeLinearFunction(const vector <float> &fcstHist, 
					   const vector <float> &obsHist,
					   const int subgridNum,
					   const int genNum,
					   const int leadNum) 
{
  vector <float> slopes;

  slopes.resize(static_cast<int>(fcstHist.size()));

  vector <float> intercepts;

  intercepts.resize(static_cast<int>(fcstHist.size()));
  
  //
  // slope of line segments of linear relationship of obs value
  // as a function of model value are recorded
  //
  for (size_t i = 1; i < fcstHist.size(); i++)
  {
    float denominator =  fcstHist[i] -  fcstHist[i-1];
    if ( obsHist[i]  != convWx::BAD_HIST_BIN_STAT &&
	 obsHist[i-1] != convWx::BAD_HIST_BIN_STAT &&
	 fcstHist[i] != convWx::BAD_HIST_BIN_STAT &&
	 fcstHist[i-1] != convWx::BAD_HIST_BIN_STAT && 
	 fabs(denominator) >  convWx::EPSILON )
    {
      slopes[i] = (obsHist[i] - obsHist[i-1])/denominator;
       
      intercepts[i] = slopes[i] * fcstHist[i] + obsHist[i];   
    }
    else
    {
      slopes[i] = convWx::BAD_HIST_BIN_STAT;
      
      intercepts[i] = convWx::BAD_HIST_BIN_STAT;
    }

    //
    // set slope of first bin == to the second bin
    //
    slopes[0] =   slopes[1];

    intercepts[0] = intercepts[1];

    pSlopes[subgridNum][genNum][leadNum] = slopes;

    pIntercepts[subgridNum][genNum][leadNum] = intercepts;
  }
}
