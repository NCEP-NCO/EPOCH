/**
 * @file OutputToThreshProj.hh
 * @brief Handling of two projections, one extends north and/or south
 * @class OutputToThreshProj 
 * @brief Handling of two projections, one extends north and/or south
 */

#ifndef OUTPUT_TO_THRESH_PROJ_HH
#define OUTPUT_TO_THRESH_PROJ_HH

#include <ConvWx/ParmProjection.hh>
#include <ConvWx/GridData.hh>

class OutputToThreshProj
{
public:
  /**
   * Empty constructor
   */
  inline OutputToThreshProj(void) {}
  
  /**
   * Constructor
   * @param[in] threshParms  Thresh (smaller) projection
   * @param[in] outputParms  Output (extended) projection
   */
  OutputToThreshProj(const ParmProjection &threshParms,
		     const ParmProjection &outputParms);
  /**
   * Destructor
   */
  ~OutputToThreshProj(void);

  /**
   * @return true if point y is inside the smaller domain, and if so
   * return the small domain index
   * @param[in] y  Extended domain y index
   * @param[out] ySmall  Smaller domain y index, if return status is true
   */
  bool isInsideSmallerDomain(int y, int &ySmall) const;
  
  /**
   * @return true if the projection extends the domain to the south
   */
  inline bool doesExtendSouth(void) const {return _doesExtendSouth;}

  /**
   * @return true if the projection extends the domain to the north
   */
  inline bool doesExtendNorth(void) const {return _doesExtendNorth;}

  /**
   * @return true if object has been set
   */
  inline bool isOk(void) const {return _ok;}

protected:
private:
  
  bool _ok; /**< Status */
  
  bool _doesExtendSouth;  /**< True if extention goes south */
  bool _doesExtendNorth;  /**< True if extention goes north */

  /**
   *  The y index in extended domain that just reaches the bottom of
   *  the smaller domain
   */
  int _minThreshYInOutputIndex; 

  /** 
   * The y index in extended domain that just reaches the top of
   * the smaller domain
   */
  int _maxThreshYInOutputIndex;

};
#endif
