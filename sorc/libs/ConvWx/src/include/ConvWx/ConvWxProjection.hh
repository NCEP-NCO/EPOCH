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
 * @file ConvWxProjection.hh
 * @brief  Use parameterized projection values to convert between grid index,
 *         x/y  and lat/lon.
 * @class ConvWxProjection
 * @brief  Use parameterized projection values to convert between grid index,
 *         x/y  and lat/lon.
 *         
 */

#ifndef CONVWX_PROJECTION_HH
#define CONVWX_PROJECTION_HH

#include <string>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ConvWxPjgMath.hh>

class ConvWxProjection
{
public:

  /**
   * Constructor, sets values to missing and empty
   */
  ConvWxProjection(void);
  
  /**
   * Destructor
   */
  virtual ~ConvWxProjection(void);

  /**
   * Initialize state using Projection parameters
   *
   * @param[in] params  Projection parameters to use
   */
  void init(const ParmProjection &params);

  /**
   * Convert from a Grid x,y index to a latitude/longitude
   *
   * @param[in] x  Index into a Grid in the projection
   * @param[in] y  Index into a Grid in the projection
   * @param[out] lat  Returned latitude
   * @param[out] lon  Returned longitude
   */
  void xyIndex2latlon(const int x, const int y, double &lat, double &lon) const;

  /**
   * Convert from latitude/longitude to Grid x,y index
   *
   * @return false if the latitude/longitude is out of range of the Grid
   *         limits
   *
   * @param[in] lat  Input latitude
   * @param[in] lon  Input longitude
   * @param[out] x  Index into Grid in the projection
   * @param[out] y  Index into Grid in the projection
   */
  bool latlon2xyIndex(const double lat, const double lon, int &x, int &y) const;

  /**
   * Convert from latitude/longitude to Grid x,y index, even if the index
   * is out of range of the grid.
   *
   * @param[in] lat  Input latitude
   * @param[in] lon  Input longitude
   * @param[out] x  Index into Grid in the projection
   * @param[out] y  Index into Grid in the projection
   */
  void latlon2xyIndexAny(const double lat, const double lon,
			 int &x, int &y) const;

  /**
   * Convert from a Grid x,y index to latitude/longitude
   * with real valued x,y allowed.
   *
   * @param[in] x  Double valued index into Grid
   * @param[in] y  Double valued index into Grid
   * @param[out] lat  Returned latitude
   * @param[out] lon  Returned longitude
   */
  void xy2latlon(const double x, const double y, double &lat, double &lon) const;

  /**
   * Convert from latitude/longitude to Grid x,y double values
   *
   * @return false if the latitude/longitude is out of range of the Grid
   *         limits
   *
   * @param[in] lat  Input latitude
   * @param[in] lon  Input longitude
   * @param[out] x  Index into Grid in the projection (double)
   * @param[out] y  Index into Grid in the projection (double)
   */
  void latlon2xy(const double lat, const double lon, double &x,
		 double &y) const;


  /**
   * @return distance (km) for an extent along the x direction in the
   *         projection
   * @param[in] x  X extent in the units of the projection
   */
  double xGrid2km(const double x) const;

  /**
   * @return distance (km) for an extent along the y direction in the
   *         projection
   * @param[in] y  Y extent in the units of the projection
   */
  double yGrid2km(const double y) const;

  /**
   * override the default earth radius in km
   * @param[in] earthRadiusKm   The new radius to use 
   */
  void setEarthRadiusKm(double earthRadiusKm);

protected:
private:
  
  ParmProjection pProjParams;        /**< Projection definition */
  ConvWxPjgMath *pMath;              /**< Base class pointer for computations */
  ParmProjection::Proj_t pProjType;  /**< Projection type */
  double pOriginLat;                 /**< Projection origin */
  double pOriginLon;                 /**< Projection origin */

  double pFalseNorthing;             /**< False northing value, added to Y */
  double pFalseEasting;              /**< False easting value, added to X */

  /**
   * Computes the the data x, y indices, in doubles, for the given
   * x/y location.
   *
   * @param[in] xx  Location X, in projection units
   * @param[in] yy  Location Y, in projection units
   * @param[out] xIndex  X index returned
   * @param[out] yIndex  Y index returned
   *
   * @return true for success, false for failure (data outside grid)
   */
  bool pXy2xyIndex(double xx, double yy, int &xIndex, int &yIndex) const;


  /**
   * Computes the the data x, y indices, in doubles, for the given
   * x/y location, returning even negative or large numbers (index out of grid)
   *
   * @param[in] xx  Location X, in projection units
   * @param[in] yy  Location Y, in projection units
   * @param[out] xIndex  X index returned
   * @param[out] yIndex  Y index returned
   *
   */
  void pXy2xyIndexAny(double xx, double yy, int &xIndex, int &yIndex) const;

  /**
   * Convert the given distance in number of grid spaces in the X direction
   * to kilometers.
   *
   * @param[in] gridDistance  Number of x direction grid spaces
   * @return distance in kilometers
   */
  double pXGrid2km(const double gridDistance) const;

  /**
   * Convert the given distance in number of grid spaces in the Y direction
   * to kilometers.
   *
   * @param[in] gridDistance  Number of y direction grid spaces
   * @return distance in kilometers
   */
  double pYGrid2km(const double gridDistance) const;

  /**
   * Convert the given distance in number of grid spaces to kilometers.
   *
   * @param[in] gridDistance Number of grid spaces
   * @param[in] gridDelta  Extent (km) per grid space
   *
   * @return distance in kilometers
   */
  double pGrid2km(double gridDistance, double gridDelta) const;
};

#endif
