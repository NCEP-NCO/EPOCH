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
 * @file Score.hh
 * @brief A phase shift and its quality measure 
 * @class Score
 * @brief A phase shift and its quality measure 
 */

# ifndef    SCORE_HH
# define    SCORE_HH

#include <vector>
#include <string>

class ParmSetUV;
class MultiGridsForPc;

//----------------------------------------------------------------
class Score
{
public:

  /**
   * Default constructor for no phase shift
   *
   * @note the quality measure is not set
   */
  Score(void);

  /**
   * Constructor for a particular phase shift
   *
   * @param[in] xoffset  X phase correction (number of grid points)
   * @param[in] yoffset  Y phase correction (number of grid points)
   *
   * @note the quality measure is not set, need to call additional methods
   */
  Score(const int xoffset, const int yoffset);
  
  /**
   * Destructor
   */
  virtual ~Score(void);

  /**
   * Operator== function, needed by PcVolume class
   * @param[in] s  Object to compare to
   */
  bool operator==(const Score &s) const;

  /**
   * @return String one line description of the object
   */
  std::string sprint(void) const;
  
  /**
   * Set and return the score (quality) from inputs
   *
   * @param[in] x0  Lower left corner of a volume within a Grid
   * @param[in] y0  Lower left corner of a volume within a Grid
   * @param[in] dataGrids  Grids used to compute the quality measure
   * @param[in] parms  Parameters used to compute the score
   * @param[out] debugMsg  String in which information about actions is
   *                       written to
   *
   * @return The quality that was computed, might be 'bad' (isBadScore)
   */
  double setScoreFromSmoothed(const int x0, const int y0,
			      const MultiGridsForPc &dataGrids,
			      const ParmSetUV &parms,
			      std::string &debugMsg);

  /**
   * Refine the quality measure (score) using internal state and inputs
   *
   * @param[in] x0  Lower left corner of a volume within a Grid
   * @param[in] y0  Lower left corner of a volume within a Grid
   * @param[in] dataGrids  Grids used to compute the quality measure
   * @param[in] parms  Parameters used to compute the score
   *
   * @return The quality that was computed, might be 'bad' (isBadScore)
   */
  double refineFromSmoothed(const int x0, const int y0, 
			    const MultiGridsForPc &dataGrids,
			    const ParmSetUV &parms);
  /**
   * @return private member pScore
   */
  inline double getScore(void) const {return pScore;}

  /**
   * @return private member pDistance
   */
  inline double getDistance(void) const {return pDistance;}

  /**
   * @return private members pXoff And pYoff
   * @param[out] xoff
   * @param[out] yoff
   */
  inline void getOffset(int &xoff, int &yoff) const
  {
    xoff = pXoff;
    yoff = pYoff;
  }

  /**
   * @return true if the score is bad
   */
  inline bool isBadScore(void) const {return pScore == pBadScore;}

  /**
   * @return true if input score is bad
   * @param[in] s  A score value
   */
  inline static bool isBadScore(const double s) {return s == pBadScore;}

protected:
private:  

  int pXoff;                     /**< offset index x */
  int pYoff;                     /**< offset index y */
  double pDistance;              /**< sqrt(pXoff*pXoff+pYoff*pYoff) */
  double pScore;                 /**< the score */
  static const double pBadScore; /**< a fixed bad score */
};

# endif 
