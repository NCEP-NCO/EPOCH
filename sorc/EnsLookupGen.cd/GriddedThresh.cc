/**
 * @file GriddedThresh.cc
 */

#include "GriddedThresh.hh"
#include "MultiThreshInfo.hh"
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//-----------------------------------------------------------------------
GriddedThresh::GriddedThresh(const ParmsEnsLookupGen &parms) :
  _parms(parms)
{
  // find the one field that has output obars to use and build up the _obarThresh
  // vector from that
  for (int i=0; i<_parms._fields.numOutputObarFields(); ++i)
  {
    _obarThresh.
      push_back(ThreshForOneObar(_parms._fields.ithOutputObarFieldParms(i),
				 _parms));
  }
}

//----------------------------------------------------------------------
void GriddedThresh::newLeadTime(const time_t &genTime, int lt,
				const MultiThreshInfo &m)
{
  // when the lead time updates, it's time to create all the
  // threshold grids for that lead time, one per obar threshold
  // for that field
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    _obarThresh[i].createThresholdGrids(genTime, lt, m.getDbThresh());
  }
}

//----------------------------------------------------------------------
void GriddedThresh::updateCount(const std::vector<double> &values,
				  int index, int x, int y)
{
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    _obarThresh[i].updateCount(values, index, x, y);
  }
}

//----------------------------------------------------------------------
void GriddedThresh::prepareOutput(MultiGrid &mOutGrid)
{
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    _obarThresh[i].prepareOutput(mOutGrid);
  }
}

//----------------------------------------------------------------------
void GriddedThresh::setEnsembleSumMissing(int index)
{
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    _obarThresh[i].setEnsembleSumMissing(index);
  }
}

//----------------------------------------------------------------------
void GriddedThresh::normalizeEnsembleSum(int index, double count, bool debug)
{
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    _obarThresh[i].normalizeEnsembleSum(index, count, debug);
  }
}
