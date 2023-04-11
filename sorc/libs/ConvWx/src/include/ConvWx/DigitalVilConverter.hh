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
/**
 * @file DigitalVilConverter.hh
 * @brief This class performs a first order bias correction of the model VIL 
 * data or re-scales VIL data to integer values. 
 * @class DigitalVilConverter
 * @brief This class performs a first order bias correction of the model VIL 
 * data or re-scales VIL data to integer values. 
 */

#ifndef STATIC_CALIBRATOR_HH
#define STATIC_CALIBRATOR_HH

#include <ConvWx/GridData.hh>

class DigitalVilConverter {
  
public:

  /**
   * Constructor
   */
   DigitalVilConverter(void);
  
  /**
   * Constructor 
   * This class performs either  a first order bias correction of the model 
   * VIL data and re-scales VIL data to integer values or re-scales observed 
   * VIL to integer values.  
   * If VIL <= lowYhresh then digVil = minVal
   * If lowThresh < VIL < highThresh then digVil =  a0 + a1 * VIL
   * If vil >= highThresh then digVil = cc * (b0 + b1 * log (b2 * vil^ee )) 
   * @param[in] lowThresh  Lower threshold of data value for application linear
   *                       calibration
   * @param[in] highThresh Upper threshold of data value for application linear
   *                       calibration
   * @param[in] a0  Constant in linear calibration
   * @param[in] a1  Slope in linear calibration
   * @param[in] b0  Constant in logarithmic calibration
   * @param[in] b1  Constant in logarithmic calibration
   * @param[in] b2  Constant in logarithmic calibration
   * @param[in] cc  Constant in logarithmic calibration
   * @param[in] ee  Constant in logarithmic calibration
   */
  DigitalVilConverter( const double lowThresh, const double highThresh, 
		    const double a0, const double a1, const double b0,
		    const double b1, const double b2, const double cc,
		    const double ee);
  /**
   *  Destructor
   */
  virtual ~DigitalVilConverter(void);
  
  /**
   * Run the algorithm
   * @param[in,out] fcstGrid  The data to convert
   */ 
  void runAlg(GridData &fcstGrid) const;
  
protected:
  
private:

  /**
   * Minimum value of scaled vil
   */
  const static double minVil;

  /**
   * Maximum value of scaled vil
   */
  const static double maxVil;
  
  /** 
   * Lower bound for interval of VIL values for which the calibration
   * function is linear 
   */
  double pLowThresh;

  /** 
   * Upper bound for interval of VIL values for which the calibration
   * function is linear. VIL values above this threshold will have 
   * logarithmic function applied.
   */
  double pHighThresh;

  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pA0; 
  
  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pA1; 

  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pB0;

  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pB1;

  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pB2;

  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pCc;

  /**
   * Constant in logarithmic static calibration equation. This constant is 
   * supplied by the user.
   */
  double pEe;

};

#endif

