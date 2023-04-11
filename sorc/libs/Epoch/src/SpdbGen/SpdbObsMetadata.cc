/**
 * @file SpdbObsMetadata.cc
 */

//------------------------------------------------------------------
#include <Epoch/SpdbObsMetadata.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//------------------------------------------------------------------
SpdbObsMetadata::SpdbObsMetadata(void): _valuesSet(false)
{
}

//------------------------------------------------------------------
SpdbObsMetadata::SpdbObsMetadata(const std::string &field,
				 const TileInfo &tiling,
				 const std::vector<double> &thresh) :
  _valuesSet(true),
  _field(field),
  _thresh(thresh),
  _tiling(tiling),
  _obsTime(0)
{
  for (size_t i=0; i<thresh.size(); ++i)
  {
    _info.push_back(ThreshObarInfo(thresh[i], tiling.numTiles()));
  }
}

//------------------------------------------------------------------
SpdbObsMetadata::~SpdbObsMetadata()
{
}

//------------------------------------------------------------------
bool SpdbObsMetadata::fromXml(const std::string &xml)
{
  if (!_valuesSetFromXml(xml))
  {
    return false;
  }
  if (!_fieldFromXml(xml))
  {
    return false;
  }
  if (!_threshFromXml(xml))
  {
    return false;
  }
  if (!_tilingFromXml(xml))
  {
    return false;
  }
  if (!_obsTimeFromXml(xml))
  {
    return false;
  }

  if (!_infoFromXml(xml))
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------
std::string SpdbObsMetadata::toXml(void) const
{
  string ret = _valuesSetToXml();
  ret += _fieldToXml();
  ret += _threshToXml();
  ret += _tiling.toXml();
  ret +=_obsTimeToXml();
  ret +=_infoToXml();
  return ret;
}

//------------------------------------------------------------------
void SpdbObsMetadata::update(int threshIndex,
			     const TileObarInfo &info)
{
  if (threshIndex < 0 || threshIndex >= (int)_info.size())
  {
    LOG(ERROR) << "indexing mismatch";
    return;
  }
  _info[threshIndex].update(info);
}

//------------------------------------------------------------------
void SpdbObsMetadata::update(int threshIndex,
			     int tileIndex,
			     const TileObarInfo &info)
{
  if (threshIndex < 0 || threshIndex >= (int)_info.size())
  {
    LOG(ERROR) << "indexing mismatch";
    return;
  }
  _info[threshIndex].update(tileIndex, info);
}

//------------------------------------------------------------------
void SpdbObsMetadata::print(void) const
{
  printf("Obar:  Field:%s Time:%s", _field.c_str(),
	 DateTime::strn(_obsTime).c_str());
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    printf("thr:%.2lf ", _thresh[i]);
  }
  printf("\n");
  bool verbose = false;
  _tiling.print(verbose);
  for (size_t i=0; i<_info.size(); ++i)
  {
    _info[i].print();
  }
}

//------------------------------------------------------------------
bool SpdbObsMetadata::getTiledGrid(
				   double obarThresh, double centerWeight,
				   double edgeWeight, int nptSmooth,
				   Grid2d &item) const
{
  int threshIndex = _threshIndex(obarThresh);
  if (threshIndex < 0)
  {
    LOG(ERROR) << "Obar threshold not found in state " << obarThresh;
    return false;
  }

  return _info[threshIndex].getTiledGrid(_field, obarThresh, _tiling,
					 centerWeight, edgeWeight, nptSmooth, item);
}

//------------------------------------------------------------------
void SpdbObsMetadata::printState( const time_t &t, const time_t &timeWritten,
				  const std::vector<int> &tiles,
				  bool verbose) const
{
  if (tiles.empty())
  {
    printState(t, timeWritten, verbose);
    return;
  }
  printf("---------Obar information %s ---written %s, for %s----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(timeWritten).c_str(), 
	 _field.c_str());
  _tiling.print(verbose);
  for (size_t i=0; i<_info.size(); ++i)
  {
    _info[i].print(tiles, _tiling, verbose);
  }
}

