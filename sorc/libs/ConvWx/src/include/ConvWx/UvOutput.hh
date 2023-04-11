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
 * @file UvOutput.hh
 * @brief U and V output grids, with metadata and units
 * @class UvOutput
 * @brief U and V output grids, with metadata and units
 *
 * U and V are typically components of a vector field such as phase correction,
 * with U being the 'x' component and V being the 'y' component
 *
 * @note A description of 'external' and 'local' UV conversions, used in some
 * methods:
 * Phase correction does not happen in time, but in space. To externally store
 * the phase correction vectors (for display) requires pretending they are
 * motion vectors with units = 'meters/second'. Hence we have to introduce a
 * fake time component to the vectors, indicating they have moved a certain
 * number of meters over some time.   That time is given by
 * UvOutput::externalSeconds(void) and is set to 3600, i.e. 1 hour.
 * This allows us to convert from internal phase correction vectors (units
 * are number of grid points moved), to and from an external representation
 * that converts number of grid points to meters (using projection), and
 * assumes 1 hour.
 */

# ifndef    UV_OUTPUT_HH
# define    UV_OUTPUT_HH

#include <string>
#include <ConvWx/MetaDataXml.hh>
#include <ConvWx/UvUnits.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/GridTraverse2d.hh>
class GridTraverse;
class ParmProjection;
class ParmSetUV;
class ParmPcAlg;
class PcStats;
class MultiGrid;
class VectorMaxCounts;
class MetaData;
class ConvWxMultiThreadMgr;

//----------------------------------------------------------------*/
class UvOutput : public MetaDataXml, public UvUnits
{
public:

  /**
   * Default constructor, no known grids or units
   */
  UvOutput(void);

  /**
   * Constructor, sets units, no known grids
   *
   * @param[in] units  Value to set into local state
   */
  UvOutput(const UvUnits &units);

  /**
   * Constructor with grid specs coming in but no data values.
   *
   * @param[in] uname  Name of U field
   * @param[in] vname  Name of V field
   * @param[in] units  Name of units for U, V
   * @param[in] nx  Grid dimension
   * @param[in] ny  Grid dimension
   * @param[in] uvUnits  The units of the motion to use.
   *
   * Sets U and V values to zero everywhere
   */
  UvOutput(const std::string &uname, const std::string &vname,
	   const std::string &units, const int nx, const int ny,
	   const UvUnits &uvUnits);

  /**
   * Constructor with an example grid.
   * Set U and V grids using inputs, set U and V values to missing everywhere.
   *
   * @param[in] templateGrid  A template Grid used mostly for dimensions
   * @param[in] ufield  Name of U field unless empty
   * @param[in] vfield  Name of V field unless empty
   * @param[in] units   Name of the units to use for U and V
   * @param[in] defaultUfield  Name of U field when ufield or vfield is empty
   * @param[in] defaultVfield  Name of V field when ufield or vfield is empty
   * @param[in] uvUnits  The units object to use
   *
   * If ufield is non empty, the U grid is given that as its name, 
   * otherwise, the U grid is given the name defUfield
   *
   * If vfield is non empty, the V grid is given that as its name, 
   * otherwise, the V grid is given the name defVfield
   *
   */
  UvOutput(const Grid &templateGrid, const std::string &ufield,
	   const std::string &vfield, const std::string &units,
	   const std::string &defaultUfield, const std::string &defaultVfield,
	   const UvUnits &uvUnits);

  /**
   * Constructor with U,V grids, units, and metadata passed in
   *
   * @param[in] u  U data grid to use
   * @param[in] v  V data grid to use
   * @param[in] uvUnits  The units to use
   * @param[in] xml  The metadata to use
   */
  UvOutput(const Grid &u, const Grid &v, const UvUnits &uvUnits,
	   const MetaDataXml &xml);

  /**
   * Destructor
   */
  virtual ~UvOutput(void);

