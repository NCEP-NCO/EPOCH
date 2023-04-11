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
/**
 * @file Subgrid.hh
 * @brief Subgrid is a class which contains location information 
 *        of the subset of the larger or super grid in the form of an Grid 
 *        object of grid indices from the larger grid that are contained in
 *        sub-grid. Subgrid has its own dimensions and a grid id. 
 * @class Subgrid
 * @brief Subgrid is a class which contains location information 
 *        of the subset of the larger or super grid in the form of an Grid 
 *        object of grid indices from the larger grid that are contained in
 *        sub-grid. Subgrid has its own dimensions and a grid id.
 */

#ifndef SUBGRID_HH
#define SUBGRID_HH

#include <ConvWx/Grid.hh>

class Subgrid {
  
public:
  
  /**
   * Constructor
   * @param[in] lowerLeftI  X-index of super-grid corresponding to lower left 
   *                        hand corner of sub-grid
   * @param[in] lowerLeftJ Y-index of super-grid corresponding to lower left 
   *                       hand corner of sub-grid
   * @param[in] nx  X dimension of sub-grid 
   * @param[in] ny  Y dimension of sub-grid
   * @param[in] superGridNx  X dimension of super-grid
   * @param[in] gridNum  Grid id
   */
  Subgrid(const int lowerLeftI, const int lowerLeftJ, const int nx, 
	  const int ny, const int superGridNx, const int gridNum);
  
  /**
   * Destructor
   */
  virtual ~Subgrid(void);

  /**
   * @return pIndexGrid  Return the Grid containing the indices of larger 
   *                     grid which define this sub-grid.
   */
  inline const Grid &getIndexGrid(void) const { return pIndexGrid;}
  
  /**
   * @return pGridNum  Return integer grid id.
   */
  inline int getGridNum(void) const { return pGridNum;} 

  /**
   * @return pNx  Return number of sub-grid points in X (or East West) direction.
   */
  inline int getNx(void) const { return pNx; }
 
  /**
   * @return pNy  Return number of sub-grid points in Y (or North South)
   *              direction.
   */
  inline int getNy(void) const { return pNy; }


  /**
   * @return number of grid points in sub-grid 
   */
  inline int getNumPts(void) const { return  pIndexGrid.getNdata(); }

  /**
   * @return the value at a grid index
   * @param[in] i  Index into grid (2 dimensional)
   * @param[in] j  Index into grid (2 dimensional)
   * @param[out] value  The value returned
   */
  inline void getValue(const int i, const int j, double &value)
  {
    pIndexGrid.getValue(i, j, value);
  }

  /**
   * @return the value at a grid index
   * @param[in] i  Index into grid (1 dimensional)
   * @param[out] value  The value returned
   */
  inline void getValue(const int i, double &value)
  {
    pIndexGrid.getValue(i, value);
  }

protected:
  
private:

  /**
   * Constant for missing data
   */
  static const int gridIndexMissing;

  /**
   * Number of sub-grid points in X (or East West) direction
   */
  int pNx;
  
  /**
   * Number of sub-grid points in Y (or North South) direction
   */
  int pNy;

  /** 
   * Grid id
   */
  int pGridNum;

  /**
   *  A grid of the indices of the section of the larger grid this sub-grid 
   *  represents.
   */
  Grid pIndexGrid;

};

#endif
