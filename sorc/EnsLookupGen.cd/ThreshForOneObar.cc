/**
 * @file ThreshForOneObar.cc
 */

#include "ThreshForOneObar.hh"
#include "DbThresh.hh"
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/ConvWxConstants.hh>

//-------------------------------------------------------------------------
static Grid _convert(const Grid2d &g)
{
  vector<double> data = g.getData();
  Grid ret(g.getName(), "none", data, g.getNx(), g.getNy(), g.getMissing());
  return ret;
}

//-------------------------------------------------------------------------
ThreshForOneObar::ThreshForOneObar(const ObarThreshParms &obarParm,
		   const ParmsEnsLookupGen &parm) :
  _oparms(obarParm),  _parms(parm)
{
  _ensembleSum = Grid(obarParm._griddedFieldName, "none", parm._projExtended.pNx,
		      parm._projExtended.pNy, -99.99);
}

//-------------------------------------------------------------------------
void ThreshForOneObar::createThresholdGrids(const time_t &genTime, int lt,
					    const std::vector<DbThresh> &dbThresh)
{
  // for each field (dbThresh), create a grid at this obar
  _thresholdGrids.clear();
  _doOutputThresholdsGrid.clear();
  for (size_t i=0; i<dbThresh.size(); ++i)
  {
    Grid2d g;
    bool doOutput;
    dbThresh[i].createThresholdGrid(genTime, lt, _oparms, g, doOutput);
    _thresholdGrids.push_back(g);
    _doOutputThresholdsGrid.push_back(doOutput);
  }
  // set all data values to 0, and counts
  _ensembleSum.setAllToValue(0.0);
}

//----------------------------------------------------------------------
void ThreshForOneObar::updateCount(const std::vector<double> &values,
				   int index, int x, int y)
{
  bool status = true;
  for (size_t i=0; i<values.size(); ++i)
  {
    double thresh;
    if (!_thresholdGrids[i].getValue(x, y, thresh))
    {
      LOG(ERROR) << "Getting threshold from database at point ("
		 << x << "," << y << ")";
      return;
    }
    if (!_parms._fields.ithFieldParms(i).threshTest(values[i], thresh))
    {
      status = false;
    }
  }

  if (status)
  {
    // here passed the tests
    _ensembleSum.incrementValueAtPoint(index, 1.0);
  }
}

//----------------------------------------------------------------------
void ThreshForOneObar::prepareOutput(MultiGrid &mOutGrid) const
{
  mOutGrid.append(_ensembleSum);
  for (size_t i=0; i<_thresholdGrids.size(); ++i)
  {
    if (_doOutputThresholdsGrid[i])
    {
      // because of the design the 'non output' thresholds grids
      // can get put in multiple times, prevent that..
      string name = _thresholdGrids[i].getName();
      if (mOutGrid.matchingFieldIndex(name) == convWx::BAD_INDEX)
      {
	// convert from grid2d to grid
	Grid g = _convert(_thresholdGrids[i]);
	mOutGrid.append(g);
      }
    }
  }    
}

//----------------------------------------------------------------------
void ThreshForOneObar::setEnsembleSumMissing(int index)
{
  _ensembleSum.setToMissing(index);
}


//----------------------------------------------------------------------
void ThreshForOneObar::normalizeEnsembleSum(int index, double count, bool debug)
{
  double val;
      
  if (count < 0.000001)  // prevent unstable and impossible
  {
    if (debug) printf("%s Setting output to missing N too small %lf\n",
		      _oparms._griddedFieldName.c_str(), count);
    setEnsembleSumMissing(index);
  }
  else
  {
    if (_ensembleSum.getValue(index, val))
    {
      if (debug) printf("%s Normalizing output:  %lf/%lf=%lf\n",
			_oparms._griddedFieldName.c_str(), val, count, val/count);
      _ensembleSum.setv(index, val/count);
    }
  }
}
