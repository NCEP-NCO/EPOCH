// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// Copyright, University Corporation for Atmospheric Research (UCAR) 2009-2017. 
// The Government's right to use this data and/or software is restricted per 
// the terms of Cooperative Agreement between UCAR and the National  Science 
// Foundation, to government use only which includes the nonexclusive, 
// nontransferable, irrevocable, royalty-free license to exercise or have 
// exercised for or on behalf of the U.S. Government throughout the world. 
// All other rights are reserved. 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file ParmsCmorphAverager.hh
 * @brief All user defined parameters.
 * @class ParmsCmorphAverager
 * @brief All user defined parameters.
 */

# ifndef    ParmsCmorphAverager_HH
# define    ParmsCmorphAverager_HH

#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmMain.hh>
#include <ConvWxIO/ParmApp.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/ParmFcstIO.hh>

class ParmsCmorphAverager
{
public:

 
  /**
   *  Return value for ParmsCmorphAverager  methods
   *  PARMS_SUCCESS indicates successful method execution
   *  PARMS_FAILURE indicates unsuccessful method execution
   */
  enum status {PARMS_SUCCESS, PARMS_FAILURE};
  
  /**
   *  Parameters to define message logging, process registration and triggering
   */
  ParmMain main;

  /**
   * Parameters to defining map projection
   */
  ParmProjection proj;

  /**
   * Parameters for input observation dataset
   */
  ParmFcst obs;

  /**
   * Parameters for output observation dataset
   */
  ParmFcstIO obsOut;

  /**
   * IF true convert from mm/hr to mm/3hr by multiplying by 3
   */
  bool unitsConvert;

protected:
private:
 
};

# endif 