  /**
   * @return true if objects are identical
   * @param[in] u  Object to compare to
   */
  bool operator==(const UvOutput &u) const;

  /**
   * Phase correction vector (PCV) compute, changes internal state
   * so that the U and V grids are the the PCV's
   *
   * @param[in] name  Name associated with data type that is used (arbitrary)
   * @param[in] lt  Lead time seconds
   * @param[in] verif  Verifying data grid
   * @param[in] fcst  Forecast data grid (the one that is moved to match verif)
   * @param[in] weight  Weight grid used to damp phase corrections
   * @param[in] parms  Parameters that control how the PCV's are computed
   * @param[in] proj  Projection Parameters 
   * @param[out] metadata  Metadata associated with the PCV computation, such
   *                       as type of phase correction (PcType)
   * @param[out] stats  Statistics on what is happening
   * @param[in,out] threads  Threads pointer, used to speed up the PC computation.
   *                         can be NULL
   *
   * @note the local grid U and V PCV's are in 'external' units on exit
   */
  void pcCompute(const std::string &name, int lt, const Grid *verif, const Grid *fcst,
		 const Grid *weight, const ParmSetUV &parms,
		 const ParmProjection &proj, MetaData &metadata,
		 PcStats &stats, ConvWxMultiThreadMgr *threads=NULL);

  /**
   * Change values to 'local' units if not already in local units, 
   * and set all points for which PCV vector length is near zero to zero
   *
   * Changing to local units modifies U and V.
   *
   * @param[in] parm  The phase correction computation params
   * @param[in] proj  Projection parameters
   */
  void setLocalCollapseZero(const ParmPcAlg &parm, const ParmProjection &proj);

  /**
   * Apply local PCV (U,V) at each point in an input grid to produce a
   * phase corrected output grid.  Whether or not the phase correction is
   * done or not depends on local values for p.c. type (metadata) and on
   * input params.
   *
   * @param[in] parms  Params for pc apply
   * @param[in] data  Data to phase correct
   * @param[out] pc  Phase corrected data
   */
  void pcApply(const ParmPcAlg &parms, const Grid &data, Grid &pc);

  /**
   * Apply local PCV (U,V) at each point in an input grid to produce a
   * phase corrected output grid.  Phase correction is always performed.
   *
   * Reduce the amount of correction by a weight passed in
   *
   * @param[in] parms  Params for pc apply
   * @param[in] weight  Scale factor to multiply local PCV before correcting
   *                    data
   * @param[in] data  Data to phase correct
   * @param[out] pc  Phase corrected data
   */
  void pcForceApply(const ParmPcAlg &parms, const double weight,
		    const Grid &data, Grid &pc);

  /**
   * Return the U,V grid dimensions 
   *
   * @param[out] nx  Grid dimension
   * @param[out] ny  Grid dimension
   */
  inline void getDim(int &nx, int &ny) const {return pUGrid.getDim(nx, ny);}

  /**
   * Set all the U,V values at all grid points to 0.0
   */
  void setAllZero(void);

  /**
   * @return True if U and V grids have magnitude zero or missing at all points
   */
  bool isZero(void) const;

  /**
   * Get U and V values at a point
   *
   * @param[in] x  The grid point X
   * @param[in] y  The grid point Y
   * @param[out] u  The U value at (x,y)
   * @param[out] v  The V value at (x,y)
   *
   * @return true If both values are non-missing, 
   *         false if either value is missing, and if so, set return u=v=0
   */
  bool getValuesOrZero(const int x,  const int y, double &u, double &v) const;

  /**
   * Set values for both U and V at a point
   *
   * @param[in] x  The grid point X
   * @param[in] y  The grid point Y
   * @param[out] u  The U value to set at (x,y)
   * @param[out] v  The V value to set at (x,y)
   */
  void setValuesAtPoint(const int x, const int y, const double u,
			const double v);

  /**
   * Set U grid and units to inputs
   * @param[in] uGrid  Grid to use as U 
   * @param[in] units   Units to replace in base class
   */
  void setUGrid(const Grid &uGrid, const UvUnits &units);

