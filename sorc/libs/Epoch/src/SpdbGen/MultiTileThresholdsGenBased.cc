/**
 * @file MultiTileThresholdsGenBased.cc
 */

//------------------------------------------------------------------
#include <Epoch/MultiTileThresholdsGenBased.hh>
#include <Epoch/TileThreshInfoGenBased.hh>
#include <Epoch/TileInfo.hh>
#include <euclid/GridAlgs.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

const std::string MultiTileThresholdsGenBased::_tag = "Lt";

//------------------------------------------------------------------
MultiTileThresholdsGenBased::
MultiTileThresholdsGenBased(const TileInfo &tiling) :
  _ok(false), _valuesSet(false)
{
  for (int i=0; i<tiling.numTiles(); ++i)
  {
    _thresh.push_back(SingleTileThresholdsGenBased());
  }
}

//------------------------------------------------------------------
MultiTileThresholdsGenBased::
MultiTileThresholdsGenBased(const std::string &xml, const TileInfo &tiling) :
  _ok(true)
{
  if (TaXml::readBoolean(xml, "ValuesSet", _valuesSet))
  {
    LOG(ERROR) << "Reading tag for ValuesSet";
    _ok = false;
    return;
  }
  
  // read in tiles array, and compare to input.
  std::vector<std::string> vstring;
  if (TaXml::readStringArray(xml, SingleTileThresholdsGenBased::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag as array " << SingleTileThresholdsGenBased::_tag;
    _ok = false;
    return;
  }

  if (static_cast<int>(vstring.size()) != tiling.numTiles())
  {
    LOG(ERROR) << "Inconsistent tiling input:" << tiling.numTiles() 
	       << " xml:" << vstring.size();
    _ok = false;
    return;
  }

  // for every element, parse it as a SingleTileThresholds object.
  for (size_t i=0; i<vstring.size(); ++i)
  {
    SingleTileThresholdsGenBased m(vstring[i]);
    if (!m.ok())
    {
      _ok = false;
    }
    _thresh.push_back(m);
  }
}

//------------------------------------------------------------------
MultiTileThresholdsGenBased::
MultiTileThresholdsGenBased(double coldstartThresh, const TileInfo &tiling) :
  _ok(true), _valuesSet(false)
{
  for (int i=0; i<tiling.numTiles(); ++i)
  {
    bool isMother = TileInfo::isMotherTile(i);
    _thresh.push_back(SingleTileThresholdsGenBased(coldstartThresh, isMother));
  }
}

//------------------------------------------------------------------
MultiTileThresholdsGenBased::~MultiTileThresholdsGenBased(void)
{
}

//------------------------------------------------------------------
std::vector<double> MultiTileThresholdsGenBased::thresholds(void) const
{
  vector<double> ret;
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    ret.push_back(_thresh[i].getThresh());
  }
  return ret;
}

//------------------------------------------------------------------
std::vector<double> MultiTileThresholdsGenBased::thresholdsMotherOnly(void) const
{
  vector<double> ret;
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (_thresh[i].getMotherTile())
    {
      ret.push_back(_thresh[i].getThresh());
    }
    else
    {
      ret.push_back(0.0);
    }
  }
  return ret;
}

//------------------------------------------------------------------
std::vector<TileThreshInfoGenBased>
MultiTileThresholdsGenBased::getTileThreshInfo(const time_t &gen,
					       int lead) const
{
  vector<TileThreshInfoGenBased> ret;
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    TileThreshInfoGenBased e(_thresh[i].getThresh(), gen, lead, i,
			     _thresh[i].getMotherTile(),
			     _thresh[i].getColdstart(), _thresh[i].getBias());
    ret.push_back(e);
  }
  return ret;
}

//------------------------------------------------------------------
bool MultiTileThresholdsGenBased::update(const TileThreshInfoGenBased &item)
{
  SingleTileThresholdsGenBased *m = _mapFromTileIndex(item.getTileIndex());
  if (m == NULL)
  {
    LOG(ERROR) << "Tile index out of range " << item.getTileIndex();
    return false;
  }
  _valuesSet = m->update(item);
  return _valuesSet;
}

//------------------------------------------------------------------
std::string MultiTileThresholdsGenBased::toXml(int indent) const
{
  string s = TaXml::writeStartTag(_tag, indent);
  s += TaXml::writeBoolean("ValuesSet", 0, _valuesSet);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    s += _thresh[i].toXml(i);
  }
  s += TaXml::writeEndTag(_tag, indent);
  return s;
}

//------------------------------------------------------------------
bool
MultiTileThresholdsGenBased::constructTiledGrid(const std::string &fieldName,
						const TileInfo &tiling,
						double centerWeight,
						double edgeWeight,
						int nptSmooth,
						Grid2d &grid,
						bool motherOnly) const
{
  // assume naming same for all tiles
  const SingleTileThresholdsGenBased *mt = _constMapFromTileIndex(0);
  if (mt == NULL)
  {
    LOG(ERROR) << "No content, no grid";
    return false;
  }

  vector<double> tileThresh;

  if (motherOnly)
  {
    tileThresh = thresholdsMotherOnly();
  }
  else
  {
    tileThresh = thresholds();
  }

  // return tiling.constructTiledGridNoOverlap(fieldName, tileThresh, 
  // 					    grid);
  return tiling.constructWeightedTiledGrid(fieldName, tileThresh, 
   					   centerWeight, edgeWeight,
   					   nptSmooth, grid);
}

//------------------------------------------------------------------
void MultiTileThresholdsGenBased::setThresh(int tileIndex, double v,
					    bool isColdstart)
{
  _thresh[tileIndex].setThreshold(v);
  _thresh[tileIndex].setColdstart(isColdstart);
}

//------------------------------------------------------------------
void
MultiTileThresholdsGenBased::print(int lt, double obarThresh, const TileInfo &info,
				   bool verbose) const
{
  printf(" --- Obar threshold %5.2lf -----\n", obarThresh);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    _thresh[i].print(lt, obarThresh, i, info, verbose);
  }
}

//------------------------------------------------------------------
void
MultiTileThresholdsGenBased::print(int lt, double obarThresh,
				   const std::vector<int> &tiles,
				    const TileInfo &info,
				    bool verbose) const
{
  if (tiles.empty())
  {
    print(lt, obarThresh, info, verbose);
  }
  else
  {
    printf(" --- Obar threshold %5.2lf -----\n", obarThresh);
    for (size_t i=0; i<_thresh.size(); ++i)
    {
      if (find(tiles.begin(), tiles.end(), (int)i) != tiles.end())
      {
	_thresh[i].print(lt, obarThresh, i, info, verbose);
      }
    }
  }
}

//------------------------------------------------------------------
SingleTileThresholdsGenBased *
MultiTileThresholdsGenBased::_mapFromTileIndex(int tileInd)
{
  return &_thresh[tileInd];
}

//------------------------------------------------------------------
const SingleTileThresholdsGenBased *
MultiTileThresholdsGenBased::_constMapFromTileIndex(int tileInd) const
{
  return &_thresh[tileInd];
}
