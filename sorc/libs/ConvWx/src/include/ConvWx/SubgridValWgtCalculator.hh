/**
 * @file SubgridValWgtCalculator.hh
 * @brief  Each point in a rectangular grid with dimensions nx and ny
 *         is assigned an integer weight. The weights are determined by
 *         the rectangular area formed by the grid edges and the
 *         shortest distances in the vertical ( or N-S ) and
 *         horizontal (or E-W) directions from the point to the closest
 *         grid edges. Hence greater weight is given to interior points
 *         and less to the edges. The minimum weight (area) assigned to
 *         any point is 1.
 * @class SubgridValWgtCalculator
 * @brief  Each point in a rectangular grid with dimensions nx and ny
 *         is assigned an integer weight. The weights are determined by
 *         the rectangular area formed by the grid edges and the 
 *         shortest distances in the vertical ( or N-S ) and 
 *         horizontal (or E-W) directions from the point to the closest 
 *         grid edges. Hence greater weight is given to interior points
 *         and less to the edges. The minimum weight (area) assigned to 
 *         any point is 1.  
 */

#ifndef SUBGRID_VAL_WGT_CALCULATOR_HH
#define SUBGRID_VAL_WGT_CALCULATOR_HH

#include <vector>

class SubgridValWgtCalculator {

public:

  /** 
   *  Constructor
   *  @param[in] nx  Horizontal dimension of grid
   *  @param[in] ny  Vertical dimension of grid
   */
   SubgridValWgtCalculator(const int nx, const int ny); 

  /**
   *  Destructor
   */
  virtual ~SubgridValWgtCalculator(void);

  /**  
   *  @param[in] i  Array index
   *  @return  Weight (or area) computed at array index i
   */
  double get(const int i) const;

protected:

private:

  /**
   * Minimum area or weight assigned to a grid point
   */ 
  const static double minArea;
  
  /**
   * Value to indicate no area data available
   */
  const static double noArea;

  /** 
   *  Return the minimum of x and y.
   *  @param[in] x  Number
   *  @param[in] y  Number
   *  @return minimum value of x and y.
   */
  inline double pMin(const double x, const double y) { return x < y ? x : y;}

  /** 
   * Container for holding areas determined for each sub-grid point by finding
   * the rectangle of minimum area formed by sub-grid edges and vertical
   * and horizontal lines through the grid point.
   */
  std::vector <double> pArea;
};

#endif
