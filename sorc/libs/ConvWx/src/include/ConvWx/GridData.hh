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
 * @file GridData.hh
 * @brief  A simple 2 dimensional data grid
 * @class GridData
 * @brief  A simple 2 dimensional data grid
 *
 * A grid has dimensions nx, ny, the two dimensional data, and a missing data
 * value.
 *
 * @note all data arrays are stored one dimensional with the assumed 2d order
 * in which the data at a grid point x,y is stored into data[y*nx + x].
 * The total number of grid points is nx*ny.
 *
 */

# ifndef    GRIDDATA_HH
# define    GRIDDATA_HH

#include <string>
#include <cstdio>
#include <vector>
#include <ConvWx/ParmProjection.hh>

class ConvWxMultiThreadMgr;
class ConvWxFuzzy;

//----------------------------------------------------------------
class GridData
{
public:

  //////////////////////////////////////////////////////////////////////
  /**
   * @name Constructors/Destructors/Copy
   *
   * @{
   */

  /**
   * All empty constructor
   */
  GridData(void);

  /**
   * Constructor that sets particular data values into the grid
   *
   * @param[in] data An array of values with assumed GridData 2 dimensional
   *                 ordering
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] missing  The data missing value
   *
   * @note assume the length of data is nx*ny
   */
  GridData(const double *data, const int nx, const int ny,
	   const double missing);
  
  /**
   * Constructor that sets particular data values into the grid, vector
   *
   * @param[in] data A vector of values with assumed GridData 2 dimensional
   *                 ordering
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] missing  The data missing value
   *
   * @note assume the length of data is nx*ny
   */
  GridData(const std::vector<double> &data, const int nx, const int ny,
	   const double missing);

   /**
   * Constructor that sets particular data values into the grid
   *
   * @param[in] data An array of values with assumed GridData 2 dimensional
   *                 ordering
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] nz  Z number of points
   * @param[in] missing  The data missing value
   *
   * @note assume the length of data is nx*ny
   */
  GridData(const double *data, const int nx, const int ny, const int nz,
	     const double missing);

  /**
   * Constructor that sets all data values to missing
   *
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] missing  The data missing value
   */
  GridData(const int nx, const int ny, const double missing);
  
  /**
   * Constructor that sets all data values to missing
   *
   * @param[in] nx  X number of points
   * @param[in] ny  Y number of points
   * @param[in] nz  Z number of points
   * @param[in] missing  The data missing value
   */
  GridData(const int nx, const int ny, const int nz, const double missing);
  

  /**
   * Copy constructor, shallow copy does not work
   *
   * @param[in] g  Object to copy in
   */
  GridData(const GridData &g);
  
  /**
   * Destructor
   */
  virtual ~GridData(void);

  /**
   * Operator=, shallow operator= does not work
   *
   * @param[in] g  Object to copy in
   *
   * @return reference to local object
   */
  GridData & operator=(const GridData &g);

  /**
   * Copy data from a grid into the local grid
   *
   * @param[in] g  GridData to copy from
   * @return  True if successful, false if grids have unequal pNptX, pNptY
   *
   * @note If grids are unequally sized, the local grid is unchanged.
   * @note Changes the missing data value to the input value
   */
  bool copyValues(const GridData &g);

  /**
   * Copy data contents of the local grid to a float array
   *
   * @param[in,out] data  A float array of length ndata
   * @param[in] ndata  Length of data 
   *
   * @return  True if ndata agrees with local state, and then 
   *          data[i] = (float)pDataPtr[i] for all i.  
   *          False if ndata is not consistent with local state.
   */
  bool copyFloat(float *data, const int ndata) const;

  /**
   * Copy data contents of the local grid to a float array, replacing any
   * non-finite data with the missing data value
   *
   * @param[in,out] data  A float array of length ndata
   * @param[in] fieldName  Name of field being processed
   * @param[in] ndata  Length of data 
   *
   * @return  True if ndata agrees with local state, and then 
   *          data[i] = (float)pDataPtr[i] for all i.  
   *          False if ndata is not consistent with local state.
   */
  bool copyFloatFilterNans(float *data, const std::string &fieldName,
			   const int ndata) const;


  /** @} */


  //////////////////////////////////////////////////////////////////////
  /**
   * @name Comparisons
   *
   * @{
   */

  /**
   * Operator==, compares all grid values as well as the other members
   *
   * @param[in] g  Object to compare
   *
   * @return true for equality
   */
  bool operator==(const GridData &g) const;

  /**
   * Check two grids to see if all the values are equal
   * 
   * @return  True if grids have same pNptX, pNptY and the same set of 
   *          non-missing grid points, each having the same values
   *
   * @param[in] g  GridData to compare against
   */
  bool valuesEqual(const GridData &g) const;

  /**
   * Check two grids to see if their dimensions are the same.
   *
   * @param[in] g  GridData to compare against
   *
   * @return  True if local nx,ny equals nx,ny from the input grid
   */
  bool sizeEqual(const GridData &g) const;

  /**
   * @return  True if data is missing at a point
   *
   * @param[in] x  X Index
   * @param[in] y  Y Index
   * @param[in] z  Z Index
   */
  bool isMissingAt(const int x, const int y, const int z = 0) const;

  /**
   * @return True if data is missing at a point
   *
   * @param[in] ipt  One dimensional grid index
   */
  bool isMissingAt(const int ipt) const;

  /**
   * @return true if data at a point has a value
   *
   * @param[in] value  Value to check against
   * @param[in] ipt  One dimensional index
   */
  bool isEqualAt(const int ipt, const double value) const;

  /**
   * @return true if data at a point is nearly equal to a value
   *
   * @param[in] ipt  One dimensional index
   * @param[in] value  Value to check against
   * @param[in] tolerance  Allowed error
   */
  bool isNearlyEqualAt(const int ipt, const double value,
		       double tolerance) const;

  /**
   * @return true if data at a point has a value
   *
   * @param[in] value  Value to check against
   * @param[in] x  X Index
   * @param[in] y  Y Index
   */
  bool isEqualAt(const int x, const int y, const double value) const;

   /**
   * @return true if data at a point has a value
   *
   * @param[in] value  Value to check against
   * @param[in] x  X Index
   * @param[in] y  Y Index
   * @param[in] z  Z Index
   */
  bool isEqualAt(const int x, const int y,const int z, 
		 const double value) const;


