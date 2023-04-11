/**
 * @file DistFile.cc
 * @brief Source for DistFile class
 */



#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/DistFile.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::string;
using std::vector;

DistFile::DistFile()
{
}

DistFile::~DistFile()
{
 
}

bool DistFile::parse(const string inputPath,  
		     std::vector<DistStats*>  &distStats, const bool isAve, const bool isInt)
{
  ILOGF(DEBUG, "Parsing file: %s",inputPath.c_str());
 
  InterfaceLL::doRegister("Parsing file");
  
  //
  // Generation time
  //
  time_t genTime;

  //
  // Lead time
  //
  int leadTime;

  //
  // Get generation and lead times from the input path
  //
  InterfaceLL::dataPathParse(inputPath, genTime, leadTime);

  FILE *fptr = NULL;

  if ( (fptr = fopen(inputPath.c_str(),"r")) == NULL)
  {
    ILOGF(WARNING, "Error opening %s", inputPath.c_str());
    
    return false;
  }
  else
  {
    char line[convWx::ARRAY_LEN_LONG];
      
    pGetNextLine( fptr, line);
      
    while ( !feof(fptr) )
    {
      //
      // Get the sub-grid number
      //
      int subgridNum;

      if ( sscanf(line, "%d", &subgridNum) !=1)
      {
         fclose(fptr);
         return false;
      }

      //
      // Get observation histogram 
      //
      pGetNextLine( fptr, line);
        
      if (isInt)
      {
	vector <int> obsHistData;

	pGetHist(line,obsHistData);
	  
	//
	// Get forecast histogram
	//
	pGetNextLine( fptr, line);
        
	vector <int> fcstHistData;
	
	pGetHist(line,fcstHistData);

	//
	// Create distribution stats object to store histogram information
	//
	DistStats *stats = new DistStats(genTime,leadTime, 
					 subgridNum, obsHistData, 
					 fcstHistData);
	
	distStats.push_back(stats);	
      }
      else
      {
	vector <float> obsHistData;

	pGetHist(line,obsHistData);
	  
	//
	// Get forecast histogram
	//
	pGetNextLine( fptr, line);
        
	vector <float> fcstHistData;
	
	pGetHist(line,fcstHistData);

	//
	// Create distribution stats object to store histogram information
	//
	DistStats *stats = new DistStats(genTime,leadTime, 
					 subgridNum, obsHistData, 
					 fcstHistData);
	
	distStats.push_back(stats);	
      }

      //
      // If this is a file with distribution averages, skip a line 
      // containing the number of elements in the averages
      //
      if(isAve)
      {
	//
	// Skip the line with number of data points in averages
	//
	pGetNextLine( fptr, line);
      }
	  
      //
      // Advance to next line of new record
      // 
      pGetNextLine( fptr, line); 
    }
    //
    // Close input file
    //
    fclose(fptr);
  }
  return true;
}

