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
 * @file  FcstGrid.hh
 * @brief  Derived class of Grid and FcstInfo, no local state
 * @class  FcstGrid
 * @brief  Derived class of Grid and FcstInfo, no local state
 *
 * FcstGrid is a Grid, plus some more information (FcstInfo).
 *
 * @note This class does not have any information about the projection.
 */

# ifndef    FCSTGRID_HH
# define    FCSTGRID_HH

#include <ConvWx/Grid.hh>
#include <ConvWx/FcstInfo.hh>

//----------------------------------------------------------------
class FcstGrid : public Grid, public FcstInfo
{
public:

  /**
   * Empty constructor
   */
  FcstGrid(void);

  /**
   * Constructor uses all inputs 
   *
   *@param[in] gt  Generation time
   *@param[in] lt  Lead seconds
   *@param[in] inputGrid   Grid to use
   *@param[in] path  Path to use
   *@param[in] metadata  Metadata to use
   */
  FcstGrid(const time_t &gt, const int lt, const Grid &inputGrid,
	   const std::string &path, const MetaData &metadata);

  /**
   * Constructor uses all inputs
   *
   *@param[in] gt  Generation time
   *@param[in] lt  Lead seconds
   *@param[in] inputGrid   Grid to use
   */
  FcstGrid(const time_t &gt, const int lt, const Grid &inputGrid);

  /**
   * Constructor for a Grid that has all missing data
   *
   *@param[in] gt  Generation time
   *@param[in] lt  Lead seconds
   *@param[in] fieldName  Name to give the Grid
   *@param[in] units  Units to give the Grid
   *@param[in] nx  Grid dimensions x
   *@param[in] ny  Grid dimensions y
   *@param[in] missingValue  The missing data value for the Grid.
   *
   * @note there is no information about the path and no metadata.
   */
  FcstGrid(const time_t &gt, const int lt, const std::string &fieldName,
	   const std::string & units, const int nx, const int ny,
	   const double missingValue);

   /**
   * Constructor for a Grid that has all missing data
   *
   *@param[in] gt  Generation time
   *@param[in] lt  Lead seconds
   *@param[in] fieldName  Name to give the Grid
   *@param[in] units  Units to give the Grid
   *@param[in] nx  Grid dimensions x
   *@param[in] ny  Grid dimensions y
   *@param[in] nz  Grid dimensions z
   *@param[in] missingValue  The missing data value for the Grid.
   *
   * @note there is no information about the path and no metadata.
   */
  FcstGrid(const time_t &gt, const int lt, const std::string &fieldName,
	   const std::string & units, const int nx, const int ny, const int nz,
	   const double missingValue);
  
  /**
   * Destructor
   */
  virtual ~FcstGrid(void);

  /**
   * Log debug information
   *
   * @param[in] severity  The severity level of the logged message
   */
  void log(const LogStream::Log_t severity) const;

  /**
   * Log debug information. Do not log the metadata portion.
   *
   * @param[in] severity  The severity level of the logged message
   */
  void logNoMetadata(const LogStream::Log_t  severity) const;

protected:
private:  
};

# endif     // FcstGrid_HH