  /**
   * @return true if data at a point is less or equal a value
   *
   * @param[in] value  Value to check against
   * @param[in] ipt  One dimensional index
   */
  bool isLeAt(const int ipt, const double value) const;

  /**
   * @return true if data at a point is less or equal a value
   *
   * @param[in] value  Value to check against
   * @param[in] x  X Index
   * @param[in] y  Y Index
   */
  bool isLeAt(const int x, const int y, const double value) const;

 /**
   * @return true if data at a point is less or equal a value
   *
   * @param[in] value  Value to check against
   * @param[in] x  X Index
   * @param[in] y  Y Index
   * @param[in] z  Z Index
   */
  bool isLeAt(const int x, const int y, const int z, const double value) const; 
  
  /**
   * @return true if data at a point is greater than a value
   *
   * @param[in] value  Value to check against
   * @param[in] ipt  One dimensional index
   */
  bool isGtAt(const int ipt, const double value) const;

  /**
   * @return true if data at a point is greater than a value
   *
   * @param[in] value  Value to check against
   * @param[in] x  X Index
   * @param[in] y  Y Index
   */
  bool isGtAt(const int x, const int y, const double value) const;

  /**
   * @return true if data at a point is greater than a value
   *
   * @param[in] value  Value to check against
   * @param[in] x  X Index
   * @param[in] y  Y Index
   * @param[in] z  Z Index
   */
  bool isGtAt(const int x, const int y, const int z, const double value) const;


  /**
   * Check if indices are valid (in range of the grid dimensions)
   *
   * @param[in] x  X Index
   * @param[in] y  Y Index
   * @return true if indices are in range. (both x and y within limits)
   */
  inline bool inRange(const int x, const int y) const
  {
    return (x >= 0 && x < pNptX && y >= 0 && y < pNptY);
  }

  /**
   * Check if indices are valid (in range of the grid dimensions)
   *
   * @param[in] x  X Index
   * @param[in] y  Y Index
   * @param[in] z  Z Index
   * @return true if indices are in range. (x, y and z all within limits)
   */
  inline bool inRange(const int x, const int y, const int z  ) const
  {
    return (x >= 0 && x < pNptX && y >= 0 && y < pNptY &&
	    z >= 0 && z < pNptZ);
  }

  /**
   * Check if an x value is out of the valid range 0 to pNptX-1
   *
   * @param[in] x  X Index into data
   * @return  True if x is out of the valid range
   */
  inline bool outOfRangeX(const int x) const { return x<0 || x>=pNptX;}

  /**
   * Check if a y value is out of the valid range 0 to pNptY-1
   *
   * @param[in] y  Y Index into data
   * @return true if y is out of the valid range
   */
  inline bool outOfRangeY(const int y) const { return y<0 || y>=pNptY;}

  /**
   * Check if a z value is out of the valid range 0 to pNptZ-1
   *
   * @param[in] z  Z Index into data
   * @return true if z is out of the valid range
   */
  inline bool outOfRangeZ(const int z) const { return z<0 || z>=pNptZ;}

  /**
   * @return true if data is missing at all grid points 
   */
  bool allMissing(void) const;
   
  /**
   * @return true if all grid points have either the missing value or a
   *         particular value at all grid points
   *
   * @param[in] value  The one value to check for
   *
   * @note This method returns true only when the grid has exactly one
   *       non-missing value (or is all missing) 
   */
  bool allDataMissingOrEqual(const double value) const;

  /** @} */


  //////////////////////////////////////////////////////////////////////
  /**
   * @name Simple accessors
   */

  /**
   * @return  The missing value
   */
  inline double getMissing(void) const {return pMissing;}

  /**
   * Retrieve the value at a point if it is not missing
   *
   * @param[in] x  X Index
   * @param[in] y  Y index
   * @param[out] value  Returned data value at x,y
   * @return  False if data is missing at x,y, true otherwise.
   */
  bool getValue(const int x, const int y, double &value) const;

  /**
   * Retrieve the value at a point if it is not missing
   *
   * @param[in] x  X Index
   * @param[in] y  Y index
   * @param[in] z  Z index 
   * @param[out] value  Returned data value at x,y,z
   * @return  False if data is missing at x,y,z true otherwise.
   */
  bool getValue(const int x, const int y,  const int z, double &value) const;

  /**
   * Retrieve the value at a point if it is not missing
   *
   * @param[in] ipt  One dimensional index
   * @param[out] value  Returned data value at index
   * @return  False if data is missing at ipt, true otherwise
   */
  bool getValue(const int ipt, double &value) const;

  /**
   * Retrieve the value at a grid point
   *
   * @param[in] ipt  One dimensional index
   * @return  Data value at this index, can be the missing value
   */
  inline double returnValue(const int ipt) const
  {
    if (ipt < 0 || ipt >= pNptTotal)
    {
      printf("ERROR in returnValue %d [0,%d]\n", ipt, pNptTotal-1);
    }
    return pDataPtr[ipt];
  }

  /**
   * Retrieve the value at a grid point
   *
   * @param[in] x  X Index
   * @param[in] y  Y index
   * @param[in] z  Z index
   * @return  Data value at x,y, can be the missing value
   */
  inline double returnValue(const int x, const int y, const int z = 0) const
  {
    return pDataPtr[pIpt(x, y, z)];
  }


  /**
   * @return  Number of data points in the grid
   */
  inline int getNdata(void) const {return pNptTotal;}

  /**
   * @return Number of points in the grid at which the data value is missing
   */
  int nmissing(void) const;

  /**
   * @return  The grid dimensions
   *
   * @param[out] nx  Grid x dimension
   * @param[out] ny  Grid y dimension
   */
  inline void getDim(int &nx, int &ny) const
  {
    nx = pNptX;
    ny = pNptY;
  }

  /**
   * @return  The grid dimensions
   *
   * @param[out] nx  Grid x dimension
   * @param[out] ny  Grid y dimension
   * @param[out] nz  Grid z dimension
   */
  inline void getDim(int &nx, int &ny, int &nz) const
  {
    nx = pNptX;
    ny = pNptY;
    nz = pNptZ;
  }

  /**
   * @return  The grid dimension
   * @param[out] nx  Grid x dimension
   */
  inline int getNx(void) const { return  pNptX; }

  /**
   * @return  The grid dimension
   * @param[out] ny  Grid y dimension
   */
  inline int getNy(void) const { return  pNptY; }

  /**
   * @return  The grid dimension
   * @param[out] nz  Grid z dimension
   */
 inline int getNz(void) const { return  pNptZ; }

