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
 * @file ParmProjection.hh
 * @brief Parameters to define a projection
 * @class ParmProjection
 * @brief Parameters to define a projection
 *
 * Any members that are hardwired are const, all are public by design
 */

# ifndef    PARM_PROJECTION_HH
# define    PARM_PROJECTION_HH

class Grid;
class ConvWxFuzzy;

//----------------------------------------------------------------
class ParmProjection
{
public:

  /**
   * @enum Proj_t
   * @brief The supported projections 
   */
  typedef enum
  {
    FLAT,
    LAMBERT_CONFORMAL2,
    LAMBERT_AZIM_EQUAL_AREA,
    LATLON
  } Proj_t;

  /**
   * Empty (invalid) constructor
   */
  ParmProjection(void);

  /**
   * constructor with all parameters passed in.
   * @param[in] proj  Projection type
   * @param[in] nx    Number of x grid points
   * @param[in] ny    Number of y grid points
   * @param[in] minx  Minimum x with respect to origin point
   * @param[in] miny  Minimum y with respect to origin point
   * @param[in] dx    Grid resolution x 
   * @param[in] dy    Grid resolution y 
   * @param[in] originLat  Origin of grid
   * @param[in] originLon  Origin of grid
   * @param[in] offsetOriginLat  Offset for Lambert Conformal projection
   * @param[in] offsetOriginLon  offset for Lambert Conformal projection
   * @param[in] lat1   Lambert Conformal projection latitude 1
   * @param[in] lat2   Lambert Conformal projection latitude 2
   * @param[in] rotation   Degrees
   * @param[in] earthRadiusKm  Earth radius in kilometers
   *
   * @note For LAMBERT_CONFORMAL2 and FLAT, distance units are kilometers
   * @note for LATLON, the distance units are degrees, except z which is 
   *       kilometers
   */
  ParmProjection(Proj_t proj, int nx, int ny,
		 double minx, double miny, double dx,
		 double dy, double originLat,
		 double originLon, double offsetOriginLat,
		 double offsetOriginLon, double lat1,
		 double lat2, double rotation,
		 double earthRadiusKm);

  /**
   * constructor with all parameters passed in.
   * @param[in] proj  Projection type
   * @param[in] nx    Number of x grid points
   * @param[in] ny    Number of y grid points
   * @param[in] nz    Number of z grid points
   * @param[in] minx  Minimum x with respect to origin point
   * @param[in] miny  Minimum y with respect to origin point
   * @param[in] minz  Minimum y with respect to origin point
   * @param[in] dx    Grid resolution x 
   * @param[in] dy    Grid resolution y
   * @param[in] dz    Grid resolution z
   * @param[in] originLat  Origin of grid
   * @param[in] originLon  Origin of grid
   * @param[in] offsetOriginLat  Offset for Lambert Conformal projection
   * @param[in] offsetOriginLon  offset for Lambert Conformal projection
   * @param[in] lat1   Lambert Conformal projection latitude 1
   * @param[in] lat2   Lambert Conformal projection latitude 2
   * @param[in] rotation   Degrees
   * @param[in] earthRadiusKm  Earth radius in kilometers
   *
   * @note For LAMBERT_CONFORMAL2 and FLAT, distance units are kilometers
   * @note for LATLON, the distance units are degrees except z which is
   *       kilometers
   */
  ParmProjection(Proj_t proj, int nx, int ny, int nz,
		 double minx, double miny, double minz, 
		 double dx, double dy, double dz, 
		 double originLat,double originLon, 
		 double offsetOriginLat, double offsetOriginLon, 
		 double lat1, double lat2, double rotation,
		 double earthRadiusKm);

  /**
   * Constructor to reduce the grid resolution
   *
   * @param[in] highResParms  The high resolution Projection parameter values
   * @param[in] res   Reduction factor, 1=keep input, 2=half, etc
   *
   * On exit the constructor has built a projection at the lower resolution
   */
  ParmProjection(const ParmProjection &highResParms, const int res);
  
  /**
   * Constructor to expand or shrink the grid size, leaving everything else
   * the same
   *
   * @param[in] proj  The projection to expand
   * @param[in] nx    Number of gridpoints to add at both ends, x
   * @param[in] ny    Number of gridpoints to add at both ends, y
   *
   *
   * If nx,ny > 0, expands the grid, otherwise shrinks it
   */
  ParmProjection(const ParmProjection &proj, const int nx, const int ny);
  
