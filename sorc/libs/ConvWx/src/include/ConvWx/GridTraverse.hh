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
 * @file  GridTraverse.hh 
 * @brief Increment in two dimensions changing only one grid point in x or y
 * @class GridTraverse 
 * @brief Increment in two dimensions changing only one grid point in x or y
 * 
 * Allows traversal of a grid moving up by one at a time, then right by one,
 * then down by one at a time, then right by one, then up by one at a time,
 * and so on. This allows efficiency in some algorithms such as
 * averaging in that you can add/subtract only one row/column of the box
 * at a time.
 *
 */

#ifndef GRID_TRAVERSE_H
#define GRID_TRAVERSE_H
#include <string>
#include <vector>

//----------------------------------------------------------------
class GridTraverse
{
public:

  /**
   * @enum State_t
   * @brief The choices for grid traversal motion
   */
  typedef enum
  {
    INIT=0,  /**< Starting (up) */
    INC_Y=1, /**< Moved plus one in Y */
    DEC_Y=2, /**< Moved minus one in Y */
    INC_X=3  /**< Moved plus one in X */
  } State_t;

  /**
   * Constructor
   *
   * @param[in] nx  Grid dimension x
   * @param[in] ny  Grid dimension y
   */
  GridTraverse(const int nx, const int ny);

  /**
   * Destructor
   */
  virtual ~GridTraverse(void);

  /**
   * Set state to GridTraverse::INIT, and set location x,y = 0,0
   */
  void reinit(void);

  /**
   * Set state to GridTraverse::INIT, leave x,y at the current point
   */
  void setStateInit(void);

  /**
   * Return the current state and the current location
   *
   * @param[out] x  Current location x
   * @param[out] y  Current location y 
   * @return  The current state
   */
  State_t getXy(int &x, int &y) const;
  
  /**
   * Perform one step of the traverse. (move up, down, or to the right)
   *
   * @return true if still moving, false if all done
   */
  bool increment(void);

  /**
   * @return the current state 
   */
  inline State_t getState(void) const {return pState;}

  /**
   * @return a string representation of any state
   * @param[in] s  State value
   */
  static std::string printState(const State_t s);

protected:

  int pNx;                /**< grid size x */
  int pNy;                /**< grid size y */
  int pCurrentX;                 /**< current state = location (x) */
  int pCurrentY;                 /**< current state = location (y) */
  State_t pState;         /**< Current state = type of motion */

private:

  State_t pPreInitState;  /**< The state prior to resetting to INIT */

  /**
   * Increment when in GridTraverse::INIT state
   * @return true if successful
   */
  bool pIncrementInit(void);

  /**
   * Increment when in GridTraverse::INC_Y state
   * @return true if successful
   */
  bool pIncrementY(void);

  /**
   * Increment when in GridTraverse::INC_X state
   * @return true if successful
   */
  bool pIncrementX(void);

  /**
   * Increment when in GridTraverse::DEC_Y state
   * @return true if successful
   */
  bool pDecrementY(void);
};

#endif