  /**
   * @return  Number of data points in the grid equal to a particular value
   * @param[in] value  Value to check against
   */
  int getNdataEqualTo(const double value) const;

  /**
   * @return  The percentage of points in the grid with values that
   *          are greater or equal a particular value
   *          
   * @param[in] value  Value to compare with
   */
  double pcntGe(const double value) const;

  /**
   * Compute the mean data value, excluding missing data points
   *          
   * @param[out] value  Mean Value
   * @return true if at least one point is non-missing
   */
  bool mean(double &value) const;

  /**
   * Compute the mean data value over a grid subset,
   * excluding missing data points
   *          
   * @param[in] x0  Minimum x index
   * @param[in] y0  Minimum y index
   * @param[in] nx  Number of x indices
   * @param[in] ny  Number of y indices
   * @param[in] xWrap  True to allow wraparound in X 
   * @param[in] yWrap  True to allow wraparound in Y 
   * @param[out] value  Mean Value
   * @param[out] outOfBounds  set True of the window goes out of bounds,
   *                          and there is not wraparound
   * @return true if at least one point is non-missing and subset is 
   *         entirely in range, false otherwise
   */
  bool meanSubset(int x0, int y0, int nx, int ny, bool xWrap,
		  bool yWrap, double &value, bool &outOfBounds) const;

  /**
   * Compute the percent of data above a threshold over a grid subset
   *          
   * @param[in] threshold  The threshold
   * @param[in] x0  Minimum x index
   * @param[in] y0  Minimum y index
   * @param[in] nx  Number of x indices
   * @param[in] ny  Number of y indices
   * @param[in] xWrap  True to allow wraparound in X 
   * @param[in] yWrap  True to allow wraparound in Y 
   * @param[out] value percentage returned
   * @param[out] outOfBounds  set True of the window goes out of bounds,
   *                          and there is not wraparound
   * @return true if at least one point is non-missing and subset is 
   *         entirely in range, false otherwise
   */
  bool percentAboveThresholdSubset(double threshold,
				   int x0, int y0, int nx, int ny, bool xWrap,
				   bool yWrap, double &value,
				   bool &outOfBounds) const;

  /**
   * Return the range of data values
   *
   * @param[out] minv  The minimum data value found in grid
   * @param[out] maxv  The maximum data value found in grid
   *
   * @return false if the grid contains only missing data.
   */
  bool getRange(double &minv, double &maxv) const;

  /**
   * Return one dimensional index associated with the two or three dimensional indices
   *
   * @param[in] x  Index into grid (2d) x
   * @param[in] y  Index into grid (2d) y
   * @param[in] z  Index into grid (3d) z
   *
   * @return One dimensional index value
   */
  inline int index(const int x, const int y, const int z = 0) const 
  {return pIpt(x,y,z); }

  /**
   * Return the two dimensional x index value at a particular one dimensional
   * index
   *
   * @param[in] ipt  Index into the grid (1d)
   *
   * @return X index for input
   */
  inline int xAtIndex(const int ipt) const {return pXpt(ipt); }

  /**
   * Return the two dimensional y index value at a particular one dimensional
   * index
   *
   * @param[in] ipt  Index into the grid (1d)
   *
   * @return Y index for input
   */
  inline int yAtIndex(const int ipt) const {return pYpt(ipt); }

  /**
   * Return one dimensional index at an offset from a two dimensional index
   *
   * @param[in] x  Index into grid (2d) x
   * @param[in] y  Index into grid (2d) y
   * @param[in] offset  An integer offset
   * @return  The one dimensional index 
   */
  inline int offsetIndex(const int x, const int y, const int offset) const
  {
    return pIpt(x,y,0) + offset;
  }

  /**
   * @return x index of input index, with wraparound for out of range
   * values
   * @param[in] x  An index into the x array dimension
   */
  inline int wraparoundX(int x) const
  {
    int ret = x;
    while (ret < 0)
    {
      ret += pNptX;
    }
    while (ret >= pNptX)
    {
      ret -= pNptX;
    }
    return ret;
  }

  /**
   * @return y index of input index, with wraparound for out of range
   * values
   * @param[in] y  An index into the x array dimension
   */
  inline int wraparoundY(int y) const
  {
    int ret = y;
    while (ret < 0)
    {
      ret += pNptY;
    }
    while (ret >= pNptY)
    {
      ret -= pNptY;
    }
    return ret;
  }


  /**
   * Return values for x0 = x - dx, x1 = x + dx, constrained to the range 
   * 0 to pNptX -1. 
   * @param[in] x  Center point
   * @param[in] dx  Value to subtract and add to get the range
   * @param[out] x0  Maximum of x - dx, 0
   * @param[out] x1  Minimum of x + dx, pNptX - 1  
   */
  void getConstrainedLimitsX(const int x, const int dx, int &x0, int &x1) const;

  /**
   * Return values for y0 = y - dy, y1 = y + dy, constrained to the range 
   * 0 to pNptY -1. 
   * @param[in] y  Center point
   * @param[in] dy  Value to subtract and add to get the range
   * @param[out] y0  Maximum of y - dy, 0
   * @param[out] y1  Minimum of y + dy, pNptY - 1  
   */
  void getConstrainedLimitsY(const int y, const int dy, int &y0, int &y1) const;

  /** @} */


  //////////////////////////////////////////////////////////////////////
  /**
   * @name Simple Actions
   *
   * @{
   */

  //////////////////////////////////////////////////////////////////////

  /**
   * Set the value at a point
   *
   * @param[in] i  Index into grid (one dimensional)
   * @param[in] value  Value to set at the point
   */
  inline void setv(const int i, const double value)
  {
    if (i < 0 || i >= pNptTotal)
    {
      printf("ERROR in setv %d [0,%d]\n", i, pNptTotal-1);
    }
    else
    {
      pDataPtr[i] = value;
    }
  }

  /**
   * Set a value at a point
   *
   * @param[in] x  Index into data x (two dimensional)
   * @param[in] y  Index into data y (two dimensional)
   * @param[in] value  Value to set at the grid point
   */
  inline void setv(const int x, const int y, const double value)
  {
    pDataPtr[pIpt(x, y, 0)] = value;
  }

  
  /**
   * Set a value at a point
   *
   * @param[in] x  Index into data x (two dimensional)
   * @param[in] y  Index into data y (two dimensional)
   * @param[in] z  Index into data y (two dimensional)
   * @param[in] value  Value to set at the grid point
   */
  inline void setv(const int x, const int y, const int z, const double value)
  {
    pDataPtr[pIpt(x, y, z)] = value;
  }

