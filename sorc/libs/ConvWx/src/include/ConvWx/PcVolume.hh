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
 * @file PcVolume.hh
 * @brief A phase correction volume box - lower left corner and shift amount
 * @class PcVolume
 * @brief A phase correction volume box - lower left corner and shift amount
 *
 * Various volume boxes with shift are tried, as part of the phase
 * correction minimization algorithm
 */

# ifndef    PC_VOLUME_HH
# define    PC_VOLUME_HH

#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/Score.hh>
#include <ConvWx/PcStats.hh>
class MultiGridsForPc;
class UvOutput;
class GridData;

//----------------------------------------------------------------
class PcVolume
{
public:

  /**
   * Default constructor
   * @param[in] x0  Lower left point of volume 
   * @param[in] y0  Lower left point of volume
   * @param[in] parms  Phase correction parameters
   *
   * The shift amount is set to 0
   */
  PcVolume(const int x0, const int y0, const ParmSetUV &parms);
  
  /**
   * Copy constructor, needed because the class has one pointer member
   * @param[in] p  Object to copy
   */
  PcVolume(const PcVolume &p);

  /**
   * Destructor
   */
  virtual ~PcVolume(void);

  /**
   * Operator=, needed because the class has one pointer member
   * @param[in] p  Object to copy
   */
  PcVolume & operator=(const PcVolume &p);

  /**
   * Operator==, needed because the class has one pointer member
   * @param[in] p  Object to compare
   */
  bool operator==(const PcVolume &p) const;

  /**
   * Debug print
   */
  void print(void) const;
  
  /**
   * Set shift offset for the local state volume box by computing scores for
   * various shifts and choosing best one. Four steps:
   *  - do the fractionalAreaTest on the input and don't shift if that fails
   *  - build initial scores (pBuildScore)
   *  - refine the scores    (pRefineScore)
   *  - do distance minimization step (pDistanceMinimization) 
   *
   * @param[in] pcGrids  Phase Correction input grids for one or more data type
   */
  void getPcOfSmoothed(const MultiGridsForPc &pcGrids);

  /** 
   * Accumulate phase correction values from the local volume into averaging
   * grids.
   * 
   * At each point in the volume, increment the input uv U Grid at 'from'
   * locations by pOffX, increment the input uv V grid at 'from' locations by
   * pYoff, and increment the input n Grid at 'from' locations by 1.
   * This is in anticipation of later dividing U and V by n to make
   * U,V average values for overlapping volumes.
   *
   * (the 'from' locations are points that are inside the box, shifted by
   *  minus (pXoff,pYoff)).
   *
   * @param[in,out] uvGrid  U and V grids, which are incremented by Xoff,
   *                        Yoff at 'from' locations
   * @param[in,out] countGrid  Grid which is incremented by 1.0 at 'from'
   *                           locations
   */
  void addToMotion(UvOutput &uvGrid, Grid &countGrid) const;

  /**
   * Update stats that are passed in
   * @param[in,out]  stats   
   */
  void updateStats(PcStats &stats) const;

protected:
private:  

  int pX0;         /**< Lower left corner of volume*/
  int pY0;         /**< Lower left corner of volume*/
  ParmSetUV pParm; /**< Params for phase correction */
  int pXoff;       /**< How much to shift */
  int pYoff;       /**< How much to shift */

  /**
   * The scores for the offset boxes
   */
  std::vector<Score> pVscore;

  /**
   * The best score
   */
  double pScoreBest;

  /**
   * The score object with the best score
   */
  Score pSbest;

  /**
   * Statistics regarding phase correction
   */
  PcStats1 pStats;

  /**
   * Build initial scores from input
   *
   * @param[in] pcGrids  Phase Correction input grids for one or more data type
   *
   * @return true if any scores were computed
   */
  bool pBuildScore(const MultiGridsForPc &pcGrids);

  /**
   * Add score for a particular grid point to the state
   * @param[in] ix  The particular point
   * @param[in] iy  The particular point
   * @param[in] pcGrids  Phase Correction input grids for one or more data type
   * @param[in,out] first  True on input if this is the first point 
   *                       processed in this way
   */
  void pBuildScore(const int ix, const int iy, const MultiGridsForPc &pcGrids,
		   bool &first);

  /**
   * Refine computed scores using refine algorithm
   *
   * @param[in] pcGrids  Phase Correction input grids for one or more data type
   *
   * @return true if scores were refined
   */
  bool pRefineScore(const MultiGridsForPc &pcGrids);

  /**
   * Perform distance minimization for scores that are similar (take the
   * one that phase corrects the least)
   */
  void pDistanceMinimization(void);

};

# endif 
