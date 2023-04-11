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
 * @file ParmsThreshHist.hh
 * @brief All the algorithm parameters for ThreshHist
 * @class ParmsThreshHist
 * @brief All the algorithm parameters for ThreshHist
 *
 * The parameters are intentionally public as it is a stateless 'struct-like'
 * class.
 */

# ifndef    ParmsThreshHist_HH
# define    ParmsThreshHist_HH

#include <ConvWx/ParmMain.hh>
#include <string>

//----------------------------------------------------------------
class ParmsThreshHist
{
public:

  /**
   * Standard main parameters for apps
   */
  ParmMain _mainParms;

  /**
   * Spdb input
   */
  std::string _inputSpdb;

  /**
   * Spdb output
   */
  std::string _outputSpdb;

  /**
   * Max days to look back for thresholds data in SPDB
   */
  int _daysBack;

  /**
   * Coldstart threshold
   */
  double _coldstartThresh;

protected:
private:  

};

# endif     // ParmsThreshHist_HH