  /**
   * Set the value at a point to the missing value
   *
   * @param[in] x  X Index 
   * @param[in] y  Y Index 
   * @param[in] z  Z Index
   */
  void setToMissing(const int x, const int y, const int z = 0);

  /**
   * Set a value at a point to the missing value 
   *
   * @param[in] ipt  One dimensional index
   */
  void setToMissing(const int ipt);

  /**
   * Set the value at all points to the missing value
   */
  void setAllMissing(void);

  /**
   * Set the value at all points to the input value
   *
   * @param[in] value  The value to fill the grid with
   */
  void setAllToValue(const double value);

  /**
   * Set the value at all points where value = vOld to a new value vNew
   *
   * @param[in] vOld  Data value to replace in the grid
   * @param[in] vNew  Data value to replace with in the grid
   */
  void changeValue(const double vOld, const double vNew);

  /**
   * Change the missing data value
   *
   * @param[in] missingValue  New missing data value to use
   *
   * Change all grid points which have the old missing value to this new value.
   */
  void changeMissing(const double missingValue);

  /** @} */



  //////////////////////////////////////////////////////////////////////
  /**
   * @name Log/print
   *
   * @{
   */

  /**
   * @return  A one line description of the GridData as a string
   */
  std::string sprintData(void) const;

  /**
   * Log all the data values to a file
   *
   * @param[in] fname  Name of the file to log values to
   * @note this is typically used for debugging only
   */
  void logData(const std::string &fname) const;

  /**
   * Log the data values in a range of indices to stdout
   *
   * @param[in] ipt0  Lower index value (one dimensional)
   * @param[in] ipt1  Upper index value (one dimensional)
   *
   * @note  This is typically used for debugging only.
   */
  void logValues(const int ipt0, const int ipt1) const;

  /**
   * Log the values in a range of indices to stdout
   *
   * @param[in] ix0  Lower x index value
   * @param[in] ix1  Upper x index value
   * @param[in] iy0  Lower y index value
   * @param[in] iy1  Upper y index value
   *
   * @note  This is typically used for debugging only.
   */
  void logValues(const int ix0, const int ix1, const int iy0,
		 const int iy1) const;

  /** @} */


  //////////////////////////////////////////////////////////////////////
  /**
   * @name Major filters
   *
   * @{
   */

  /**
   * Apply a sx by sy smoothing (averaging) filter to the local grid
   *
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   * @param[in] rejectCenterExclude  True to suppress smoothing when
   *                                 the center point = excludeValue
   *                                 (isExclude = true only)
   *            
   * On exit  The data at x,y is the average of all data in the box bounded
   *          by [x-sx,y-sy],  [x+sx,y+xy]
   *
   *
   * With excluding of points (isExclude=true), two modes are possible. In the
   * rejectCenterExclude=TRUE mode, the smoothed value at points where the
   * data is equal to excludeValue is the excludeValue.  At all other points
   * the smoothed value is equal to the average of all non-excludeValue
   * points in the box.
   *
   * In the rejectCenterExclude=FALSE mode, if a box has only excludeValue
   * data in it, the smoothed value is equal to the excludeValue, otherwise
   * it is equal to the average of all non-excludeValue point in the box.
   * 
   */
  void smooth(int sx, int sy, bool isExclude = false, double excludeValue=0.0,
	      bool rejectCenterExclude=true);

  /**
   * Apply a sx by sy smoothing (averaging) filter to the local grid
   * This is a fast algorithm implementation of the GridData::smooth() technique
   *
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   * @param[in] rejectCenterExclude  True to suppress smoothing when
   *                                 the center point = excludeValue
   *                                 (isExclude = true only)
   *            
   * On exit  The data at x,y is the average of all data in the box bounded
   *          by [x-sx,y-sy],  [x+sx,y+xy]
   *
   *
   * With excluding of points (isExclude=true), two modes are possible. In the
   * rejectCenterExclude=TRUE mode, the smoothed value at points where the
   * data is equal to excludeValue is the excludeValue.  At all other points
   * the smoothed value is equal to the average of all non-excludeValue
   * points in the box.
   *
   * In the rejectCenterExclude=FALSE mode, if a box has only excludeValue
   * data in it, the smoothed value is equal to the excludeValue, otherwise
   * it is equal to the average of all non-excludeValue point in the box.
   * 
   */
  void fastsmooth(int sx, int sy, bool isExclude = false, double excludeValue=0.0,
		  bool rejectCenterExclude=true);

  /**
   * Apply a sx by sy smoothing (averaging) filter to the local grid,
   * but only at points where a mask grid has a particular value.
   *
   * @param[in] mask  A grid where presumably some grid points have
   *                  value=maskValue
   * @param[in] maskValue  The mask value 
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   * @param[in] isExclude  True to exclude a value from the smoothing
   * @param[in] excludeValue  Value to exclude from smoothing when
   *                          isExclude = true
   *
   * @return  False if the mask and local grid don't have the same dimensions,
   *          True otherwise
   */
  bool smoothInMask(const GridData &mask, const double maskValue, const int sx,
		    const int sy, const bool isExclude=false,
		    const double excludeValue=0.0);

  /**
   * Apply a sx by sy high smoothing filter to the local grid
   *
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   *            
   * On exit   the data at x,y is the 'high' average of all data in the box
   *           bounded by [x-sx,y-sy],  [x+sx,y+xy]
   *
   * High smoothing is the same as regular smoothing except for:
   *
   * At a point x,y, the smoothing is only over points xi,yi in the box for
   * which data[xi,yi] >= data[x,y].
   * If all the points in the box centered at x,y have values less than
   * data[x,y], the filter has no effect at that point.
   *
   * When points are excluded (isExclude=true), the smoothed value at points
   * where the data is equal to excludeValue is the excludeValue.
   * At all other points the smoothed value is equal to the high average of all
   * the data except for the excludeValue, which is not added to the high
   * average.
   */
  void highSmooth(const int sx, const int sy, const bool isExclude = false,
		  const double excludeValue=0.0);