  /**
   * Set V grid and units to inputs
   * @param[in] vGrid  Grid to use as V
   * @param[in] units   Units to replace in base class
   */
  void setVGrid(const Grid &vGrid, const UvUnits &units);

  /**
   * Change names of U and V grids to inputs
   *
   * @param[in] uName  Name to use in U Grid
   * @param[in] vName  Name to use in V Grid
   */
  void setNames(const std::string &uName, const std::string &vName);

  /**
   * Return range of values found in U and V (maximum from either,
   * minimum from either)
   *
   * @param[out] minVal  Minimum value found in U and V
   * @param[out] maxVal  Maximum value found in U and V
   */
  void getRange(double &minVal, double &maxVal) const;

  /** 
   * Multiply U and V by values in an input grid.
   * - At each point (x,y):
   *    - U[x,y] = U[x,y]*multGrid[x,y]
   *    - V[x,y] = V[x,y]*multGrid[x,y]
   *
   * @param[in] multGrid  Grid to use as multiplier
   *
   * @return true for success
   */
  bool multiply(const Grid &multGrid);

  /** 
   * Multiply U and V by a fixed value.
   * - At each point (x,y):
   *    - U[x,y] = U[x,y]*scale
   *    - V[x,y] = V[x,y]*scale
   *
   * @param[in] scale  Value to multiply by
   */
  void multiply(const double scale);

  /** 
   * Multiply U and V by a fixed scale at one point.
   *    - U[x,y] = U[x,y]*scale
   *    - V[x,y] = V[x,y]*scale
   * @param[in] x  Point to modify
   * @param[in] y  Point to modify
   * @param[in] scale  Value to multiply by
   */
  void multiply(const int x, const int y, const double scale);

  /** 
   * multiply U and V, each by their own scale.
   * -  At each point (x,y):
   *      - U[x,y] = U[x,y]*uScale
   *      - V[x,y] = V[x,y]*vScale
   * @param[in] uScale  Value to multiply U grid by
   * @param[in] vScale  Value to multiply V grid by
   */
  void multiply(const double uScale, const double vScale);

  /**
   * Divide U and V by values in another grid
   * - At each point (x,y):
   *      - U[x,y] = U[x,y]/normGrid[x,y]
   *      - V[x,y] = V[x,y]/normGrid[x,y]
   *
   * @param[in] normGrid  Grid to use as divisor
   *
   * @return true for success
   */
  bool normalize(const Grid &normGrid);

  /**
   * Smooth U and V by applying sx by sy averaging filter at each point
   *
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   *
   * On exit  The data at x,y is the average of all data in the box bounded
   *          by [x-sx,y-sy],  [x+sx,y+xy]
   */
  void smooth(const int sx, const int sy);

  /**
   * Smooth U and V by applying sx by sy averaging filter at each point,
   * optionally excluding the special value 0.0 from the smoothing.
   *
   * @param[in] sx  Smoothing box size x
   * @param[in] sy  Smoothing box size y
   *
   * @param[in] excludeZero  True to exclude the value 0.0 from smoothing
   */
  void smooth(const int sx, const int sy, const bool excludeZero);

  /**
   * Reduce resolution of U, V Grids by a factor.
   *
   * @param[in] res  Reduction, 1 = no resolution reduction, 2 = 
   *                 resolution reduction by a factor of 2, and so on.
   *
   * @return true if the local grids have a reduced resolution 
   *
   * @note see GridData::reduce()
   */
  bool reduce(const int res);

  /**
   * Create and return a magnitude grid.
   * @return  Grid with  magnitude at each point
   * 
   * The magnitude at x,y = sqrt(U(x,y)*U(x,y) + V(x,y)*V(x,y))
   */
  Grid magnitude(void) const;

  /**
   * @return reference to the U grid
   */
  const Grid &getU(void) const;

  /**
   * @return reference to the V grid
   */
  const Grid &getV(void) const;

