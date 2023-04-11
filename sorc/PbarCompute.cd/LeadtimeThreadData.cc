/**
 * @file LeadtimeThreadData.cc
 */

#include "LeadtimeThreadData.hh"
#include <toolsa/TaThreadDoubleQue.hh>
#include "PbarVector.hh"

#include <Epoch/TileRange.hh>

//----------------------------------------------------------------------
LeadtimeThreadData::
LeadtimeThreadData(const ParmsPbarCompute &params, const time_t &gt, int lt) :
  _params(params),
  _additionalInputs(params),
  _genTime(gt),
  _leadTime(lt),
  _aGridSet1(false),
  _aGridSet2(false)
{
}

//----------------------------------------------------------------------
LeadtimeThreadData::~LeadtimeThreadData()
{
  if (_aGridSet1)
  {
    for (size_t i=0; i<_aGrid1.size(); ++i)
    {
      delete _aGrid1[i];
    }
  }
  if (_aGridSet2)
  {
    for (size_t i=0; i<_aGrid2.size(); ++i)
    {
      delete _aGrid2[i];
    }
  }
}

//----------------------------------------------------------------------
void LeadtimeThreadData::setupCountSums(const Grid &egrid)
{
  int i=0;
  for (double thresh = _params._threshMin1; thresh <= _params._threshMax1;
       thresh += _params._threshDelta1)
  {
    if (!_aGridSet1)
    {
      // use that to initialize for all thresholds
      _aGrid1.push_back(new AveragingGrids(_params._inputThresholdedField1, egrid));
    }
    else
    {
      _aGrid1[i]->initialize();
      i++;
    }
  }
  _aGridSet1 = true;
  _aGridModified1 = false;

  i=0;
  for (double thresh2 = _params._threshMin2; thresh2 <= _params._threshMax2;
       thresh2 += _params._threshDelta2)
  {
    for (double thresh1 = _params._threshMin1; thresh1 <= _params._threshMax1;
	 thresh1 += _params._threshDelta1)
    {
      if (!_aGridSet2)
      {
	// use that to initialize for all thresholds
	_aGrid2.push_back(new AveragingGrids(_params._inputThresholdedField2, egrid));
      }
      else
      {
	_aGrid2[i]->initialize();
	i++;
      }
    }
  }
  _aGridSet2 = true;
  _aGridModified2 = false;
  _additionalInputs.reset();
}

//------------------------------------------------------------------------
bool LeadtimeThreadData::setAdditionalGridPointers(const MultiGrid &mInGrid)
{
  return _additionalInputs.setGridPointers(mInGrid);
}

//----------------------------------------------------------------------
bool LeadtimeThreadData::setAdditionalValues(int gridIndex, int which)
{
  return _additionalInputs.setAllValues(gridIndex, which);
}

//-----------------------------------------------------------------------
void LeadtimeThreadData::updateCountSums(int gridIndex, double thresholdedValue1,
					 bool hasValue2, double thresholdedValue2)
{
  int i=0;
  _aGridModified1 = true;
  for (double thresh1 = _params._threshMin1; thresh1 <= _params._threshMax1;
       thresh1 += _params._threshDelta1,++i)
  {
    _aGrid1[i]->incrementCount(gridIndex);
    if (_params.passesTests1(thresholdedValue1, thresh1) &&
	_additionalInputs.passesTests(1))
    {
      _aGrid1[i]->increment(gridIndex);
    }
  }
  int i1=0;
  for (double thresh1 = _params._threshMin1; thresh1 <= _params._threshMax1;
       thresh1 += _params._threshDelta1)
  {
    int i2=0;
    for (double thresh2 = _params._threshMin2; thresh2 <= _params._threshMax2;
	 thresh2 += _params._threshDelta2)
    {
      int index = _params.index2d(i1, i2);
      _aGrid2[index]->incrementCount(gridIndex);
      if (hasValue2 && _params.passesTests2(thresholdedValue1, thresholdedValue2,
					    thresh1, thresh2) &&
	_additionalInputs.passesTests(2))
      {
	_aGrid2[index]->increment(gridIndex);
      }
      ++i2;
    }
    ++i1;
  }
}	

//----------------------------------------------------------------------
void LeadtimeThreadData::normalizeCountSums(void)
{
  for (int i=0; i<_params._numThresh1; ++i)
  {
    _aGrid1[i]->normalize();
  }
  for (int i=0; i<_params._numThresh1*_params._numThresh2; ++i)
  {
    _aGrid2[i]->normalize();
  }
}

//------------------------------------------------------------------------
 PbarVector LeadtimeThreadData::runAlg(int tileIndex, int which)
{
  // get the tile dimensions
  TileRange r = _params._tileInfo.range(tileIndex);

  // create a new object based on which 1 or 2
  PbarVector ret(_params, which);

  if (which == 1)
  {
    for (size_t i=0; i<_aGrid1.size(); ++i)
    {
      double pBar;
      // pull pbar out of averaging grid at this index (threshold)
      if (_fcstComputePbarAtThresh(r, _aGrid1[i]->dataGridRef(), pBar))
      {
	ret.setValue(i, pBar, which);
      }
    }
  }
  else
  {
    for (size_t i=0;i <_aGrid2.size(); ++i)
    {
      double pBar;
      // pull pbar out of averaging grid at this index (threshold pair)
      if (_fcstComputePbarAtThresh(r, _aGrid2[i]->dataGridRef(), pBar))
      {
	ret.setValue(i, pBar, which);
      }
    }
  }
  ret.setGood(which);
  return ret;
}

//-----------------------------------------------------------------------
bool LeadtimeThreadData::useTileBelow(int tileIndex, int &belowTile)
{
  if (_params._tileInfo.outOfBoundsY(tileIndex, belowTile))
  {
    return true;
  }
  else
  {
    return false;
  }
}


//------------------------------------------------------------------------
bool LeadtimeThreadData::_fcstComputePbarAtThresh(const TileRange &r,
						  const Grid &fcst, 
						  double &pBar) const
{
  bool outOfBounds;
  if (!fcst.meanSubset(r.getX0(), r.getY0(), r.getNx(), r.getNy(),
		       true, false, pBar, outOfBounds))
  {
    if (outOfBounds)
    {
      LOG(ERROR) << "Tile " << r.sprint() << " goes out of bounds";
    }
    else
    {
      LOG(WARNING) << "Cannot compute mean forecast data above threshold";
    }
    return false;
  }
  if (pBar == 0.0)
  {
    // LOG(WARNING) << "No forecast data above threshold " << thresh;
  }
  return true;
}