  /**
   * Apply a sx by sy high smoothing (averaging) filter to the local grid,
   * but only at points where a mask grid has a particular value.
   *
   * @param[in] mask  A grid where presumably some grid points have
   *                  value=maskValue
   * @param[in] maskValue  The mask value
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   * @param[in] isExclude  True to exclude a value from the smoothing
   * @param[in] excludeValue  Value to exclude when isExclude = true
   *
   * @return  False if the mask and local grid don't have the same dimensions,
   *          True otherwise
   *
   * High smoothing has the same meaning as in the GridData::highSmooth() method
   */
  bool highSmoothInMask(const GridData &mask, const double maskValue,
			const int sx, const int sy,
			const bool isExclude=false,
			const double excludeValue=0.0);

  /**
   * Apply a sx by sy maximum filter to the local grid
   *
   * @param[in] sx  Box size x
   * @param[in] sy  Box size y
   * @param[in] isAbsoluteMax  True if the maximum is of the absolute values,
   *                           false if max is simply the largest signed number.
   *
   * @note when isAbsoluteMax=true, the filter result at a point is the
   *       SIGNED value 
   *       for which the absolute value was maximum in the box.
   *
   * On exit   the data at x,y is the max of all data in the box bounded
   *           by [x-sx,y-sy],  [x+sx,y+xy]
   */
  void max(const int sx, const int sy, const bool isAbsoluteMax);

  /**
   * Apply a sx by xy smoothing (averaging) filter, but only at points
   * where data is missing.  This fills missing data 'gaps' with the
   * average of nearby data.
   *
   * @param[in] sx  Box size x
   * @param[in] sy  Box size y
   */
  void fillGaps(const int sx, const int sy);

  /**
   * Multiply local grid by and input grid, point wise
   *
   * @param[in] g  Grid to use for point by point multiplication
   * 
   * @return true for success 
   *
   * @note the multiplication only happens at grid points where both the
   * local value and the input value are not missing
   */
  bool multiply(const GridData &g);

  /**
   * Divide local grid by and input grid, point wise
   *
   * @param[in] g  Grid to use for point by point multiplication
   * 
   * @return true for success, return false if input grid has zero values 
   *
   * @note the divions only happens at grid points where both the
   * local value and the input value are not missing, 
   */
  bool divide(const GridData &g);

  /**
   * Take the square root of each value in grid 
   *
   * @return true for success, false if any elements of grid are negative
   *
   * @note if the data a point is missing, it remains missing
   */
  bool squareRoot(void);

  /**
   * Calculate sin(x) for every grid value x 
   *
   * @return true for success
   *
   * @note if the data a point is missing, it remains missing
   */
  bool sine(void);

  /**
   * Calculate cos(x) for every grid value x 
   *
   * @return true for success
   *
   * @note if the data a point is missing, it remains missing
   */
  bool cosine(void);
  
  /**
   * Calculate asin(x) for every grid value x 
   *
   * @return true for success
   *
   * @note if the data a point is missing, it remains missing
   */
  bool arcsine(void);

  /**
   * Calculate atan(x) for every grid value x 
   *
   * @return true for success
   *
   * @note if the data a point is missing, it remains missing
   */
  bool arctan(void);
  
  /**
   * Add contents of the input grid to local grid, point wise
   *
   * @param[in] g  Grid to use for point by point adding
   * @return true for success
   *
   * @note If a local point has missing data, and the input value at that point
   *       is not missing, the local point is given the input value.
   */
  bool add(const GridData &g);

  /**
   * Subtract contents of the input grid from local grid, point wise
   *
   * @param[in] g  Grid to use for point by point subtracting
   * @return true for success
   *
   * @note If a local point has missing data, the output local stays missing.
   *       If input is missing, the local point is unchanged.
   */
  bool subtract(const GridData &g);

  /**
   * Add constant to each element of the input grid
   * 
   * @param[in] val  Value to add every element of the input grid
   * @return true for success
   * 
   * @note If a local point has missing data, and the input value at that point
   *       is not missing, the local point is given the input value.
   */
  bool add(const double &val);

  /**
   * Identify the point close to another point at which the grid value is
   * maximum
   *
   * @param[in] centerX  Center point x     
   * @param[in] centerY  Center point y
   * @param[in] sx  Box radius x
   * @param[in] sy  Box radius y
   * @param[out] x  Returned x index where maximum was found
   * @param[out] y  Returned y index where maximum was found
   *
   * @return False If all points in the box around the point are missing.
   *
   * The box to look at is bounded by [centerX-sx,centerY-sy] 
   * [centerX+sx, centerY+sy]
   */
  bool nearbyMax(const int centerX, const int centerY, const int sx, 
		 const int sy, int &x, int &y) const;

  /**
   * Reduce resolution of the local grid by a factor 
   *
   * @param[in] res  Reduction, 1 = no resolution reduction, 2 = 
   *                 resolution reduction by a factor of 2, and so on.
   *
   * @return  True if successful, false if some problem was encountered,
   *          and in that case no change is made to the grid.
   *          Problems include res < 2 or res so high that nothing is left.
   *
   * @note this method subsamples the original grid
   */
  bool reduce(const int res);

  /**
   * Interpolate a grid from low to higher resolution, with results put into
   * the local grid. Uses a bilinear interpolation method.
   *
   * @param[in] lowres  The low resolution grid to interpolate
   * @param[in] res  Resolution factor, 1 = no change, 2 = increase resolution
   *                 by a factor of two, etc.
   *
   * @return  True if the interpolation was successful,
   *          false if inputs were not as expected.
   *
   * @note  Expect (local nx) = (input nx)*res, (local ny)=(input ny)*res.
   *        An error occurs it this is not true.
   */
  bool interpolate(const GridData &lowres, const int res);

  /**
   * Interpolate a grid from low to higher resolution, with results put into
   * the local grid. Uses a bilinear interpolation method.
   *
   * @param[in] lowres  The low resolution grid to interpolate
   * @param[in] res  Resolution factor, 1 = no change, 2 = increase resolution
   *                 by a factor of two, etc.
   * @param[out] status  True if the interpolation was successful,
   *                     false if inputs were not as expected.
   *
   * @note  Expect (local nx) = (input nx)*res, (local ny)=(input ny)*res.
   *        An error occurs it this is not true, and status is set to false.
   */
  void interpolate(const GridData &lowres, const int res, bool &status);

  /**
   * Multiply the data at a particular grid point by a value, unless the data
   * at that point is missing.
   *
   * @param[in] x  Index into data x
   * @param[in] y  Index into data y
   *
   * @param[in] value  Value to multiply by
   *
   * on exit, data[x,y] = data[x,y]*value
   */
  void multiply(const int x, const int y, const double value);

