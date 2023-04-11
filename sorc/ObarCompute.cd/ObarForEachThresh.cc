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
 * @file ObarForEachThresh.cc
 */

#include "ObarForEachThresh.hh"
#include <toolsa/LogStream.hh>

//----------------------------------------------------------------
ObarForEachThresh::ObarForEachThresh(int numThresh, int numTiles)
{
  _obarInfo.reserve(numThresh);
  for (int i=0; i<numThresh; ++i)
  {
    _obarInfo.push_back(ObarForEachTile(numTiles));
  }
}

//----------------------------------------------------------------
ObarForEachThresh::~ObarForEachThresh()
{
}

//----------------------------------------------------------------
void ObarForEachThresh::update(SpdbObsMetadata &spdb) const
{
  for (size_t i=0; i<_obarInfo.size(); ++i)
  {
    _obarInfo[i].update(i, spdb);
  }
}

// #ifdef NOTYET
// //----------------------------------------------------------------
// void ObarForEachThresh::clear(void)
// {
//   for (size_t i=0; i<_obarInfo.size(); ++i)
//   {
//     _obarInfo[i] = TileObarInfo();
//   }
// }

// //----------------------------------------------------------------
// bool ObarForEachThresh::useTileBelow(int tileIndex, int belowTile,
// 					TileObarInfo  &data)
// {
//   // This approach does assume the tiles are ordered bottom to top.
//   if (_obarInfo[belowTile].ok())
//   {
//     // can use the one below, change tile index
//     data = _obarInfo[belowTile];
//     data.setTileIndex(tileIndex);
//     return true;
//   }
//   else
//   {
//     LOG(ERROR) << "Tile order is not as expected index= " << tileIndex 
// 	       << " index below = " << belowTile;
//     return false;
//   }
// }

// #endif

// #ifdef NOTYET
// void ObarForEachThresh   ::print(void) const
// {
//   for (size_t i=0; i<_obarInfo.size(); ++i)
//   {
//     _obarInfo[i].print();
//   }
// }  

// #endif
