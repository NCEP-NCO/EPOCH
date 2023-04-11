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
 * @file  ConvWxFuzzy2.hh
 * @brief  Fuzzy (linear) mapping from two variables to another.
 * @class  ConvWxFuzzy2
 * @brief  Fuzzy (linear) mapping from two variables to another.
 *
 * z = f(x,y) is a 2 variable fuzzy function.  This class implements
 * this kind of function.  It also contains names for the x and y
 * variables.
 */

# ifndef    CONVWX_FUZZY2F_HH
# define    CONVWX_FUZZY2F_HH

#include <vector>
#include <string>

class ConvWxFuzzy2
{
public:

  /**
   * @struct Fuzzy2_t
   * @brief A 3 dimensional point in the fuzzy function space, i.e. z=f(x,y)
   */
  typedef struct
  {
    double x;  /**< x value */
    double y;  /**< y value */
    double z;  /**< f(x,y) */
  } Fuzzy2_t;

  /**
   * Null function.
   *
   * @note sets pXname="X", pYname="Y"
   */
  ConvWxFuzzy2(void);

  /**
   * Null function, with names
   *
   * @param[in] xName  Name for the x variable
   * @param[in] yName  Name for the y variable
   */
  ConvWxFuzzy2(const std::string &xName, const std::string &yName);

  /**
   * Constructed function, with names
   *
   * @param[in] xName  Name for the x variable
   * @param[in] yName  Name for the y variable
   * @param[in] points  The x,y,z points that make up the function
   */
  ConvWxFuzzy2(const std::string &xName, const std::string &yName,
	       const std::vector<Fuzzy2_t> &points);

  /**
   * Destructor
   */
  virtual ~ConvWxFuzzy2(void);

  /**
   * Print out contents
   */
  void print(void) const;

  /**
   * @return The fuzzy function value at x,y  i.e. f(x,y)
   *
   * @note if the function is not set, return badValue()
   *
   * @param[in] x  Point at which to evaluate the function
   * @param[in] y  Point at which to evaluate the function
   */
  double apply(double x, double y) const;

  /**
   * Return the range of x over which the function is defined.
   *
   * @param[out] x0  Minimum x for which f(x,y) is defined
   * @param[out] x1  Maximum x for which f(x,y) is defined
   *
   * @return false if its a null function
   */
  bool xRange(double &x0, double &x1) const;

  /**
   * Return the range of y over which the function is defined.
   *
   * @param[out] y0  Minimum y for which f(x,y) is defined
   * @param[out] y1  Maximum y for which f(x,y) is defined
   *
   * @return false if its a null function
   */
  bool yRange(double &y0, double &y1) const;

  /**
   * Add a column to the fuzzy function, using arrays.
   * A new column is an x value and a set of <y, f(x,y)> values
   *
   * Make it so that f(x, y[i]) = v[i]  at i=0,...,n-1
   *
   * @param[in] x  A particular x value
   * @param[in] y  An array of Y values 0,..,n-1
   * @param[in] v  Corresponding array of function values
   * @param[in] n  Length of the arrays
   */
  void addColumn(const double x, const double *y, const double *v,
		 const int n);

  /**
   * Add a column to the fuzzy function, using vectors.
   * A new column is an x value and a set of <y, f(x,y)> values
   *
   * Make it so that f(x, y[i]) = v[i]  at i=0,...,n-1
   *
   * @param[in] x  A particular x value
   * @param[in] y  A vector of y values
   * @param[in] v  A vector of function values
   */
  void addColumn(const double x, const std::vector<double> &y, 
		 const std::vector<double> &v);

  /**
   * Add a row to the fuzzy function using arrays.
   * A new row is a y value and a set of <x, f(x,y)> values
   *
   * Make it so that f(x[i], y) = v[i], i=0,...,n-1
   *
   * @param[in] y  A particular value
   * @param[in] x  An array of X values 0,..n-1
   * @param[in] v  Corresponding function values
   * @param[in] n  Length of arrays
   */
  void addRow(const double y, const double *x, const double *v, const int n);

  /**
   * Add a row to the fuzzy function using vectors.
   * A new row is a y value and a set of <x, f(x,y)> values
   *
   * Make it so that f(x[i], y) = v[i], i=0,...,n-1
   *
   * @param[in] y  A particular value
   * @param[in] x  Vector of x values
   * @param[in] v  Corresponding function values
   */
  void addRow(const double y, const double *x, const std::vector<double> &v);

  /**
   * @return the local bad data value
   */
  inline static double badValue(void) {return pBadValue;}

  /**
   * @return true if the fuzzy function has not been set to anything meaningful
   */
  inline bool isEmpty(void) const {return pZ.empty();}

protected:
private:  

  /** 
   * The fuzzy bad value
   */
  static const double pBadValue;

  /**
   * The function itself f(pX[i],pY[j]) = (pZ[i])[j]
   * that is to say pZ[x] is a vector through y
   */
  std::vector< std::vector<double> > pZ;  

  /**
   * The vector of x values that have a pZ defined for them
   */
  std::vector<double> pX;

  /**
   * The vector of y values that have a pZ defined for them
   */
  std::vector<double> pY;

  /**
   * The x name
   */
  std::string pXName;

  /**
   * The y name
   */
  std::string pYName;

