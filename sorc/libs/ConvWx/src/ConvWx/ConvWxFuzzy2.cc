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
 * @file ConvWxFuzzy2.cc
 */

#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <ConvWx/ConvWxFuzzy2.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWxIO/ILogMsg.hh>
using std::vector;
using std::string;

/**
 * set the bad value
 */
const double ConvWxFuzzy2::pBadValue = -999.999;

//----------------------------------------------------------------
/**
 * Add input x to a vector, preserving increasing order
 * @param[in] v  Values in ascending order
 * @param[in] x  New value
 */
static void sAdd(vector<double> &v, const double x)
{
  vector<double>::iterator i;
  i = find(v.begin(), v.end(), x);
  if (i == v.end())
  {
    for (i = v.begin(); i!=v.end(); ++i)
    {
      if (x < *i)
      {
	// insert here
	v.insert(i, x);
	return;
      }
    }
    v.push_back(x);
  }
}
//----------------------------------------------------------------
/**
 * sets ix0, ix1 for which v[ix0] <= x < v[ix1].
 * assumes v is non-decreasing ordered.
 * if x < v[0] return ix0=ix1=0.
 * if x > v[last] return ix0 = ix1 = last.
 * if nothing to work with return ix0 = ix1 = BAD_INDEX, and false.
 *
 * @return true if successful
 * @param[in] x  Value to compare against
 * @param[in] v  Values to search
 * @param[out] ix0  Value that meets criteria
 * @param[out] ix1  Value that meets criteria
 */
static bool sIndex(const double x, const vector<double> &v, int &ix0, int &ix1)
{
  vector<double>::const_iterator i;
  int j;

  if (v.empty())
  {
    ix0 = ix1 = convWx::BAD_INDEX;
    return false;
  }
  for (j=0,i=v.begin(); i != v.end(); ++i,++j)
  {
    if (x < *i)
    {
      // first time this has been true, so must belong between
      // the previous and this index, unless j = 0 in which case
      // no previous..
      ix1 = j;
      if (j == 0)
      {
	ix0 = j;
      }
      else
      {
	ix0 = j-1;
      }
      return true;
    }
  }

  // off the upper end, i.e. x > v[last].
  ix0 = v.size() - 1;
  ix1 = ix0;
  return true;
}

//----------------------------------------------------------------
/**
 * Set ix so that v[ix] = x
 * @return true if successful
 * @param[in] x  Value to check against
 * @param[in] v  Values to search
 * @param[out] ix 
 */
static bool sExactIndex(const double x, const vector<double> &v, int &ix)
{
  vector<double>::const_iterator i;

  i = find(v.begin(), v.end(), x);
  if (i == v.end())
  {
    return false;
  }
  else
  {
    ix = i-v.begin();
    return true;
  }
}

//----------------------------------------------------------------
/**
 * Figure out what the index to insert newv is in v, returning that index
 * The index is the one before the first thing that is bigger than newv.
 * If newv is bigger than anything, return the size of v.
 * @param[in] v  Values to search
 * @param[in] newv  New value
 * @return the index
 */
static int sNewIndex(const vector<double> &v, double newv)
{
  int j;
  vector<double>::const_iterator i;

  for (j=0,i=v.begin(); i != v.end(); ++i, ++j)
  {
    if (newv < *i)
    {
      // first time this has been true, so insert before here.
      return j;
    }
  }
  return v.size();
}

//----------------------------------------------------------------
/**
 * Interpolate in two dimensions, returning the interpolated value
 * @param[in] xx  first dimension lower and upper value
 * @param[in] yy  second dimension lower and upper value
 * @param[in] v   values at the 4 extreme points
 * @param[in] x  Interpolation point
 * @param[in] y  Interpolation point
 * @return interpolated value
 */
static double sInterpolate(const double xx[2], const double yy[2],
			   const double v[2][2], const double x, const double y)
{
  double c0, c1, c2, c3;
  double dx, dy;

  dx = xx[1] - xx[0];
  dy = yy[1] - yy[0];

  c0 = v[0][0];
  if (fabs(dy) < convWx::EPSILON)
  {
    c2 = 0.0;
  }
  else
  {
    c2 = (v[0][1] - c0)/dy;
  }
  if (fabs(dx) < convWx::EPSILON)
  {
    c1 = 0.0;
  }
  else
  {
    c1 = (v[1][0] - c0)/dx;
  }
  if (fabs(dx) < convWx::EPSILON || fabs(dy) < convWx::EPSILON)
  {
    c3 = 0.0;
  }
  else
  {
    c3 = (v[1][1] - c0 - c1*dx - c2*dy)/(dx*dy);
  }
    
  return c0 + c1*(x-xx[0]) + c2*(y-yy[0]) + c3*(x-xx[0])*(y-yy[0]);
}
  
