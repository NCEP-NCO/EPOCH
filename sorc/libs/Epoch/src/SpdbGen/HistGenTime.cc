/**
 * @file HistGenTime.cc
 */
#include <Epoch/HistGenTime.hh>
#include <Epoch/TileThreshInfoGenBased.hh>
#include <Epoch/TileInfo.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//-------------------------------------------------------------------------
HistGenTime::HistGenTime(const time_t &gen, std::vector<int> &leadSeconds,
			 int numObarThresh, int numTiles) :
  _genTime(gen), _leadSeconds(leadSeconds)
{
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    _histAtLead.push_back(HistLeadTime(_leadSeconds[i], numObarThresh, numTiles));
  }
}

//-------------------------------------------------------------------------
void HistGenTime::update(const std::vector<TileThreshInfoGenBased> &info,
			 int ltSec, int obarThreshIndex)
{
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Bad lead  " << ltSec;
    return;
  }
  _histAtLead[i].update(info, obarThreshIndex);
}

//-------------------------------------------------------------------------
std::vector<double> HistGenTime::getAverages(int ltSec, int obarThreshIndex) const
{
  std::vector<double> ret;
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Bad lead  " << ltSec;
  }
  else
  {
    ret = _histAtLead[i].getAverages(obarThreshIndex);
  }
  return ret;
}
  
//-------------------------------------------------------------------------
void HistLeadTime::update(const std::vector<TileThreshInfoGenBased> &info,
			  int obarThreshIndex)
{
  if (obarThreshIndex < 0 || obarThreshIndex >= (int)_histOneObarThresh.size())
  {
    LOG(ERROR) << "Index bad " << obarThreshIndex;
    return;
  }
  _histOneObarThresh[obarThreshIndex].update(info);
}

//-------------------------------------------------------------------------
std::vector<double> HistLeadTime::getAverages(int obarThreshIndex) const
{
  if (obarThreshIndex < 0 || obarThreshIndex >= (int)_histOneObarThresh.size())
  {
    LOG(ERROR) << "Index bad " << obarThreshIndex;
    std::vector<double> ret;
    return ret;
  }
  return _histOneObarThresh[obarThreshIndex].getAverages();
}


//-------------------------------------------------------------------------
void HistOneObarThresh::update(const std::vector<TileThreshInfoGenBased> &info)

{
  for (size_t i=0; i<info.size(); ++i)
  {
    _histOneTile[i].update(info[i]);
  }
}

//-------------------------------------------------------------------------
std::vector<double> HistOneObarThresh::getAverages(void) const
{
  std::vector<double> ret;
  for (size_t i=0; i<_histOneTile.size(); ++i)
  {
    double v;
    if (_histOneTile[i].average(v))
    {
      ret.push_back(v);
    }
    else
    {
      ret.push_back(-1);
    }
  }
  return ret;
}

//-------------------------------------------------------------------------
void HistOneTile::update(const TileThreshInfoGenBased &info)
{
  bool isMother = TileInfo::isMotherTile(_index);
  if (info.getMotherTile() && !isMother)
  {
    return;
  }
  if (info.getColdstart())
  {
    return;
  }
  
  _count += 1.0;
  _sum = _sum + info.getThresh();
}

//-------------------------------------------------------------------------
int HistGenTime::_leadIndex(int leadTime) const
{
  std::vector<int>::const_iterator i;
  i = find(_leadSeconds.begin(), _leadSeconds.end(), leadTime);
  if (i == _leadSeconds.end())
  {
    return -1;
  }
  else
  {
    return (int)(i - _leadSeconds.begin());
  }
}