  /**
   * Multiply the entire grid by a particular value, except at points where
   * data is missing
   *
   * @param[in] value  Value to multiply by
   *
   * on exit:
   *  data[x,y,z] = data[x,y,z]*value for all x,y,z where data[x,y,z] != missing
   */
  void multiply(const double value);

  /**
   * Increment the data value at a particular point by a value, unless
   * the data at that point is missing.
   *
   * @param[in] value  Value to add
   * @param[in] ipt  One dimensional grid index
   *
   * on exit:
   *  data[ipt] = data[ipt] + value (except when data[ipt] is missing)
   */
  void incrementValueAtPoint(const int ipt, const double value);

  /**
   * Increment the data value at a particular point by a value, unless
   * the data at that point is missing.
   *
   * @param[in] x  Index into data x
   * @param[in] y  Index into data y
   * @param[in] value  Value to add
   *
   * on exit:
   *  data[x,y] = data[x,y] + value (except when data[x,y] is missing)
   */
  void incrementValueAtPoint(const int x, const int y, const double value);

  /**
   * Replace all occurrences of a value in the grid with another one
   *
   * @param[in] oldv  Old value
   * @param[in] newv  Replacement value
   *
   * @return  Number of points at which the value was changed
   */
  int replace(const double oldv, const double newv);

  /**
   * Replace all occurrences of a value in a grid with a nearest non missing
   * value. If there is more than one nearest point with non missing value,
   * replace with the maximum of these nearest values.
   * If no non missing values are found, replace the value with the missing
   * value.
   *
   * @param[in] value  The value in the grid at which to do this replacement
   */
  void replaceValueWithNearestMax(const double value);

  /**
   * Apply a fuzzy function to the data value each grid point 
   *
   * @param[in] f  Mapping from one set of values to another.
   *
   * @note The function is not applied where data is missing
   */
  void applyFuzzyF(const ConvWxFuzzy &f);

  /**
   * Set data to missing at all points where a mask grid has missing data
   *
   * @param[in] mask  The grid to use as a mask
   *
   * @return True if masking was successful
   */
  bool maskMissingToMissing(const GridData &mask);

  /**
   * Set data to a value at all points where a mask grid has missing data
   *
   * @param[in] mask  The mask grid
   * @param[in] replacev  Replacement value
   * @param[in] overwriteMissing  If true, do the replacement even when
   *                              the original value is missing. If false,
   *                              don't overwrite in this case.
   * @return true if successful
   */
  bool maskMissingToValue(const GridData &mask, const double replacev,
			  const bool overwriteMissing);

  /**
   * Set data to a value at all points where a mask grid has missing data
   * or is less or equal a threshold (i.e. the mask data is 'low or missing')
   *
   * @param[in] mask  The mask grid
   * @param[in] maskThresh  Threshold to use
   * @param[in] replacev   Replacement value in the local grid
   * @param[in] overwriteMissing  If true, do the replacement even when
   *                              the original value is missing. If false,
   *                              don't overwrite in this case.
   *
   * @return true if the masking was successful
   */
  bool maskThreshOrMissingToValue(const GridData &mask, const double maskThresh,
				  const double replacev,
				  const bool overwriteMissing);

  /**
   * Set data at points where below a threshold in a mask field to missing
   * 
   * @param[in] maskGrid  The mask grid
   * @param[in] maskThresh  Threshold to use
   * 
   */
  bool maskBelowThreshToMissing(const GridData &maskGrid, 
				const double maskThresh);

  /**
   * Set data at points where above a threshold in a mask field to missing
   * 
   * @param[in] maskGrid  The mask grid
   * @param[in] maskThresh  Threshold to use
   * 
   */
  bool maskAboveThreshToMissing(const GridData &maskGrid, 
				const double maskThresh);

  /**
   * Return the minimum value at all points where a mask grid has a certain
   * value 
   *
   * @param[in] mask  The mask Grid
   * @param[in] maskValue  The value to look for in mask
   * @param[out] minv  The minimum value found locally
   * @return  True if successful
   */
  bool minValueInMask(const GridData &mask, const double maskValue,
		      double &minv) const;

  /**
   * Return the maximum value at all points where a mask grid does not have
   * missing data
   *
   * @param[in] mask  The mask grid
   * @param[out] maxX  The returned location of the maximum value in the
   *                   local grid
   * @param[out] maxY  The returned location of the maximum value in the
   *                   local grid
   * @param[out] maxValue  The maximum value found in the local grid
   *
   * @return true if successful
   */
  bool maxInMask(const GridData &mask, int &maxX, int &maxY,
		 double &maxValue) const;

  /**
   * Set data to a value at all points where a mask grid has a particular value
   *
   * @param[in] mask  The mask grid
   * @param[in] maskValue  The value to look for 
   * @param[in] value  Value to put into local grid
   * @param[in] maskTolerance  Allowed diff between mask data and maskValue
   *                           to consider it a match, default = 0
   * @return true if successful
   */
  bool setValueInMask(const GridData &mask, const double maskValue,
		      const double value, double maskTolerance = 0.0);

  /**
   * Set data to a value at all points where locally data is missing and the
   * input mask grid is not missing
   *
   * @param[in] mask  Mask grid
   * @param[in] value  Value to put into local grid when conditions are met
   *
   * @return true for success
   */
  bool setMissingAndMaskNonMissingToValue(const GridData &mask,
					  const double value);

  /**
   * Set data to the minimum of the local value and the value from
   * input grid, point wise
   *
   * @param[in] inp  Grid
   *
   * @return true for success
   * 
   * At each point i, the local value is set to
   *  minimum(pDataPtr[i],inp.pDataPtr[i])
   */
  bool minimum(const GridData &inp);

  /**
   * Set data to the maximum of the local value and the value from
   * input grid, point wise
   *
   * @param[in] inp  Grid
   *
   * @return true for success
   * 
   * At each point i, the local value is set to
   *  maximum(pDataPtr[i],inp.pDataPtr[i])
   */
  bool maximum(const GridData &inp);

  /**
   * Set data to a value whenever the data is within a range
   *
   * @param[in] v0  Lower data value in the range
   * @param[in] v1  Higher data value in the range
   * @param[in] newValue  Replacement data value
   */
  void setDataInRangeToValue(const double v0, const double v1,
			     const double newValue);

  /** @} */

