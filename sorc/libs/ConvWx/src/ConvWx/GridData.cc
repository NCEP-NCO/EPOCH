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
 * @file GridData.cc
 * @brief 3 or 2 dimensional data grid 
 */
//----------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>

#include <ConvWxIO/ILogMsg.hh>
#include <ConvWxIO/ConvWxMultiThreadMgr.hh>
#include <ConvWx/GridData.hh>
#include <ConvWx/GridTraverse2d.hh>
#include <ConvWx/GridDistToNonMissing.hh>
#include <ConvWx/ConvWxFuzzy.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/GridLoopA.hh>
#include <ConvWx/GridLoopAlg.hh>
// #include <toolsa/LogStream.hh>

using std::string;
using std::vector;

//----------------------------------------------------------------
/**
 * Set signed value and a flag regarding positive values from inputs
 * @param[in] abs  True if can reverse value if it is negative
 * @param[in] v  Value
 * @param[out] signedv  The signed value, same as v except when abs=true
 *                       and v was negative
 * @param[out] positive  True for value == signedv
 */
static void sSetSigned(const bool abs, const double v, double &signedv,
		       bool &positive)
{
  if (abs)
  {
    if (v < 0)
    {
      signedv = -v;
      positive = false;
    }
    else
    {
      signedv = v;
      positive = true;
    }
  }
  else
  {
    signedv = v;
    positive = true;
  }
}

//----------------------------------------------------------------
GridData::GridData(void) :
  pDataPtr(NULL),
  pNptTotal(0),
  pNptX(0),
  pNptY(0),
  pNptZ(0),
  pMissing(0.0)
{
}

//----------------------------------------------------------------
GridData::GridData(const double *data, const int nx, const int ny,
		   const double bad) :
  pDataPtr(NULL),
  pNptTotal(nx*ny),
  pNptX(nx),
  pNptY(ny),
  pNptZ(1),
  pMissing(bad)
{
  pDataPtr = new double[pNptTotal];
  memcpy(pDataPtr, data, pNptTotal*sizeof(double));
}

//----------------------------------------------------------------
GridData::GridData(const std::vector<double> &data, const int nx,
		   const int ny, const double bad) :
  pDataPtr(NULL),
  pNptTotal(nx*ny),
  pNptX(nx),
  pNptY(ny),
  pNptZ(1),
  pMissing(bad)
{
  pDataPtr = new double[pNptTotal];
  for (size_t i=0; i<data.size(); ++i)
  {
    if (static_cast<int>(i) >= pNptTotal)
    {
      ILOGF(WARNING, "Out of range %d  [0,%d] ignored", i, 
	    pNptTotal);
    }
    else
    {
      pDataPtr[i] = data[i];
    }
  }
}

//----------------------------------------------------------------
GridData::GridData(const double *data, const int nx, const int ny,
		   const int nz, const double bad) :
  pDataPtr(NULL),
  pNptTotal(nx*ny*nz),
  pNptX(nx),
  pNptY(ny),
  pNptZ(nz),
  pMissing(bad)
{
  pDataPtr = new double[pNptTotal];
  memcpy(pDataPtr, data, pNptTotal*sizeof(double));
}

//----------------------------------------------------------------
GridData::GridData(const int nx, const int ny, const double bad) :
  pDataPtr(NULL),
  pNptTotal(nx*ny),
  pNptX(nx),
  pNptY(ny),
  pNptZ(1),
  pMissing(bad)
{
  pDataPtr = new double[pNptTotal];
  for (int i=0; i<pNptTotal; ++i)
  {
    pDataPtr[i] = pMissing;
  }
}

//----------------------------------------------------------------
GridData::GridData(const int nx, const int ny, const int nz, const double bad) :
  pDataPtr(NULL),
  pNptTotal(nx*ny*nz),
  pNptX(nx),
  pNptY(ny),
  pNptZ(nz),
  pMissing(bad)
{
  pDataPtr = new double[pNptTotal];
  for (int i=0; i<pNptTotal; ++i)
  {
    pDataPtr[i] = pMissing;
  }
}

//----------------------------------------------------------------
GridData::GridData(const GridData &b) :
  pDataPtr(NULL),
  pNptTotal(b.pNptTotal),
  pNptX(b.pNptX),
  pNptY(b.pNptY),
  pNptZ(b.pNptZ),
  pMissing(b.pMissing)
{
  // pDataPtr needs a memory allocation, then a copy
  if (pNptTotal > 0)
  {
    pDataPtr = new double[pNptTotal];
    memcpy(pDataPtr, b.pDataPtr, pNptTotal*sizeof(double));
  }
}

//----------------------------------------------------------------
GridData::~GridData()
{
  if (pNptTotal > 0)
  {
    delete [] pDataPtr;
  }
  pNptTotal = 0;
  pDataPtr = NULL;
}

//----------------------------------------------------------------
GridData & GridData::operator=(const GridData &b)
{
  if (this == &b)
  {
    return *this;
  }

  // re allocate pDataPtr unless size is the same as input.
  if (pNptTotal != b.pNptTotal)
  {
    if (pNptTotal > 0)
    {
      delete [] pDataPtr;
    }
    if (b.pNptTotal == 0)
    {
      pDataPtr = NULL;
    }
    else
    {
      pDataPtr = new double[b.pNptTotal];
    }
  }

  // set all the values from input
  pNptX = b.pNptX;
  pNptY = b.pNptY;
  pNptZ = b.pNptZ;
  pNptTotal = b.pNptTotal;
  pMissing = b.pMissing;
  if (pNptTotal > 0)
  {
    memcpy(pDataPtr, b.pDataPtr, pNptTotal*sizeof(double));
  }
  return *this;
}

//----------------------------------------------------------------
bool GridData::copyValues(const GridData &g)
{
  if (pNptX != g.pNptX || pNptY != g.pNptY ||  pNptZ != g.pNptZ)
  {
    ILOGF(ERROR, "dims unequal %d,%d,%d,  %d,%d,%d", pNptX, pNptY,
	  pNptZ, g.pNptX, g.pNptY, g.pNptZ );
    return false;
  }
  for (int i=0; i<pNptTotal; ++i)
  {
    pDataPtr[i] = g.pDataPtr[i];
  }
  pMissing = g.pMissing;
  return true;
}

