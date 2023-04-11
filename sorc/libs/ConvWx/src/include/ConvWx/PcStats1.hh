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
 * @file PcStats1.hh
 * @brief store info for statistical look at what happened with PhaseCorrection
 * @class PcStats1
 * @brief store info for statistical look at what happened with PhaseCorrection
 */

# ifndef    PC_STATS1_HH
# define    PC_STATS1_HH

#include <cstdio>

class PcStats1
{
public:
  /**
   * Constructor 
   * Set member values to corresponding input arguments
   * @param[in] xInit Initial x offset for phase correction
   * @param[in] yInit Initial y offset for phase correction
   * @param[in] numRefine  Number of refined offsets that were tested
   * @param[in] xRefine  Refined offset index X
   * @param[in] yRefine  Refined offset index Y
   * @param[in] scoreInit  Initial offset score
   * @param[in] scoreRefine  Refined offset score
   */
  inline PcStats1(int xInit, int yInit, int numRefine, int xRefine, int yRefine,
		  double scoreInit, double scoreRefine) :
    pXOffInitial(xInit), pYOffInitial(yInit), 
    pNumRefineTested(numRefine), 
    pXOffRefined(xRefine), pYOffRefined(yRefine),
    pScoreInitial(scoreInit), pScoreRefined(scoreRefine),
    pXDelta(xRefine - xInit), pYDelta(yRefine - yInit) {}

  /**
   * Constructor
   * Set member values to 0
   */
  inline PcStats1(void) :
    pXOffInitial(0), pYOffInitial(0), pNumRefineTested(0), 
    pXOffRefined(0), pYOffRefined(0), pScoreInitial(0.0), 
    pScoreRefined(0.0), pXDelta(0), pYDelta(0) {}

  /**
   * Destructor
   */
  inline ~PcStats1(void) {}

  /**
   * Operator==
   * @param[in] p   Object to compare to
   */
  inline bool operator==(const PcStats1 &p) const
  {
    return (pXOffInitial == p.pXOffInitial &&
	    pYOffInitial == p.pYOffInitial &&
	    pNumRefineTested == p.pNumRefineTested &&
	    pXOffRefined == p.pXOffRefined &&
	    pYOffRefined == p.pYOffRefined &&
	    pScoreInitial == p.pScoreInitial &&
	    pScoreRefined == p.pScoreRefined &&
	    pXDelta == p.pXDelta &&
	    pYDelta == p.pYDelta);
  }

  /**
   * Debug print method
   */
  inline void print(void) const
  {
    printf("initial:[%d,%d](%.4lf)   numRefine:%d    final:[%d,%d](%.4lf)"
	   "  delta=%d,%d\n",
	   pXOffInitial, pYOffInitial, pScoreInitial, pNumRefineTested,
	   pXOffRefined, pYOffRefined, pScoreRefined,
	   pXOffRefined - pXOffInitial, pYOffRefined - pYOffInitial);
  }

  /**
   * Update initial and refined member values with inputs
   * @param[in] score  The initial score
   * @param[in] xoff  The initial x offset
   * @param[in] yoff  The initial y offset
   */
  inline void updateInitial(double score, int xoff, int yoff)
  {
    pScoreInitial = score;
    pXOffInitial = xoff;
    pYOffInitial = yoff;
    pScoreRefined = score;
    pXOffRefined = xoff;
    pYOffRefined = yoff;
  }

  /**
   * Update final member values with inputs
   * @param[in] score  The final score
   * @param[in] xoff  The final x offset
   * @param[in] yoff  The final y offset
   */
  inline void updateFinal(double score, int xoff, int yoff)
  {
    pScoreRefined = score;
    pXOffRefined = xoff;
    pYOffRefined = yoff;
    pXDelta = pXOffRefined - pXOffInitial;
    pYDelta = pYOffRefined - pYOffInitial;
  }

  int pXOffInitial;      /**< Initial x offset for phase correction */
  int pYOffInitial;      /**< Initial y offset for phase correction */
  int pNumRefineTested;  /**< Number of refined offsets that were tested */
  int pXOffRefined;      /**< Refined offset index X */
  int pYOffRefined;      /**< Refined offset index Y */
  double pScoreInitial;  /**< The initial score */
  double pScoreRefined;  /**< The refined score */
  int pXDelta;           /**< How much motion from initial to refined X */
  int pYDelta;           /**< How much motion from initial to refined Y */

protected:
private:
};

# endif
