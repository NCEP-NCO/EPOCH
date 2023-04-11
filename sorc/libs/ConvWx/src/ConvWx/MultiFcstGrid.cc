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
 * @file MultiFcstGrid.cc
 */

//----------------------------------------------------------------
#include <string>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/Grid.hh>
using std::string;

//----------------------------------------------------------------
MultiFcstGrid::MultiFcstGrid() : MultiGrid(), FcstInfo()
{
}

//----------------------------------------------------------------
MultiFcstGrid::MultiFcstGrid(const time_t &gt, const int lt,
			     const Grid &gridData,
			     const string &path, const MetaData &metadata) :
  MultiGrid(gridData), FcstInfo(gt, lt, path, metadata)
{
  
}

//----------------------------------------------------------------
MultiFcstGrid::MultiFcstGrid(const time_t &gt, const int lt,
			     const string &fieldName, const string &units,
			     const int nx, const int ny, const double missing) :
  MultiGrid(), FcstInfo(gt, lt)
{
  // construct a grid and append it.
  Grid g(fieldName, units, nx, ny, missing);
  append(g);
}
  
//----------------------------------------------------------------
MultiFcstGrid::~MultiFcstGrid()
{
}

//----------------------------------------------------------------
void MultiFcstGrid::init(const MultiGrid &inputGrids, const time_t gt, 
			 const int lt, const string &path,
			 const MetaData &metadata)
{
  // copy from inputGrids into local MultiGrid object
  inputGrids.copyOut(*this);

  // create a new info from inputs
  FcstInfo f(gt, lt, path, metadata);

  // copy info into local FcstInfo object
  f.copyOut(*this);
}

//----------------------------------------------------------------
void MultiFcstGrid::copyInfo(const MultiFcstGrid &g)
{
  FcstInfo::operator=(g);
}
