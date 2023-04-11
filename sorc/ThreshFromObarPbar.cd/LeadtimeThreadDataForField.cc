/**
 * @file LeadtimeThreadDataForField.cc
 */

#include "LeadtimeThreadDataForField.hh"
#include <Epoch/SpdbGenBasedThreshHandler.hh>
#include <toolsa/TaThreadDoubleQue.hh>
#include "PbarVector.hh"
#include <toolsa/LogStream.hh>
#include <algorithm>

//----------------------------------------------------------------------
LeadtimeThreadDataForField::
LeadtimeThreadDataForField(const ParmsThreshFromObarPbar &params, int which,
			   const time_t &genTime, int leadTime) :
  _params(params),
  _genTime(genTime),
  _leadTime(leadTime),
  _threshInfo(params._tileInfo.numTiles()),
  _currentThresh(0)
{
  if (which == 1)
  {
    _spdb = params._thresholdsSpdb1;
    _coldstartThresh = params._thresholdedFieldColdstartThresh1;
    _greaterOrEqual = true;
    for (size_t i=0; i<params._obarThreshTargetBias1.size(); ++i)
    {
      _targetBias.push_back(params._obarThreshTargetBias1[i].second);
    }
  }
  else
  {
    _spdb = params._thresholdsSpdb2;
    _coldstartThresh = params._thresholdedFieldColdstartThresh2;
    _greaterOrEqual = false;
    for (size_t i=0; i<params._obarThreshTargetBias2.size(); ++i)
    {
      _targetBias.push_back(params._obarThreshTargetBias2[i].second);
    }
  }
}

//----------------------------------------------------------------------
LeadtimeThreadDataForField::~LeadtimeThreadDataForField()
{
}

//----------------------------------------------------------------
void LeadtimeThreadDataForField::setInitialThresholds(int obsThreshIndex)
{
  _threshInfo.clear();
  _motherSet = false;
  _motherFail = false;

  // try to pull older thresholds from database
  SpdbGenBasedThreshHandler database(_spdb);
  _bestOld.clear();
  if (database.readBestOlder(_genTime, _params._thresholdsMaxSecondsBack))
  {
    // use them if there
    _bestOld = database.getTileThresholds(_leadTime, obsThreshIndex);
  }
  if (_bestOld.empty())
  {
    // didn't get thresholds, set to coldstart for all tiles
    for (int i=0; i<_params._tileInfo.numTiles(); ++i)
    {
      _bestOld.push_back(_coldstartThresh);
    }
  }
}