  /**
   * Add in an x array to the state, if not already there
   *
   * @param[in] x  Array of x values to add
   * @param[in] n  Number of x values
   */
  void pUpdateExistingX(const double *x, const int n);

  /**
   * Add in an x vector to the state, if not already there
   *
   * @param[in] x  Vector of x values to add
   */
  void pUpdateExistingX(const std::vector<double> &x);

  /**
   * Add in a y array to the state, if not already there
   *
   * @param[in] y  Array of y values to add
   * @param[in] n  Number of y values
   */
  void pUpdateExistingY(const double *y, const int n);

  /**
   * Add in a y vector to the state, if not already there
   *
   * @param[in] y  Vector of y values to add
   */
  void pUpdateExistingY(const std::vector<double> &y);

  /**
   * Insert an empty y column into the state
   * This is a column for which all function returned values are badValue()
   *
   * @param[in] y  Value for the y column
   */
  void pInsertEmptyY(const double y);

  /**
   * Insert an empty x row into the state
   * This is a row for which all function returned values are badValue()
   *
   * @param[in] x = value for the x row
   */
  void pInsertEmptyX(const double x);
  
  /**
   * Replace a row in pZ with input values
   *
   * @param[in] xIndex  Index to the row
   * @param[in] v   The values to use
   * @param[in] n   Number of values
   */
  void pReplaceAtX(const int xIndex, const double *v, const int n);

  /**
   * Replace a row in pZ with input values
   *
   * @param[in] xIndex   Index to the row
   * @param[in] v  The values to use
   */
  void pReplaceAtX(const int xIndex, const std::vector<double> &v);

  /**
   * Add a row into pZ
   *
   * @param[in] x  Row x value
   * @param[in] v  Values for that row
   * @param[in] n  Number of z values
   */
  void pAddAtX(const double x, const double *v, const int n);

  /**
   * Add a row into pZ
   *
   * @param[in] x  Row x value
   * @param[in] v  Values for that row
   */
  void pAddAtX(const double x, const std::vector<double> &v);

  /**
   * replace a column in pZ with input values
   *
   * @param[in] yIndex  Index to the column
   * @param[in] v  Values to use
   * @param[in] n  Number of values
   */
  void pReplaceAtY(const int yIndex, const double *v, const int n);

  /**
   * replace a column in pZ with input values
   *
   * @param[in] yIndex  Index to the column
   * @param[in] v  Values to use
   */
  void pReplaceAtY(const int yIndex, const std::vector<double> &v);

  /**
   * Add a column into pZ
   *
   * @param[in] y  Column y value
   * @param[in] v  Values for that column
   * @param[in] n  Number of values
   */
  void pAddAtY(const double y, const double *v, const int n);

  /**
   * Add a column into pZ
   *
   * @param[in] y  Column y value
   * @param[in] v  Values for that column
   */
  void pAddAtY(const double y, const std::vector<double> &v);

  /**
   * Set pZ(x,y) to input
   *
   * @note assumes x and y are part of state
   *
   * @param[in] x  data value (x)
   * @param[in] y  data value (y)
   * @param[in] z  pZ value to put at this location
   */
  void pFillPoint(const double x, const double y, const double z);


  /**
   * Expand replacing bad values at each end with replication of 
   * smallest/largest valid values, plus fills all gaps calling pFillGap
   * method.
   *
   * @param[in,out] row  Z values for a particular x.
   * 
   * Example:
   *
   * - input:  row = [bad, bad, ..., 3, 4, bad]
   * - output: row = [3,   3, ....,  3, 4, 4]
   *
   * See pFilLGap for example of gap filling
   */
  void pExpandRow(std::vector<double> &row);

  /**
   * Find smallest index with good value, then repeat that good value
   * back to initial position in row.
   *
   * @param[in,out] row  Vector of values to edit
   *
   * @return The smallest index with a good value in row (on input)
   */
  int pBackFill(std::vector<double> &row);

  /**
   * Find largest index with good value, then repeat that good value
   * up to last position.
   *
   * @param[in] ind0  The smallest index in row with good value
   * @param[in,out] row  Vector of values to edit
   * @return The largest index with good value in row (on input)
   */
  int pForwardFill(const int ind0, std::vector<double> &row);

  /**
   * On step in building runs of gaps in a vector of values (gaps being where
   * data is  missing)
   *
   * @param[in] ind  Current index into row
   * @param[in,out] row  Vector of values to edit
   * @param[in,out] inGap  True if within a gap, adjusted by method
   * @param[in,out] gap0   If inGap, this is the lower index of the gap
   * @param[in,out] gap1  If inGap this is the upper index of the gap thus far
   * 
   * If the method detects a gap is just ending, inGap is set to false, 
   * and pFillGap is called
   */
  void pExpandRow1(const int ind, std::vector<double> &row, bool &inGap,
		   int &gap0, int &gap1);


  /**
   * fill any gaps with interpolated values in a row
   * @param[in,out] r  A vector of values to edit
   * @param[in] i0  Index to smallest index of gap
   * @param[in] i1  Index of largest index of gap
   *
   * example:
   * - input:
   *    -  r = [3, bad, bad, bad, 7, 8]
   *    -  i0 = 1
   *    -  i1 = 3
   * - output:
   *    -   r = [3, 4, 5, 6, 7, 8]
   */
  void pFillGap(const int i0, const int i1, std::vector<double> &r);
};

# endif 
