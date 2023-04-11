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
 * @file ObarForEachTile.cc
 */

#include "ObarForEachTile.hh"
#include <Epoch/SpdbObsMetadata.hh>
#include <toolsa/LogStream.hh>

//----------------------------------------------------------------
ObarForEachTile::ObarForEachTile(int numTiles)
{
  _obarInfo.reserve(numTiles);
  for (int i=0; i<numTiles; ++i)
  {
    _obarInfo.push_back(TileObarInfo());
  }
}

//----------------------------------------------------------------
ObarForEachTile::~ObarForEachTile()
{
}

//----------------------------------------------------------------
void ObarForEachTile::clear(void)
{
  for (size_t i=0; i<_obarInfo.size(); ++i)
  {
    _obarInfo[i] = TileObarInfo();
  }
}

//----------------------------------------------------------------
bool ObarForEachTile::useTileBelow(int tileIndex, int belowTile,
				   TileObarInfo  &data)
{
  // This approach does assume the tiles are ordered bottom to top.
  if (_obarInfo[belowTile].ok())
  {
    // can use the one below, change tile index
    data = _obarInfo[belowTile];
    data.setTileIndex(tileIndex);
    return true;
  }
  else
  {
    LOG(ERROR) << "Tile order is not as expected index= " << tileIndex 
	       << " index below = " << belowTile;
    return false;
  }
}

//----------------------------------------------------------------
void ObarForEachTile::update(int threshIndex, SpdbObsMetadata &spdb) const
{
  for (size_t i=0; i<_obarInfo.size(); ++i)
  {
    spdb.update(threshIndex, i, _obarInfo[i]);
  }
}


#ifdef NOTYET
void ObarForEachTile::print(void) const
{
  for (size_t i=0; i<_obarInfo.size(); ++i)
  {
    _obarInfo[i].print();
  }
}  

#endif