//------------------------------------------------------------------
void SpdbObsMetadata::printState(const time_t &t, const time_t &timeWritten,
				  bool verbose) const
{
  printf("---------Obar information %s ---written %s, for %s----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(timeWritten).c_str(), 
	 _field.c_str());
  _tiling.print(verbose);
  for (size_t i=0; i<_info.size(); ++i)
  {
    _info[i].print(_tiling, verbose);
  }
}

//------------------------------------------------------------------
void SpdbObsMetadata::showTilingInfo(const time_t &time) const
{
  printf("---------Obar tile information %s ----------\n",
	 DateTime::strn(time).c_str());
  _tiling.print(false);
}

//------------------------------------------------------------------
int SpdbObsMetadata::_threshIndex(double obarThresh) const
{
  std::vector<double>::const_iterator i;
  i = find(_thresh.begin(), _thresh.end(), obarThresh);
  if (i == _thresh.end())
  {
    LOG(ERROR) << "Unknown obar threshold " << obarThresh;
    return -1;
  }
  else
  {
    return (int)(i - _thresh.begin());
  }
}

//------------------------------------------------------------------
std::string SpdbObsMetadata::_valuesSetToXml(void) const
{
  string s = TaXml::writeBoolean("ValuesSet", 0, _valuesSet);
  return s;
}

//------------------------------------------------------------------
std::string SpdbObsMetadata::_fieldToXml(void) const
{
  string s2 = TaXml::writeString("Field", 0, _field);
  return s2;
}

//------------------------------------------------------------------
std::string SpdbObsMetadata::_threshToXml(void) const
{
  string s2 = "";
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    s2 += TaXml::writeDouble("ObarThresh", 0, _thresh[i]);
  }
  return s2;
}

//------------------------------------------------------------------
std::string SpdbObsMetadata::_obsTimeToXml(void) const
{
  string s2 = TaXml::writeTime("ObsTime", 0, _obsTime);
  return s2;
}

//------------------------------------------------------------------
std::string SpdbObsMetadata::_infoToXml(void) const
{
  string s = "";
  for (size_t i=0; i<_info.size(); ++i)
  {
    s += _info[i].toXml(0);
  }
  return s;
}

//------------------------------------------------------------------
bool SpdbObsMetadata::_valuesSetFromXml(const std::string &xml)
{
  if (TaXml::readBoolean(xml, "ValuesSet", _valuesSet))
  {
    LOG(ERROR) << "No XML for ValuesSet";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbObsMetadata::_fieldFromXml(const std::string &xml)
{
  if (TaXml::readString(xml, "Field", _field))
  {
    LOG(ERROR) << "No XML with key Field";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbObsMetadata::_threshFromXml(const std::string &xml)
{
  vector<string> vstring;
  if (TaXml::readStringArray(xml, "ObarThresh", vstring))
  {
    LOG(ERROR) << "String array tag missing, ObarThresh";
    return false;
  }
  
  _thresh.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    double v;
    if (sscanf(vstring[i].c_str(), "%lf", &v) != 1)
    {
      LOG(ERROR) << "Could not scan string as double " << vstring[i];
      return false;
    }
    _thresh.push_back(v);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbObsMetadata::_tilingFromXml(const std::string &xml)
{
  TileInfo tinfo(xml);
  if (!tinfo.isOk())
  {
    return false;
  }
  _tiling = tinfo;
  return true;
}

//------------------------------------------------------------------
bool SpdbObsMetadata::_obsTimeFromXml(const std::string &xml)
{
  if (TaXml::readTime(xml, "ObsTime", _obsTime))
  {
    LOG(ERROR) << "No XML for ObsTime";
    return false;
  }
  return true;
}
//------------------------------------------------------------------
bool SpdbObsMetadata::_infoFromXml(const std::string &xml)
{
  vector<string> vstring;
  if (TaXml::readStringArray(xml, ThreshObarInfo::_tag, vstring))
  {
    LOG(ERROR) << "String array tag missing, " << ThreshObarInfo::_tag;
    return false;
  }
  
  _info.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    ThreshObarInfo m(vstring[i]);
    if (m.ok())
    {
      _info.push_back(m);
    }
    else
    {
      return false;
    }
  }
  return true;
}