  /**
   * Return one dimensional grid index value associated with two dimensional
   * indices, for a grid with a particular x dimension
   *
   * @param[in] x  Index into grid x
   * @param[in] y  Index into grid y
   * @param[in] nx  Grid dimension x
   * @return one dimensional index value
   */
  static int gridIndex(const int x, const int y, const int nx);

  /**
   * Return one dimensional grid index value associated with two or three dimensional
   * indices, for a grid with a particular x and y dimension
   *
   * @param[in] x  Index into grid x
   * @param[in] y  Index into grid y
   * @param[in] z  Index into grid z
   * @param[in] nx  Grid dimension x
   * @param[in] ny  Grid dimension y
   * @return one dimensional index value
   */
  static int gridIndex(const int x, const int y, const int z, 
		       const int nx, const int ny);

  /**
   * Compute the derivative of with respect to the x coordinate at each
   * point in the grid. It is assumed that grid spacing is in kilometers
   *
   * @param[in] halfDeltaX  Difference quotient is formed with grid
   *                        vales at x + halfDeltaX and x - halfDeltaX
   *                        and a divisor of distance in kilometers
   *                        between (x - halfDeltaX,  x + halfDeltaX)
   * @param[in] proj  ParmProjection object necessary for finding
   *                     the distance between grid points
   * @param[in] outGrid  Output grid containing the derivative with 
   *                     respect to x at each point.
   * @return true if derivatives are successfully calculated and otherwise false
   */
  bool ddx( const int halfDeltaX, const ParmProjection  &proj, 
	    GridData &outGrid) const;
 
  /**
   * Compute the derivative of with respect to the y coordinate at each
   * point in the grid. It is assumed that grid spacing is in kilometers
   *
   * @param[in] halfDeltaY  Difference quotient is formed with grid
   *                        vales at y + halfDeltaY and y - halfDeltaY
   *                        and a divisor of distance in kilometers
   *                        between (y - halfDeltaY,  y + halfDeltaY)
   * @param[in] proj  ParmProjection object necessary for finding
   *                     the distance between grid points
   * @param[in] outGrid  Output grid containing the derivative with 
   *                     respect to y at each point.
   * @return true if derivatives are successfully calculated and otherwise false
   */
  bool ddy( const int halfDeltaY, const ParmProjection  &proj, 
	    GridData &outGrid) const;
 
  /**
   * Compute distance to non-missing data and put the distance values
   * into an output grid, along with the closest value
   * @param[in] searchScale  The search region for non missing data in a 
   *                         neighborhood about a grid point is increased 
   *                         or extended by this number of grid points 
   *                         if search fails.
   * @param[in] maxRadius  Maximum radius in kilometers within which to look
   *                       for non missing data around missing data point
   * @param[in] proj  Projection parameters used to convert from grid to
   *                  kilometers
   * @param[out] distOut  Distance values at each gridpoint (kilometers)
   * @param[out] valOut  For each grid point the value at the closest point
   *                     with non-missing data
   *
   * @return true if successfully able to do the computations
   */
  bool distanceToNonMissing(const int searchScale,
			    const double maxRadius,
			    const ParmProjection &proj,
			    GridData &distOut, GridData &valOut) const;
  // bool distanceToNonMissingNew(const int searchScale,
  // 			       const double maxRadius,
  // 			       const ParmProjection &proj,
  // 			       GridData &distOut,
  // 			       GridData &valOut) const;


  /**
   * Set all data that is not missing to the missing value, and set all
   * data that is missing to a non-missing value
   */
  void invertMissing(void);

  /**
   * @return the square difference in values at the two points
   *  local(x0,yo)  input(x0+xoff, y0+yoff)  or missing if out of
   *  range or data is missing at either point.  If data values are below
   *  thresh they are set to 0 in the comparison
   *
   * @param[in] x0  Index, local data
   * @param[in] y0  Index, local data
   * @param[in] xoff  Index, input data
   * @param[in] yoff  Index, input data
   * @param[in] input  The input data
   * @param[in] thresh   The threshold
   */
  double diffSq(int x0, int y0, int xoff, int yoff, const GridData &input,
		double thresh) const;

  /**
   * Modify local grid so at each point the value is <= the input grid
   * @param[in] g  Grid that must be >= local grid everywhere
   */
  void monotonicDecreasing(const GridData &g);
  
  /**
   * Modify local grid so at each point the value is >= the input grid
   * @param[in] g  Grid that must be < local grid everywhere
   */
  void monotonicIncreasing(const GridData &g);
  
  /**
   *  At each point set data value to 1 - the value from g
   * @param[in] g   Grid to use
   */
  void oneMinus(const GridData &g);

protected:

  double *pDataPtr;   /**< data pointer */
  int pNptTotal;       /**< number of data grid points */
  int pNptX;        /**< number of x dimension data grid points */
  int pNptY;        /**< number of y dimension data grid points */
  int pNptZ;        /**< number of z dimension data grid points */
  double pMissing;/**< data missing value */

private:  

  /**
   * @return One dimensional index from the two or three dimensional indices
   *
   * @param[in] ix  Index x 
   * @param[in] iy  Index y 
   * @param[in] iz  Index z
   */
  inline int pIpt(int ix, int iy, int iz = 0 ) const
  {
    if (ix <0 || ix >= pNptX || iy < 0 || iy >= pNptY ||
	iz <0 || iz>= pNptZ)
    {
      printf("ERROR in ipt (%d,%d,%d) [0,%d] [0,%d]  [0,%d]\n",
	     ix, iy, iz, pNptX, pNptY, pNptZ);
      return 0;
    }
    return  iz*pNptX*pNptY + iy*pNptX + ix;
  }

  /**
   * @return Three dimensional z index for one dimensional index
   * @param[in] ipt  One dimensional index
   */
  inline int pZpt(const int ipt) const 
  {
    return (ipt)/(pNptX * pNptY);
  }


  /**
   * @return Two dimensional y index for one dimensional index
   * @param[in] ipt  One dimensional index
   */
  inline int pYpt(const int ipt) const 
  { 
    return (ipt - pZpt(ipt) * pNptX * pNptY)/pNptX;
  }

  /**
   * @return Two dimensional x index for one dimensional index
   * @param[in] ipt  One dimensional index
   */
  inline int pXpt(const int ipt) const
  {
    return ipt -  (pZpt(ipt) * pNptX * pNptY) - (pYpt(ipt) * pNptX);
  }

