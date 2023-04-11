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
 * @file InterfaceLL.cc
 * @brief 
 */

#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ConvWxProjection.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ConvWxPath.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/EdgeFinder.hh>
#include <ConvWx/InsideBuilder.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/ILogMsg.hh>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <sys/stat.h>


using std::string;
using std::vector;
using std::pair;

// Initialize static variable
string InterfaceLL::lastFSSClimoDate = string("");
//------------------------------------------------------------------
/**
 * heap sort of indexes
 */
static void sIdxSiftDown( int *index, double *array, int root, int bottom )
{
  int done, maxChild, temp;

  done = 0;
  while (((root<<1) <= bottom) && (!done))
  {
    if ( root<<1 == bottom ||
         array[index[root<<1]] > array[index[(root<<1) + 1]])
    {
      maxChild = root<<1;
    }
    else
    {
      maxChild = (root<<1) + 1;
    }

    if (array[index[root]] < array[index[maxChild]])
    {
      temp = index[root];
      index[root] = index[maxChild];
      index[maxChild] = temp;
      root = maxChild;
    }
    else
    {
      done = 1;
    }
  }
}

//------------------------------------------------------------------
void InterfaceLL::doRegister(const string &msg)
{
  InterfaceIO::doRegister(msg);
}

//------------------------------------------------------------------
void InterfaceLL::finish(void)
{
  ILOG_FINISH();
  InterfaceIO::finish();
}

//------------------------------------------------------------------
void InterfaceLL::sort(double *data, int n, int *index)
{
  int temp;
  int i;

  // initialize the index array
  for ( i=0; i<n; ++i )
  {
    index[i] = i;
  }
  
  for ( i=(n/2); i>=0; --i )
  {
    sIdxSiftDown(index, data, i, n-1);
  }

  for ( i=n-1; i>=1; --i )
  {
    temp = index[0];
    index[0] = index[i];
    index[i] = temp;
    sIdxSiftDown(index, data, 0, i-1);
  }
}
  
//------------------------------------------------------------------
bool InterfaceLL::loadFile(const string &fname, string &inp)
{
  struct stat statBuf;
  if (stat(fname.c_str(), &statBuf) != 0)
  {
    ILOGF(ERROR, "Cannot stat file %s",fname.c_str());
    return false;
  }
  int fileLen = static_cast<int>(statBuf.st_size);
  
  // open file
  FILE *in;
  if ((in = fopen(fname.c_str(), "r")) == NULL)
  {
    ILOGF(ERROR, "Cannot open file %s",fname.c_str());
    return false;
  }

  // read in buffer

  char *fileBuf = new char[fileLen + 1];
  if (static_cast<int>(fread(fileBuf, 1, fileLen, in)) != fileLen)
  {
    ILOGF(ERROR, "Cannot read file %s", fname.c_str());
    fclose(in);
    delete[] fileBuf;
    return false;
  }

  // ensure null termination
  fileBuf[fileLen] = '\0';

  // close file
  fclose(in);

  // put into the string
  inp = fileBuf;
  delete[] fileBuf;
  return true;
}

//------------------------------------------------------------------
bool InterfaceLL::makeDirRecurse(const string &path)
{
  ConvWxPath p(path);
  return p.makeDirRecurse();
}

//------------------------------------------------------------------
int InterfaceLL::stringParse(const char *inpstr, const int nchars,
			     const int maxFLen, vector<string> &outstr)
{
  register const char * instr;
  register char * tbuf;
  register int i, endFlag, inProgress;
  int	k = 0;
  char	tmpbuf[1000];
  char  outtmp[1000];

  instr = inpstr;
  tbuf = tmpbuf;
  endFlag = 0;
  inProgress = 0;
  i = 0;

  while(*instr == ' ' || *instr == '\t')
  {
    // skip leading white space
    instr++; 
    i++;
  }

  while(*instr != '\0' && *instr != '\n' && i < nchars )
  {
    if(*instr == ' ' || *instr == '\t' )
    {
      if (inProgress)
      {
	// signal end of field
	endFlag = 1;	
      }
      instr++;
    }
    else
    {
      inProgress = 1;	
      *tbuf++ = *instr++;	// move chars
    }
    i++;

    if(endFlag & inProgress)
    {	
      // tmpbuf is filled, terminate 
      *tbuf = '\0';	
      strncpy(outtmp, tmpbuf, maxFLen-1);
      string s = outtmp;
      outstr.push_back(s);
      k++;

      // reset temp buffer
      tbuf = tmpbuf;
      endFlag = 0;
      inProgress = 0;
    }
  }

  if(inProgress)
  {
    // still something in temp buffer
    *tbuf = '\0';
    strncpy(outtmp, tmpbuf, maxFLen-1);
    string s= outtmp;
    outstr.push_back(s);
    k++;
  }

  return(k);
}

