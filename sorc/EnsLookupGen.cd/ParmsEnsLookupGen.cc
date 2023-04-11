/**
 * @file ParmsEnsLookupGen.cc
 */
#include "ParmsEnsLookupGen.hh"
#include <euclid/Grid2d.hh>

//-----------------------------------------------------------
void ParmsEnsLookupGen::
createFixedTiledGrid(const std::string &fname,
			     double obarThresh, double thresh,
			     Grid2d &thresholdGrid) const
{
  char buf[1000];
  sprintf(buf, "%04.2lf", obarThresh);
  string name = fname;
  name = name + buf;
  thresholdGrid = Grid2d(name, _proj.pNx, _proj.pNy, -99.99);
  thresholdGrid.setAllToValue(thresh);
}

//-----------------------------------------------------------
double ParmsEnsLookupGen::insideWeight(int nptIn) const
{
  if (_maxNptInside <= 0)
  {
    return 1.0;
  }
  if (nptIn > _maxNptInside)
  {
    return 1.0;
  }
  else
  {
    return (double)nptIn / (double)_maxNptInside;
  }
}

//-----------------------------------------------------------
bool ParmsEnsLookupGen::isDebugPoint(int x, int y) const
{
  if (_debugXY)
  {
    return x == _debugX && y == _debugY;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------
bool ParmsEnsLookupGen::checkDb(const std::vector<int> &lt,
				const TileInfo &tileInfo) const
{
  bool stat = true;
  if (lt.size() != _leadSeconds.size())
  {
    LOG(ERROR) << "DB leads size not same as config";
    stat = false;
  }
  else
  {
    for (size_t i=0; i<lt.size(); ++i)
    {
      if (lt[i] != _leadSeconds[i])
      {
	LOG(ERROR) << "leadtimeh mismatch";
	stat = false;
      }
    }
  }
  return stat;
}
