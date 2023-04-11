/**
 * @file ThreshObarInfo.cc
 */

#include <Epoch/ThreshObarInfo.hh>
#include <Epoch/TileInfo.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>

const std::string ThreshObarInfo::_tag = "ObarThreshInfo";

//-----------------------------------------------------------
ThreshObarInfo::ThreshObarInfo(const std::string &xml) :
  _ok(true)
{
  if (TaXml::readDouble(xml, "Threshold", _thresh))
  {
    LOG(ERROR) << "No oBar tag for Threshold";
    _ok = false;
  }
  vector<string> vstring;
  if (TaXml::readStringArray(xml, TileObarInfo::_tag, vstring))
  {
    LOG(ERROR) << "String array tag missing, " << TileObarInfo::_tag;
    _ok = false;
  }
  
  _info.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    TileObarInfo m(vstring[i]);
    if (m.ok())
    {
      _info.push_back(m);
    }
    else
    {
      _ok = false;
    }
  }
}

//-----------------------------------------------------------
std::string ThreshObarInfo::toXml(int indent) const
{
  string s = TaXml::writeStartTag(_tag, indent);
  s += TaXml::writeDouble("Threshold", indent, _thresh);
  for (size_t i=0; i<_info.size(); ++i)
  {
    s += _info[i].toXml(indent);
  }
  s += TaXml::writeEndTag(_tag, indent);
  return s;
}

//-----------------------------------------------------------
void ThreshObarInfo::update(const TileObarInfo &info)
{
  int index = info.getTileIndex();
  _info[index] = info;
}

//-----------------------------------------------------------
void ThreshObarInfo::update(int tileIndex, const TileObarInfo &info)
{
  int index = info.getTileIndex();
  if (index != tileIndex)
  {
    LOG(ERROR) << "Mismatch in indices";
    return;
  }
  _info[index] = info;
}

//-----------------------------------------------------------
void ThreshObarInfo::print(void) const
{
  printf("Thresh:%.2lf\n", _thresh);
  for (size_t i=0; i<_info.size(); ++i)
  {
    _info[i].print();
  }
}

//-----------------------------------------------------------
void ThreshObarInfo::print(const TileInfo &tiling, bool verbose) const
{
  printf("Thresh:%.2lf\n", _thresh);
  for (size_t i=0; i<_info.size(); ++i)
  {
    _info[i].print(tiling, verbose);
  }
}

//-----------------------------------------------------------
void ThreshObarInfo::print(const std::vector<int> &tiles,
			   const TileInfo &tiling, bool verbose) const
{
  printf("Thresh:%.2lf\n", _thresh);
  for (size_t i=0; i<tiles.size(); ++i)
  {
    _info[tiles[i]].print(tiling, verbose);
  }
}

//------------------------------------------------------------------
bool ThreshObarInfo::getTiledGrid(const std::string &inputName,
				  double obarThresh,
				  const TileInfo &tiling,
				  double centerWeight, double edgeWeight,
				  int nptSmooth,
				  Grid2d &item) const
{
  char buf[1000];
  sprintf(buf, "_%05.2lf_obar", obarThresh);
  std::string name = inputName + buf;

  vector<double> tileObar;
  for (size_t i=0; i<_info.size(); ++i)
  {
    tileObar.push_back(_info[i].getObar());
  }

  return tiling.constructWeightedTiledGrid(name, tileObar, 
   					   centerWeight, edgeWeight,
   					   nptSmooth, item);
}