//----------------------------------------------------------------
ConvWxFuzzy2::ConvWxFuzzy2(void) :
pZ(),
  pX(),
  pY(),
  pXName("x"),
  pYName("y")
{
}

//----------------------------------------------------------------
ConvWxFuzzy2::ConvWxFuzzy2(const string &xname, const string &yname) :
pZ(),
  pX(),
  pY(),
  pXName(xname),
  pYName(yname)
{
}

//----------------------------------------------------------------
ConvWxFuzzy2::ConvWxFuzzy2(const string &xname, const string &yname,
			   const vector<Fuzzy2_t> &points) :
pZ(),
  pX(),
  pY(),
  pXName(xname),
  pYName(yname)
{
  // find all x's and y's put in in order
  for (int i=0; i<static_cast<int>(points.size()); ++i)
  {
    double x = points[i].x;
    double y = points[i].y;
    sAdd(pX, x);
    sAdd(pY, y);
  }

  // now go back through and build up each x with initially bad values
  vector<double> bads;
  for (int i=0; i<static_cast<int>(pY.size()); ++i)
  {
    bads.push_back(badValue());
  }
  for (int i=0; i<static_cast<int>(pX.size()); ++i)
  {
    pZ.push_back(bads);
  }

  // now fill in non-bads
  for (int i=0; i<static_cast<int>(points.size()); ++i)
  {
    pFillPoint(points[i].x, points[i].y, points[i].z);
  }

  // now expand out to completely fill the grid I hope.
  for (int x=0; x<static_cast<int>(pX.size()); ++x)
  {
    pExpandRow(pZ[x]);
  }
}

//----------------------------------------------------------------
ConvWxFuzzy2::~ConvWxFuzzy2()
{
  pX.clear();
  pY.clear();
  pZ.clear();
}

//----------------------------------------------------------------
void ConvWxFuzzy2::print(void) const
{
  vector<double>::const_iterator ix, iy, i;
  vector< vector<double> >::const_iterator iz;

  for (iz=pZ.begin(), ix=pX.begin(); iz!=pZ.end() && ix != pX.end();
       ++iz,++ix)
  {
    printf("%s=%f:\n", pXName.c_str(), *ix);
    for (iy=pY.begin(), i=iz->begin(); iy!=pY.end() && i!=iz->end();
	 ++iy,++i)
    {
      printf("\t[%s,v]=[%f,%f]\n", pYName.c_str(), *iy, *i);
    }
  }
}

//----------------------------------------------------------------
bool ConvWxFuzzy2::xRange(double &x0, double &x1) const
{
  if (pX.empty())
  {
    return false;
  }
  else
  {
    x0 = *(pX.begin());
    x1 = *(pX.end()-1);
    return true;
  }
}

//----------------------------------------------------------------
bool ConvWxFuzzy2::yRange(double &y0, double &y1) const
{
  if (pY.empty())
  {
    return false;
  }
  else
  {
    y0 = *(pY.begin());
    y1 = *(pY.end()-1);
    return true;
  }
}

//----------------------------------------------------------------
double ConvWxFuzzy2::apply(double x, double y) const
{
  int ix0, ix1, iy0, iy1;
  double v[2][2], xx[2], yy[2];

  // find proper x and y indices, which define 4 corners of a 
  // box in x and y.
  if (!sIndex(x, pX, ix0, ix1))
  {
    return badValue();
  }
  if (!sIndex(y, pY, iy0, iy1))
  {
    return badValue();
  }

  // Define the box.  Sometimes the x or y dimension is collapsed
  // to a single point in which case you get either a line in x
  // or a line in y, or a single point.
  xx[0] = pX[ix0];
  xx[1] = pX[ix1];
  yy[0] = pY[iy0];
  yy[1] = pY[iy1];

  // now lookup pZ at each box corner to get the 3rd dimension.
  v[0][0] = pZ[ix0][iy0];
  v[0][1] = pZ[ix0][iy1];
  v[1][0] = pZ[ix1][iy0];
  v[1][1] = pZ[ix1][iy1];

  // finally, interpolate to get the answer.
  return sInterpolate(xx, yy, v, x, y);
}

//----------------------------------------------------------------
void ConvWxFuzzy2::addColumn(const double x, const double *y, const double *v,
			     const int n)
{
  vector<double>::iterator i;

  // add in y values as needed.
  pUpdateExistingY(y, n);

  // replace or add in x value.
  if ((i=find(pX.begin(), pX.end(), x)) != pX.end())
  {
    pReplaceAtX(i - pX.begin(), v, n);
  }
  else
  {
    pAddAtX(x, v, n);
  }
}

