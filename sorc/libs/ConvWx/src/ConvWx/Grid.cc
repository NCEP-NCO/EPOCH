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
 * @file Grid.cc
 * @brief 2 dimensional data grid  with i/o spec.
 */

//----------------------------------------------------------------
#include <cstring>
#include <cmath>
#include <ConvWx/Grid.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
Grid::Grid(void) :
  GridData(),
  pName(),
  pUnits(),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}

//----------------------------------------------------------------
Grid::Grid(const string &name, const string &units, const double *data,
	   const int nx, const int ny, const double bad) :
  GridData(data, nx, ny, bad),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}


//----------------------------------------------------------------
Grid::Grid(const string &name, const string &units,const double *data, 
	   const int nx, const int ny, const int nz, const double bad) : 
  GridData(data, nx, ny, nz, bad),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}

//----------------------------------------------------------------
Grid::Grid(const std::string &name, const std::string &units,
	   const std::vector<double> &data,
	   const int nx, const int ny, const double missing) :
  GridData(data, nx, ny, missing),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}


//----------------------------------------------------------------
Grid::Grid(const string &name, const string &units, const int nx, const int ny,
           const double bad) :
  GridData(nx, ny, bad),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}

//----------------------------------------------------------------
Grid::Grid(const string &name, const string &units, const int nx, const int ny,
           const int nz, const double bad) :
  GridData(nx, ny, nz, bad),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}


//----------------------------------------------------------------
Grid::Grid(const Grid &g, const string &name, const string &units) :
  GridData(g),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}

//----------------------------------------------------------------
Grid::Grid(const std::string &name, const std::string &units, const GridData &g) :
  GridData(g),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
}  
  
//----------------------------------------------------------------
Grid::Grid(const Grid &g, const string &name, const string &backupName,
	   const string &units) :
  GridData(g),
  pName(name),
  pUnits(units),
  pEncoding(Grid::ENCODING_INT8),
  pScaling(Grid::SCALING_DYNAMIC),
  pScale(0),
  pBias(0),
  pCompression(true)
{
  // choose backup name if name is empty
  if (name.empty())
  {
    pName = backupName;
  }

  // set all data to missing
  setAllMissing();
}

//----------------------------------------------------------------
Grid::Grid(const Grid &b) : 
  GridData(b),
  pName(b.pName),
  pUnits(b.pUnits),
  pEncoding(b.pEncoding),
  pScaling(b.pScaling),
  pScale(b.pScale),
  pBias(b.pBias),
  pCompression(b.pCompression)
{
}

//----------------------------------------------------------------
Grid::~Grid()
{
}

//----------------------------------------------------------------
Grid & Grid::operator=(const Grid &b)
{
  if (&b == this)
  {
    return *this;
  }

  // set all the values from input
  pName = b.pName;
  pUnits = b.pUnits;
  pEncoding = b.pEncoding;
  pScaling = b.pScaling;
  pScale = b.pScale;
  pBias = b.pBias;
  pCompression = b.pCompression;
  GridData::operator=(b);
  return *this;
}

//----------------------------------------------------------------
bool Grid::operator==(const Grid &b) const
{
  // check all simple members
  return (pName == b.pName &&
	  pUnits == b.pUnits &&
	  pEncoding == b.pEncoding &&
	  pScaling == b.pScaling &&
	  fabs(pScale - b.pScale)< convWx::EPSILON &&
	  fabs(pBias - b.pBias) < convWx::EPSILON &&
	  pCompression == b.pCompression &&
	  GridData::operator==(b));
}

//----------------------------------------------------------------
void Grid::changeNameAndUnits(const string &name, const string &units)
{
  if (!name.empty())
  {
    pName = name;
    pUnits = units;
  }
}

//----------------------------------------------------------------
string Grid::sprint(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "%s[%s]:", pName.c_str(), pUnits.c_str());
  string ret = buf;
  ret += sprintData();
  return ret;
}

//----------------------------------------------------------------
void Grid::logGrid(void) const
{
  string p = sprint();
  ILOG(FORCE, p);
}

//----------------------------------------------------------------
void Grid::logGrid(LogStream::Log_t severity) const
{
  string p = sprint();
  ILOGV(severity, p);
}

//----------------------------------------------------------------
void Grid::setScalingDynamic(void)
{
  pScaling = Grid::SCALING_DYNAMIC;
}

//----------------------------------------------------------------
void Grid::setScalingSpecified(const double scale, const double bias)
{
  pScaling = Grid::SCALING_SPECIFIED;
  pScale = scale;
  pBias = bias;
}

//----------------------------------------------------------------
void Grid::setEncoding(const Grid::Encoding_t encoding)
{
  pEncoding = encoding;
}

//----------------------------------------------------------------
void Grid::setCompression(const bool status)
{
  pCompression = status;
}

//----------------------------------------------------------------
void Grid::digitalFilter(void)
{
  //
  // round and cap values into the range 0 to DIGITAL_MAX
  //
  for (int k = 0; k < pNptTotal; ++k)
  {      
    double val;
    
    if (getValue(k, val))
    {
      if (val > convWx::DIGITAL_MAX)
      {
	val = convWx::DIGITAL_MAX;
      }
      setv(k, round(val));
    }
  }
  setScalingSpecified(1,0);
}

//----------------------------------------------------------------
void Grid::filterNans(void)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    if (!finite(pDataPtr[i]))
    {
      ILOGF(DEBUG, "%s[%d] = NaN, replace with missing",
	    pName.c_str(), i);
      pDataPtr[i] = pMissing;
    }
  }
}

//----------------------------------------------------------------
void Grid::pSetDefaultScalingAndEncoding(void)
{
  pEncoding = Grid::ENCODING_INT8;
  pScaling = Grid::SCALING_DYNAMIC;
  pScale = 0;
  pBias = 0;
  pCompression = true;
}