//------------------------------------------------------------------
bool InterfaceLL::griddedDataPathFilter(string &s, const time_t &time)
{
  // The path will be something such as this:
  //    <path>/yyyymmdd/hhmmss.extension
  // or this:
  //    <path>/yyyymmdd/g_hhmmss/f_sssssss.extension
  // In these cases we want to remove  '<path>/'
  //
  int y = ConvWxTime::getYear(time);
  char buf[100];
  sprintf(buf, "%04d", y);
  string want = buf;
  
  size_t found = s.find(want);
  if (found == string::npos)
  {
    return false;
  }
  else
  {
    s = s.substr(found);
    return true;
  }
}

//------------------------------------------------------------------
bool InterfaceLL::dataPathParse(const string &input,
				time_t &gt, int &lt)
{
  // path is assumed of form yyyymmdd/g_hhmmss/f_<seconds>.*

  size_t foundGen = input.find("/g_");
  if (foundGen == string::npos)
  {
    ILOGF(ERROR, "string not a path %s", input.c_str());
    return false;
  }
  size_t foundSec = input.find("/f_");
  if (foundSec == string::npos)
  {
    ILOGF(ERROR, "string not a path %s", input.c_str());
    return false;
  }


  string ymd = input.substr(static_cast<int>(foundGen) -8, 8);
  string hms = input.substr(static_cast<int>(foundGen) + 3, 6);
  string seconds = input.substr(static_cast<int>(foundSec) + 3, 8);

  int y, m, d, h, min, sec;
  if (sscanf(ymd.c_str(), "%04d%02d%02d", &y, &m, &d) != 3)
  {
    ILOGF(ERROR, "scanning for yyyymmdd '%s'", ymd.c_str());
    return false;
  }
  if (sscanf(hms.c_str(), "%02d%02d%02d", &h, &min, &sec) != 3)
  {
    ILOGF(ERROR, "scanning for hhmmss '%s'", hms.c_str());
    return false;
  }
  if (sscanf(seconds.c_str(), "%d.mdv", &lt) != 1)
  {
    ILOGF(ERROR, "scanning for lead seconds '%s'", seconds.c_str());
    return false;
  }
  gt = ConvWxTime::setTime(y, m, d, h, min, sec);
  return true;
}

//------------------------------------------------------------------
string InterfaceLL::dataPathString(const time_t &gt, const int &lt)
{
  string ret;
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  int y, m, d, h, min, sec;

  ConvWxTime::expandTime(gt, y, m, d, h, min, sec);
  sprintf(buf, "%04d%02d%02d/g_%02d%02d%02d/f_%08d.mdv", 
	  y, m, d, h, min, sec, lt);
  ret = buf;
  return ret;
}

//------------------------------------------------------------------
void InterfaceLL::xyIndexToLatlon(const ParmProjection &p,
				  const int x, const int y,
				  double &lat, double &lon)
{
  ConvWxProjection proj;
  proj.init(p);
  proj.xyIndex2latlon(x,y,lat,lon);
}

//------------------------------------------------------------------
bool InterfaceLL::latlonToXyIndex(const ParmProjection &p,
				  const double lat, const double lon,
				  int &x, int &y)
{
  ConvWxProjection proj;
  proj.init(p);
  return proj.latlon2xyIndex(lat, lon, x, y);
}

//------------------------------------------------------------------
void InterfaceLL::latlonToXyIndexAny(const ParmProjection &p,
				     const double lat, const double lon,
				     int &x, int &y)
{
  ConvWxProjection proj;
  proj.init(p);
  return proj.latlon2xyIndexAny(lat, lon, x, y);
}

//------------------------------------------------------------------
bool InterfaceLL::latlonToXy(const ParmProjection &p,
			     const double lat, const double lon,
			     double &x, double &y)
{
  ConvWxProjection proj;
  proj.init(p);
  proj.latlon2xy(lat, lon, x, y);
  x = (x - p.pMinx)/p.pDx;
  y = (y - p.pMiny)/p.pDy;
  return (x >= 0.0 && x < (p.pMinx + static_cast<double>(p.pNx)*p.pDx) &&
	  y >= 0.0 && y < (p.pMiny + static_cast<double>(p.pNy)*p.pDy));
}

