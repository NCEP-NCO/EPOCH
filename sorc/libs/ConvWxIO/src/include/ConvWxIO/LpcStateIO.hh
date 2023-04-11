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
 * @file LpcStateIO.hh 
 * @brief Lagrangian Phase Correction State information for a particular model
 *        run, including latency adjustment.
 *
 * @class LpcStateIO
 * @brief Lagrangian Phase Correction State information for a particular model
 *        run, including latency adjustment.
 *
 * The state information describes a particular model run, and is used by the
 * Lagrangian Phase Correction algorithm.
 *
 * The state information is contained in the base class LpcState.
 *
 * @note This class queries a server to determine availability status
 *       information for individual forecasts, and uses that information to
 *       initialize the base class LpcState
 */

#ifndef LPC_STATE_IO_H
#define LPC_STATE_IO_H

#include <ConvWx/LpcState.hh>

class ParmFcst;
class ParmFcstIO;

//----------------------------------------------------------------
class LpcStateIO : public LpcState
{
public:

  /**
   * Empty constructor
   */
  LpcStateIO(void);

  /**
   * Constructor, queries the server to fill in state
   *
   * @param[in] inputModelParms  Input model data parameters. The lead times
   *                             are used to build the input status vector.
   * @param[in] outputModelParms  Output (latency adjusted) model data
   *                              parameters. The lead times are used to build
   *                              the output status vector.
   * @param[in] modelGt  Input model generation time.
   * @param[in] outputGt  Output latency adjusted generation time.
   */
  LpcStateIO(const ParmFcstIO &inputModelParms,
	     const ParmFcst &outputModelParms,
	     const time_t &modelGt, const time_t &outputGt);

  /**
   * Destructor
   */
  virtual ~LpcStateIO(void);


  /**
   * Reinitialize, queries the server to fill in state
   *
   * @param[in] inputModelParms  Input model data parameters. The lead times
   *                             are used to build the input status vector.
   * @param[in] outputModelParms  Output (latency adjusted) model data
   *                              parameters. The lead times are used to build
   *                              the output status vector.
   * @param[in] modelGt  Input model generation time.
   * @param[in] outputGt  Output latency adjusted generation time.
   */
  void reInitialize(const ParmFcstIO &inputModelParms,
		    const ParmFcst &outputModelParms,
		    const time_t &modelGt, const time_t &outputGt);

protected:
private:

};

#endif
