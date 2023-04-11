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
 * @file ParmSetUV.hh
 * @brief Parameters for setting phase correction UV vectors
 * @class ParmSetUV
 * @brief Parameters for setting phase correction UV vectors
 *
 *
 * Has a ParmConv base class, which has the convergence parameters
 * (convergence is part of the setting of phase correction UV)
 *
 * See scientific phase correction documentation for more detailed
 * parameter descriptions
 */

# ifndef    PARM_SET_UV_HH
# define    PARM_SET_UV_HH

#include <string>
#include <vector>
#include <ConvWx/ParmConv.hh>
#include <ConvWx/ParmPcFcst.hh>

class ParmProjection;
class Grid;
class FcstGrid;

//---------------------------------------------------------------
class ParmSetUV : public ParmConv
{
public:

  /**
   * default constructor. values are not set
   */
  ParmSetUV(void);

  /**
   * Constructor with all values passed in, except Dxkm and DyKm
   * which are set through another method (storeGridDim()).
   * 
   * @param[in] fcst  Parameters for one or more forecast data types
   * @param[in] fcstInputSmooth   box size for smoothing of forecast data
   *                              (number of grid points)
   * @param[in] verifInputSmooth  box size for smoothing of verification data
   *                              (number of grid points)
   * @param[in] lowResNgridpts  See external documentation for meaning
   * @param[in] volSize  See external documentation for meaning
   * @param[in] volOverlap  See external documentation for meaning
   * @param[in] shiftResNpt  See external documentation for meaning
   * @param[in] maxShift  See external documentation for meaning
   * @param[in] refineShiftResNpt  See external documentation for meaning
   * @param[in] refineMaxShift  See external documentation for meaning
   * @param[in] lowResMotionSmooth  See external documentation for meaning
   * @param[in] nLowResMotionSmooth  Number of values in lowResMotionSmooth
   * @param[in] highResMotionSmooth  See external documentation for meaning
   * @param[in] nHighResMotionSmooth  Number of values in highResMotionSmooth
   * @param[in] fcstGapFillSmooth  See external documentation for meaning
   * @param[in] nFcstOutputSmooth  Number of values in fcstOutputSmooth
   * @param[in] fcstOutputSmooth  See external documentation for meaning
   * @param[in] excludeZeroFromSmoothing  See external documentation for meaning
   * @param[in] highResMaxExpandNpt  See external documentation for meaning
   * @param[in] goodScaling  See external documentation for meaning
   * @param[in] goodDistScaling  See external documentation for meaning
   * @param[in] fractionalAreaMinPcnt  See external documentation for meaning
   * @param[in] gridAreaMinPcnt  See external documentation for meaning
   * @param[in] fuzzyGridDist  See external documentation for meaning
   */
  ParmSetUV(const std::vector<ParmPcFcst> &fcst, const int fcstInputSmooth,
	    const int verifInputSmooth, const int lowResNgridpts,
	    const int *volSize, const int *volOverlap, const int shiftResNpt,
	    const int maxShift, const int refineShiftResNpt,
	    const int refineMaxShift, const int nLowResMotionSmooth,
	    const int *lowResMotionSmooth, const int nHighResMotionSmooth,
	    const int *highResMotionSmooth, const int fcstGapFillSmooth,
	    const int nFcstOutputSmooth, const int *fcstOutputSmooth,
	    const bool excludeZeroFromSmoothing, const int highResMaxExpandNpt,
	    const double goodScaling, const double goodDistScaling,
	    const double fractionalAreaMinPcnt, const double gridAreaMinPcnt,
	    const ConvWxFuzzy &fuzzyGridDist);


  /**
   * Destructor
   */
  virtual ~ParmSetUV(void);

  /**
   * Debug print
   */
  void print(void) const;

  /**
   * Set the values for members pDxKm and pDyKm from inputs
   * @param[in] dxKm  Value to use for pDxKm
   * @param[in] dyKm  Value to use for pDyKm
   */
  bool storeGridDim(const double dxKm, const double dyKm);

  /**
   * @return  Number of shifts per volume box (2*pMaxShift+1)
   */
  int numScore(void) const;

  /**
   * @return Offset to a particular phase shift
   * @param[in] i  Index 0,1,..numScore-1
   */
  int ithOffset(const int i) const;

  /**
   * @return Names of all forecasting types that are configured for
   */
  std::vector<std::string> getFcstNames(void) const;

  /**
   * @return Parameters for a named forecasting type
   * @param[in] name  The forecasting type name
   */
  ParmPcFcst matchingFcstParms(const std::string &name) const;

  /**
   * @return ALL the forecast data generation time/lead pairs that are valid at
   * a particular time
   * @param[in] vt  The valid time
   */
  std::vector<std::pair<time_t,int> > fcstsValidAt(const time_t &vt) const;

  std::vector<ParmPcFcst> pFcst; /**< one or more forecast data types */
  int pFcstInputSmooth; /**< see external documentation */
  int pVerifInputSmooth;/**< see external documentation */
  int pLowResNgridpts;/**< see external documentation */
  int pVolSize[2];/**< see external documentation */
  int pVolOverlap[2];/**< see external documentation */
  int pShiftResNpt;/**< see external documentation */
  int pMaxShift;/**< see external documentation */
  int pRefineShiftResNpt;/**< see external documentation */
  int pRefineMaxShift;/**< see external documentation */
  std::vector<int> pLowResMotionSmooth;/**< see external documentation */
  std::vector<int> pHighResMotionSmooth;/**< see external documentation */
  int pFcstGapFillSmooth;/**< see external documentation */
  std::vector<int> pFcstOutputSmooth;/**< see external documentation */
  bool pExcludeZeroFromSmoothing;/**< see external documentation */
  int pHighResMaxExpandNpt;/**< see external documentation */
  double pGoodScaling;/**< see external documentation */
  double pGoodDistScaling;/**< see external documentation */
  double pFractionalAreaMinPcnt;/**< see external documentation */
  double pGridAreaMinPcnt;  /**< See external documentation */
  ConvWxFuzzy pFuzzyGridDist;/**< see external documentation */

  double pDxKm;/**< km per grid point x */
  double pDyKm; /**< km per grid point y */

protected:
private:  
};

# endif