  /**
   * Destructor
   */
  virtual ~ParmProjection(void);

  /**
   * Check for equality
   *
   * @param[in] proj
   */
  bool operator==(const ParmProjection &proj) const;

  /**
   * Use the projection to build and return a Grid with an 'interior' value
   * at each point.
   *
   * At each grid point the distance to the edge of the grid (km) is determined,
   * and the input fuzzy function maps this distance to a value in the range
   * [0,1], the so called 'interior-ness' of the grid point. 
   * A high value means very interior, a low value means very close to the
   * edge. The fuzzy function determines the exact mapping.
   *
   * @param[in] fuzzyDistToInterest  The fuzzy function mapping distance to
   *                                 interest
   * 
   * @return Grid in the input projection with value at each point set to
   * the interior measure based on the fuzzy function
   */
  Grid interior(const ConvWxFuzzy &fuzzyDistToInterest) const;

  /**
   * @return true if the input projection and local projection are consistent
   * as regards use for tiling. This means the dx, and dy and the projection
   * types must be the same, and they must align exactly.
   *
   * @param[in] proj  The projection to compare with
   */
  bool consistentForTiling(const ParmProjection &proj) const;

  /**
   * @return true if local projection is a superset of the input projection
   *
   * @param[in] proj the projection to test
   *
   * If the grids align, dx and dy are the same, and the input fits entirely
   * within the local grid, the local projection is a super set.
   * 
   */
  bool isSuperset(const ParmProjection &proj) const;

  /** 
   * @return number of kilometers per pixel for the projection
   *
   * Takes average of x and y km per pixel
   *
   * @note if a lat lon projection, uses an average (center) projection
   * latitude to convert longitude (X) changes to kilometers from degrees
   */
  double kmPerPixel(void) const;

  /**
   * @return the number of kilometers per pixel in X dimension
   *
   * @note if a lat lon projection, uses an average (center) projection
   * latitude to convert a change in longitude (X) to kilometers from degrees
   */
  double kmPerPixelX(void) const;

  /**
   * @return the number of kilometers per pixel in Y dimension
   */
  double kmPerPixelY(void) const;

  /**
   * Convert from lat/lon value to a grid index x,y
   *
   * @param[in] lat  Latitude value
   * @param[in] lon  Longitude value
   * @param[out] x   Grid index
   * @param[out] y   Grid index
   *
   * @return false if lat/lon outside the grid
   */
  bool latlonToGridIndex(const double lat, const double lon, int &x,
			 int &y) const;

  /**
   * Convert from lat/lon value to a grid index x,y, allowing any index value,
   * including those out of range of the grid
   *
   * @param[in] lat  Latitude value
   * @param[in] lon  Longitude value
   * @param[out] x   Grid index
   * @param[out] y   Grid index
   */
  void latlonToGridIndexAny(const double lat, const double lon, int &x,
			    int &y) const;

  /**
   * Convert from lat/lon value to a grid index x,y, allowing any index value,
   * including those out of range of the grid, double return values
   *
   * @param[in] lat  Latitude value
   * @param[in] lon  Longitude value
   * @param[out] x   Grid index
   * @param[out] y   Grid index
   */
  void latlonToGridIndexAnyD(const double lat, const double lon, double &x,
			     double &y) const;

  /**
   * Convert from a lat/lon value to a grid index x,y, double return values
   *
   * @param[in] lat  Latitude value
   * @param[in] lon  Longitude value
   * @param[out] x   Grid index, can be between points
   * @param[out] y   Grid index, can be between points
   *
   * @return false if lat/lon outside the grid
   */
  bool latlonToGridIndexD(const double lat, const double lon,
			  double &x, double &y) const;


  /**
   * Convert from lat/lon value to km from projection 0,0 grid index
   *
   * @param[in] lat  Latitude value
   * @param[in] lon  Longitude value
   * @param[out] x   km from 0,0
   * @param[out] y   km from 0,0
   */
  void latlonToXyKm(const double lat, const double lon, double &x,
		    double &y) const;

  /**
   * Convert from  a grid x, y index to the associated lat/lon
   * @param[in] x  Grid index
   * @param[in] y  Grid index
   * @param[out] lat  Latitude
   * @param[out] lon  Longitude
   */
  void gridIndexToLatlon(const int x, const int y, double &lat,
			 double &lon) const;