//----------------------------------------------------------------
void ConvWxFuzzy2::addColumn(const double x, const vector<double> &y, 
			     const vector<double> &v)
{
  vector<double>::iterator i;

  // add in y values as needed.
  pUpdateExistingY(y);

  // replace or add in x value.
  if ((i=find(pX.begin(), pX.end(), x)) != pX.end())
  {
    pReplaceAtX(i - pX.begin(), v);
  }
  else
  {
    pAddAtX(x, v);
  }
}

//----------------------------------------------------------------
void ConvWxFuzzy2::addRow(const double y, const double *x, const double *v,
			  const int n)
{
  vector<double>::iterator i;

  // add in x values as needed.
  pUpdateExistingX(x, n);

  // replace or add y value.
  if ((i=find(pY.begin(), pY.end(), y)) != pY.end())
  {
    pReplaceAtY(i - pY.begin(), v, n);
  }
  else
  {
    pAddAtY(y, v, n);
  }
}

//----------------------------------------------------------------
// add in input y array, if needed.
void ConvWxFuzzy2::pUpdateExistingY(const double *y, const int n)
{
  int i;
  vector<double>::iterator ii;

  for (i=0; i<n; ++i)
  {
    // is y[i] already in place?
    if ((ii=find(pY.begin(), pY.end(), y[i])) == pY.end())
    {
      // no insert it.
      pInsertEmptyY(y[i]);
    }
  }
}

//----------------------------------------------------------------
// add in input y array, if needed.
void ConvWxFuzzy2::pUpdateExistingY(const vector<double> &y)
{
  int i;
  vector<double>::iterator ii;

  for (i=0; i<static_cast<int>(y.size()); ++i)
  {
    // is y[i] already in place?
    if ((ii=find(pY.begin(), pY.end(), y[i])) == pY.end())
    {
      // no insert it.
      pInsertEmptyY(y[i]);
    }
  }
}

//----------------------------------------------------------------
// add in input x array, if needed.
void ConvWxFuzzy2::pUpdateExistingX(const double *x, const int n)
{
  int i;
  vector<double>::iterator ii;

  for (i=0; i<n; ++i)
  {
    // is x[i] already in place?
    if ((ii=find(pX.begin(), pX.end(), x[i])) == pX.end())
    {
      // no, insert it.
      pInsertEmptyX(x[i]);
    }
  }
}

//----------------------------------------------------------------
// add in input x array, if needed.
void ConvWxFuzzy2::pUpdateExistingX(const vector<double> &x)
{
  int i;
  vector<double>::iterator ii;

  for (i=0; i<static_cast<int>(x.size()); ++i)
  {
    // is x[i] already in place?
    if ((ii=find(pX.begin(), pX.end(), x[i])) == pX.end())
    {
      // no, insert it.
      pInsertEmptyX(x[i]);
    }
  }
}

//----------------------------------------------------------------
// insert the input Y value.
void ConvWxFuzzy2::pInsertEmptyY(const double y)
{
  vector< vector<double> >::iterator v;
  int j;

  // determine where to insert:
  j = sNewIndex(pY, y);
  if (j < static_cast<int>(pY.size()))
  {
    pY.insert(pY.begin()+j, y);
  }
  else
  {
    pY.push_back(y);
  }

  // now go through pZ.  Since pZ[ix] = vector of values for each y,
  // want to insert a missing value for this new y at each such ix in
  // its proper place (j'th).
  for (v=pZ.begin(); v != pZ.end(); ++v)
  {
    if (j < static_cast<int>(v->size()))
    {
      v->insert(v->begin()+j, badValue());
    }
    else
    {
      v->push_back(badValue());
    }
  }
}

//----------------------------------------------------------------
// insert input x value.
void ConvWxFuzzy2::pInsertEmptyX(const double x)
{
  vector<double>::iterator ix;
  int j;
  vector<double> d;

  // pZ[ix] = vector of values for each y.  Want to insert a new
  // vector of missing values into pZ for some new ix, and
  // a new value of x into the vector pX
  
  // create the empty new vector d of missing values, one for each y:
  for (ix=pY.begin(); ix != pY.end(); ++ix)
  {
    d.push_back(badValue());
  }

  // figure out where to insert this vector of missing values.
  // the pX vector will tell us where to insert or append to pX
  j = sNewIndex(pX, x);
  if (j < static_cast<int>(pX.size()))
  {
    // not at end
    pX.insert(pX.begin()+j, x);
    pZ.insert(pZ.begin()+j, d);
  }
  else
  {
    //at end
    pX.push_back(x);
    pZ.push_back(d);
  }
}