  /**
   * Interpolate input U,V grids (assumed at a lower resolution than the
   * local grids) back onto local grids using a bilinear interpolation method
   *
   * @param[in] lowresMotion   The low res grid to interpolate
   * @param[in] res  Resolution factor, 1 = lowresMotion and local grids
   *                 should have
   *                 same dimensions, a simple copy, 2 = lowresModion input
   *                 grids should have half resolution, and so on.
   *
   * @return  True if the interpolation was successful,
   *          false if inputs were not as expected.
   *
   * @note  Expect (local nx) = (input nx)*res, (local ny)=(input ny)*res
   * @note  See GridData::interpolate()
   */
  bool interpolate(const UvOutput &lowresMotion, const int res);

  /**
   * Apply a 2d vector max filter to the local grids
   * @param[in] boxSize  Box size x and y
   *
   * @note For details see UvOutput::vectorMaxFast(). This method
   *       is here for backwards compatibility. The "Fast" method is
   *       needed for efficiency.
   */
  void expand(const int boxSize);

  /**
   * Apply a sx by sy 2d vector max filter to the local grids
   *
   * @param[in] sx  Box size x
   * @param[in] sy  Box size y
   *
   * At each point (x,y) replace the data in both U and V grids by the values
   * at that point within an sx by sy box around (x,y) with the maximum vector
   * magnitude  |U(x,y),V(x,y)|.
   *
   * @note this is a fast (i.e. complex) technique, needed for efficiency
   */
  void vectorMaxFast(const int sx, const int sy);

  /**
   * Return a Grid of convergence values and partial derivative Grids
   * derived from U,V Grid data
   *
   * @param[out] conv  Convergence Grid (external units)
   * @param[out] du   dU/dx Grid (change per km)
   * @param[out] dv   dV/dy Grid (change per km)
   * @param[in] proj Projection information used to compute convergence
   *
   * - at each point x.y
   *  - du = U[x+1,y] - U[x-1,y]
   *  - dv = V[x,y+1] - V[x,y-1]
   *  - conv[x,y] = -(du+dv)
   *  - dv[x,y]  = -1*dv[x,y]
   *  - du[x,y]  = -1*du[x,y]
   */
  void convergence(Grid &conv, Grid &du, Grid &dv,
		   const ParmProjection &proj) const;
  /**
   * Adjust local U,V using NCAR convergence thresholded UV smoothing technique.
   *
   * @param[in] convThresh  The threshold to use
   * @param[in] nptToExpansion  ConvWxFuzzy from # of points to expansion amount
   * @param[in] du  Partial derivative grid (output of convergence method)
   * @param[in] dv  Partial derivative grid (output of convergence method)
   *
   * @return true for success
   *
   * - for each y
   *   - look for runs in x where du exceeds threshold from [x0,y] to [x1,y]
   *      - width = [x1-x0+1]*expansion_factor
   *      - for a moving window of size 2*width+1 centered at x0-width up to
   *        x1+width:
   *          - U[center,y] = average of U values in window 
   * - for each x
   *      - look for runs in y where dv exceeds threshold from [x,y0] to [x,y1]
   *      - width = [y1-y0+1]*expansion_factor
   *      - for a moving window of size 2*width+1 centered at y0-width up to
   *        y1+width:
   *          - V[x, center] = average of V values in window
   */
  bool convThreshFilter(const double convThresh,
			const ConvWxFuzzy &nptToExpansion,
			const Grid &du, const Grid &dv);

  /**
   * Replace data at all places where the U,V vector magnitude <= threshold or
   * U,V is missing with the values from an input UvOutput object.
   *
   * @param[in] replacementUV  Input U,V to use for replacement under these
   *                             circumstances
   * @param[in] threshold  Vector magnitude threshold to use
   */
  void replaceLeThreshOrMissing(const UvOutput &replacementUV,
				const double threshold);