//------------------------------------------------------------------
void InterfaceLL::xyToLatlon(const ParmProjection &p,
			     const double x, const double y,
			     double &lat, double &lon)
{
  ConvWxProjection proj;
  proj.init(p);
  double x0, y0;
  x0 = x*p.pDx + p.pMinx;
  y0 = y*p.pDy + p.pMiny;
  proj.xy2latlon(x0, y0, lat, lon);
}

//------------------------------------------------------------------
void InterfaceLL::distanceKm( const ParmProjection &p, const int arrayIndex1, 
			      const int arrayIndex2, double &dist)
{
  ConvWxProjection proj;
  proj.init(p);

  int indexI1, indexJ1, indexI2, indexJ2;
  indexJ1 = arrayIndex1/p.pNx;
  indexI1 = arrayIndex1 - indexJ1 * p.pNx;
  indexJ2 = arrayIndex2/p.pNx;
  indexI2 = arrayIndex2 - indexJ2 * p.pNx;
  distanceKm(p, indexI1, indexJ1, indexI2, indexJ2, dist);
}

//------------------------------------------------------------------
void InterfaceLL::distanceKm( const ParmProjection &p, const int indexI1,
			      const int indexJ1, const int indexI2,
			      const int indexJ2, double &dist)
{
  ConvWxProjection proj;
  proj.init(p);

  double deltaX = fabs( indexI1 -  indexI2);
  double deltaY = fabs( indexJ1 -  indexJ2);
  double deltaXkm =  proj.xGrid2km(deltaX);
  double deltaYkm =  proj.yGrid2km(deltaY);
  dist = sqrt( deltaXkm * deltaXkm + deltaYkm*deltaYkm);
}

//------------------------------------------------------------------
void InterfaceLL::distanceKmD( const ParmProjection &p, const double indexI1,
			       const double indexJ1, const double indexI2,
			       const double indexJ2, double &dist)
{
  ConvWxProjection proj;
  proj.init(p);

  double deltaX = fabs( indexI1 -  indexI2);
  double deltaY = fabs( indexJ1 -  indexJ2);
  double deltaXkm =  proj.xGrid2km(deltaX);
  double deltaYkm =  proj.yGrid2km(deltaY);
  dist = sqrt( deltaXkm * deltaXkm + deltaYkm*deltaYkm);
}

//------------------------------------------------------------------
bool
InterfaceLL::polygonsToGrid(const ParmProjection &proj, 
			    const vector<vector<pair<double,double> > > &latlon,
			    const double value, GridData &grid,
			    GridData &edge)
{
  bool ret = false;
  vector<vector<pair<double,double> > >::const_iterator i;
  for (i=latlon.begin(); i!=latlon.end(); ++i)
  {
    if (polygonToGrid(proj, *i, value, grid, edge))
    {
      ret = true;
    }
  }
  return ret;
}

//------------------------------------------------------------------
bool InterfaceLL::polygonToGrid(const ParmProjection &proj, 
				const vector<pair<double,double> >&latlon,
				const double value, GridData &grid,
				GridData &edge)
{
  // add all points to an EdgeFinder object
  EdgeFinder edges(grid);
  ILOG(DEBUG_VERBOSE, "START Adding lat/lon");
  vector<pair<double,double> >::const_iterator i;
  for (i=latlon.begin(); i!=latlon.end(); ++i)
  {
    int x, y;
    ILOGF(DEBUG_VERBOSE, "Adding lat/lon %lf, %lf", i->first,
	  i->second);
    proj.latlonToGridIndexAny(i->first, i->second, x, y);
    edges.add(x, y);
  }
  if (edges.bad())
  {
    ILOG(ERROR, "constructing the edges");
    return false;
  }

  // finish adding points to the edges
  edges.putToGrid(edge);

  // now construct object that figures out what is inside
  InsideBuilder inside(edges);
  for (int y=0; y<inside.ny(); ++y)
  {
    for (int x=0; x<inside.nx(); ++x)
    {
      int xi, yi;
      // is point inside?
      if (inside.isInside(x, y))
      {
	// is point in range??
	if (edges.dataGridIndex(x, y, xi, yi))
	{
	  // yes.
	  grid.setv(xi, yi, value);
	}
      }
    }
  }
  return true;
}


//------------------------------------------------------------------
void InterfaceLL::setEarthRadiusKm(const ParmProjection &p)
{
  ConvWxProjection proj;
  proj.init(p);
}

//------------------------------------------------------------------
void InterfaceLL::log(const string &msg)
{
  printf("%s\n", msg.c_str());
}