//----------------------------------------------------------------
// replace pZ[xIndex] with v,n
void ConvWxFuzzy2::pReplaceAtX(const int xIndex, const double *v, const int n)
{
  vector< vector<double> >::iterator i;
  vector<double>::iterator d;
  int j;

  // the vector pZ[xIndex] is where the new values go.
  i = pZ.begin() + xIndex;
  for (j=0,d = i->begin(); d!= i->end(); ++d,++j)
  {
    *d = v[j];
  }
}

//----------------------------------------------------------------
// for each xi, replace pZ[xi][y] with v[i]
void ConvWxFuzzy2::pReplaceAtY(const int yIndex, const double *v, const int n)
{
  vector< vector<double> >::iterator i;
  int j;

  for (j=0,i=pZ.begin(); i != pZ.end(); ++i,++j)
  {
    // replace the yIndex'th element:
    *(i->begin() + yIndex) = v[j];
  }
}

//----------------------------------------------------------------
// replace pZ[xIndex] with v,n
void ConvWxFuzzy2::pReplaceAtX(const int xIndex, const vector<double> &v)
{
  vector< vector<double> >::iterator i;
  vector<double>::iterator d;
  int j;

  // the vector pZ[xIndex] is where the new values go.
  i = pZ.begin() + xIndex;
  for (j=0,d = i->begin(); d!= i->end(); ++d,++j)
  {
    *d = v[j];
  }
}

//----------------------------------------------------------------
// for each xi, replace pZ[xi][y] with v[i]
void ConvWxFuzzy2::pReplaceAtY(const int yIndex, const vector<double> &v)
{
  vector< vector<double> >::iterator i;
  int j;

  for (j=0,i=pZ.begin(); i != pZ.end(); ++i,++j)
  {
    // replace the yIndex'th element:
    *(i->begin() + yIndex) = v[j];
  }
}

//----------------------------------------------------------------
// for input x, put x into array of pX and into _v with input values.
void ConvWxFuzzy2::pAddAtX(const double x, const double *v, const int n)
{
  vector<double> d;
  int j;

  // create the vector of values.
  for (j=0; j<n; ++j)
  {
    d.push_back(v[j]);
  }

  // figure out where to add the x (which index into pZ, pX)
  j = sNewIndex(pX, x);
  if (j < static_cast<int>(pX.size()))
  {
    // add x to pX and pZ before j.
    pX.insert(pX.begin()+j, x);
    pZ.insert(pZ.begin()+j, d);
  }
  else
  {
    // add at the end.
    pX.push_back(x);
    pZ.push_back(d);
  }
}

//----------------------------------------------------------------
// add a new element to each column of pZ, and add one thing to Y.
void ConvWxFuzzy2::pAddAtY(const double y, const double *v, const int n)
{
  vector< vector<double> >::iterator i;
  int j, k;

  // figure out where to insert new values.
  j = sNewIndex(pY, y);

  // put in new y.
  if (j < static_cast<int>(pY.size()))
  {
    // insert the new y:
    pY.insert(pY.begin()+j, y);
  }
  else
  {
    pY.push_back(y);
  }
    
  // add the new element to each vector in pZ.
  for (k=0,i=pZ.begin(); i!= pZ.end(); ++i,++k)
  {
    if (j < static_cast<int>(i->size()))
    {
      i->insert(i->begin()+j, v[k]);
    }
    else
    {
      i->push_back(v[k]);
    }
  }
}

//----------------------------------------------------------------
// for input x, put x into array of pX and into _v with input values.
void ConvWxFuzzy2::pAddAtX(const double x, const vector<double> &v)
{
  vector<double> d;
  int j;

  // create the vector of values.
  for (j=0; j<static_cast<int>(v.size()); ++j)
  {
    d.push_back(v[j]);
  }

  // figure out where to add the x (which index into pZ, pX)
  j = sNewIndex(pX, x);
  if (j < static_cast<int>(pX.size()))
  {
    // add x to pX and pZ before j.
    pX.insert(pX.begin()+j, x);
    pZ.insert(pZ.begin()+j, d);
  }
  else
  {
    // add at the end.
    pX.push_back(x);
    pZ.push_back(d);
  }
}