  /**
   * Multiply local UV by a factor (time tapering)
   * @param[in] weight  Weight to multiply by
   *
   * @note this is equivalent to another method 
   *       UvOutput::multiply()
   */
  void timeTaper(const double weight);

  /**
   * Return maximum vector magnitude sqrt(U*U + V*V) in a box
   *
   * @param[in] x0 Lower left of box
   * @param[in] y0  Lower left of box
   * @param[in] nx  Box number of points x
   * @param[in] ny  Box number of points y
   * @return maximum magnitude, or 0.0 if all data in the box is missing
   */
  double maxMagnitudeInBox(const int x0, const int y0, const int nx,
			   const int ny) const;


  /**
   * Return the mean U,V values in a box
   *
   * @param[in] x0 Lower left of box
   * @param[in] y0  Lower left of box
   * @param[in] nx  Box number of points x
   * @param[in] ny  Box number of points y
   * @param[out] ubar  Mean of U values in box
   * @param[out] vbar  Mean of V values in box
   *
   * @return true for success
   *
   * @note returns false if all data in the box is missing
   */
  bool meanInBox(const int x0, const int y0, const int nx, 
		 const int ny, double &ubar, double &vbar) const;

  /**
   * Return the variance of U,V in a box when means are passed in.
   *
   * @param[in] x0 Lower left of box
   * @param[in] y0  Lower left of box
   * @param[in] nx  Box number of points x
   * @param[in] ny  Box number of points y
   * @param[in] ubar  Mean of U values in box
   * @param[in] vbar  Mean of V values in box
   *
   * @return variance, or 0.0 if all data in the box is missing
   */
  double varianceInBox(const int x0, const int y0, const int nx, 
		       const int ny, const double &ubar, 
		       const double &vbar) const;

  /**
   * Return variance of U,V in a box when means are not passed in.
   *
   * @param[in] x0 Lower left of box
   * @param[in] y0  Lower left of box
   * @param[in] nx  Box number of points x
   * @param[in] ny  Box number of points y
   *
   * @return variance, or 0.0 if all data in the box is missing
   */
  double varianceInBox(const int x0, const int y0, const int nx,
		       const int ny) const;

  /**
   * Copy from input U,V data into local U,V, within a box.
   *
   * @param[in] inputUV  Input to copy from 
   * @param[in] x0  Lower left of box
   * @param[in] y0  Lower left of box
   * @param[in] nx  Box number of points x
   * @param[in] ny  Box number of points y
   */
  void copyBox(const UvOutput &inputUV, const int x0, const int y0,
	       const int nx, const int ny);

  /**
   * Multiply UV values by a scale within a box
   *
   * @param[in] x0  Lower left of box
   * @param[in] y0  Lower left of box
   * @param[in] nx  Box number of points x
   * @param[in] ny  Box number of points y
   * @param[in] scale  Scale factor
   */
  void normalizeInBox(const int x0, const int y0, const int nx, 
		      const int ny, const double scale);

  /**
   * Increment U,V values at a point
   *
   * @param[in] x  Point at which to increment
   * @param[in] y  Point at which to increment
   * @param[in] iu  Value to add to U at the point
   * @param[in] iv  Value to add to V at the point
   *
   * @note The increment value inputs are integer valued intentionally
   */
  void incrementValueAtPoint(const int x, const int y, const int iu,
			     const int iv);

  /**
   * Append local U and V Grid data to a MultiGrid, setting the names
   *
   * @param[in] uname  Name to give U in the MultiGrid
   * @param[in] vname  Name to give V in the MultiGrid
   * @param[in] dxKm  Kilometers per grid point (x)
   * @param[in] dyKm  Kilometers per grid point (x)
   * @param[in,out] grids  MultiGrid into which U and V are appended
   *
   * @note  The grids put into the MultiGrid are given units meters/second
   *        which is the external units. If this is not the local units state,
   *        the values in U and V that are appended are modified to have
   *        the correct units
   */
  void appendOutput(const std::string &uname, const std::string &vname,
		    const double dxKm,  const double dyKm,
		    MultiGrid &grids) const;