//-----------------------------------------------------------------
bool InterfaceLL::writeFssClimoAscii(const string outputDir,
				     const int year,
				     const int month,
				     const int day,
				     const float genHourBegin,
				     const float genHourEnd,
				     const float leadHourBegin,
				     const float leadHourEnd,
				     const vector< string> threshScale,
				     const vector< double> fssVal,
				     bool last)
{
   
  //
  // Make directory command string
  //
  char mkdirCmd[convWx::ARRAY_LEN_SHORT];
  
  sprintf(mkdirCmd,"mkdir -p %s", outputDir.c_str());
  
  system(mkdirCmd);
  
  char filename[convWx::ARRAY_LEN_SHORT];
  char relFilename[convWx::ARRAY_LEN_SHORT];

  //
  // Output filepath format is outputDir/yyyymmdd.ascii
  //
  sprintf(filename, "%s/%04d%02d%02d.000000.ascii.tmp",
          outputDir.c_str(),  year, month, day);

  sprintf(relFilename, "%04d%02d%02d.000000.ascii.tmp",
          year, month, day);

  FILE *fptr;

  char date[9];

  sprintf(date,"%4d%2d%2d", year, month, day);

  string dateStr = string(date);

  //
  // Open new file if date has changed                 
  //
  if (dateStr != lastFSSClimoDate)
  {
    if ( (fptr = fopen(filename,"w"))== NULL)
    {
      ILOGF(ERROR,"Could not open file %s for writing", filename);

      return false;
    }
  }
  else
  {
    //
    // Open new file for appending if date has not changed
    //
    if ( (fptr = fopen(filename,"a"))== NULL)
    {
      ILOGF(ERROR,"Could not open file %s for appending", filename);
      
      return false;
    }
  }

  fprintf(fptr, "%f %f %f %f ", genHourBegin, genHourEnd, 
	  leadHourBegin, leadHourEnd);
  
  int numScalesThresh = static_cast<int>(threshScale.size());

  for (int i = 0; i <  numScalesThresh ; i++)
  {
    fprintf(fptr, "%s,fss=%f ", threshScale[i].c_str(),fssVal[i]);
  }
 
  fprintf(fptr, "\n");
  
  if (fptr)
  {
    fclose(fptr);
  }
  if (last)
  {
    char finalFilename[convWx::ARRAY_LEN_SHORT];
    char finalRelFilename[convWx::ARRAY_LEN_SHORT];
    sprintf(finalFilename, "%s/%04d%02d%02d.000000.ascii",
	    outputDir.c_str(),  year, month, day);
    sprintf(finalRelFilename, "%04d%02d%02d.000000.ascii",
	    year, month, day);
    if (rename(filename, finalFilename))
    {
      ILOGF(ERROR,"Could not rename %s to %s",
	    filename, finalFilename);
      return false;
    }      
    else
    {
      time_t time = ConvWxTime::setTime(year, month, day, 0, 0, 0);
      InterfaceIO::writeLatestDataInfo(outputDir, finalRelFilename, time);
    }
  }
 
  lastFSSClimoDate = dateStr;
 
  return true;
}

//------------------------------------------------------------------
bool InterfaceLL::writeFcstObsHists(const string outputDir, 
				    const int year,
				    const int month,
				    const int day,
				    const int genHour,
				    const int leadSecs,
				    const int gridNum,
				    const vector <int> &obsHist, 
				    const vector <int> &fcstHist,
				    const int missingVal,
				    const bool isAve,
				    const int numInObsAve,
				    const int numInFcstAve)
			       