//----------------------------------------------------------------
// add a new element to each column of pZ, and add one thing to Y.
void ConvWxFuzzy2::pAddAtY(const double y, const vector<double> &v)
{
  vector< vector<double> >::iterator i;
  int j, k;

  // figure out where to insert new values.
  j = sNewIndex(pY, y);

  // put in new y.
  if (j < static_cast<int>(pY.size()))
  {
    // insert the new y:
    pY.insert(pY.begin()+j, y);
  }
  else
  {
    pY.push_back(y);
  }
    
  // add the new element to each vector in pZ.
  for (k=0,i=pZ.begin(); i!= pZ.end(); ++i,++k)
  {
    if (j < static_cast<int>(i->size()))
    {
      i->insert(i->begin()+j, v[k]);
    }
    else
    {
      i->push_back(v[k]);
    }
  }
}

//----------------------------------------------------------------
void ConvWxFuzzy2::pFillPoint(const double x, const double y, const double z)
{
  int ix, iy;
  if (!sExactIndex(x, pX, ix))
  {
    ILOG(FATAL, "SERIOUS logic error in ConvWxFuzzy2 constructor");
    exit(convWx::BAD_EXIT);
  }
  if (!sExactIndex(y, pY, iy))
  {
    ILOG(FATAL, "SERIOUS logic error in ConvWxFuzzy2 constructor");
    exit(convWx::BAD_EXIT);
  }
  pZ[ix][iy] = z;
}

//----------------------------------------------------------------
void ConvWxFuzzy2::pExpandRow(vector<double> &row)
{
  if (row.size() != pY.size())
  {
    ILOG(FATAL, "Logic error in expanding row");
    exit(convWx::BAD_EXIT);
  }
  // find the first one and fill back to 0
  int ind0 = pBackFill(row);

  // find the last one and fill up to end
  int ind1 = pForwardFill(ind0, row);

  // now fill in any gaps between ind0 and ind1 with interpolation
  int gap0, gap1;
  bool inGap = false;
  for (int ind = ind0; ind <= ind1; ++ind)
  {
    pExpandRow1(ind, row, inGap, gap0, gap1);
  }
}

//----------------------------------------------------------------
int ConvWxFuzzy2::pBackFill(vector<double> &row)
{
  int ind0 = convWx::BAD_INDEX;
  for (int i=0; i<static_cast<int>(row.size()); ++i)
  {
    if (row[i] != badValue())
    {
      ind0 = i;
      for (int j=i-1; j>=0; --j)
      {
	row[j] = row[i];
      }
      break;
    }
  }
  if (ind0 == convWx::BAD_INDEX)
  {
    ILOG(FATAL, "ERROR in design, a row with no leading data");
    exit(convWx::BAD_EXIT);
  }
  return ind0;
}

//----------------------------------------------------------------
int ConvWxFuzzy2::pForwardFill(const int ind0, vector<double> &row)
{
  int ind1 = convWx::BAD_INDEX;
  for (int i=static_cast<int>(row.size())-1; i>= ind0; --i)
  {
    if (row[i] != badValue())
    {
      ind1 = i;
      for (int j=i+1; j<static_cast<int>(row.size()); ++j)
      {
	row[j] = row[i];
      }
      break;
    }
  }
  if (ind1 == convWx::BAD_INDEX)
  {
    ILOG(FATAL, "ERROR in design, a row with no trailing data");
    exit(convWx::BAD_EXIT);
  }
  return ind1;
}

//----------------------------------------------------------------
void ConvWxFuzzy2::pExpandRow1(const int ind, vector<double> &row,
			       bool &inGap, int &gap0, int &gap1)
{
  if (row[ind] == badValue())
  {
    if (!inGap)
    {
      inGap = true;
      gap0 = gap1 = ind;
    }
    else
    {
      gap1 = ind;
    }
  }
  else
  {
    if (inGap)
    {
      inGap = false;
      pFillGap(gap0, gap1, row);
    }      
  }
}

//----------------------------------------------------------------
void ConvWxFuzzy2::pFillGap(const int i0, const int i1, vector<double> &r)
{
  // equation of line has 4 points.. r assumed along y direction
  // values = z
  double y0 = pY[i0-1];
  double y1 = pY[i1+1];
  double z0 = r[i0-1];
  double z1 = r[i1+1];
  
  if (fabs(y0 - y1) < convWx::EPSILON)
  {
    ILOG(FATAL, "ERROR fuzzy function is vertical can't interpolate");
    exit(convWx::BAD_EXIT);
  }
  double slope = (z1-z0)/(y1-y0);
  double intercept = z1 - slope*y1;
  
  // now fill in interpolation values
  for (int i=i0; i<=i1; ++i)
  {
    double y = pY[i];
    double z = y*slope + intercept;
    r[i] = z;
  }
}