bool DistFile::pGetNextLine( FILE *fptr, char *line)
{
  fgets(line, convWx::ARRAY_LEN_LONG , fptr);

  while( line[0] == 10 and !feof(fptr))
  {
    fgets(line, convWx::ARRAY_LEN_LONG , fptr);
  }

  if ( feof(fptr) ||  line[0]== 10)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void DistFile::pGetHist( char *line,  vector <int> &histData)
{
  //
  // replace LF with NULL char
  //
  int len = strlen(line);

  if (line[len-1] == '\n') 
  {
    line[len-1] = '\0';
  }
  
  //
  // Get histogram data
  //
  char *strPtr = strtok (line," ");
  
  while (strPtr!= NULL)
  {
    int val = atoi(strPtr);
   
    strPtr = strtok (NULL, " ");
      
    histData.push_back(val); 
  }
}

void DistFile::pGetHist( char *line,  vector <float> &histData)
{
  //
  // replace LF with NULL char
  //
  int len = strlen(line);

  if (line[len-1] == '\n') 
  {
    line[len-1] = '\0';
  }
  
  //
  // Get histogram data
  //
  char *strPtr = strtok (line," ");
  
  while (strPtr!= NULL)
  {
    float val = atof(strPtr);
   
    strPtr = strtok (NULL, " ");
      
    histData.push_back(val); 
  }
}

void DistFile::writeAve(vector< vector< vector<DistStats*> > > &aveStats, 
			const time_t outTime, const string &outputDir, const bool isInt)
{  

  //
  // Get the date corresponding to outTime. Output data will be assigned this
  // date
  //
  int year, month, day, hour, minute, second;
  
  ConvWxTime::expandTime(outTime, year, month, day, hour, minute, second); 

  //
  // For each generation hour, for each lead time write 
  // sub-grid stats to file
  //
  //  For each generation hour
  //
  for (int i = 0; i < static_cast<int>(aveStats.size()); i++)
  {
    if (static_cast<int>(aveStats[i].size() > 0 ))
    {

        ILOGF(DEBUG, "Writing data to %s for %4d-%02d-%02d generation hour %d",
              outputDir.c_str(), year, month, day, i);
        //
        // For each lead time
        //
        for (int j = 0; j < static_cast<int>(aveStats[i].size()); j++)
        {
            //
            // If we have sub-grid data, then write it
            //
            if (static_cast<int>(aveStats[i][j].size()) > 0)
            {
                InterfaceLL::doRegister("Writing data");

                //
                // Get generation hour
                //
                int genHour = aveStats[i][j][0]->getGenHour();

                //
                // Get lead time in seconds
                //
                int leadSecs = aveStats[i][j][0]->getLeadSecs();

                //
                // For each sub-grid, write the the data
                //
                for (int k = 0; k < static_cast<int>(aveStats[i][j].size()); k++)
                {

                    //
                    // Get sub-grid number
                    //
                    int gridNum = aveStats[i][j][k]->getSubgridNum();

                    if (isInt) {
                        //
                        // Get observation histogram
                        //
                        const vector<int> obsHist = aveStats[i][j][k]->getObsHist();

                        //
                        // Get forecast histogram
                        //
                        const vector<int> fcstHist = aveStats[i][j][k]->getFcstHist();
                        //
                        // Get the number of elements in the observation histogram
                        // bin averages
                        //
                        int nObsAve = aveStats[i][j][k]->getNObsHists();

                        //
                        // Get the number of elements in the observation histogram
                        // bin averages
                        //
                        int nFcstAve = aveStats[i][j][k]->getNFcstHists();

                        ILOGF(DEBUG_VERBOSE, "Writing data for year month day %04d-%02d-%02d, "
                                             " generation hour %d, lead time %d, subgrid %d "
                                             "to %s", year, month, day, genHour, leadSecs,
                              gridNum, outputDir.c_str());

                        //
                        // Write the data
                        //
                        InterfaceLL::writeFcstObsHists(outputDir, year, month, day,
                                                       genHour, leadSecs, gridNum,
                                                       obsHist, fcstHist,
                                                       static_cast<int>(convWx::BAD_HIST_BIN_TOTAL),
                                                       true, nObsAve, nFcstAve);
                    } else // we have float data for histogram stats
                    {
                        //
                        // Get observation histogram
                        //
                        const vector<float> obsHist = aveStats[i][j][k]->getObsHistStats();

                        //
                        // Get forecast histogram
                        //
                        const vector<float> fcstHist = aveStats[i][j][k]->getFcstHistStats();

                        //
                        // Get the number of elements in the observation histogram
                        // bin averages
                        //
                        int nObsAve = aveStats[i][j][k]->getNObsHists();

                        //
                        // Get the number of elements in the observation histogram
                        // bin averages
                        //
                        int nFcstAve = aveStats[i][j][k]->getNFcstHists();

                        ILOGF(DEBUG_VERBOSE,
                              "Writing data for year month day %04d-%02d-%02d, "
                              " generation hour %d, lead time %d, subgrid %d "
                              "to %s", year, month, day, genHour, leadSecs,
                              gridNum, outputDir.c_str());

                        //
                        // Write the data
                        //
                        InterfaceLL::writeFcstObsQuantileStats(outputDir, year, month, day,
                                                               genHour, leadSecs, gridNum,
                                                               obsHist, fcstHist,
                                                               static_cast<int>(convWx::BAD_HIST_BIN_TOTAL),
                                                               true, nObsAve, nFcstAve);
                    }

                }// End for each sub-grid
            }// If we have sub-grid data
        }// End for each lead time
    }// end if we have data at this gen
    else
    {
        ILOGF(DEBUG, "No data written to %s for %4d-%02d-%02d generation hour %d",
                      outputDir.c_str(), year, month, day, i);
    }
  }// End for each generation hour
}
