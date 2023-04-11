/**
 * @file EdgeFinder.hh
 * @brief An algorithm that follows an edge in a grid.
 * @class EdgeFinder
 * @brief An algorithm that follows an edge in a grid.
 */

# ifndef    EDGEFINDER_H
# define    EDGEFINDER_H

#include <vector>
#include <string>
#include <ConvWx/GridData.hh>

class EdgeFinder 
{
 public:

  /**
   * @note Pass in the grid to give dimensions.
   * @param[in] g  The grid data
   */
  EdgeFinder(const GridData &g);

  /**
   * Destructor
   */
  virtual ~EdgeFinder(void);

  /**
   * Add grid index values for a point to the local state as the next
   * consecutive edge point.
   *
   * @param[in] x  Grid index to add
   * @param[in] y  Grid index to add
   *
   * @note Adjusts ranges of index values also.
   * @note Fills gaps as it builds the edge, assumes calls to this method
   *       are sequential (along a polygon)
   */
  void add(const int x, const int y);

  /**
   * @return true if nothing was added
   */
  bool bad(void) const;
  
  /**
   * @return range of x,y indices in the edge points
   *
   * @param[out] x0  Returned lower left X index
   * @param[out] x1  Returned upper right X index
   * @param[out] y0  Returned lower left Y index
   * @param[out] y1  Returned upper right Y index
   */
  inline void getRange(int &x0, int &x1, int &y0, int &y1) const
  {
    x0 = pX0;
    x1 = pX1;
    y0 = pY0;
    y1 = pY1;
  }

  /**
   * Store the full set of edge points as a grid locally, and
   * put those edge points that are in the range of the grid to
   * the input GridData object
   *
   * @param[in] edgeData  GridData object in which to write edge points
   */
  void putToGrid(GridData &edgeData);

  /**
   * @return true if input point is not an edge point
   * @param[in] x  Index into the subgrid where the edge data is
   * @param[in] y  Index into the subgrid where the edge data is
   *
   * @note actual grid index = pX0 + x, pY0 + y
   */
  bool notEdge(const int x, const int y) const;

  /**
   * @return true if input point is in bounds, and return the
   * actual grid index
   * @param[in] x  Index into the subgrid where the edge data is
   * @param[in] y  Index into the subgrid where the edge data is
   * @param[out] xi  actual grid index = pX0 + x
   * @param[out] yi  actual grid index = pY0 + y
   */
  bool dataGridIndex(const int x, const int y, int &xi, int &yi) const;

protected:
private:

  /**
   * The edge index values, can be out of range of actual grid
   */
  std::vector<std::pair<int,int> > pPoints;  

  /**
   * The subgrid that is just big enough to contain the edge points
   *
   * Its lower left hand corner is pX0,pY0 in the original grid
   */
  GridData pEdge;

  int pX0;          /**< Lower left X index in actual grid */
  int pX1;          /**< Upper right X index in actual grid */
  int pY0;          /**< Lower left Y index in actual grid */
  int pY1;          /**< Upper right Y index in actual grid */

  int pLastX;       /**< Last point added to edge */
  int pLastY;        /**< Last point added to edge */

  const double pBad;  /**< Bad data value for edge grid */
  const double pGood; /**< Edge data value for edge grid */

  int pNx;            /**< Actual grid nx */
  int pNy;            /**< Actual grid ny */

  /**
   * Fill gaps between input point and last point
   * @param[in] x  Input point
   * @param[in] y  Input point
   */
  void pFillGaps(const int x, const int y);
};

# endif
