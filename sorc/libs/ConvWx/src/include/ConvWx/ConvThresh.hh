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
 * @file ConvThresh.hh 
 * @brief the convergence threshold algorithm to reduce phase correction
 * @class ConvThresh
 * @brief the convergence threshold algorithm to reduce phase correction
 * 
 * Convergence thresholding is used to reduce phase correction UV and prevent it
 * from 'shrinking' features in regions of convergence
 */

#ifndef CONV_THRESH_H
#define CONV_THRESH_H
#include <ConvWx/ParmConv.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/UvOutput.hh>

class MultiGrid;

//----------------------------------------------------------------
class ConvThresh
{
public:
  /**
   * Empty constructor, parameters are not set
   */
  ConvThresh(void);

  /**
   * Constructor - copies parameter values into local state
   *
   * @param[in] parm  The convergence parameters to copy in
   */
  ConvThresh(const ParmConv &parm);

  /**
   * Destructor
   */
  virtual ~ConvThresh(void);

  /** 
   * Apply the convergence threshold algorithm to inputs to produce outputs
   *
   * The algorithm:
   *   - UvOutput::convergence() to get convergence, and partial derivatives
   *   - UvOutput:convThreshFilter() to reduce U and V
   *
   * @param[in] unfilteredPcv  Input phase correction vector grid
   * @param[in] proj  Projection info.
   * @param[out] filteredPcv  The phase correction vector grid after
   *                          convergence filtering
   */
  void apply(const UvOutput &unfilteredPcv, const ParmProjection &proj,
	     UvOutput &filteredPcv);

protected:
private:

  /**
   * The convergence algorithm parameters
   */
  ParmConv pParm;
};

#endif