//-----------------------------------------------------------------------
bool LeadtimeThreadDataForField::usedTileBelow(int tileIndex, bool isMotherTile,
					       std::vector<double> &thresholds,
					       int &belowTile,
					       int &threshIndex)
{
  TileThreshInfoGenBased info;
  if (_params._tileInfo.outOfBoundsY(tileIndex, belowTile))
  {
    // try to use the tile below
    if (_threshInfo.useTileBelow(tileIndex, belowTile, info))
    {
      _threshInfo[tileIndex] = info;
      vector<double>::const_iterator i;
      i = find(thresholds.begin(), thresholds.end(), info.getThresh());
      if (i != thresholds.end())
      {
	threshIndex = (int)(i - thresholds.begin());
	return true;
      }
    }
    LOG(ERROR) << "Tile order not as expected index=" << tileIndex
	       << " belowIndex=" << belowTile;
    threshIndex = setToMotherOrColdstart(tileIndex, isMotherTile);
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------
int LeadtimeThreadDataForField::setToMotherOrColdstart(int tileIndex, bool isMotherTile)
{
  if (isMotherTile)
  {
    // this means setting the mother tile has failed
    _motherFail = true;
    _motherSet = false;
    _motherPbarIndex = -1;
    _threshInfo[tileIndex] =
      TileThreshInfoGenBased(_coldstartThresh, _genTime, _leadTime, tileIndex, true, true);
    return -1;
  }
  else
  {
    if (_motherSet)
    {
      TileThreshInfoGenBased ret(_motherInfo);
      ret.setTileIndex(tileIndex);
      _threshInfo[tileIndex] = ret;
      return _motherPbarIndex;
    }
    else
    {
      _threshInfo[tileIndex] = 
	TileThreshInfoGenBased(_coldstartThresh, _genTime, _leadTime,
			       tileIndex, false, true);
      return -1;
    }
  }
}

//----------------------------------------------------------------------
void LeadtimeThreadDataForField::handleMotherResults(int motherIndex, int pbarIndex)
{
  if (!_motherFail)
  {
    _motherSet = true;
    _motherInfo = _threshInfo[motherIndex];
    _motherPbarIndex = pbarIndex;
  }
}

//------------------------------------------------------------------------
// use max threshold (or min threshold that gives 0), unless all thresolds
// give 0, in which case use default or mothertile threshold
int LeadtimeThreadDataForField::setupAndRunObsZero(int tileIndex,
						   const std::vector<double> &pbar,
						   const std::vector<double> &thresh,
						   bool isMotherTile, int obsThreshIndex)
{
  _currentThresh = _bestOld[tileIndex];
  double tb;
  if (obsThreshIndex < 0 || obsThreshIndex >= (int)_targetBias.size())
  {
    LOG(ERROR) << "logic error in accessing target bias values, use targetbias 0";
    LOG(ERROR) << "   obsThreshIndex=" << obsThreshIndex << " bias array size = " << _targetBias.size();
    tb = _targetBias[0];
  }
  else
  {
    tb = _targetBias[obsThreshIndex];
  }
  PbarVector pbarV(tb, thresh);
  for (size_t i=0; i<pbar.size(); ++i)
  {
    pbarV.setValue(i, pbar[i]);
  }
  pbarV.setGood();
  
  double bestThresh;
  double bestBias;
  int pbarIndex;
  if (!pbarV.bestWhenObsZero(_greaterOrEqual, bestThresh, bestBias, pbarIndex))
  {
    return setToMotherOrColdstart(tileIndex, isMotherTile);
  }
  else
  {
    if (isMotherTile)
    {
      _motherSet = true;
      _motherFail = false;
      _motherPbarIndex = pbarIndex;
    }
    _threshInfo[tileIndex] =
	TileThreshInfoGenBased(bestThresh,  _genTime, _leadTime, tileIndex,
			       isMotherTile, false, bestBias);
    return pbarIndex;
  }
}

//------------------------------------------------------------------------
// try current best, and if it gives a result of 0, only go down.
// and if all are zero use that minimum,
// if it gives a nonzero result go both up and down.
int LeadtimeThreadDataForField::setupAndRunObsNonZero(double oBar, int tileIndex,
						      const std::vector<double> &pbar,
						      const std::vector<double> &thresh,
						      bool isMotherTile, int obsThreshIndex)
{
  _currentThresh = _bestOld[tileIndex];
  double tb;
  if (obsThreshIndex < 0 || obsThreshIndex >= (int)_targetBias.size())
  {
    LOG(ERROR) << "logic error in accessing target bias values, use targetbias 0";
    LOG(ERROR) << "   obsThreshIndex=" << obsThreshIndex << " bias array size = " << _targetBias.size();
    tb = _targetBias[0];
  }
  else
  {
    tb = _targetBias[obsThreshIndex];
  }

  PbarVector pbarV(tb, thresh);
  for (size_t i=0; i<pbar.size(); ++i)
  {
    pbarV.setValue(i, pbar[i]);
  }
  pbarV.setGood();
  double bestThresh;
  double bestBias;
  int pbarIndex;
  if (!pbarV.bestWhenObsNonZero(oBar, _currentThresh,
				_greaterOrEqual, bestThresh, bestBias, pbarIndex))
  {
    if (!isMotherTile)
    {
      LOG(WARNING) << "No pbar. obar=" << oBar << " tileIndex=" << tileIndex
		   << " use mothertile";
    }
    else
    {
      LOG(ERROR) << "No pbar for mothertile, obar=" << oBar;
    }
    return setToMotherOrColdstart(tileIndex, isMotherTile);
  }
  else
  {
    if (isMotherTile)
    {
      _motherSet = true;
      _motherFail = false;
      _motherPbarIndex = pbarIndex;
    }
    _threshInfo[tileIndex] =
	TileThreshInfoGenBased(bestThresh, _genTime, _leadTime, tileIndex,
			       isMotherTile, false, bestBias);
    return pbarIndex;
  }
}    

//------------------------------------------------------------------------
void LeadtimeThreadDataForField::updateSpdbForAllTiles(SpdbGenBasedThreshHandler &spdb,
						       int index,
						       TaThreadDoubleQue &threads)
{
  threads.lockForIO();
  _threshInfo.update(spdb, index);
  threads.unlockAfterIO();
}