//----------------------------------------------------------------
bool GridData::copyFloat(float *data, const int npt) const
{
  if (npt != pNptTotal)
  {
    ILOGF(ERROR, "npt:%d  pNptTotal:%d", npt, pNptTotal);
    return false;
  }
  for (int i=0; i<pNptTotal; ++i)
  {
    data[i] = static_cast<float>(pDataPtr[i]);
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::copyFloatFilterNans(float *data, const std::string &fieldName,
				   const int npt) const
{
  if (npt != pNptTotal)
  {
    ILOGF(ERROR, "Field %s npt:%d  pNptTotal:%d",
	  fieldName.c_str(), npt, pNptTotal);
    return false;
  }
  int nnan = 0;
  for (int i=0; i<pNptTotal; ++i)
  {
    if (!finite(pDataPtr[i]))
    {
      ++nnan;
      ILOGF(DEBUG_VERBOSE, "%s[%d] = NaN, replace with missing",
	    fieldName.c_str(), i);
      data[i] = static_cast<float>(pMissing);
    }
    else
    {
      data[i] = static_cast<float>(pDataPtr[i]);
    }
  }
  if (nnan > 0)
  {
    ILOGF(DEBUG, "%s replaced %d NaN values with missing",
	  fieldName.c_str(), nnan);
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::operator==(const GridData &b) const
{
  // check all simple members
  if (pNptX != b.pNptX ||
      pNptY != b.pNptY ||
      pNptZ != b.pNptZ ||
      pNptTotal != b.pNptTotal ||
      pMissing != b.pMissing)
  {
    return false;
  }

  // compare each data point
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != b.pDataPtr[i])
    {
      return false;
    }
  }  
  return true;
}


//----------------------------------------------------------------
bool GridData::valuesEqual(const GridData &g) const
{
  if (pNptX != g.pNptX || pNptY != g.pNptY || pNptZ != g.pNptZ ) 
  {
    return false;
  }

  for (int i=0; i<pNptX*pNptY*pNptZ; ++i)
  {
    if (pDataPtr[i] == pMissing)
    {
      if (g.pDataPtr[i] != g.pMissing)
      {
	return false;
      }
    }
    else
    {
      if (g.pDataPtr[i] == g.pMissing)
      {
	return false;
      }
      else
      {
	if (pDataPtr[i] != g.pDataPtr[i])
	{
	  return false;
	}
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::sizeEqual(const GridData &g) const
{
  return pNptX == g.pNptX || pNptY == g.pNptY ||  pNptZ == g.pNptZ;
}

//----------------------------------------------------------------
bool GridData::isMissingAt(const int x, const int y, const int z) const
{
  return pDataPtr[pIpt(x,y,z)] == pMissing;
}

//----------------------------------------------------------------
bool GridData::isMissingAt(const int ipt) const
{
  return pDataPtr[ipt] == pMissing;
}

//----------------------------------------------------------------
bool GridData::isNearlyEqualAt(const int ipt, const double value,
			       double tolerance) const
{
  return fabs(pDataPtr[ipt] - value) < tolerance;
}

//----------------------------------------------------------------
bool GridData::isEqualAt(const int ipt, const double value) const
{
  return pDataPtr[ipt] == value;
}

//----------------------------------------------------------------
bool GridData::isEqualAt(const int x, const int y, const double value) const
{
  return pDataPtr[pIpt(x,y,0)] == value;
}

//----------------------------------------------------------------
bool GridData::isEqualAt(const int x, const int y,  const int z, 
			 const double value) const
{
  return pDataPtr[pIpt(x,y,z)] == value;
}

//----------------------------------------------------------------
bool GridData::isLeAt(const int ipt, const double value) const
{
  if (isMissingAt(ipt))
  {
    return false;
  }
  else
  {
    return pDataPtr[ipt] <= value;
  }
}

//----------------------------------------------------------------
bool GridData::isLeAt(const int x, const int y, const double value) const
{
  if (isMissingAt(x, y, 0))
  {
    return false;
  }
  else
  {
    return pDataPtr[pIpt(x,y,0)] <= value;
  }
}
//----------------------------------------------------------------
bool GridData::isLeAt(const int x, const int y, const int z, 
		      const double value) const
{
  if (isMissingAt(x, y, z))
  {
    return false;
  }
  else
  {
    return pDataPtr[pIpt(x, y, z)] <= value;
  }
}

//----------------------------------------------------------------
bool GridData::isGtAt(const int ipt, const double value) const
{
  if (isMissingAt(ipt))
  {
    return false;
  }
  else
  {
    return pDataPtr[ipt] > value;
  }
}

//----------------------------------------------------------------
bool GridData::isGtAt(const int x, const int y, const double value) const
{
  if (isMissingAt(x, y, 0))
  {
    return false;
  }
  else
  {
    return pDataPtr[pIpt(x,y, 0)] > value;
  }
}

//----------------------------------------------------------------
bool GridData::isGtAt(const int x, const int y,  const int z, 
		      const double value) const
{
  if (isMissingAt(x, y, z))
  {
    return false;
  }
  else
  {
    return pDataPtr[pIpt(x, y, z)] > value;
  }
}

//----------------------------------------------------------------
bool GridData::allMissing(void) const
{
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::allDataMissingOrEqual(const double value) const
{
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != pMissing && pDataPtr[i] != value)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::getValue(const int x, const int y, double &value) const
{
  value = pDataPtr[pIpt(x,y,0)];
  return value != pMissing;
}

//----------------------------------------------------------------
bool GridData::getValue(const int x, const int y, const int z, 
			double &value) const
{
  value = pDataPtr[pIpt(x,y,z)];
  return value != pMissing;
}


//----------------------------------------------------------------
bool GridData::getValue(const int ipt, double &value) const
{
  value = pDataPtr[ipt];
  return value != pMissing;
}

//----------------------------------------------------------------
int GridData::nmissing(void) const
{
  int count = 0;
  for (int i=0; i<pNptX*pNptY*pNptZ; ++i)
  {
    if (pDataPtr[i] == pMissing)
    {
      ++count;
    }
  }
  return count;
}

//----------------------------------------------------------------
int GridData::getNdataEqualTo(const double value) const
{
  int count = 0;
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] == value)
    {
      ++count;
    }
  }
  return count;
}

//----------------------------------------------------------------
double GridData::pcntGe(const double value) const
{
  long numBelow = 0;
  long numAbove = 0;
  for(int k=0; k < pNptTotal; k++)
  {
    double d = pDataPtr[k];
    if (d == pMissing)
    {
      continue;
    }
    else
    {
      if (d >= value)
      {
	numAbove++;
      }
      else
      {
	numBelow++;
      }
    }	    
  }
    
  if (numAbove == 0 && numBelow == 0)
  {
    return 0.0;
  }
  else
  {
    return static_cast<double>(numAbove)/
      static_cast<double>(numAbove + numBelow);
  }
}

//----------------------------------------------------------------
bool GridData::mean(double &value) const
{
  double count = 0;
  double sum = 0;
  for(int k=0; k < pNptTotal; k++)
  {
    double d = pDataPtr[k];
    if (d == pMissing)
    {
      continue;
    }
    else
    {
      sum += d;
      count += 1.0;
    }
  }
    
  if (count == 0.0)
  {
    value = 0.0;
    return false;
  }
  else
  {
    value = sum/count;
    return true;
  }
}

//----------------------------------------------------------------
bool GridData::meanSubset(int x0, int y0, int nx, int ny, bool xWrap,
			  bool yWrap, double &value, bool &outOfBounds) const
{
  if (x0 < 0 || x0+nx > pNptX)
  {
    if (!xWrap)
    {
      ILOGF(ERROR, "subset out of grid range X [%d,%d]  [0,%d]",
	    x0, x0+nx-1, pNptX-1);
      outOfBounds = true;
      return false;
    }
  }
  if (y0 < 0 || y0+ny > pNptY)
  {
    if (!yWrap)
    {
      ILOGF(ERROR, "subset out of grid range Y [%d,%d]  [0,%d]",
	    y0, y0+ny-1, pNptY-1);
      outOfBounds = true;
      return false;
    }
  }
  
  outOfBounds = false;
  double count = 0;
  double sum = 0;

  for (int y=y0; y < y0+ny; ++y)
  {
    int iy = wraparoundY(y);
    for (int x=x0; x<x0+nx; ++x)
    {
      int ix = wraparoundX(x);
      double d = pDataPtr[pIpt(ix, iy)];
      if (d == pMissing)
      {
	continue;
      }
      else
      {
	sum += d;
	count += 1.0;
      }
    }
  }    
  if (count == 0.0)
  {
    value = 0.0;
    return false;
  }
  else
  {
    value = sum/count;
    return true;
  }
}

//----------------------------------------------------------------
bool GridData::percentAboveThresholdSubset(double threshold, int x0, int y0,
					   int nx, int ny, bool xWrap,
					   bool yWrap, double &value,
					   bool &outOfBounds) const
{
  if (x0 < 0 || x0+nx > pNptX)
  {
    if (!xWrap)
    {
      ILOGF(ERROR, "subset out of grid range X [%d,%d]  [0,%d]",
	    x0, x0+nx-1, pNptX-1);
      outOfBounds = true;
      return false;
    }
  }
  if (y0 < 0 || y0+ny > pNptY)
  {
    if (!yWrap)
    {
      ILOGF(ERROR, "subset out of grid range Y [%d,%d]  [0,%d]",
	    y0, y0+ny-1, pNptY-1);
      outOfBounds = true;
      return false;
    }
  }
  
  outOfBounds = false;
  double count = 0;
  double sum = 0;

  for (int y=y0; y < y0+ny; ++y)
  {
    int iy = wraparoundY(y);
    for (int x=x0; x<x0+nx; ++x)
    {
      int ix = wraparoundX(x);
      double d = pDataPtr[pIpt(ix, iy)];
      if (d == pMissing)
      {
	continue;
      }
      else
      {
	count += 1.0;
	if (d > threshold)
	{
	  sum += 1.0;
	}
      }
    }
  }    
  if (count == 0.0)
  {
    value = 0.0;
    return false;
  }
  else
  {
    value = sum/count;
    return true;
  }
}

//----------------------------------------------------------------
bool GridData::getRange(double &minv, double &maxv) const
{
  // initialize to missing
  minv = maxv = pMissing;
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      if (minv == pMissing)
      {
	// first one 
	minv = maxv = pDataPtr[i];
      }
      else
      {
	// compare and adjust
	if (pDataPtr[i] < minv)
	{
	  minv = pDataPtr[i];
	}
	if (pDataPtr[i] > maxv)
	{
	  maxv = pDataPtr[i];
	}
      }
    }
  }
  return minv != pMissing;
}

//----------------------------------------------------------------
void GridData::getConstrainedLimitsX(const int x, const int dx,
				     int &x0, int &x1) const
{  
  x0 = x - dx;
  if (x0 < 0)
  {
    x0 = 0;
  }
  x1 = x + dx;
  if (x >= pNptX)
  {
    x1 = pNptX - 1;
  }
}
//----------------------------------------------------------------
void GridData::getConstrainedLimitsY(const int y, const int dy,
				     int &y0, int &y1) const
{  
  y0 = y - dy;
  if (y0 < 0)
  {
    y0 = 0;
  }
  y1 = y + dy;
  if (y >= pNptY)
  {
    y1 = pNptY - 1;
  }
}

//----------------------------------------------------------------
void GridData::setToMissing(const int ix, const int iy, const int iz)
{
  setv(ix, iy, iz,  pMissing);
}



//----------------------------------------------------------------
void GridData::setToMissing(const int ipt)
{
  setv(ipt, pMissing);
}

//----------------------------------------------------------------
void GridData::setAllMissing(void)
{
  setAllToValue(pMissing);
}

//----------------------------------------------------------------
void GridData::setAllToValue(const double value)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    pDataPtr[i] = value;
  }
}

