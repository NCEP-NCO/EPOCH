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

class ParmsPbarCompute;

//----------------------------------------------------------------
class PbarVector 
{
public:
  /**
   * Constructor from params, one pbar per configured threshold
   * is initialized to -1
   *
   * @param[in] parm
   * @param[in] index 1 or 2
   */
  PbarVector(const ParmsPbarCompute &parm, int index);

  /**
   * Destructor
   */
  ~PbarVector(void);

  /**
   * @return the pbar values (one per threshold)
   */
  inline std::vector<double> getPbar(void) const {return _pBar;}

  /**
   * Set the pbar value for a threshold index to a value
   * @param[in] index  Threshold index agree with params
   * @param[in] value  pbar
   * @param[in] which 1 or 2
   */
  void setValue(int index, double value, int which);

  /**
   * Fill in the good pbar threshold pairs by filtering down the _threshPbar pairs
   * @param[in] which 1 or 2
   */
  void setGood(int which);

  
protected:
private:

  /**
   * @class ThreshPbar2
   * @brief Two thresholds and the pbar that goes with it
   */
  class ThreshPbar2
  {
  public:
    /**
     * Set from inputs
     */
    inline ThreshPbar2(double thr1, double thr2, double pbar) :
      _thresh1(thr1), _thresh2(thr2), _pbar(pbar) {}
    /**
     * Trivial
     */
    inline ~ThreshPbar2(void) {}

    double _thresh1;  /**< Threshold */
    double _thresh2;  /**< Threshold */
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
   * @class ThreshPbar1
   * @brief one threshold and the pbar that goes with it
   */
  class ThreshPbar1
  {
  public:
    /**
     * Set from inputs
     */
    inline ThreshPbar1(double thr, double pbar) :
      _thresh(thr), _pbar(pbar) {}
    /**
     * Trivial
     */
    inline ~ThreshPbar1(void) {}

    double _thresh;  /**< Threshold */
    double _pbar;    /**< Pbar */

    /**
     * @return bias for this pbar using obar  bias = pbar -obar
     * @param[in] obar
     */
    inline double bias(double obar) const {return _pbar-obar;}

  protected:
  private:
  };
  
  int _index; /**< 1 or 2 */

  /**
   * The pbar value at each threshold (1 or 2)
   */
  std::vector<double> _pBar;  

  /**
   * The thresh/pbar pairs (1 or 2)
   */
  std::vector<ThreshPbar1> _threshPbar1;
  std::vector<ThreshPbar2> _threshPbar2;

  /**
   * The good thresh/pbar pairs (non missing) (1 or 2)
   */
  std::vector<ThreshPbar1> _goodThreshPbar1;
  std::vector<ThreshPbar2> _goodThreshPbar2;
  

};

#endif
