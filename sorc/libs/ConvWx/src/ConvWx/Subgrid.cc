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
 * @file Subgrid.cc
 * @brief Source for Subgrid class 
 */
#include <ConvWx/Subgrid.hh>

//
// Missing value for grid index
// 
const int Subgrid::gridIndexMissing = -1;

Subgrid::Subgrid(const int lowerLeftI, const int lowerLeftJ,
		 const int nx, const int ny, const int superGridNx,
		 const int gridNum):
  pNx(nx),
  pNy(ny),
  pGridNum(gridNum),
  pIndexGrid("indices", "none", nx, ny, gridIndexMissing)
{  
  
  //
  // pIndexGrid is filled with corresponding indices of super grid
  // represented in this subgrid
  //
  for(int m = 0; m < ny; m++)
  {
    for (int l = 0; l < nx; l++)
    {
      int modelGridIndex = (lowerLeftJ + m)* superGridNx + (lowerLeftI + l); 
      
      pIndexGrid.setv(l, m, modelGridIndex);
    }
  }
}

Subgrid::~Subgrid(void)
{
}