//----------------------------------------------------------------
void GridData::changeValue(const double vold, const double vnew)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] == vold)
    {
      pDataPtr[i] = vnew;
    }
  }
}

//----------------------------------------------------------------
void GridData::changeMissing(const double missingValue)
{
  if (missingValue == pMissing)
  {
    return;
  }

  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] == pMissing)
    {
      pDataPtr[i] = missingValue;
    }
  }
  pMissing = missingValue;
}

//----------------------------------------------------------------
string GridData::sprintData(void) const
{
  char buf[convWx::ARRAY_LEN_LONG];
  sprintf(buf, "nx:%d ny:%d nz:%d miss:%lf", pNptX, pNptY, pNptZ, pMissing);
  string ret = buf;
  return ret;
}

//----------------------------------------------------------------
void GridData::logData(const string &fname) const
{
  FILE *fp = fopen(fname.c_str(), "w");

  if (fp != NULL)
  {
     char buf[convWx::ARRAY_LEN_SHORT];
     buf[0] = 0;
     // print 5 chars per value (including space), 65 wide total, i.e. 13
     int count =0;
     for (int i=0; i<=pNptTotal; ++i)
     {
        sprintf(buf+strlen(buf), "%4.2lf ", pDataPtr[i]);
       if (++count >= 13)
       {
         fprintf(fp, "%s\n", buf);
         buf[0] = 0;
         count = 0;
       }
     }
     if (strlen(buf) > 0)
     {
       fprintf(fp, "%s\n", buf);
     }
     fclose(fp);
  } 
}
//----------------------------------------------------------------
void GridData::logValues(const int ipt0, const int ipt1) const
{
  int i0 = ipt0;
  if (i0 < 0)
  {
    i0 = 0;
  }
  int i1 = ipt1;
  if (i1 >= pNptTotal)
  {
    i1 = pNptTotal - 1;
  }
  printf("From %d to %d\n", ipt0, ipt1);
  char buf[convWx::ARRAY_LEN_SHORT];
  buf[0] = 0;
  // print 5 chars per value (including space), 65 wide total, i.e. 13
  int count =0;
  for (int i=i0; i<=i1; ++i)
  {
    sprintf(buf+strlen(buf), "%4.2lf ", pDataPtr[i]);
    if (++count >= 13)
    {
      printf("%s\n", buf);
      buf[0] = 0;
      count = 0;
    }
  }
  if (strlen(buf) > 0)
  {
    printf("%s\n", buf);
  }
}

//----------------------------------------------------------------
void GridData::logValues(const int ix0, const int ix1, const int iy0,
			 const int iy1) const
{
  int x0 = ix0;
  if (x0 < 0)
  {
    x0 = 0;
  }
  int x1 = ix1;
  if (x1 >= pNptX)
  {
    x1 = pNptX-1;
  }

  int y0 = iy0;
  if (y0 < 0)
  {
    y0 = 0;
  }
  int y1 = iy1;
  if (y1 >= pNptY)
  {
    y1 = pNptY-1;
  }

  printf("[%d,%d] to [%d,%d]\n", x0, y0, x1, y1);
  char buf[convWx::ARRAY_LEN_SHORT];
  buf[0] = 0;
  // print 12 chars per value (including space), 80 wide total, i.e. 6
  int count =0;
  for (int iy=y0; iy<=y1; ++iy)
  {
    for (int ix=x0; ix<=x1; ++ix)
    {
      sprintf(buf+strlen(buf), "%11.7lf ", pDataPtr[index(ix, iy)]);
      if (++count >= 6)
      {
	printf("%s\n", buf);
	buf[0] = 0;
	count = 0;
      }
    }
  }
  if (strlen(buf) > 0)
  {
    printf("%s\n", buf);
  }
}

//----------------------------------------------------------------
void GridData::smooth(int sx, int sy, bool isExclude,
		      double excludeValue, bool rejectCenterExclude)
{
  // make a temporary grid
  GridData tmp(*this);

  // for each point
  for (int iy=0; iy<pNptY; ++iy)
  {
    for (int ix=0; ix<pNptX; ++ix)
    {
      // compute a local average
      double v = pLocalAverage(ix, iy, sx, sy, isExclude, excludeValue,
			       rejectCenterExclude);

      // store to temporary grid
      tmp.setv(ix, iy, v);
    }
  }

  // replace local state with temp.
  *this = tmp;
}

