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
 * @file ObarComputeMgr.cc
 */

#include "ObarComputeMgr.hh"
#include <Epoch/TileRange.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/Trigger.hh>
#include <ConvWx/Grid.hh>
#include <toolsa/LogStream.hh>

#include <vector>
using std::vector;

//----------------------------------------------------------------
ObarComputeMgr::
ObarComputeMgr(const ParmsObarComputeIO &parms, void tidyAndExit(int)) :
  _parms(parms),
  _obsTime(0),
  _spdb(parms._obarSpdb, parms._inputField, parms._tileInfo,
	parms._obsThreshold),
  _obarInfo((int)parms._obsThreshold.size(), parms._tileInfo.numTiles())
{
  // Standard initialization
  if (!InterfaceParm::driverInit(_parms._main, tidyAndExit))
  {
    tidyAndExit(convWx::BAD_EXIT);
  }
}

//----------------------------------------------------------------
ObarComputeMgr::~ObarComputeMgr()
{
}

//----------------------------------------------------------------
void ObarComputeMgr::run(void)
{
  // Infinite loop of triggering new data 
  time_t obsTime;

  while (Trigger::trigger(_parms._main, obsTime))
  {
    _process(obsTime);
  }
}

//----------------------------------------------------------------
void ObarComputeMgr::_process(const time_t &obsTime)
{
  Grid obsGrid;
  bool hasObs = InterfaceIO::loadObs(obsTime, _parms._proj,
				     _parms._obs.pUrl,
				     _parms._obs.pField,
				     _parms._obs.pRemap,
				     obsGrid);
  if (!hasObs)
  {
    LOG(ERROR) << "Reading Obs data, no processing";
    return;
  }

  _obsTime = obsTime;
  _spdb.updateTime(_obsTime);
  _processTiles(obsGrid);
  _obarInfo.update(_spdb);
  //_spdb.print();
  _spdb.write(obsTime);
}

//----------------------------------------------------------------
void ObarComputeMgr::_processTiles(const Grid &obsGrid)
{

  // for each obar threshold
  for (size_t i=0; i<_parms._obsThreshold.size(); ++i)
  {
    // do the mothertile first
    int motherIndex = TileInfo::motherTileIndex();
    TileObarInfo info = _setupAndRunAlg(obsGrid, motherIndex,
					i, true);
    _obarInfo[i][motherIndex] = info;
    if (!_motherFail)
    {
      _motherSet = true;
      _motherInfo = info;
    }

    // now do all the other tiles
    for (int tileIndex=0; tileIndex<_parms._tileInfo.numTiles(); ++tileIndex)
    {
      if (tileIndex != motherIndex)
      {
	info = _setupAndRunAlg(obsGrid, tileIndex,
			       i, false);
	_obarInfo[i][tileIndex] = info;
      }
    }
  }
}

//----------------------------------------------------------------
TileObarInfo ObarComputeMgr::_setupAndRunAlg(const Grid &obsGrid,
					     int tileIndex,
					     int threshIndex,
					     bool isMotherTile)
{
  int belowTile;
  TileObarInfo ret;
  if (_parms._tileInfo.outOfBoundsY(tileIndex, belowTile))
  {
    // try to use the tile below
    if (!_obarInfo[threshIndex].useTileBelow(tileIndex, belowTile, ret))
    {
      LOG(ERROR) << "Tile order not as expected index=" << tileIndex
		 << " belowIndex=" << belowTile;
      ret = _setToMotherOrColdstart(tileIndex, isMotherTile);
    }
    return ret;
  }

  double oBar;
  TileRange r = _parms._tileInfo.range(tileIndex);
  if (!_obsSetup(r, _parms._obsThreshold[threshIndex], obsGrid, oBar))
  {
    ret = _setToMotherOrColdstart(tileIndex, isMotherTile);
    return ret;
  }
  else
  {
    return TileObarInfo(oBar, tileIndex, isMotherTile, false);
  }
}

//-----------------------------------------------------------------------
bool ObarComputeMgr::_obsSetup(const TileRange &r,
			       double thresh,
			       const Grid &obsGrid,
			       double &oBar)
{
  bool outOfBounds;
  if (!obsGrid.percentAboveThresholdSubset(thresh,
					   r.getX0(), r.getY0(),
					   r.getNx(), r.getNy(),
					   true, false, oBar, outOfBounds))
  {
    if (outOfBounds)
    {
      LOG(ERROR) << "Tile " << r.sprint() << " goes out of bounds";
    }
    else
    {
      LOG(DEBUG_VERBOSE) << "Cannot compute percent obs data above threshold, all missing";
    }
    return false;
  }
  if (oBar == 0.0)
  {
    LOG(DEBUG_VERBOSE) << "No obs data above threshold " << thresh;
  }
  return true;
}

//-----------------------------------------------------------------------
TileObarInfo ObarComputeMgr::_setToMotherOrColdstart(int tileIndex,
						     bool isMotherTile)
{
  if (isMotherTile)
  {
    // this means setting the mother tile has failed
    _motherFail = true;
    _motherSet = false;
    return TileObarInfo(0.0, tileIndex, true, true);
  }
  else
  {
    if (_motherSet)
    {
      TileObarInfo ret(_motherInfo);
      ret.setTileIndex(tileIndex);
      return ret;
    }
    else
    {
      return TileObarInfo(0.0, tileIndex,false, true);
    }
  }
}