  /**
   * Apply a sx by sy smoothing (averaging) filter to the local grid
   * at a point
   *
   * @param[in] ix  Center x
   * @param[in] iy  Center y
   * @param[in] sx  Box radius x
   * @param[in] sy  Box radius y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   * @param[in] rejectCenterExclude  True to suppress smoothing when
   *                                 the center point = excludeValue 
   *                                 (isExclude = true only).
   *            
   * @return the average of all data in the box bounded by
   * [ix-sx,iy-sy],[ix+sx,iy+xy]
   *
   * called by GridData::smooth()
   * 
   */
  double pLocalAverage(const int ix, const int iy, const int sx, const int sy,
		       const bool isExclude = false,
		       const double excludeValue=0.0,
		       const bool rejectCenterExclude=true) const;

  /**
   * Apply a sx by sy high smoothing (averaging) filter to the local grid
   * at a point
   *
   * @param[in] ix  Center x
   * @param[in] iy  Center y
   * @param[in] sx  Box radius x
   * @param[in] sy  Box radius y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   *            
   * @return the average of all data in the box bounded by
   * [ix-sx,iy-sy],[ix+sx,iy+xy]
   *
   *
   * called by GridData::highSmooth()
   */
  double pLocalHighAverage(const int ix, const int iy, const int sx,
			   const int sy, const bool isExclude = false,
			   const double excludeValue=0.0) const;

  /**
   * Return the maximum value found in a box around grid point
   *
   * @param[in] ix  Center point x
   * @param[in] iy  Center point x
   * @param[in] sx  Box size x
   * @param[in] sy  Box size y
   * @param[in] isAbsoluteMax  True if the maximum is of the absolute values,
   *                           false if max is simply the largest signed number.
   *
   * @return the maximum
   */
  double pLocalMax(const int ix, const int iy, const int sx, 
		   const int sy, const bool isAbsoluteMax) const;


  /**
   * Do bilinear interpolation from low to high resolution at a point
   *
   * @param[in] ry0  Low resolution point in the input grid
   * @param[in] rx0  Low resolution point in the input grid
   * @param[in] res  Factor to convert from low to high resolution 1, 2, ..
   * @param[in] y   High resolution output point (in the local grid)
   * @param[in] x   High resolution output point (in the local grid)
   * @param[in] lowres  The grid with low resolution data
   *
   * @return interpolated value for position x, y in the local grid
   */
  double pBilinear(const int ry0, const int rx0, const int res, const int y,
		   const int x, const GridData &lowres) const;

  /**
   * Return maximum value not equal to a particular value at a fixed distance
   * from a particular point. 
   *
   * @param[in] x  The point x
   * @param[in] y  The point point y
   * @param[in] excludeValue  Value to exclude from the maximum
   * @param[in] radius  number of grid points from the point 1, 2, 3, ..
   * @param[out] maxValue The returned maximum value
   *
   * @return true if there is any data at this distance that is not missing
   */
  bool pMaxValueNotEqualAtRadius(const int x, const int y,
				 const double excludeValue,
				 const int radius, double &maxValue) const;

  /**
   * Adjust output arguments based on the value at a particular point.
   *
   * @param[in] x  Point x
   * @param[in] y  Point y
   * @param[in] excludeValue  If this value is found at x, y, no change is
   *                          made to the output arguments
   * @param[out] gotMissing  Set to true if data is missing at x,y
   * @param[in,out] gotV  True if maxV has been set
   * @param[in,out] maxV  Maximum value, adjusted by this method if the
   *                      value at x,y exceeds entry value, or if gotV is
   *                      false.  If adjusted by this method, gotV is set to
   *                      true.
   */
  void pMaxValueNotEqualAtRadius1(const int x, const int y,
				  const double excludeValue,
				  bool &gotMissing, bool &gotV,
				  double &maxV) const;

  /**
   * Update running averages at a point for a smoothing filter
   *
   * @param[in] x  Point x
   * @param[in] y  Point y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   * @param[out] hasExclude  Set to true if isExclude=true and the value found
   *                         at x,y is the excludeValue
   * @param[in,out] sumValue  Running sum of values, adjusted by this method
   * @param[in,out] numValue  Running count of values, adjusted by this method
   */
  void pLocalAverage1(const int x, const int y, const bool isExclude,
		      const double excludeValue, bool &hasExclude, 
		      double &sumValue, double &numValue) const;

  /**
   * Update running averages at a point for a high smoothing filter
   *
   * @param[in] x  Point x
   * @param[in] y  Point y
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   * @param[in] minV  The center value for the average. For high filtering,
   *                  this is the minimum allowed value for averaging.
   * @param[in,out] sumValue  Running sum of values, adjusted by this method
   * @param[in,out] numValue  Running count of values, adjusted by this method
   */
  void pLocalHighAverage1(const int x, const int y, const bool isExclude,
			  const double excludeValue, const double minV,
			  double &sumValue, double &numValue) const;


  /**
   * Search the neighborhood of grid point with coordinates (x,y) for non 
   * missing data. The search is conducted along edges of a square with center 
   * (x,y). If a valid data value is found, the distance assigned is the
   * shortest distance to an edge of the search region. If a valid data value 
   * is not found, then the shortest distance to the edge of the search 
   * region is increased by 'searchScale' grid points. A widening
   * search may continue until a user defined maximum distance is reached and 
   * the maximum distance is assigned to the grid point.
   * @param[in] i  X coordinate of grid point
   * @param[in] j  Y coordinate of grid point
   * @param[in] searchScale  Region for non missing data in a 
   *                         neighborhood about a grid point is increased 
   *                         or extended by this integer number of grid points 
   *                         if search fails. 
   * @param[in] maxRadius  Maximum radius in kilometers within which to look
   *                       for non missing data around missing data point
   * @param[in] proj  Projection parameters used to convert from grid to
   *                  kilometers
   * @param[out] minDist  Minimum distance to valid data from grid point (i,j)
   * @param[out] closeVal  Data value at point of minimum distance. This allows
   *                       for the filling of missing data with a closest 
   *                       neighborhood value if available.
   * @return TRUE is returned to indicate data was found and closeVal has been
   *         set with a valid value. FALSE is returned to indicate valid data
   *         was not found and minDist was set to the user defined maximum
   *         search distance.
   */
  bool pSearchNbhd(const int i, const int j, const int searchScale, 
		   const double maxRadius, const ParmProjection &proj,
		   double &minDist,  double &closeVal) const;

};

# endif     // GRIDDATA_HH 
