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
 * @file MultiFcstGrid.hh
 * @brief Zero or more Grid objects (a MultiGrid) with additional information
 *        (FcstInfo)
 * @class MultiFcstGrid
 * @brief Zero or more Grid objects (a MultiGrid) with additional information
 *        (FcstInfo)
 *
 * The MultiGrid is any number of named Grid objects.
 * The FcstInfo gives generation time, lead time, path, and MetaData.
 *
 * This is supposed to represent a collection of Grid objects all from the
 * same source
 *
 */

# ifndef    MULTI_FCSTGRID_HH
# define    MULTI_FCSTGRID_HH

#include <ConvWx/MultiGrid.hh>
#include <ConvWx/FcstInfo.hh>
class Grid;

//----------------------------------------------------------------
class MultiFcstGrid : public MultiGrid, public FcstInfo
{
public:

  /**
   * Empty constructor
   */
  MultiFcstGrid(void);

  /**
   * Constructor for a single grid
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] gridData   The single Grid
   * @param[in] path  Path associated with the Grid
   * @param[in] metadata  MetaData associated with the Grid
   */
  MultiFcstGrid(const time_t &gt, const int lt, const Grid &gridData,
		const std::string &path, const MetaData &metadata);

  /**
   * Constructor for a single grid with all data in grid set to missing
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] fieldName  Name to give the Grid
   * @param[in] units Units to give the Grid
   * @param[in] nx  Grid dimension x
   * @param[in] ny  Grid dimension y
   * @param[in] missing  Data missing value
   *
   * @note No path or MetaData is specified, these are given default empty
   *       values
   */
  MultiFcstGrid(const time_t &gt, const int lt, const std::string &fieldName,
		const std::string & units, const int nx, const int ny,
		const double missing);
  
  /**
   * Destructor
   */
  virtual ~MultiFcstGrid(void);

  /**
   * Set the local object using all the inputs
   *
   * @param[in] inputGrids  MultiGrid to use in local state
   * @param[in] gt  Generation time to use in local state
   * @param[in] lt  Lead time seconds to use in local state
   * @param[in] path  Path to use in local state
   * @param[in] metadata  MetaData to use in local state
   */
  void init(const MultiGrid &inputGrids, const time_t gt, const int lt,
	    const std::string &path, const MetaData &metadata);

  /**
   * Copy the FcstInfo base class from input to local object
   * @param[in] g  Object to copy from 
   */
  void copyInfo(const MultiFcstGrid &g);

protected:
private:  
};

# endif