  /**
   * Append local U and V Grid data to a Multigrid , using names already
   * in the local objects
   *
   * @param[in] dxKm  Kilometers per grid point (x)
   * @param[in] dyKm  Kilometers per grid point (x)
   * @param[in,out] grids  MultiGrid into which U and V are appended
   *
   * @note  The grids put into the MultiGrid are given units meters/second
   *        which is the external units. If this is not the local units state,
   *        the values in U and V that are appended are modified to have
   *        the correct units
   */
  void appendOutput(const double dxKm,  const double dyKm,
		    MultiGrid &grids) const;

  /**
   * return individual grids, converted to external units if needed.
   *
   * @param[in] dxKm  Kilometers per grid point (x)
   * @param[in] dyKm  Kilometers per grid point (x)
   * @param[out] uGrid  copy of local U grid, with units changed as needed
   *                    to 'external'
   * @param[out] vGrid  copy of local V grid, with units changed as needed
   *                    to 'external'
   */
  void getOutput(const double dxKm, const double dyKm, Grid &uGrid,
		 Grid &vGrid) const;

  /**
   * Print U and V values at a point to stdout
   *
   * @param[in] name  Name to put into print output
   * @param[in] x  The point is x[0]
   * @param[in] y  The point is y[0]
   */
  void debugAtPoint(const std::string &name, const int *x,
		    const int *y) const;

  /**
   * Convert a Grid from 'external' units to 'local' units
   *
   * @param[in] kmPerGridpt  Grid resolution (km)
   * @param[in,out] grid  Grid to convert
   */
  static void gridExternalToLocal(const double kmPerGridpt, Grid &grid);

  /**
   * Convert a Grid from 'local' units to 'external' units
   *
   * @param[in] kmPerGridpt  Grid resolution (km)
   * @param[in,out] grid  Grid to convert
   */
  static void gridLocalToExternal(const double kmPerGridpt, Grid &grid);

  /**
   * Convert an 'external' value to 'local' units
   *
   * @param[in] value  A value in external units
   * @param[in] kmPerGridpt  Grid resolution (km)
   *
   * @return The local value
   */
  static double externalToLocal(const double value, const double kmPerGridpt);

  /**
   * Convert a 'local' value to 'external' units
   *
   * @param[in] value  A value in local units.
   * @param[in] kmPerGridpt  Grid resolution (km)
   *
   * @return The external value 
   */
  static double localToExternal(const double value, const double kmPerGridpt);

protected:
private:  

  Grid pUGrid;  /**< U grid */
  Grid pVGrid;  /**< V grid */
  bool pUvAllZero;  /**< true if all zeros or missing in u and v */

  /**
   * @struct UvMax_t
   * @brief Structure used to figure out maximum magnitude of u,v vectors
   */
  typedef struct
  {
    bool valuesSet; /**< true if the other values have been set */
    double uvMagnitude; /**< magnitude of UV vector */
    double uValue;   /**< u value */
    double vValue;   /**< v value */
    int gridX;      /**< grid index x */
    int gridY;      /**< grid index y */
  } UvMax_t; 

  /**
   * Build up vector max
   *
   * @param[in] gridTraverse  Object used to traverse magnitude grid magGrid
   * @param[in] magGrid  The magnitude of U,V vector at each point
   * @param[in,out] maxStatus  Updated to follow the max (it is set with values
   *                           at the max)
   * @param[in,out] outUv  Vector max grid, filled in as we go
   * @param[in,out] counts  Counter for what is happening in the algorithm
   *
   * @note The GridTraverse2d object is a moving box, and inside the
   * box is where we do the maximization stuff.
   *
   * @note This is complex (efficient)
   *
   */
  void pVectorMax1(const GridTraverse2d &gridTraverse, const Grid &magGrid,
		   UvMax_t &maxStatus, UvOutput &outUv,
		   VectorMaxCounts &counts);

