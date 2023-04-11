/**
 * @file LeadtimeThreadData.cc
 */

#include "LeadtimeThreadData.hh"

//----------------------------------------------------------------------
LeadtimeThreadData::
LeadtimeThreadData(const ParmsThreshFromObarPbar &params, const time_t &gt, int lt) :
  _params(params),
  _genTime(gt),
  _leadTime(lt),
  _is1(true),
  _field1(params, 1, gt, lt),
  _field2(params, 2, gt, lt)
{
}

//----------------------------------------------------------------------
LeadtimeThreadData::~LeadtimeThreadData()
{
}

//----------------------------------------------------------------
void LeadtimeThreadData::setInitialThresholds(int which, int obsThreshIndex)
{
  if (which == 1)
  {
    _is1 = true;
    _field1.setInitialThresholds(obsThreshIndex);
  }
  else
  {
    _is1 = false;
    _field2.setInitialThresholds(obsThreshIndex);
  }    
}

//-----------------------------------------------------------------------
bool LeadtimeThreadData::usedTileBelow(int tileIndex, bool isMotherTile,
				       std::vector<double> &thresholds,
				       int &belowTile,  int &threshIndex)
{
  if (_is1)
  {
    return _field1.usedTileBelow(tileIndex, isMotherTile, thresholds, belowTile, threshIndex);
  }
  else
  {
    return _field2.usedTileBelow(tileIndex, isMotherTile, thresholds, belowTile, threshIndex);
  }
}

//-----------------------------------------------------------------------
int LeadtimeThreadData::setToMotherOrColdstart(int tileIndex, bool isMotherTile)
{
  if (_is1)
  {
    return _field1.setToMotherOrColdstart(tileIndex, isMotherTile);
  }
  else
  {
    return _field2.setToMotherOrColdstart(tileIndex, isMotherTile);
  }
}

//----------------------------------------------------------------------
void LeadtimeThreadData::handleMotherResults(int motherIndex, int pbarIndex)
{
  if (_is1)
  {
    return _field1.handleMotherResults(motherIndex, pbarIndex);
  }
  else 
  {
    return _field2.handleMotherResults(motherIndex, pbarIndex);
  }
}

//------------------------------------------------------------------------
// use max threshold (or min threshold that gives 0), unless all thresolds
// give 0, in which case use default or mothertile threshold
int LeadtimeThreadData::setupAndRunObsZero(int tileIndex,
					   const std::vector<double> &pbar,
					   const std::vector<double> &thresh,
					   bool isMotherTile, int obsThreshIndex)
{
  if (_is1)
  {
    return _field1.setupAndRunObsZero(tileIndex, pbar, thresh, isMotherTile, obsThreshIndex);
  }
  else
  {
    return _field2.setupAndRunObsZero(tileIndex, pbar, thresh, isMotherTile, obsThreshIndex);
  }
}

//------------------------------------------------------------------------
// try current best, and if it gives a result of 0, only go down.
// and if all are zero use that minimum,
// if it gives a nonzero result go both up and down.
int LeadtimeThreadData::setupAndRunObsNonZero(double oBar, int tileIndex,
					      const std::vector<double> &pbar,
					      const std::vector<double> &thresh,
					      bool isMotherTile, int obsThreshIndex)
{
  if (_is1)
  {
    return _field1.setupAndRunObsNonZero(oBar, tileIndex, pbar, thresh, isMotherTile, obsThreshIndex);
  }
  else
  {
    return _field2.setupAndRunObsNonZero(oBar, tileIndex, pbar, thresh, isMotherTile, obsThreshIndex);
  }
}    

//------------------------------------------------------------------------
void LeadtimeThreadData::updateSpdbForAllTiles(SpdbGenBasedThreshHandler &spdb,
					       int index, int which,
					       TaThreadDoubleQue &threads)
{
  if (which == 1)
  {
    _field1.updateSpdbForAllTiles(spdb, index, threads);
  }
  else
  {
    _field2.updateSpdbForAllTiles(spdb, index, threads);
  }
}




