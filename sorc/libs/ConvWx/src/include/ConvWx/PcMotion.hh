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
 * @file PcMotion.hh
 * @brief Compute the averages for phase correction on a UV grid
 * @class PcMotion
 * @brief Compute the averages for phase correction on a UV grid
 *
 * Has a UvOutput grid as a base class in which averages are accumulated.
 * Adds phase correction U,V vectors one PcVolume at a time, then normalizes
 * the results when done.
 */

# ifndef    PC_MOTION_HH
# define    PC_MOTION_HH
#include <ConvWx/UvOutput.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/PcType.hh>

class UvUnits;
class MultiGridsForPc;
class PcVolume;
class MetaData;

//----------------------------------------------------------------
class PcMotion : public UvOutput
{
public:

  /**
   * Constructor with an unknown grid (unknown dimensions).
   *
   * Initialize the units of UvOutput base class to input.
   * Set the phase correction type to PcType::UNKNOWN
   *
   * @param[in] units Units used to initialize with
   */
  PcMotion(const UvUnits &units);

  /**
   * Constructor for known grid dimensions.
   *
   * Fills U,V grids with 0.0 everywhere.
   * The phase correction type is set to PcType::UNKNOWN, 
   * Initialize the units of UvOutput base class to input.
   *
   * @param[in] nx  Grid dimension
   * @param[in] ny  Grid dimension
   * @param[in] units  Units used to initialize with
   */
  PcMotion(const int nx, const int ny, const UvUnits &units);
  
  /**
   * Destructor
   */
  virtual ~PcMotion(void);

  /**
   * Initialize the local state using the input object as a template
   *
   * @param[in] templateData  object with hopefully one good grid to use as
   *                          a template for the local grids
   *
   * Fills U,V grids with 0.0 everywhere.
   * The phase correction type is set to PcType::UNKNOWN, 
   */
  void init(const MultiGridsForPc &templateData);

  /**
   * Reduce resolution of Grid data in this object by a factor 
   *
   * @param[in] reduction  Reduction, 1 = no resolution reduction, 2 = 
   *                       resolution reduction by a factor of 2, and so on.
   *
   * @return true For success, false for some problem and no change
   */
  bool pcmotionReduce(const int reduction);

  /**
   * Interpolate input grids (assumed lower resolution than local grids)
   * back onto local grids,  using the GridData::interpolate() method
   *
   * @param[in] lowresGrids  The low resolution grids to interpolate
   * @param[in] res  Resolution factor, 1 = low resolution and local grids
   *                 should have
   *                 same dimensions, a simple copy, 2 = low resolution
   *                 input grid should have half resolution, and so on.
   *
   * @return true  If the interpolation was successful
   */
  bool pcmotionInterpolate(const PcMotion &lowresGrids, const int res);

  /**
   * Add contents of an input PcVolume to the local object, modifying
   * the base class U,V and the counter Grid pN
   *
   * @param[in] volume   Offset and volume location information
   *
   * For each point in the PcVolume add its offset to the 'from' locations
   * in U,V, and increment the counter by 1.0 at these locations.
   *
   * For volume point x,y if the volume offset is dx, dy, the 'from' point
   * is x-dx, y-dy.
   *
   * Think of this as moving the volume over by -dx,-dy, then 
   * changing UV and counter data in the shifted box.
   */
  void addVolume(const PcVolume &volume);

  /**
   * Divide the UVoutput base class U,V Grid data by the values in the pN Grid
   */
  void pcNormalize(void);

  /** 
   * Perform Two weight adjustments to the U,V data.
   * First, multiply U,V values by input weight values at each point
   *  -  UV(x,y) = UV(x,y)*weight(x,y)
   * Second, use the adjusted UV's to reduce the motion UV(x,y) using the
   * weights at the 'to' location:
   *
   *  -  UV(x,y) = UV(x,y)*weight(x+U(x,y), y+V(x,y))
   *
   * @param[in] weightGrid  Grid with weights
   *
   * @return true for success
   */
  bool pcMultiply(const Grid &weightGrid);

  /** 
   * Set U,V to 0.0 everywhere, and set the type to an input value
   * @param[in] pcType   Phase correction type
   */
  void setToZero(const PcType::Pc_t pcType);

  /**
   * Set phase correction type 
   * @param[in] pcType   Phase correction type to use
   */
  void setPhaseCorrect(const PcType::Pc_t pcType);

  /**
   * Create and return Metadata that has the local phase correction type
   * and units.
   * 
   * @return  The created metadata
   */
  MetaData getMetadata(void) const;

  /**
   * Perform phase correction using local U,V UvOutput, with post correction
   * gap filling and smoothing
   *
   * @param[in] gapFill Parameter for gap filling (see UvOutput)
   * @param[in] smoothSize  Smoothing box sizes, any number, with smoothing
   *                        filtering performed in the order provided.
   * @param[in,out] dataGrids  The data to phase correct
   */
  void phaseShift(const int gapFill, const std::vector<int> &smoothSize,
		  MultiGridsForPc &dataGrids);
				  
protected:
private:  

  Grid pCounterGrid;  /**< a counter grid used in normalize and add methods */
  PcType::Pc_t pPcType; /**< phase correction type set in setPhaseCorrect */
};

# endif
