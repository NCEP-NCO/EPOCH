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
 * @file ThreshHistMgr.hh
 * @brief The process layer for the ThreshHist algorithm. 
 *        Handles data input and output and algorithm calls
 * @class ThreshHistMgr
 * @brief The process layer for the ThreshHist algorithm.
 *        Handles data input and output and algorithm calls
 *
 * The constructor sets up app for an infinite processing loop.
 * The run method does the infinite looping of triggering, getting data,
 * invoking the algorithm, and writing results.
 */

# ifndef    ThreshHistMgr_H
# define    ThreshHistMgr_H

#include "ParmsThreshHistIO.hh"
#include <Epoch/SpdbGenBasedThreshHandler.hh>
#include <dsdata/DsUrlTrigger.hh>
#include <vector>

//----------------------------------------------------------------
class ThreshHistMgr
{
public:

  /**
   * Default constructor
   *
   * @param[in] parms  The parameters to put into state
   * @param[in] tidyAndExit  Cleanup function to call at exit
   *
   * Prepare the class for a call to the run() method
   */
  ThreshHistMgr(const ParmsThreshHistIO &parms, void tidyAndExit(int));
  
  /**
   *  Destructor
   */
  virtual ~ThreshHistMgr(void);

  /**
   * Run the app in an infinite loop of triggering, getting data,
   * invoking the algorithm, and writing results.
   */
  void run(void);

protected:
private:  

  /**
   * The Algorithm parameters, kept as internal state
   */
  ParmsThreshHistIO _parms;

  /**
   * Triggering mechanism
   */
  DsUrlTrigger *_trigger;

  /**
   * Input database reader
   */
  SpdbGenBasedThreshHandler _reader;

  /**
   * database writer
   */
  SpdbGenBasedThreshHandler _writer;


  void _process(const time_t &t);

};

# endif     // ThreshHist_HH