//----------------------------------------------------------------
void GridData::fastsmooth(int sx, int sy, bool isExclude,
			  double excludeValue, bool rejectCenterExclude)
{
  // make an object to loop through the grid
  GridLoopA g(pNptX, pNptY, sx, sy);

  // make a copy of the local object
  GridData tmp(*this);

  GridLoopAlgMean a = GridLoopAlgMean(isExclude, excludeValue,
				      rejectCenterExclude);

  while (g.increment(tmp, a))
  {
    int x, y;
    double result;
    if (g.getXyAndResult(a, 0, tmp, x, y, result))
    {
      // LOG(DEBUG) << " Result[" << x << "," << y << "]=" << result;
      pDataPtr[y*pNptX + x] = result;
    }
    else
    {
      pDataPtr[y*pNptX + x] = pMissing;
    }
  }
}

//----------------------------------------------------------------
bool GridData::smoothInMask(const GridData &mask, const double maskValue,
			    const int sx, const int sy, const bool isExclude,
			    const double excludeValue)
{
  // does mask look like local grid?
  if (!sizeEqual(mask))
  {
    ILOGF(ERROR, "npt:%d  pNptTotal:%d", mask.pNptTotal, pNptTotal);
    return false;
  }

  // make a copy of local grid
  GridData tmp(*this);

  for (int iy=0; iy<pNptY; ++iy)
  {
    for (int ix=0; ix<pNptX; ++ix)
    {
      if (mask.isEqualAt(ix, iy, maskValue))
      {
	// mask === maskValue at this point, do the averaging
	double v = pLocalAverage(ix, iy, sx, sy, isExclude, excludeValue, true);
	tmp.setv(ix, iy, v);
      }
    }
  }
  *this = tmp;
  return true;
}

//----------------------------------------------------------------
void GridData::highSmooth(const int sx, const int sy, const bool isExclude,
			  const double excludeValue)
{
  // make a temporary grid
  GridData tmp(*this);

  // for each point
  for (int iy=0; iy<pNptY; ++iy)
  {
    for (int ix=0; ix<pNptX; ++ix)
    {
      // compute a local average
      double v = pLocalHighAverage(ix, iy, sx, sy, isExclude, excludeValue);

      // store to temporary grid
      tmp.setv(ix, iy, v);
    }
  }

  // replace local state with temp.
  *this = tmp;
}

//----------------------------------------------------------------
bool GridData::highSmoothInMask(const GridData &mask, const double maskValue,
				const int sx, const int sy,
				const bool isExclude,
				const double excludeValue)
{
  // does mask look like local grid?
  if (!sizeEqual(mask))
  {
    ILOGF(ERROR, "npt:%d  pNptTotal:%d", mask.pNptTotal, pNptTotal);
    return false;
  }

  // make a copy of local grid
  GridData tmp(*this);

  for (int iy=0; iy<pNptY; ++iy)
  {
    for (int ix=0; ix<pNptX; ++ix)
    {
      if (mask.isEqualAt(ix, iy, maskValue))
      {
	// mask === maskValue at this point, do the averaging
	double v = pLocalHighAverage(ix, iy, sx, sy, isExclude, excludeValue);
	tmp.setv(ix, iy, v);
      }
    }
  }
  *this = tmp;
  return true;
}

//----------------------------------------------------------------
void GridData::max(const int sx, const int sy, const bool isAbsoluteMax)
{
  // make a temporary grid
  GridData tmp(*this);

  // for each point
  for (int iy=0; iy<pNptY; ++iy)
  {
    for (int ix=0; ix<pNptX; ++ix)
    {
      // compute a local max
      double v = pLocalMax(ix, iy, sx, sy, isAbsoluteMax);

      // store to temporary grid
      tmp.setv(ix, iy, v);
    }
  }

  // replace local state with temp.
  *this = tmp;
}

//----------------------------------------------------------------
void GridData::fillGaps(const int sx, const int sy)
{
  GridData tmp(*this);
  double v;
  for (int iy=0; iy<pNptY; ++iy)
  {
    for (int ix=0; ix<pNptX; ++ix)
    {
      if (getValue(ix, iy, v))
      {
	continue;
      }
      double v = pLocalAverage(ix, iy, sx, sy);
      tmp.setv(ix, iy, v);
    }
  }
  *this = tmp;
}

