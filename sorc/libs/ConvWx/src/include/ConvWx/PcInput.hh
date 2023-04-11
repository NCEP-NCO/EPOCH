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
 * @file PcInput.hh
 * @brief The input verification (observation) and forecast grids used for phase
 *        correction
 * @class PcInput 
 * @brief The input verification (observation) and forecast grids used for phase
 *        correction
 *
 * Also contains parameters used in phase correction for this data type
 *
 * There can be one or more input data types, each will have one of these
 * objects
 */

# ifndef    PC_INPUT_HH
# define    PC_INPUT_HH

#include <ConvWx/Grid.hh>
#include <ConvWx/ParmPcFcst.hh>

//----------------------------------------------------------------
class PcInput
{
public:

  /**
   * Store inputs into local objects
   *
   * @param[in] name  Name used for logging output
   * @param[in] verifGrid  The verifying observation grid
   * @param[in] fcstGrid  The forecast grid
   * @param[in] parms  Parameters
   */
  PcInput(const std::string &name, const Grid *verifGrid, const Grid *fcstGrid,
	  const ParmPcFcst &parms);

  /**
   * Destructor
   */
  virtual ~PcInput(void);

  /**
   * @return name of this object
   */
  inline std::string getName(void) const {return pName;}

  /**
   * @return pointer to the verification grid
   */
  inline const Grid *getVerifPtr(void) const { return pVerifGrid;}

  /**
   * @return pointer to the forecast grid
   */
  inline const Grid *getFcstPtr(void) const { return pFcstGrid;}

  /**
   * @return parameter object
   */
  inline ParmPcFcst getParms(void) const {return pParm;}

protected:

private:  

  const Grid *pVerifGrid;   /**< the verification grid */
  const Grid *pFcstGrid;    /**< the forecast grid */
  ParmPcFcst pParm;         /**< parameters that associate the two grids */
  std::string pName;        /**< a name for this object */
};

# endif
