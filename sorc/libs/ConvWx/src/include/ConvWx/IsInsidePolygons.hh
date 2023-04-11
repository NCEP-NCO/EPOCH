/**
 * @file IsInsidePolygons.hh
 * @class IsInsidePolygons  Figures out if a gridpoint is inside a set of polygons or
 *                  not
 */
#ifndef IS_INSIDE_HH
#define IS_INSIDE_HH

#include <vector>
#include <ConvWx/Grid.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/MultiGrid.hh>

class IsInsidePolygons
{
public:
  /**
   * Constructor
   */
  IsInsidePolygons(void);

  /**
   * Destructor
   */
  ~IsInsidePolygons(void);

  /**
   * Read in polygon points from ascii file
   * @param[in] fname  File
   * @return true for success
   *
   * This can be called any number of times to add more polygons
   */
  bool readParms(const std::string &fname);

  /**
   * Add polygon points
   * @param[in] latlon  The polygon points
   *
   * This can be called any number of times to add more polygons
   */
  void addPolygon(const std::vector<std::pair<double,double> > &latlon);

  /**
   * Create a mask grid  after adding the polygon(s) you want.
   *
   * @param[in] proj  Projection parameters to use
   * @param[in] nx  Number of x grid points to add at each end of the grid,x
   * @param[in] ny  Number of x grid points to add at each end of the grid,y
   *
   * @return true for success
   *
   * @note the nx,ny params are used to create a grid larger than the proj
   *       parameter for polygons that go outside the grid.
   */
  bool createMask(const ParmProjection &proj, const int nx=0, const int ny=0);

  /**
   * @return true if input projection is the one used to build the mask
   * @param[in] proj
   */
  bool consistent(const ParmProjection &proj) const;

  /**
   * @return true if grid index values relative to the projection are
   *         inside a polygon
   * @param[in] x
   * @param[in] y
   */
  bool isInside(const int x, const int y) const;

  /**
   * Append a Grid representing the interior of the polygons to input MultiGrid
   *
   * @param[in] fieldName  fieldname to use
   * @param[in] proj projection info used to create the Grid
   * @param[in,out] mg  MultiGrid to append to
   */
  void appendGrid(const std::string &fieldName,
		  const ParmProjection &proj, MultiGrid &mg);

  /**
   * Write internal grids in their native projection
   * @param[in] t  Time
   * @param[in] url  Where to write
   */
  void writeGrids(const time_t &t, const std::string &url);
  
protected:
private:

  /**< Boundary points, one vector per polygon, lat=first, lon=second */
  std::vector<std::vector<std::pair<double,double> > > pLatlon; 

  bool pMaskIsSet;                       /**< True if mask grid is set */
  Grid pMask;                            /**< Mask grid */
  Grid pEdges;                /**< Grid with just the polygon edges */
  ParmProjection pProj;                  /**< Projection for mask grid,
					  * may have expansion */
  int pXexpand;  /**< # of gridpts to expand grid at both ends (x), default=0 */
  int pYexpand;  /**< # of gridps to expand grid at both ends (y), default=0 */

};

#endif