  /**
   * Convert from a grid x, y real valued index to the associated lat/lon
   * @param[in] x  Grid index
   * @param[in] y  Grid index
   * @param[out] lat  Latitude
   * @param[out] lon  Longitude
   */
  void gridIndexDToLatlon(const double x, const double y,
			  double &lat, double &lon) const;

  /**
   * Determine distance between two points in grid.
   *
   * @param[in] indexI1  Index to x at first point
   * @param[in] indexJ1  Index to y at first point
   * @param[in] indexI2  Index to x at second point
   * @param[in] indexJ2  Index to y at second point
   * @param[out] dist  The distance, in kilometers
   */
  void distanceKm( const int indexI1, const int indexJ1, const int indexI2,
		   const int indexJ2, double &dist) const;

  /**
   * Determine distance between two real valued points in grid. The real values
   * are partial grid index values
   *
   * @param[in] indexI1  Index to x at first point
   * @param[in] indexJ1  Index to y at first point
   * @param[in] indexI2  Index to x at second point
   * @param[in] indexJ2  Index to y at second point
   * @param[out] dist  The distance, in kilometers
   */
  void distanceKmD( const double indexI1, const double indexJ1,
		    const double indexI2, const double indexJ2,
		    double &dist) const;

  /**
   * Determine distance between two points in grid.
   * @param[in] arrayIndex1  One dimensional index into grid at first point
   * @param[in] arrayIndex2  One dimensional index into grid at second point
   * @param[out] dist  The distance, in kilometers
   */
  void distanceKm( const int arrayIndex1, const int arrayIndex2,
		   double &dist) const;

  Proj_t pProjection;      /**< the projection  */
  int pNx;                 /**< grid dimension x */
  int pNy;                 /**< grid dimension y */
  int pNz;                 /**< grid dimension z */
  double pDx;              /**< grid resolution x */
  double pDy;              /**< grid resolution y */
  double pDz;              /**< grid resolution z */
  double pMinx;            /**< grid minimum x value */
  double pMiny;            /**< grid minimum y value */
  double pMinz;            /**< grid minimum z value */
  double pOriginLat;       /**< grid origin latitude degrees */
  double pOriginLon;       /**< grid origin longitude degrees */
  double pLat1;            /**< the Lambert Conformal lat1 degrees */
  double pLat2;            /**< the Lambert Conformal lat2 degrees */
  double pOffsetOriginLat; /**< offset parameter for Lambert Conformal
			    * projection (degrees) */
  double pOffsetOriginLon; /**< offset parameter for Lambert Conformal
			    * projection (degrees) */
  double pRotation;        /**< degrees */
  double pEarthRadiusKm;   /**< Earth radius in kilometers */
  
protected:
private:  

  /**
   * @return distance (km) to edge along one dimension (x or y)
   *
   * @param[in] y  Grid index (x or y)
   * @param[in] dy  Kilometers per gridpt
   * @param[in] maxd  Maximum distance value from fuzzy function (km)
   * @param[in] maxy  Width of the entire grid (kilometers)
   * @param[in] missingv  Data missing value
   */
  double pInterior(const int y, const double dy, const double maxd,
		   const double maxy, const double missingv) const;

  /**
   * @return distance (km) to edge along both dimensions
   *
   * @param[in] xd  Distance to edge x
   * @param[in] yd  Distance to edge y
   * @param[in] maxd  Maximum depth of fuzzy function used (km)
   * @param[in] missingv  Data missing value
   */
  double pInteriorV(const double xd, const double yd, const double maxd,
		    const double missingv) const;

  /**
   * Use the projection, which has already been determined to be a lat/lon
   * projection, to build and return a Grid with an 'interior' value
   * at each point.
   *
   * At each grid point the distance to the edge of the grid (km) is determined,
   * and the input fuzzy function maps this distance to a value in the range
   * [0,1], the so called 'interior-ness' of the grid point. 
   * A high value means very interior, a low value means very close to the
   * edge. The fuzzy function determines the exact mapping.
   *
   * @param[in] fuzzyF  The fuzzy function mapping distance to interest
   * @param[in] missingv  Data missing value
   * @param[out] ret Grid filled with the interior values.
   */
  void pInteriorLatlon(const ConvWxFuzzy &fuzzyF, const double missingv,
		       Grid &ret) const;

};

# endif     // ParmProjection_HH