//----------------------------------------------------------------
bool GridData::multiply(const GridData &weight)
{
  if (pNptX != weight.pNptX || pNptY != weight.pNptY ||  pNptZ != weight.pNptZ)
  {
    ILOG(ERROR, "dimensions are off");
    return false;
  }
  double w, v;
  for (int i=0; i<pNptTotal; ++i)
  {
    if (weight.getValue(i, w))
    {
      if (w != 1.0)
      {
	if (getValue(i, v))
	{
	  v *= w;
	  setv(i, v);
	}
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::divide(const GridData &divisor)
{
  if (pNptX != divisor.pNptX || pNptY != divisor.pNptY)
  {
    ILOG(ERROR, "dimensions are off");
    return false;
  }

  for (int i=0; i<pNptTotal; ++i)
  {
    double v;
    if (getValue(i, v))
    {
      double w;
      if (divisor.getValue(i, w))
      {
	if (w != 0)
	{
	  v = v/w;

	  setv(i, v);
	}
	else
	{
	  setv(i,pMissing);
	}
      }
      else
      {
	setv(i,pMissing);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::squareRoot(void)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    double w;
    if ( getValue(i, w) )
    {
      if (w >= 0)
      {
        double sqrRt = sqrt(w); 
        setv(i, sqrRt);
      }
      else
      {
	return false;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::sine(void)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    double w;
    if ( getValue(i, w) )
    {
      double sinW = sin(w);
      setv(i, sinW);
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::cosine(void)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    double w;
    if ( getValue(i, w) )
    {
      double cosW = cos(w);
      setv(i, cosW);
    }
  }
  return true;
}
//----------------------------------------------------------------
bool GridData::arctan(void)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    double w;
    if ( getValue(i, w) )
    {
      double atanW = atan(w);
      setv(i, atanW);
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::arcsine(void)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    double w;
    if (getValue(i, w))
    {
      if ( w >1 || w < -1)
      {
	setv(i, pMissing);
      }
      else
      {
	double asinW = asin(w);
	setv(i, asinW);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::add(const GridData &inp)
{
  if (inp.pNptTotal != pNptTotal)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }
  for (int i=0; i<pNptTotal; ++i)
  {
    double v;
    if (inp.getValue(i, v))
    {
      if (pDataPtr[i] != pMissing)
      {
	pDataPtr[i] += v;
      }
      else
      {
	pDataPtr[i] = v;
      }
    }
  }
  return true;
}    

//----------------------------------------------------------------
bool GridData::subtract(const GridData &inp)
{
  if (inp.pNptTotal != pNptTotal)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      double v;
      if (inp.getValue(i, v))
      {
	pDataPtr[i] -= v;
      }
    }
  }
  return true;
}    

//----------------------------------------------------------------
bool GridData::add(const double &val)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      pDataPtr[i] += val;
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::nearbyMax(const int centerX, const int centerY, const int sx,
			 const int sy, int &rx, int &ry) const
{
  double max=0;
  bool first = true;
  rx = ry = -1;
  for (int y=centerY-sy; y<=centerY+sy; ++y)
  {
    if (y < 0 || y >= pNptY)
    {
      continue;
    }
    for (int x=centerX-sx; x<=centerX+sx; ++x)
    {
      if (x < 0 || x >= pNptX)
      {
	continue;
      }
      double v;
      if (getValue(x, y, v))
      {
	if (first)
	{
	  first = false;
	  max = v;
	  rx = x;
	  ry = y;
	}
	else
	{
	  if (v > max)
	  {
	    max = v;
	    rx = x;
	    ry = y;
	  }
	}
      }
    }
  }
  return !first;
}

//----------------------------------------------------------------
bool GridData::reduce(const int f)
{
  if (f < 2)
  {
    ILOGF(WARNING, "cant reduce grid f=%d (too small)",f);
    return false;
  }
  int nx = pNptX/f;
  int ny = pNptY/f;
  if (nx < 1 || ny < 1)
  {
    ILOGF(WARNING, "cant reduce grid f=%d (too big)",f);
    return false;
  }

  // make a grid for results
  GridData g(nx, ny, pMissing);

  // simply subsample the grid
  for (int y=0; y<ny; ++y)
  {
    int fullY = y*f;
    if (fullY >= pNptY)
    {
      ILOG(ERROR, "fullY > pNptY");
      continue;
    }
    for (int x=0; x<nx; ++x)
    {
      int fullX = x*f;
      if (fullX >= pNptX)
      {
	ILOG(ERROR, "fullX > pNptX");
	continue;
      }

      // store from fullX, fullY to grid at x,y
      g.setv(x, y, returnValue(fullX, fullY));
    }
  }
  *this = g;
  return true;
}

//----------------------------------------------------------------
bool GridData::interpolate(const GridData &lowres, const int res)
{
  if (lowres.pNptX != pNptX/res)
  {
    ILOG(ERROR, "lowrespNptX != pNptX/res");
    return false;
  }
  if (lowres.pNptY != pNptY/res)
  {
    ILOG(ERROR, "lowrespNptY != pNptY/res");
    return false;
  }

  // loop through high res grid, compute low res point 
  for (int y=0; y<pNptY; ++y)
  {
    int ry0 = y/res;
    for (int x=0; x<pNptX; ++x)
    {
      int rx0 = x/res;

      // do bilinear interpolation 
      double v = pBilinear(ry0, rx0, res, y, x, lowres);

      // store to high res (local) grid
      setv(x, y, v);
    }
  }
  return true;
}

//----------------------------------------------------------------
void GridData::interpolate(const GridData &lowres, const int res,
			   bool &status)
{
  if (lowres.pNptX != pNptX/res)
  {
    ILOG(ERROR, "lowrespNptX != pNptX/res");
    status = false;
    return;
  }
  if (lowres.pNptY != pNptY/res)
  {
    ILOG(ERROR, "lowrespNptY != pNptY/res");
    status = false;
    return;
  }

  // loop through high res grid, compute low res point 
  for (int y=0; y<pNptY; ++y)
  {
    int ry0 = y/res;
    for (int x=0; x<pNptX; ++x)
    {
      int rx0 = x/res;

      // do bilinear interpolation 
      double v = pBilinear(ry0, rx0, res, y, x, lowres);

      // store to high res (local) grid
      setv(x, y, v);
    }
  }
}

//----------------------------------------------------------------
void GridData::multiply(const int x, const int y, const double value)
{
  double v;
  if (getValue(x, y, v))
  {
    pDataPtr[pIpt(x,y)] = v*value;
  }
}

//----------------------------------------------------------------
void GridData::multiply(const double value)
{
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      pDataPtr[i] *= value;
    }
  }
}

//----------------------------------------------------------------
void GridData::incrementValueAtPoint(const int ipt, const double value)
{
  if (pDataPtr[ipt] != pMissing)
  {
    pDataPtr[ipt] += value;
  }
  else
  {
    pDataPtr[ipt] = value;
  }
}

//----------------------------------------------------------------
void GridData::incrementValueAtPoint(const int x, const int y,
				     const double value)
{
  int ipt = pIpt(x, y);
  if (pDataPtr[ipt] != pMissing)
  {
    pDataPtr[ipt] += value;
  }
  else
  {
    pDataPtr[ipt] = value;
  }
}


//----------------------------------------------------------------
int GridData::replace(const double oldv, const double newv)
{
  ILOGF(DEBUG_VERBOSE, "replacing %lf with %lf", oldv, newv);
  int count = 0;
  for (int i=0; i<pNptTotal; ++i)
  {
    if (pDataPtr[i] == oldv)
    {
      ++count;
      pDataPtr[i] = newv;
    }
  }
  return count;
}

//----------------------------------------------------------------
void GridData::replaceValueWithNearestMax(const double value)
{
  // need a copy to do this
  GridData tmp(*this);

  // set up a max search radius to maximum of nx and ny
  int maxRadius = pNptY;
  if (pNptX > maxRadius)
  {
    maxRadius = pNptX;
  }

  // look everywhere for values equal to v
  for (int y=0; y<pNptY; ++y)
  {
    for (int x=0; x<pNptX; ++x)
    {
      if (pDataPtr[y*pNptX + x] == value)
      {
	// value, replace at this point

	ILOGF(DEBUG_VERBOSE, "replacing at %d,%d", x, y);

	// search for something not equal to value v.
	bool rep = false;
	for (int r=1; r<maxRadius; ++r)
	{
	  double vnew;

	  // look at all points at radius r from x,y not equal to value
	  if (tmp.pMaxValueNotEqualAtRadius(x, y, value, r, vnew))
	  {
	    pDataPtr[y*pNptX + x] = vnew;
	    rep = true;
	    break;
	  }
	}
	// did we replace or not?
	if (!rep)
	{
	  // no means set to missing
	  pDataPtr[y*pNptX + x] = pMissing;
	}
      }
    }
  }
}

//----------------------------------------------------------------
void GridData::applyFuzzyF(const ConvWxFuzzy &f)
{
  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      pDataPtr[i] = f.fuzzyF(pDataPtr[i]);
    }
  }
}

//----------------------------------------------------------------
bool GridData::maskMissingToMissing(const GridData &mask)
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (mask.isMissingAt(i))
    {
      setToMissing(i);
    }
  }
  return true;
}
  

//----------------------------------------------------------------
bool GridData::maskMissingToValue(const GridData &mask,
				  const double replaceV,
				  const bool overwritepMissing)
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (mask.isMissingAt(i))
    {
      if (overwritepMissing)
      {
	setv(i, replaceV);
      }
      else
      {
	if (!isMissingAt(i))
	{
	  setv(i, replaceV);
	}
      }
    }
  }
  return true;
}
  

//----------------------------------------------------------------
bool GridData::maskThreshOrMissingToValue(const GridData &mask,
					  const double maskThresh,
					  const double replaceV,
					  const bool overwritepMissing)
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (mask.isMissingAt(i) || mask.isLeAt(i, maskThresh))
    {
      if (overwritepMissing)
      {
	setv(i, replaceV);
      }
      else 
      {
	if (!isMissingAt(i))
	{
	  setv(i, replaceV);
	}
      }
    }
  }
  return true;
}
  
