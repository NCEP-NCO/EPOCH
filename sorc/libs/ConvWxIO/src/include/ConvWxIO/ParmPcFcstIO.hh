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
 * @file ParmPcFcstIO.hh
 * @brief Derived class of ParmPcFcst, that includes server access
 * @class ParmPcFcstIO
 * @brief Derived class of ParmPcFcst, that includes server access
 *
 */

# ifndef    PARM_PC_FCST_IO_HH
# define    PARM_PC_FCST_IO_HH
# include <ConvWxIO/ParmFcstIO.hh>
# include <ConvWx/ParmPcFcst.hh>

//----------------------------------------------------------------
class ParmPcFcstIO : public ParmPcFcst
{
public:

  /**
   * Constructor empty 
   */
  ParmPcFcstIO(void);

  /**
   * Constructor which passes in base class to use
   *
   * @param[in] params ParmPcFcst base class parameters
   */
  ParmPcFcstIO(const ParmPcFcst &params);

  /**
   * Destructor
   */
  virtual ~ParmPcFcstIO(void);

  /**
   * Load verifying observation data into a Grid
   *
   * @param[in] ot  Observations time
   * @param[in] name  Field name for the observations data
   * @param[in] proj  Projection to use
   * @param[out] outGrid  Grid that is filled with data on exit
   *
   * @return true if verifying data is observations and data was 
   *         successfully put into the Grid
   */
  bool loadVerifObsData(const time_t ot, const std::string &name,
			const ParmProjection &proj, Grid &outGrid) const;

  /**
   * Load verifying forecast data into a FcstGrid
   *
   * @param[in] gt  Forecast generation time
   * @param[in] lt  Forecast lead seconds
   * @param[in] name  Field name for the forecast data
   * @param[in] proj  Projection to use
   * @param[out] outGrid  FcstGrid that is filled with data on exit
   *
   * @return true if verifying data is forecast data and data was 
   *         successfully put into the FcstGrid
   */
  bool loadVerifFcstData(const time_t gt, const int lt, const std::string &name,
			 const ParmProjection &proj, FcstGrid &outGrid) const;

  /**
   * Load forecast data into a FcstGrid
   *
   * @param[in] gt  Forecast generation time
   * @param[in] lt  Forecast lead seconds
   * @param[in] name  Field name for the forecast data
   * @param[in] proj  Projection to use
   * @param[out] outGrid  FcstGrid that is filled with data on exit
   *
   * @return true if data was successfully put into the FcstGrid
   */
  bool loadFcstData(const time_t gt, const int lt, const std::string &name,
		    const ParmProjection &proj, FcstGrid &outGrid) const;


protected:
private:  

};

# endif
