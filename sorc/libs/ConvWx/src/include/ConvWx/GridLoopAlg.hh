// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/**
 * @file GridLoopAlg.hh 
 * @brief Algorithms to apply at each step of GridLoopA.
 * @class GridLoopAlg
 * @brief Algorithms to apply at each step of GridLoopA.
 *
 * The algorithms are over a set of points, which get added and subtracted
 * individually
 */

#ifndef GRIDLOOPALG_H
#define GRIDLOOPALG_H

class GridData;

//------------------------------------------------------------------
class GridLoopAlg
{
public:
    
  /**
   * Constructor
   */
  inline GridLoopAlg(void) {}

  /**
   * Destructor
   */
  inline virtual ~GridLoopAlg(void) {}

  /**
   * Add data at a point to the state 
   * @param[in] x  index
   * @param[in] y  index
   * @param[in] g  Data
   */
  virtual void increment(int x, int y, const GridData &g) = 0;

  /**
   * Subtract data at a point from the state 
   * @param[in] x  index
   * @param[in] y  index
   * @param[in] g  Data
   */
  virtual void decrement(int x, int y, const GridData &g) = 0;

  /**
   * Compute the algorithm result and return it
   * @param[in] minGood  Minimum number of non-missing data values in the
   *                     state.
   * @param[in] g  The data grid
   * @param[in] x  The current centerpoint
   * @param[in] y  The current centerpoint
   * @param[out] result  The algorithm result
   * @return true if result is returned
   */
  virtual bool getResult(int minGood, const GridData &g,
			 int x, int y, double &result) const = 0;

  protected:
  private:
};

//------------------------------------------------------------------
/**
 * @class GridLoopAlgMean
 * @brief Mean data value algorithm
 */
class GridLoopAlgMean : public GridLoopAlg
{
public:
  /**
   * Constructor
   * @param[in] isExclude  True to exclude a particular value from the
   *                       smoothing, false to allow all values
   * @param[in] excludeValue  The value to exclude when isExclude = true
   * @param[in] rejectCenterExclude  True to suppress smoothing when
   *                                 the center point = excludeValue 
   *                                 (isExclude = true only).
   */
  inline GridLoopAlgMean(bool isExclude, double excludeValue, 
			 bool rejectCenterExclude)
    : GridLoopAlg(), pSum(0), pCount(0), pNumExclude(0),
      pIsExclude(isExclude), pExcludeValue(excludeValue),
      pRejectCenterExclude(rejectCenterExclude) {}

  /**
   * Destructor
   */
  inline virtual ~GridLoopAlgMean(void) {}

  /**
   * Add data at a point to the state 
   * @param[in] x  index
   * @param[in] y  index
   * @param[in] g  Data
   */
  virtual void increment(int x, int y, const GridData &g);

  /**
   * Subtract data at a point from the state 
   * @param[in] x  index
   * @param[in] y  index
   * @param[in] g  Data
   */
  virtual void decrement(int x, int y, const GridData &g);

  /**
   * Compute the algorithm result and return it
   * @param[in] minGood  Minimum number of non-missing data values in the
   *                     state.
   * @param[in] g  The data grid
   * @param[in] x  The current centerpoint
   * @param[in] y  The current centerpoint
   * @param[out] result  The algorithm result
   * @return true if result is returned
   */
  virtual bool getResult(int minGood, const GridData &g,
			 int x, int y, double &result) const;

protected:
private:
  double pSum;  /**< Sum of all the data values in state */
  double pCount;  /**< Number of data points in state */
  double pNumExclude;  /**< Number of data points that have the exclude
			* value, when pIsExclude=true */
  bool pIsExclude;    /**< True to exclude a particular value */
  double pExcludeValue; /**< The particular value when pIsExclude */
  bool pRejectCenterExclude; /**< Flag on how to handle centerpoints when
			      * excluding a value (see constructor) */
};


#endif
