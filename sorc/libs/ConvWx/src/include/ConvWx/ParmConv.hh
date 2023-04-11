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
 * @file ParmConv.hh
 * @brief Parameters for convergence filtering
 * @class ParmConv
 * @brief Parameters for convergence filtering
 *
 * Any members that are hardwired are const, all are public by design
 */

# ifndef    PARM_CONV_HH
# define    PARM_CONV_HH

#include <string>
#include <ConvWx/ConvWxFuzzy.hh>

//----------------------------------------------------------------
class ParmConv
{
public:

  /**
   * default constructor. values not set
   */
  ParmConv(void);

  /**
   * Constructor with values passed in, 
   *
   * @param[in] convThresh  Convergence algorithm threshold
   * @param[in] nptToExpansion  Fuzzy function from number of points in a run
   *                            to expansion value 
   */
  ParmConv(const double convThresh, const ConvWxFuzzy &nptToExpansion);

  /**
   * Destructor
   */
  virtual ~ParmConv(void);


  /**
   * Debug print
   */
  void print(void) const;

  double pConvThresh;  /**< convergence threshold */
  ConvWxFuzzy pNptToExpansion; /**< fuzzy function from number of points
				* in a run to expansion value */

protected:
private:  
};

# endif