  /**
   * @return true if the maxStatus should be adjusted, false if a new
   *         maxStatus should be figured out from an empty state.
   *
   * @param[in] maxStatus  Set to have values at the maximum
   * @param[in] gridTraverse  Object used to traverse
   * @param[in,out] counts  Counter for what is happening in the algorithm
   *
   * @note This is complex (efficient)
   */
  bool pVectorMaxShouldAdjust(const UvMax_t &maxStatus,
			      const GridTraverse2d &gridTraverse,
			      VectorMaxCounts &counts) const;

  /**
   * Set maxStatus using inputs
   *
   * @param[in] magGrid  The magnitude of U,V at each point
   * @param[in] gridTraverse   Object used to traverse the grid
   * @param[out] maxStatus  If it can be done, maximum information is set into
   *                        this structure
   * @param[out] outUv  Adjusted on exit to have the maximum in the 
   *                    center point of the current traverse box
   *
   * @note This is complex (efficient)
   */
  void pVectorMaxSet(const Grid &magGrid, const GridTraverse2d &gridTraverse,
		     UvMax_t &maxStatus, UvOutput &outUv);

  /**
   * Adjust maximum using inputs
   *
   * @param[in] name  Debug name for prints
   * @param[in] magGrid  The magnitude of U,V at each point
   * @param[in] range  The range of values for the box to use
   * @param[in] hasRange  True if box range is o.k.
   * @param[in] gridTraverse  Traverse object, used to define points to take
   *                          maximum
   * @param[out] maxStatus Adjusted on exit as needed to indicate the maximum
   * @param[out] outUv  Adjusted on exit to have the maximum in the 
   *                    center point of the current traverse box
   *
   * @note This is complex (efficient)
   */
  void pVectorMaxAdjust(const std::string &name, const Grid &magGrid,
			const GridTraverse2d::Range_t &range,
			const bool hasRange,
			const GridTraverse &gridTraverse, UvMax_t &maxStatus,
			UvOutput &outUv) ;

  /**
   * Adjust maximum stat at a point
   * @param[in] x  The point
   * @param[in] y  The point
   * @param[in] v  The value at the point
   * @param[out] maxStatus  If value is a new maximum, adjusted to indicate so.
   */
  void pVectorMaxAdjustStat(const int x, const int y, const double v,
			    UvMax_t &maxStatus);

  /**
   * Set a special value into an output Grid where U and V are 'moving away'
   * (i.e. where U and/or V are nonzero).  Set the same value into a mask
   * grid at the same points.
   * 
   * @param[in] x  Point to evaluate
   * @param[in] y  Point to evaluate
   * @param[in] uvWeight  Weight applied to U,V values prior to the action
   * @param[in] value  The special value to use
   * @param[out] maskGrid  Mask grid in which to put value where moving away.
   * @param[out] outGrid  Grid in which to put special value where moving away.
   *
   */
  void pSetMaskandOutput(const int x, const int y, const double uvWeight,
			 const double value, Grid &maskGrid,
			 Grid &outGrid) const;

  /**
   * Do the phase correction of a data grid using local U,V as phase 
   * correction vectors.  After phase correction, do filtering to the data:
   *   - fillGaps   (see GridData::fillGaps())
   *   - smooth (several choices depending on parameters)
   *   -   high smooth  or not. (See GridData::highSmooth())
   *   -   smooth only where phase corrected or not.
   *
   * @param[in] uvWeight  Weight to apply to U,V values before phase correction
   * @param[in] gapFill  Argument to GridData::fillGaps() filter
   * @param[in] smooth   Smoothing box size (x and y # of points)
   * @param[in] useHighSmooth  True to smooth using the high smoothing algorithm
   * @param[in] smoothWhereCorrected  True to smooth only where phase
   *                                  correction was applied.
   * @param[in] isExclude  True to exclude a particular value from smoothing
   * @param[in] excludeValue  The value to exclude from smoothing when isExclude
   * @param[in,out] dataGrid  The Grid to phase correct
   *
   * @note This is the main phase correction grid transformation method.
   *
   * @note U and V are assumed in 'PURE_PC_GRIDPOINTS' units, this is
   *       not checked locally.
   *
   */
  void pPhaseCorrect(const double uvWeight, const int gapFill[2],
		    const int smooth[2], const bool useHighSmooth,
		    const bool smoothWhereCorrected,
		    const bool isExclude, const double excludeValue,
		    Grid &dataGrid) const;

