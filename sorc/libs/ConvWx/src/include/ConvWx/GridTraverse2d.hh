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
 * @file GridTraverse2d.hh 
 * @brief  GridTraverse with a moving window (box) 
 * @class GridTraverse2d 
 * @brief  GridTraverse with a moving window (box)
 * 
 * Allows traversal of a grid moving up by one at a time, then right by one,
 * then down by one at a time, then right by one, then up by one at a time,
 * and so on. This allows efficiency in some algorithms such as
 * averaging in that you can add/subtract only one row/column of the box
 * at a time.
 */

#ifndef GRID_TRAVERSE_2D_H
#define GRID_TRAVERSE_2D_H
#include <string>
#include <vector>
#include <ConvWx/GridTraverse.hh>

//----------------------------------------------------------------
class GridTraverse2d : public GridTraverse
{
public:

  /**
   * @struct  Range_t
   * @brief The x,y limits for a box that moves around as you traverse the grid
   */
  typedef struct
  {
    int minx; /**< current minimum x grid point of the box */
    int maxx; /**< current maximum x grid point of the box */
    int miny; /**< current minimum y grid point of the box */
    int maxy; /**< current maximum y grid point of the box */
  } Range_t;

  /**
   * Constructor
   *
   * @param[in] nx  Grid dimension
   * @param[in] ny  Grid dimension
   * @param[in] sx  Box size x
   * @param[in] sy  Box size y
   */
  GridTraverse2d(const int nx, const int ny, const int sx, const int sy);

  /**
   * Destructor
   */
  virtual ~GridTraverse2d(void);

  /**
   * @return The current box location
   */
  inline Range_t currentBox(void) const { return pCurrentBox;}

  /**
   * @return The next box location 
   */
  inline Range_t newBox(void) const {return pNewBox;}

  /**
   * @return The previous box location
   */
  inline Range_t oldBox(void) const {return pOldBox;}

  /**
   * @return true if there can be a next box 
   */
  inline bool hasNew(void) const {return pHasNewBox;}

  /**
   * @return true if there is an old box
   */
  inline bool hasOld(void) const {return pHasOldBox;}

  /**
   * Perform one step of the traverse.
   *
   * @return true if still moving, false if all done
   */
  bool increment2d(void);

  /**
   * @return true if a particular point is an element of the old box
   *
   * @param[in] x  Index to the point
   * @param[in] y  Index to the point
   */
  bool isInOldBox(const int x, const int y) const;

protected:
private:

   
  int pBoxSizeX;           /**< Size of the box x */
  int pBoxSizeY;           /**< Size of the box y */
  Range_t pCurrentBox;  /**< The current box */
  Range_t pOldBox;      /**< The Previous (old) box */
  Range_t pNewBox;      /**< The Next (new) box */
  bool pHasNewBox;      /**< True if the new box is in bounds */
  bool pHasOldBox;      /**< True if the old box is in bounds */

  /**
   * Increment y
   */
  void pIncy(void);

  /**
   * Decrement y
   */
  void pDecy(void);

  /**
   * Increment x
   */
  void pIncx(void);
};

#endif
