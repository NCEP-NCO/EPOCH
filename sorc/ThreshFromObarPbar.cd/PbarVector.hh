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
 * @file  PbarVector.hh
 * @brief  Struct like class to contain pbar for every threshold
 * @class  PbarVector
 * @brief  Struct like class to contain pbar for every threshold
 */

# ifndef    PBAR_VECTOR_H
# define    PBAR_VECTOR_H

#include <vector>

class ParmsThreshFromObarPbar;

//----------------------------------------------------------------
class PbarVector 
{
public:
  /**
   * Constructor from params, one pbar per configured threshold
   * is initialized to -1
   *
   * @param[in] parm
   */
  PbarVector(double targetBias,
	     const std::vector<double> &thresh);

  /**
   * Destructor
   */
  ~PbarVector(void);

  /**
   * @return the pbar values (one per threshold)
   */
  inline std::vector<double> getPbar(void) const {return _pBar;}

  /**
   * @return true if all pbar values are -1 (initial state)
   */
  bool noGood(void) const;

  /**
   * Return the best threshold and bias at that threshold when the observation 
   * is zero
   * @param[in] isGreaterOrEqualTest  True for >=, false for <=
   * @param[out] thresh 
   * @param[out] bias
   * @return true if successful
   */
  bool bestWhenObsZero(bool isGreaterOrEqualTest, double &thresh, double &bias,
		       int &pbarIndex) const;

  /**
   * Return the best threshold and bias at that threshold when the observation 
   * is nonzero
   * @param[in] obar  The average obs value to compare to
   * @param[in] currentThresh A historical best threshold starting point
   * @param[in] isGreaterOrEqualTest  True for >=, false for <=
   * @param[out] thresh 
   * @param[out] bias
   * @return true if successful
   */
  bool bestWhenObsNonZero(double obar, double currentThresh,
			  bool isGreaterOrEqualTest,
			  double &thresh, double &bias,
			  int &pbarIndex) const;
  
  /**
   * Set the pbar value for a threshold index to a value
   * @param[in] index  Threshold index agree with params
   * @param[in] value  pbar
   */
  void setValue(int index, double value);

  /**
   * Fill in the good pbar threshold pairs by filtering down the _threshPbar pairs
   */
  void setGood(void);

  
protected:
private:

  /**
   * @class ThreshPbar
   * @brief one threshold and the pbar that goes with it
   */
  class ThreshPbar
  {
  public:
    /**
     * Set from inputs
     */
    inline ThreshPbar(double thr, int ind, double pbar) :
      _thresh(thr), _index(ind), _pbar(pbar) {}
    /**
     * Trivial
     */
    inline ~ThreshPbar(void) {}

    double _thresh;  /**< Threshold */
    int _index;
    double _pbar;    /**< Pbar */

    /**
     * @return bias for this pbar using obar  bias = pbar -obar
     * @param[in] obar
     */
    inline double bias(double obar) const {return _pbar-obar;}

  protected:
  private:
  };
  

  /**
   * The pbar value at each threshold
   */
  std::vector<double> _pBar;  

  /**
   * The thresh/pbar pairs
   */
  std::vector<ThreshPbar> _threshPbar;

  /**
   * The good thresh/pbar pairs (non missing)
   */
  std::vector<ThreshPbar> _goodThreshPbar;
  

  /**
   * Target bias
   */
  double _targetBias;

  bool _bestWhenObsNonZeroAndCurrentBestIsZero(double oBar,  int startingInd,
					       bool isGreaterOrEqualTest, 
					       double &thresh, double &bias,
					       int &pbarIndex) const;
  bool _bestSearchAll(double oBar, double &thresh, double &bias, int &pbarIndex) const;
  bool _allAreZeroBelow(int startingInd) const;
  bool _allAreZeroAbove(int startingInd) const;
  bool _allZero(void) const;
};

#endif
