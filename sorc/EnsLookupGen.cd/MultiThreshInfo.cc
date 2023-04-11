/**
 * @file MultiThreshInfo.cc
 */

#include "MultiThreshInfo.hh"
#include "DbThresh.hh"
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//-----------------------------------------------------------------------
MultiThreshInfo::MultiThreshInfo(const ParmsEnsLookupGen &parms) :
  _parms(parms), _dbGenTime(0)
{
  // // find the one field that has output obars to use and build up the _obarThresh
  // // vector from that
  // for (int i=0; i<_parms._fields.numOutputObarFields(); ++i)
  // {
  //   _obarThresh.
  //     push_back(ThreshForOneObar(_parms._fields.ithOutputObarFieldParms(i),
  // 				 _parms));
  // }

  for (int i=0; i<_parms._fields.numFieldParms(); ++i)
  {
    _dbThresh.push_back(DbThresh(_parms._fields.ithFieldParms(i),  _parms));
  }
}

//-----------------------------------------------------------------------
void MultiThreshInfo::newGenTime(const time_t &genTime)
{
  // when gen time changes, time to access all the databases and update
  vector<time_t> gtimes;
  for (size_t i=0; i<_dbThresh.size(); ++i)
  {
    time_t gt;
    if (_dbThresh[i].newGenTime(genTime, gt))
    {
      if (find(gtimes.begin(), gtimes.end(), gt) == gtimes.end())
      {
	gtimes.push_back(gt);
      }
    }
    else
    {
      LOG(ERROR) << "Could not acess database";
    }
  }
  
  if (gtimes.empty())
  {
    LOG(ERROR) << "Database not available for this gen time";
  }
  else if (gtimes.size() == 1)
  {
    _dbGenTime = gtimes[0];
  }
  else
  {
    _dbGenTime = gtimes[0];
    for (size_t i=1; i<gtimes.size(); ++i)
    {
      if (gtimes[i] < _dbGenTime)
      {
	_dbGenTime = gtimes[i];
      }
    }
    LOG(ERROR) << "mismatch in db gen times use oldest one "
	       << DateTime::strn(_dbGenTime);
    // now recalibrate to this old one
    for (size_t i=0; i<_dbThresh.size(); ++i)
    {
      time_t gt;
      _dbThresh[i].newGenTime(_dbGenTime, gt);
    }
  }
}

// //----------------------------------------------------------------------
// void MultiThreshInfo::newLeadTime(const time_t &genTime, int lt)
// {
//   // when the lead time updates, it's time to create all the
//   // threshold grids for that lead time, one per obar threshold
//   // for that field
//   for (size_t i=0; i<_obarThresh.size(); ++i)
//   {
//     _obarThresh[i].createThresholdGrids(genTime, lt, _dbThresh);
//   }
// }

// //----------------------------------------------------------------------
// void MultiThreshInfo::updateCount(const std::vector<double> &values,
// 				  int index, int x, int y)
// {
//   for (size_t i=0; i<_obarThresh.size(); ++i)
//   {
//     _obarThresh[i].updateCount(values, index, x, y);
//   }
// }

//----------------------------------------------------------------------
void MultiThreshInfo::prepareOutput(//MultiGrid &mOutGrid,
				    std::string &xml) const
{
  // for (size_t i=0; i<_obarThresh.size(); ++i)
  // {
  //   _obarThresh[i].prepareOutput(mOutGrid);
  // }
  for (size_t i=0; i<_dbThresh.size(); ++i)
  {
    _dbThresh[i].prepareOutput(xml);
  }
}

// //----------------------------------------------------------------------
// void MultiThreshInfo::setEnsembleSumMissing(int index)
// {
//   for (size_t i=0; i<_obarThresh.size(); ++i)
//   {
//     _obarThresh[i].setEnsembleSumMissing(index);
//   }
// }


// //----------------------------------------------------------------------
// void MultiThreshInfo::normalizeEnsembleSum(int index, double count, bool debug)
// {
//   for (size_t i=0; i<_obarThresh.size(); ++i)
//   {
//     _obarThresh[i].normalizeEnsembleSum(index, count, debug);
//   }
// }
