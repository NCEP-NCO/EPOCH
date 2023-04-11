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
 * @file PcStats.hh
 * @brief store info for statistical look at what happened with PhaseCorrection
 * @class PcStats
 * @brief store info for statistical look at what happened with PhaseCorrection
 */

# ifndef    PC_STATS_HH
# define    PC_STATS_HH

#include <ConvWx/PcStats1.hh>
#include <vector>

//----------------------------------------------------------------
class PcStats 
{
public:

  /**
   * Constructor
   */
  PcStats(void);

  /**
   * Destructor
   */
  virtual ~PcStats(void);

  /**
   * Debug print to stdout 
   */
  void print(void) const;

  /**
   * Update using input object
   * @param[in] stats  Input, if has number of refined > 0, update numWithPc
   *                   and add to pStats, otherwise update numWithoutPc
   */
  void update(const PcStats1 &stats);

  /**
   * Merge contents of input with local, by incrementing objects
   * @param[in] stats   Object to merge in
   */
  void merge(const PcStats &stats);
  
protected:
private:  

  int pNumWithPc;        /**< Number of places where phase correction happened*/
  int pNumWithoutPc;     /**< Number of places without phase correction */
  std::vector<PcStats1> pStats;  /**< The individual stats, phase correction */

};

# endif
