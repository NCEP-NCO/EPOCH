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
 * @file ParmSetUvIO.hh
 * @brief Derived class of ParmSetUV to handle input/output
 * @class ParmSetUvIO
 * @brief Derived class of ParmSetUV to handle input/output
 */

# ifndef    PARM_SET_UV_IO_HH
# define    PARM_SET_UV_IO_HH

#include <string>
#include <vector>
#include <ConvWx/ParmSetUV.hh>

//---------------------------------------------------------------
class ParmSetUvIO : public ParmSetUV
{
public:

  /**
   * default constructor. values are not set
   */
  ParmSetUvIO(void);

  /**
   * Constructor with base class values passed in
   * @param[in] parm Base class values
   */
  ParmSetUvIO(const ParmSetUV &parm);

  /**
   * Destructor
   */
  virtual ~ParmSetUvIO(void);

  /**
   * Load verifying observation data for a particular named forecast type
   * (Verification data can be observation or forecast data)
   *
   * @param[in] t  Time of data to get
   * @param[in] name  Forecasting type name
   * @param[in] proj  Projection information
   * @param[out] g  Grid that contains observation data on exit
   * @return true if successful
   */
  bool loadVerifObsData(const time_t &t, const std::string &name, 
			const ParmProjection &proj, Grid &g) const;

  /**
   * Load verifying forecast data for a particular named forecast type
   * (Verification data can be observation or forecast data)
   *
   * @param[in] gt  Generation time of the forecast
   * @param[in] lt  Lead time seconds of the forecast
   * @param[in] name  Forecasting type name
   * @param[in] proj  Projection information
   * @param[out] g  Grid that contains forecast data on exit
   * @return true if successful
   */
  bool loadVerifFcstData(const time_t &gt, const int lt,
			 const std::string &name, const ParmProjection &proj,
			 FcstGrid &g) const;
  /**
   * Load forecast data for a particular named forecast type
   *
   * @param[in] gt  Generation time of the forecast
   * @param[in] lt  Lead time seconds of the forecast
   * @param[in] name  Forecasting type name
   * @param[in] proj  Projection information
   * @param[out] g  Grid that contains forecast data on exit
   *
   * @return true if successful
   */
  bool loadFcstData(const time_t &gt, const int lt, const std::string &name,
		    const ParmProjection &proj, FcstGrid &g) const;

protected:
private:  
};

# endif