{
  //
  // String for output directory
  //
  char dirStr[convWx::ARRAY_LEN_SHORT];

  //
  // Output directory format is outputDir/yyyymmdd/g_hhmmss 
  //
  sprintf(dirStr, "%s/%02d%02d%02d/g_%02d0000", 
	  outputDir.c_str(),
	  year, month, day, genHour);
  
  //
  // Make directory command string
  //
  char mkdirCmd[convWx::ARRAY_LEN_SHORT];
  
  sprintf(mkdirCmd,"mkdir -p %s", dirStr);
  
  system(mkdirCmd);
  
  char filename[convWx::ARRAY_LEN_SHORT];
  
  //
  // Output filepath format is outputDir/yyyymmdd/g_hhmmss/f_ssssssss.ascii
  //
  sprintf(filename,"%s/f_%08d.ascii", dirStr, leadSecs);
  
  FILE *fptr;
 
  //
  // Open new file if subgrid number is zero
  //
  if (gridNum == 0)
  {
    if ( (fptr = fopen(filename,"w"))== NULL)
    { 
      return false;
    }
  }
  else
  {
    //
    // Open file for appending if subgrid numbers are greater than zero
    //
    if ( (fptr = fopen(filename,"a"))== NULL)
    { 
      return false;
    }
  } 
  
  fprintf(fptr, "%d\n", gridNum);
  
  //
  // Print observation histogram
  //    
  for( int j = 0; j < static_cast<int>(obsHist.size()); j++)
  {
    fprintf(fptr,"%5d ", obsHist[j]);
  }
  
  fprintf(fptr,"\n");
  
  //
  // Print Fcst histogram
  //  
  if( fcstHist.empty())
  {
    for( int j = 0; j < static_cast<int>(obsHist.size()); j++)
    {
      fprintf(fptr, "%5d ", missingVal);
    }
  }
  else
  {
    for( int j = 0; j < static_cast<int>(fcstHist.size()); j++)
    {
      fprintf(fptr, "%5d ", fcstHist[j]);
    }
  }

  fprintf(fptr,"\n");

  if(isAve)
  {
    fprintf(fptr,"%d %d\n", numInObsAve, numInFcstAve);
  }
  
  fprintf(fptr,"\n");
  
  if (fptr)
  {
    fclose(fptr);
  }
  
  return true;
}


//------------------------------------------------------------------
bool InterfaceLL::writeFcstObsQuantileStats(const string outputDir,
                                    const int year,
                                    const int month,
                                    const int day,
                                    const int genHour,
                                    const int leadSecs,
                                    const int gridNum,
                                    const vector <float> &obsHist,
                                    const vector <float> &fcstHist,
                                    const float missingVal,
                                    const bool isAve,
                                    const int numInObsAve,
                                    const int numInFcstAve)

{
  //
  // String for output directory
  //
  char dirStr[convWx::ARRAY_LEN_SHORT];

  //
  // Output directory format is outputDir/yyyymmdd/g_hhmmss
  //
  sprintf(dirStr, "%s/%02d%02d%02d/g_%02d0000",
          outputDir.c_str(),
          year, month, day, genHour);
 
  //
  // Make directory command string
  //
  char mkdirCmd[convWx::ARRAY_LEN_SHORT];
 
  sprintf(mkdirCmd,"mkdir -p %s", dirStr);
 
  system(mkdirCmd);
 
  char filename[convWx::ARRAY_LEN_SHORT];
 
  //
  // Output filepath format is outputDir/yyyymmdd/g_hhmmss/f_ssssssss.ascii
  //
  sprintf(filename,"%s/f_%08d.ascii", dirStr, leadSecs);
 
  FILE *fptr;

  //
  // Open new file if subgrid number is zero
  //
  if (gridNum == 0)
  {
    if ( (fptr = fopen(filename,"w"))== NULL)
    {
      return false;
    }
  }
  else
  {
    //
    // Open file for appending if subgrid numbers are greater than zero
    //
    if ( (fptr = fopen(filename,"a"))== NULL)
    {
      return false;
    }
  }

  fprintf(fptr, "%d\n", gridNum);
 
  //
  // Print observation histogram
  //
  if ( obsHist.empty())
  {
      for( int j = 0; j < static_cast<int>(obsHist.size()); j++)
      {
         fprintf(fptr,"%f", missingVal);
     }
  }
  else
  {
    for( int j = 0; j < static_cast<int>(obsHist.size()); j++)
    {  
       if(std::isnan( obsHist[j]))
       {
          fprintf(fptr,"%f ", missingVal);
       }
       else
       {
         fprintf(fptr,"%f ", obsHist[j]);
       }
     }
  }
  fprintf(fptr,"\n");
 
  //
  // Print Fcst histogram
  //
  if( fcstHist.empty())
  {
    for( int j = 0; j < static_cast<int>(obsHist.size()); j++)
    {
      fprintf(fptr, "%f ", missingVal);
    }
  }
  else
  {
    for( int j = 0; j < static_cast<int>(fcstHist.size()); j++)
    {
      if (std::isnan( fcstHist[j]))
      {
        fprintf(fptr,"%f ", missingVal);
      }
      else
      {
       fprintf(fptr, "%f ", fcstHist[j]);
      }
    }
  }

  fprintf(fptr,"\n");

  if(isAve)
  {
    fprintf(fptr,"%d %d\n", numInObsAve, numInFcstAve);
  }
 
  fprintf(fptr,"\n");
 
  if (fptr)
  {
    fclose(fptr);
  }
 
  return true;
}