  /**
   * Set phase corrected data into an output Grid at the 'to' point, and set 
   * mask data to a special value at this destination location (where you
   * phase correct 'to')
   *
   * @param[in] x  The point to phase correct at ('from')
   * @param[in] y  The point to phase correct at ('from')
   * @param[in] uvWeight  Weight to apply to U,V values before correction
   * @param[in] dataGrid  The data to be phase corrected.
   * @param[in] value  The special value to put into the mask grid at 'to'
   * @param[out] maskGrid  The mask grid
   * @param[out] outGrid  The phase corrected data grid, updated at the 'to'
   *                      location.
   * 
   */
  void pPhaseCorrect1(const int x, const int y, const double  uvWeight, 
		      const Grid &dataGrid, const double value,
		      Grid &maskGrid, Grid &outGrid) const;

  /**
   * @return True if U/V at x,y are not both missing or zero after multiplying
   * by a weight, and return the values
   *
   * @param[in] x  The point to check
   * @param[in] y  The point to check
   * @param[in] uvWeight Weight to multiply U and V by
   * @param[out] u  Return value (U(x,y)*uvWeight)
   * @param[out] v  Return value (V(x,y)*uvWeight)
   */
  bool pNonZeroUV(const int x, const int y, const double uvWeight, 
		  double &u, double &v) const;

  /**
   * @return True if input grids have the same dimensions as local
   * U,V grids, and return those dimensions.
   *
   * @param[in] duGrid  Grid to check against
   * @param[in] dvGrid  Grid to check against
   * @param[out] nx  Grid dimension
   * @param[out] ny  Grid dimension
   */
  bool pGetConsistentDimensions(const Grid &duGrid, const Grid &dvGrid,
				int &nx, int &ny) const;

  /**
   * Compute partial derivatives at a point
   *
   * @param[in] x0  The point 
   * @param[in] y0  The point
   * @param[in] nx  Grid dimension used to check bounds
   * @param[in] ny  Grid dimension used to check bounds
   * @param[out] du  Partial derivative in X
   * @param[out] dv  Partial derivative in Y
   *
   * @return true for success
   *
   * The formula used is: 
   *  - dv = (V[x0,   y0+1] - V[x0  , y0-1])/3
   *  - du = (U(x0+1, y0  ] - U[x0-1, y0  ])/3
   */
  bool pConvergence1(const int x0, const int y0, const int nx, const int ny,
		     double &du, double &dv) const;


  /**
   * Append input U and V Grid data to a MultiGrid.
   *
   * @param[in] dxKm  Kilometers per grid point (x)
   * @param[in] dyKm  Kilometers per grid point (y)
   * @param[in,out] uGrid  U grid to append after units conversion
   * @param[in,out] vGrid  V grid to append after units conversion
   * @param[in,out] outGrids  MultiGrid to append U and V to
   *
   * @note Input grids put into the MultiGrid are given units M/S
   * which is the external units, and values are changed as needed. The
   * changed Grid data is returned in the uGrid/vGrid objects.
   */
  void pAppend(const double dxKm, const double dyKm,
	       Grid &uGrid, Grid &vGrid, MultiGrid &outGrids) const;

  /**
   * @return number of 'seconds' in 'external' units.
   *
   * UvOutput data is
   * written out is if the phase correction is a time shift by this many
   * seconds, so external units are meters per 'second'
   */
  static int pExternalSeconds(void);

};

# endif
