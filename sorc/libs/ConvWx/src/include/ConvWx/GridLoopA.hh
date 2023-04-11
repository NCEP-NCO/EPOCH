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
 * @file GridLoopA.hh 
 * @brief Traverse a Grid with a moving box in a way that the difference
 * is only one index at a time in x or y but not both, with a GridLoopAlg
 * object handling the algorithm within the box.
 *
 * @class GridLoopA 
 * @brief Traverse a Grid with a moving box in a way that the difference
 * is only one index at a time in x or y but not both, with a GridLoopAlg
 * object handling the algorithm within the box.
 *
 * @note Move up then right, then down, then right, then up, etc.
 * The class was created for efficient averaging techniques that allow
 * subtraction and addition of only a subset of values for each step.
 */

#ifndef GRID_LOOPA_H
#define GRID_LOOPA_H

#include <string>
#include <vector>

class GridData;
class GridLoopAlg;

//------------------------------------------------------------------
class GridLoopA
{
public:

  /**
   * @enum State_t
   * @brief Can move up in Y, down in Y or right in X
   */
  typedef enum
  {
    INIT=0,     /**< Initial state */
    INC_Y=1,    /**< Moving up in Y */
    DEC_Y=2,    /**< Moving down in Y */
    INC_X=3     /**< Moving up in X */
  } State_t;

  /**
   * @param[in] nx  Number of Grid2d points x
   * @param[in] ny  Number of Grid2d points y
   * @param[in] sx  The box width (x)
   * @param[in] sy  The box width (y)
   */
  GridLoopA(int nx, int ny, int sx, int sy);

  /**
   * Destructor
   */
  virtual ~GridLoopA(void);

  /**
   * Set the state to initial state. This puts you in the lower left corner.
   */
  void reinit(void);

  /**
   * @return current state and current location
   *
   * @param[out] x  Current location x
   * @param[out] y  Current location y
   */
  State_t getXy(int &x, int &y) const;
  
  /**
   * Get the current location, and the algorithm result within the box
   * @param[in] alg  The algorithm state
   * @param[in] minGood  Minimum number of nonmissing points within the box
   * @param[in] g  The data grid
   * @param[out]  x  Current center point index of box
   * @param[out]  y  Current center point index of box
   * @param[out] result  The algorithm result in the box
   *
   * @return true for success, false for no more points or error
   */
  bool getXyAndResult(const GridLoopAlg &alg, int minGood, const GridData &g,
		      int &x, int &y, double &result) const;

  /**
   * Increment by one gridpoint 
   *
   * @return true if still moving, false if all done
   *
   * @param[in] g  GridData with data
   * @param[in,out] data  The algorithm state, updated
   */
  bool increment(const GridData &g, GridLoopAlg &data);

  /**
   * @return string description of a state value
   *
   * @param[in] s  State 
   */
  static std::string printState(const State_t s);

protected:
private:



  int pNx;  /**< Grid size */
  int pNy;  /**< Grid size */

  int pX;         /**< Current centerpoint index */
  int pY;         /**< Current centerpoint index */

  int pMinx;  /**< Current minimum x for the box */
  int pMaxx;  /**< Current maximum x for the box */
  int pMiny;  /**< Current minimum y for the box */
  int pMaxy;  /**< Current maximum y for the box */

  int pSx; /**< Box width x */
  int pSy; /**< Box width y */


  State_t pState; /**< Current state = type of motion */

  /**
   * Completely recompute at current internal state position in the grid
   * @param[in] g  Data grid to use
   * @param[in,out]  alg  Algorithm object
   */
  void pFullCompute(const GridData &g, GridLoopAlg &alg) const;

  /**
   * Add data to the algorithm object at the internal state position,
   * adding in index x
   * @param[in] x  Index that is added
   * @param[in] g  The grid data
   * @param[in,out] alg  the Algorithm object
   */
  void pAddX(int x, const GridData &g, GridLoopAlg &alg) const;

  /**
   * Subtract data from the algorithm object at the internal state position,
   * subtract index x
   * @param[in] x  Index that is subtracted
   * @param[in] g  The grid data
   * @param[in,out] alg the Algorithm object
   */
  void pSubtractX(int x, const GridData &g, GridLoopAlg &alg) const;

  /**
   * Add data to the algorithm object at the internal state position,
   * adding in index y
   * @param[in] y  Index that is added
   * @param[in] g  The grid data
   * @param[in,out] alg the Algorithm object
   */
  void pAddY(int y, const GridData &g, GridLoopAlg &alg) const;

  /**
   * Subtract data from the algorithm object at the internal state position,
   * subtract index y
   * @param[in] y  Index that is subtracted
   * @param[in] g  The grid data
   * @param[in,out] alg the Algorithm object
   */
  void pSubtractY(int y, const GridData &g, GridLoopAlg &alg) const;
};

#endif