//----------------------------------------------------------------
bool GridData::maskBelowThreshToMissing(const GridData &maskGrid, 
					const double maskThresh)
{
  if (pNptX != maskGrid.pNptX || pNptY != maskGrid.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (maskGrid.isLeAt(i, maskThresh))
    {
      setv(i, pMissing);
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::maskAboveThreshToMissing(const GridData &maskGrid, 
					const double maskThresh)
{
  if (pNptX != maskGrid.pNptX || pNptY != maskGrid.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (maskGrid.isGtAt(i, maskThresh) || 
        maskGrid.isEqualAt(i, maskThresh))
    {
      setv(i, pMissing);
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::minValueInMask(const GridData &mask, const double maskValue,
			      double &minv) const
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  double v;
  bool first = true;
  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (mask.isEqualAt(i, maskValue))
    {
      if (getValue(i, v))
      {
	if (first)
	{
	  first = false;
	  minv = v;
	}
	else
	{
	  if (v < minv)
	  {
	    minv = v;
	  }
	}
      }
    }
  }
  return !first;
}

//----------------------------------------------------------------
bool GridData::maxInMask(const GridData &mask, 
			 int &maxX, int &maxY, double &maxValue) const
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  bool first = true;
  for (int y=0; y<pNptY; ++y)
  {
    for (int x=0; x<pNptX; ++x)
    {
      if (mask.isMissingAt(x, y))
      {
	continue;
      }
      double v;
      if (getValue(x, y, v))
      {
	if (first)
	{
	  first = false;
	  maxValue = v;
	  maxX = x;
	  maxY = y;
	}
	else
	{
	  if (v > maxValue)
	  {
	    maxValue = v;
	    maxX = x;
	    maxY = y;
	  }
	}
      }
    }
  }
  return !first;
}

//----------------------------------------------------------------
bool GridData::setValueInMask(const GridData &mask, const double maskValue,
			      const double value, double maskTolerance)
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }
  if (maskTolerance > 0.0)
  {
    for (int i=0; i<pNptX*pNptY; ++i)
    {
      if (mask.isNearlyEqualAt(i, maskValue, maskTolerance))
      {
	setv(i, value);
      }
    }
  }
  else
  {
    for (int i=0; i<pNptX*pNptY; ++i)
    {
      if (mask.isEqualAt(i, maskValue))
      {
	setv(i, value);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::setMissingAndMaskNonMissingToValue(const GridData &mask,
						  const double value)
{
  if (pNptX != mask.pNptX || pNptY != mask.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (pDataPtr[i] == pMissing)
    {
      if (!mask.isMissingAt(i))
      {
	setv(i, value);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool GridData::minimum(const GridData &inp)
{
  if (pNptX != inp.pNptX || pNptY != inp.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  double iv, v;
  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (inp.getValue(i, iv))
    {
      if (getValue(i, v))
      {
	if (iv < v)
	{
	  setv(i, iv);
	}
      }
      else
      {
	setv(i, iv);
      }
    }
  }
  return true;
}
  
//----------------------------------------------------------------
bool GridData::maximum(const GridData &inp)
{
  if (pNptX != inp.pNptX || pNptY != inp.pNptY)
  {
    ILOG(ERROR, "grids not same size");
    return false;
  }

  double iv, v;
  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (inp.getValue(i, iv))
    {
      if (getValue(i, v))
      {
	if (iv > v)
	{
	  setv(i, iv);
	}
      }
      else
      {
	setv(i, iv);
      }
    }
  }
  return true;
}
  
//----------------------------------------------------------------
void GridData::setDataInRangeToValue(const double v0, const double v1, 
				     const double newValue)
{
  for (int i=0; i<pNptX*pNptY; ++i)
  {
    double v;
    if (getValue(i, v))
    {
      if (v >= v0 && v <= v1)
      {
	setv(i, newValue);
      }
    }
  }
}

//----------------------------------------------------------------
int GridData::gridIndex(const int x, const int y, const int nx)
{
  return y*nx + x;
}

//----------------------------------------------------------------
int GridData::gridIndex(const int x, const int y, const int z, const int nx,
			const int ny)
{
  return z*nx*ny +y*nx + x;
}

//----------------------------------------------------------------
bool GridData::ddx( const int halfDeltaX, 
		    const ParmProjection &proj, 
		    GridData &outGrid) const
{
  if (pNptX != outGrid.pNptX || pNptY != outGrid.pNptY 
      ||  pNptZ != outGrid.pNptZ)
  {
    ILOGF(ERROR, "dims unequal %d,%d,%d,  %d,%d,%d", 
	  pNptX, pNptY, pNptZ,outGrid.pNptX, outGrid.pNptY, 
	  outGrid.pNptZ );
    return false;
  }

  for(int k = 0; k < pNptZ; k++)
  {
    for (int j = 0; j < pNptY; j++)
    {
      //
      // difference quotient with respect to x using interior 
      // points of grid
      //
      for (int i = halfDeltaX; i < pNptX -halfDeltaX; i++)
      {
	double y1;
	
	double y2;
	
	if (getValue(i-halfDeltaX, j, k, y1) && 
	    getValue(i+halfDeltaX , j, k, y2))
	{
	  
	  double diffX;
	  
	  InterfaceLL::distanceKm(proj, i - halfDeltaX, j, 
				  i + halfDeltaX, j, diffX);
	  
	  double deriv = (y2 - y1)/diffX;  
	  
	  outGrid.setv(i,j,k,deriv);
	}
	else
	{
	  outGrid.setv(i,j,k, outGrid.getMissing());
	}
      }
      
      //
      // one sided difference quotient at left edge points
      //
      for (int i = 0; i < halfDeltaX; i++)
      {
	double y2;
	
	double y1;
	
	if( getValue(i,j,k,y1) &&
	    getValue(i + halfDeltaX, j, k, y2))
	{
	  double diffX;
	  
	  InterfaceLL::distanceKm(proj, i, j, 
				  i + halfDeltaX, j, diffX);
	  
	  double deriv = (y2 - y1)/diffX;
	  
	  outGrid.setv(i,j,k,deriv);
	}
	else
	{
	  outGrid.setv(i,j,k,outGrid.getMissing());
	}
      }
      
      //
      // one sided difference quotient at right edge points
      //
      for (int i = pNptX - halfDeltaX; i < pNptX; i++)
      {
	double y1;
	
	double y2;
	
	if (getValue(i - halfDeltaX, j, k, y1) &&
	    getValue(i, j, k, y2))
	{ 
	  double diffX;
	  
	  InterfaceLL::distanceKm(proj, i - halfDeltaX, j, 
				  i, j, diffX);
	  
	  double deriv = (y2 - y1)/diffX;
	  
	  outGrid.setv(i,j,k,deriv);
	}
	else
	{
	  outGrid.setv(i,j,k,outGrid.getMissing());
	}
      }
    }
  }

  return true;
}

//----------------------------------------------------------------
bool GridData::ddy( const int halfDeltaY, 
		    const ParmProjection  &proj, 
		    GridData &outGrid) const
{
  if (pNptX != outGrid.pNptX || pNptY != outGrid.pNptY 
      ||  pNptZ != outGrid.pNptZ)
  {
    ILOGF(ERROR, "dims unequal %d,%d,%d,  %d,%d,%d", 
	  pNptX, pNptY, pNptZ,outGrid.pNptX, outGrid.pNptY, 
	  outGrid.pNptZ );
    return false;
  }

  for (int k = 0; k < pNptZ; k++)
  {
    for (int i = 0; i < pNptX; i++)
    {
      //
      // difference quotient with respect to y using interior points of grid
      //
      for (int j = halfDeltaY; j < pNptY - halfDeltaY; j++)
      {
	double y1;
	
	double y2;
	
	if( getValue(i, j - halfDeltaY, k, y1) &&
	    getValue(i, j + halfDeltaY, k, y2))
	{ 
	  double diffY;
	  
	  InterfaceLL::distanceKm(proj, i,j - halfDeltaY, 
				  i, j + halfDeltaY, diffY);
	  
	  double deriv = (y2 - y1)/diffY;  
	  
	  outGrid.setv(i, j, k, deriv);
	}
	else
	{
	  outGrid.setv(i, j, k, outGrid.getMissing());
	}
      }
      
      //
      // One sided difference quotient at bottom edge points
      //
      for (int j = 0; j < halfDeltaY; j++)
      {
	double y1;
	
	double y2;
	
	if( getValue(i,j,k,y1) &&
	    getValue(i, j + halfDeltaY, k, y2))
	{
	  double diffY;
	  
	  InterfaceLL::distanceKm(proj, i, j, 
				  i, j + halfDeltaY, diffY);
	  
	  double deriv = (y2 - y1)/diffY;
	  
	  outGrid.setv(i,j,k,deriv);
	}
	else
	{
	  outGrid.setv(i, j, k, outGrid.getMissing());
	}
      }
      
      //
      // one sided difference quotient at top edge points
      //
      for (int j = pNptY - halfDeltaY; j < pNptY; j++)
      {
	double y1;
	
	double y2;
	
	if ( getValue(i, j, k, y1) &&
	     getValue(i, j - halfDeltaY, k, y2))
	{
	  double diffY;
	  
	  InterfaceLL::distanceKm(proj, i, j, 
				  i, j - halfDeltaY, diffY);
	  
	  double deriv = (y2 - y1)/diffY;
	  
	  outGrid.setv(i, j, k, deriv);
	}
	else
	{
	  outGrid.setv(i, j, k, outGrid.getMissing());
	}
      }
    }
  }
  
  return true;
}

//----------------------------------------------------------------
bool GridData::distanceToNonMissing(const int searchScale,
				    const double maxRadius,
				    const ParmProjection &proj,
				    GridData &distOut, GridData &valOut) const
{      
  //
  // check dimensions for consistency
  if (!sizeEqual(distOut) || !sizeEqual(valOut))
  {
    ILOG(ERROR, "Dimensions inconsistent");
    return false;
  }

  //
  // At each grid point x,y find and record minimum distances to non-missing
  // data and the non missing data value  
  //
  for (int i = 0; i < pNptX; i++)
  { 
    for (int j = 0; j < pNptY; j++)
    {
      //
      // Approximation for minimum distance to non-missing data from (i,j)
      //
      double minDist = maxRadius; 

      //
      // Value of data at minimum distance
      //
      double closeVal;

      //
      // Flag to indicate successful search
      //
      bool distFound = false;
      
      //
      // Check right away if point is not missing or bad
      // since we need not go further and distance = 0 at such points
      //
      if (!isMissingAt(i, j))
      { 
	distFound = true;

	minDist = 0.0;

	getValue(i, j, closeVal);
      }
      else
      {
	distFound = pSearchNbhd(i, j, searchScale, maxRadius,
				proj, minDist, closeVal);
      }
      
      if (!distFound)
      {
	/**
	 * At points where no nearby data is non-missing, set result to 
	 * max_radius
	 */
	distOut.setv(i, j, maxRadius);
	valOut.setToMissing(i, j);
      }
      else
      {
	//
	// Set the minimum distance to non missing data and the value 
	//
	distOut.setv(i, j, minDist);
	valOut.setv(i, j, closeVal);
      }
    } // End for
  }// End for

  return true;
}

//----------------------------------------------------------------
void GridData::invertMissing(void)
{
  GridData tmp(*this);
  double v = pMissing + 100.0;
  for (int i=0; i<pNptTotal; ++i)
  {
    if (isMissingAt(i))
    {
      tmp.setv(i, v);
    }
    else
    {
      tmp.setToMissing(i);
    }
  }
  *this = tmp;
}

//----------------------------------------------------------------
double GridData::diffSq(int x0, int y0, int xoff, int yoff,
			const GridData &input, double thresh) const
{
  if (inRange(x0,y0) && inRange(x0+xoff,y0+yoff))
  {
    double v1, v2;
    if (getValue(x0,y0,v1) && input.getValue(x0+xoff,y0+yoff, v2))
    {
      if (v1 < thresh)
      {
	v1 = 0.0;
      }
      if (v2 < thresh)
      {
	v2 = 0.0;
      }
      return (v1-v2)*(v1-v2);
    }
  }
  return pMissing;
}

//----------------------------------------------------------------
void GridData::monotonicDecreasing(const GridData &g)
{
  if (pNptX != g.pNptX || pNptY != g.pNptY)
  {
    ILOG(ERROR, "Dimensions inconsistent");
    return;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      if (g.pDataPtr[i] != g.pMissing)
      {
	if (pDataPtr[i] > g.pDataPtr[i])
	{
	  pDataPtr[i] = g.pDataPtr[i];
	}
      }
    }
  }
}

//----------------------------------------------------------------
void GridData::monotonicIncreasing(const GridData &g)
{
  if (pNptX != g.pNptX || pNptY != g.pNptY)
  {
    ILOG(ERROR, "Dimensions inconsistent");
    return;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (pDataPtr[i] != pMissing)
    {
      if (g.pDataPtr[i] != g.pMissing)
      {
	if (pDataPtr[i] < g.pDataPtr[i])
	{
	  pDataPtr[i] = g.pDataPtr[i];
	}
      }
    }
  }
}

//----------------------------------------------------------------
void GridData::oneMinus(const GridData &g)
{
  if (pNptX != g.pNptX || pNptY != g.pNptY)
  {
    ILOG(ERROR, "Dimensions inconsistent");
    return;
  }

  for (int i=0; i<pNptX*pNptY; ++i)
  {
    if (g.pDataPtr[i] != g.pMissing)
    {
      pDataPtr[i] = 1.0 - g.pDataPtr[i];
    }
    else
    {
      pDataPtr[i] = pMissing;
    }
  }
}

//----------------------------------------------------------------
double GridData::pLocalAverage(const int ix, const int iy, 
			       const int sx, const int sy,
			       const bool isExclude,
			       const double excludeValue,
			       const bool rejectCenterExclude) const
{
  if (isExclude && rejectCenterExclude)
  {
    if (isEqualAt(ix, iy, excludeValue))
    {
      return excludeValue;
    }
  }

  double vt=0;
  double n=0;
  bool hasExclude = false;
  for (int y=iy-sy; y<=iy+sy; ++y)
  {
    for (int x=ix-sx; x<=ix+sx; ++x)
    {
      pLocalAverage1(x, y, isExclude, excludeValue, hasExclude, vt, n);
    }
  }

  if (n==0)
  {
    if (isExclude && !rejectCenterExclude && hasExclude)
    {
      return excludeValue;
    }
    else
    {
      return pMissing;
    }
  }
  else
  {
    return vt/n;
  }
}

//----------------------------------------------------------------
double GridData::pLocalHighAverage(const int ix, const int iy, 
				   const int sx, const int sy,
				   const bool isExclude,
				   const double excludeValue) const
{
  double minV;
  if (!getValue(ix, iy, minV))
  {
    return pMissing;
  }
  if (isExclude)
  {
    if (isEqualAt(ix, iy, excludeValue))
    {
      return excludeValue;
    }
  }
  double vt=0;
  double n=0;
  for (int y=iy-sy; y<=iy+sy; ++y)
  {
    for (int x=ix-sx; x<=ix+sx; ++x)
    {
      pLocalHighAverage1(x, y, isExclude, excludeValue, minV, vt, n);
    }
  }

  if (n==0)
  {
    return pMissing;
  }
  else
  {
    return vt/n;
  }
}

//----------------------------------------------------------------
double GridData::pLocalMax(const int ix, const int iy, const int sx, 
			   const int sy, const bool isAbsoluteMax) const
{
  double max=0;
  bool first = true;
  bool isPos=false;

  for (int y=iy-sy; y<=iy+sy; ++y)
  {
    if (y < 0 || y >= pNptY)
    {
      continue;
    }
    for (int x=ix-sx; x<=ix+sx; ++x)
    {
      if (x < 0 || x >= pNptX)
      {
	continue;
      }
      double v;
      if (getValue(x, y, v))
      {
	double vi;
	bool posi;
	sSetSigned(isAbsoluteMax, v, vi, posi);
	if (first)
	{
	  first = false;
	  max = vi;
	  isPos = posi;
	}
	else
	{
	  if (vi > max)
	  {
	    max = vi;
	    isPos = posi;
	  }
	}
      }
    }
  }
  if (first)
  {
    return pMissing;
  }
  else
  {
    if (isPos)
    {
      return max;
    }
    else
    {
      return -max;
    }
  }
}

//----------------------------------------------------------------
double GridData::pBilinear(const int ry0, const int rx0, const int res,
			   const int y, const int x,
			   const GridData &lowres)
  const
{
  int x0 = rx0*res;
  int y0 = ry0*res;
  int x1 = x0 + res;
  int y1 = y0 + res;
  double f00=0, f01=0, f10=0, f11=0;
  int rx1 = rx0 + 1;
  int ry1 = ry0 + 1;
  if (lowres.inRange(rx0, ry0))
  {
    if (!lowres.getValue(rx0, ry0, f00))
    {
      f00 = 0;
    }
  }
  if (lowres.inRange(rx0, ry1))
  {
    if (!lowres.getValue(rx0, ry1, f01))
    {
      f01 = 0;
    }
  }
  if (lowres.inRange(rx1, ry0))
  {
    if (!lowres.getValue(rx1, ry0, f10))
    {
      f10 = 0;
    }
  }
  if (lowres.inRange(rx1, ry1))
  {
    if (!lowres.getValue(rx1, ry1, f11))
    {
      f11 = 0;
    }
  }

  double ret = (f00*(x1-x)*(y1-y) +
		f10*(x-x0)*(y1-y) +
		f01*(x1-x)*(y-y0) +
		f11*(x-x0)*(y-y0));
  ret = ret/((x1-x0)*(y1-y0));
  return ret;
}

//----------------------------------------------------------------
bool GridData::pMaxValueNotEqualAtRadius(const int x, const int y,
					 const double excludeValue,
					 const int radius,
					 double &maxValue) const
{
  bool gotMissing = false;
  bool gotV = false;
  double maxv;

  // look at points along the horizontal box boundary below
  for (int ix=x-radius; ix <= x+radius; ++ix)
  {
    pMaxValueNotEqualAtRadius1(ix, y-radius, excludeValue, gotMissing, gotV,
			       maxv);
  }

  // look at points along the horizontal box boundary above
  for (int ix=x-radius; ix <= x+radius; ++ix)
  {
    pMaxValueNotEqualAtRadius1(ix, y+radius, excludeValue, gotMissing, gotV,
			       maxv);
  }

  // look at points along the vertical box boundary to left
  for (int iy=y-radius; iy <= y+radius; ++iy)
  {
    pMaxValueNotEqualAtRadius1(x-radius, iy, excludeValue, gotMissing, gotV,
			       maxv);
  }

  // look at points along the vertical box boundary to right
  for (int iy=y-radius; iy <= y+radius; ++iy)
  {
    pMaxValueNotEqualAtRadius1(x+radius, iy, excludeValue, gotMissing, gotV,
			       maxv);
  }
  if (gotV)
  {
    maxValue = maxv;
    return true;
  }
  else
  {
    if (gotMissing)
    {
      maxValue = pMissing;
      return true;
    }
    else
    {
      return false;
    }
  }
}

//----------------------------------------------------------------
void GridData::pMaxValueNotEqualAtRadius1(const int x, const int y,
					  const double excludeValue,
					  bool &gotMissing,
					  bool &gotV, double &maxV) const
{
  if (x < 0 || x >= pNptX || y < 0 || y >= pNptY)
  {
    return;
  }
  ILOGF(DEBUG_VERBOSE, "checking %d, %d", x, y);
  double vi = pDataPtr[y*pNptX + x];
  if (vi == excludeValue)
  {
    return;
  }
  if (vi == pMissing)
  {
    gotMissing = true;
  }
  else
  {
    if (!gotV)
    {
      gotV = true;
      maxV = vi;
    }
    else
    {
      if (vi > maxV)
      {
        maxV = vi;
      }
    }
  }
}

//----------------------------------------------------------------
void GridData::pLocalAverage1(const int x, const int y, const bool isExclude,
			      const double excludeValue, bool &hasExclude, 
			      double &sumValue, double &numValue) const
{
  if (y < 0 || y >= pNptY || x < 0 || x >= pNptX)
  {
    return;
  }
  double v;
  if (getValue(x, y, v))
  {
    if (isExclude)
    {
      if (v == excludeValue)
      {
	hasExclude = true;
      }
      else
      {
	sumValue += v;
	numValue++;
      }
    }
    else
    {
      sumValue += v;
      numValue++;
    }
  }
}

//----------------------------------------------------------------
void GridData::pLocalHighAverage1(const int x, const int y,
				  const bool isExclude,
				  const double excludeValue,
				  const double minV, double &sumValue,
				  double &numValue) const
{
  if (y < 0 || y >= pNptY || x < 0 || x >= pNptX)
  {
    return;
  }
  double v;
  if (getValue(x, y, v))
  {
    if (v >= minV)
    {
      if ((isExclude && v != excludeValue) || !isExclude)
      {
	sumValue += v;
	numValue++;
      }
    }
  }
}

//----------------------------------------------------------------
bool GridData::pSearchNbhd(const int i, const int j, const int searchScale, 
			   const double maxRadius, const ParmProjection &proj,
			   double &minDist,  double &closeVal) const
{
  //
  // Loop through nearby points of box of length to 2*pParams.maxRadius
  // finding closest point that is not missing or bad
  //
  int r = searchScale; 

  minDist = maxRadius;

  double xDist = static_cast<double>(r) * proj.kmPerPixel();

  bool distFound = false;

  while(!distFound && xDist < maxRadius)
  {
    //
    // Search horizontal edges of box surrounding point (i,j)
    // with box lengths 2r
    //
    for (int y = j - r; y <= j + r; y = y + 2*r )
    { 
      for (int x = i - r; x <= i + r; x+=searchScale)
      {
	if (inRange(x, y) && !isMissingAt(x, y))
	{ 
	  //
	  // Record distance and value at (x,y)
	  //
	  minDist = xDist;
	  
	  getValue(x, y, closeVal);
	  
	  distFound = true;
	  
	  //
	  // Stop looping
	  //
	  x = pNptX;
	  
	  y = pNptY;
	  
	} //end if		
      }//end for
    }//end for
    
    if (!distFound)
    {
      //
      // Search vertical edges of box surrounding point (i,j)
      // with box lengths 2r
      // 
      for ( int x = i-r ; x <= i + r ; x = x + 2*r)
      {
	for (int y = j - r + searchScale; y <= j + r -searchScale; 
	     y+=searchScale)
	{
	  if (inRange(x,y) && !isMissingAt(x, y))
	  {
	    //
	    // Record distance and value at (x,y)
	    //
	    minDist = xDist;
	    
	    getValue(x,y,closeVal);
	    
	    distFound = true;
	    
	    //
	    // Stop looping
	    //
	    x = pNptX;
	    
	    y = pNptY;	    
	    
	  }// end if
	}//end for
      }//end for

      r+=searchScale;

      //
      // Convert distances to km from number of pixels
      //
      xDist = static_cast<double>(r) * proj.kmPerPixel();
    }// End if
  }// End while

  return distFound;
}

