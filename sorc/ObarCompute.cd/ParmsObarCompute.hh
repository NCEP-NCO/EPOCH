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
 * @file ParmsObarCompute.hh
 * @brief Parms
 * @class ParmsObarCompute
 * @brief Parms
 */

#ifndef ParmsObarCompute_hh
#define ParmsObarCompute_hh

#include <ConvWx/ParmMain.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmTiling.hh>
#include <Epoch/TileInfo.hh>
#include <ConvWx/ParmFcst.hh>
#include <vector>

class ParmsObarCompute
{
public:

  ParmMain _main;          /**< Standard main parms */
  ParmProjection _proj;    /**< Projection parms */
  ParmTiling _tiling;      /**< Tiling info */
  TileInfo _tileInfo;      /**< Tiling specification */
  ParmFcst _obs;           /**< Obs gridded data params */
  std::string _obarSpdb;   /**< Obar database URL */
  std::string _inputField;  /**< gridded field name */
  std::vector<double> _obsThreshold;    /**< Observation threshold to compute obar*/
  
protected:
private:
};
  
#endif
