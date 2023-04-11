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
 * @file ThresholdForEachTile.cc
 */

#include "ThresholdForEachTile.hh"
#include <Epoch/SpdbGenBasedThreshHandler.hh>
#include <toolsa/LogStream.hh>

//----------------------------------------------------------------
ThresholdForEachTile::ThresholdForEachTile(int numTiles)
{
  _thresholdInfo.reserve(numTiles);
  for (int i=0; i<numTiles; ++i)
  {
    _thresholdInfo.push_back(TileThreshInfoGenBased());
  }
}

//----------------------------------------------------------------
ThresholdForEachTile::~ThresholdForEachTile()
{
}

//----------------------------------------------------------------
void ThresholdForEachTile::clear(void)
{
  for (size_t i=0; i<_thresholdInfo.size(); ++i)
  {
    _thresholdInfo[i] = TileThreshInfoGenBased();
  }
}

//----------------------------------------------------------------
bool ThresholdForEachTile::useTileBelow(int tileIndex, int belowTile,
					TileThreshInfoGenBased &data)
{
  // This approach does assume the tiles are ordered bottom to top.
  if (_thresholdInfo[belowTile].ok())
  {
    // can use the one below, change tile index
    data = _thresholdInfo[belowTile];
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
void ThresholdForEachTile::print(void) const
{
  for (size_t i=0; i<_thresholdInfo.size(); ++i)
  {
    _thresholdInfo[i].print();
  }
}  

//----------------------------------------------------------------
void ThresholdForEachTile::update(SpdbGenBasedThreshHandler &spdb,
				  int obarThreshIndex) const
{
  for (size_t i=0; i<_thresholdInfo.size(); ++i)
  {
    spdb.update(_thresholdInfo[i], obarThreshIndex);
  }
}

