/**
 * @file SpdbGenBasedMetadata.cc
 */

//------------------------------------------------------------------
#include <Epoch/SpdbGenBasedMetadata.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//------------------------------------------------------------------
SpdbGenBasedMetadata::SpdbGenBasedMetadata(void): _fixedValuesSet(false),
						  _genTime(0),
						  _hasFixedField(false),
						  _thresholdsSet(false)
{
}

//------------------------------------------------------------------
SpdbGenBasedMetadata::SpdbGenBasedMetadata(const time_t &genTime,
					   const std::vector<double> &ltHours,
					   const std::string &threshField,
					   double threshColdstartThresh,
					   const std::string &fixedField,
					   double fixedThresh,
					   const TileInfo &tiling,
					   const std::vector<double> &obarThresh) :
  _fixedValuesSet(true),
  _genTime(genTime),
  _threshField(threshField),
  _threshColdstartThresh(threshColdstartThresh),
  _hasFixedField(true),
  _fixedField(fixedField),
  _fixedThresh(fixedThresh),
  _tiling(tiling),
  _thresholdsSet(true)
{
  for (size_t i=0; i<ltHours.size(); ++i)
  {
    int lts = static_cast<int>(ltHours[i]*3600.0);
    _leadSeconds.push_back(lts);
    MultiObarThreshTileThresholds m(_threshColdstartThresh, _tiling,
				    obarThresh);
    _thresholdsAtLead.push_back(m);
  }
}

//------------------------------------------------------------------
SpdbGenBasedMetadata::SpdbGenBasedMetadata(const time_t &genTime,
					   const std::vector<double> &ltHours,
					   const std::string &threshField,
					   double threshColdstartThresh,
					   const TileInfo &tiling,
					   const std::vector<double> &obarThresh) :
  _fixedValuesSet(true),
  _genTime(genTime),
  _threshField(threshField),
  _threshColdstartThresh(threshColdstartThresh),
  _hasFixedField(false),
  _fixedField("None"),
  _fixedThresh(0.0),
  _tiling(tiling),
  _thresholdsSet(true)
{
  for (size_t i=0; i<ltHours.size(); ++i)
  {
    int lts = static_cast<int>(ltHours[i]*3600.0);
    _leadSeconds.push_back(lts);
    MultiObarThreshTileThresholds m(_threshColdstartThresh, _tiling,
				    obarThresh);
    _thresholdsAtLead.push_back(m);
  }
}

//------------------------------------------------------------------
SpdbGenBasedMetadata::~SpdbGenBasedMetadata()
{
}

//------------------------------------------------------------------
int SpdbGenBasedMetadata::getNumObarThresh(void) const
{
  if (_thresholdsAtLead.empty())
  {
    return 0;
  }
  else
  {
    return _thresholdsAtLead[0].getNumObarThresh();
  }
}

//------------------------------------------------------------------
std::vector<double> SpdbGenBasedMetadata::getObarThresh(void) const
{
  if (_thresholdsAtLead.empty())
  {
    std::vector<double> ret;
    return ret;
  }
  else
  {
    return _thresholdsAtLead[0].getObarThresh();
  }
}

//------------------------------------------------------------------
std::vector<double>
SpdbGenBasedMetadata::getTileThresholds(int leadSeconds,
					int obarThreshIndex) const
{
  vector<double> ret;
  if (!_thresholdsSet)
  {
    return ret;
  }
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    return ret;
  }
  return _thresholdsAtLead[ltIndex].getTileThresholds(obarThreshIndex);
}

//------------------------------------------------------------------
std::vector<TileThreshInfoGenBased>
SpdbGenBasedMetadata::getTileThreshInfo(int leadSeconds,
					int obarThreshIndex) const
{
  vector<TileThreshInfoGenBased> ret;
  if (!_thresholdsSet)
  {
    return ret;
  }
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    return ret;
  }
  return _thresholdsAtLead[ltIndex].getTileThreshInfo(_genTime,
						      leadSeconds,
						      obarThreshIndex);
}

//------------------------------------------------------------------
void SpdbGenBasedMetadata::update(const TileThreshInfoGenBased &item,
				  int obarThreshIndex)
{
  int lt = item.getLeadTime();
  int i = _leadIndex(lt);
  if (i < 0)
  {
    LOG(ERROR) << "Bad lead time " << lt;
    return;
  }
  if (_thresholdsAtLead[i].update(item, obarThreshIndex))
  {
    if (!_thresholdsSet)
    {
      LOG(DEBUG) << "Setting first lead time values for gen time " 
		 << DateTime::strn(_genTime);
      _thresholdsSet = true;
    }
  }
  else
  {
    LOG(ERROR) << "Could not update for obsIndex " << obarThreshIndex;
  }
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::fromXml(const std::string &xml)
{
  if (!_fixedValuesSetFromXml(xml))
  {
    return false;
  }
  if (!_genFromXml(xml))
  {
    return false;
  }
  if (!_leadsFromXml(xml))
  {
    return false;
  }
  if (!_threshFieldFromXml(xml))
  {
    return false;
  }
  if (!_fixedFieldFromXml(xml))
  {
    return false;
  }
  if (!_tilingFromXml(xml))
  {
    return false;
  }
  if (!_thresholdsFromXml(xml))
  {
    return false;
  }
  if (!_thresholdsSetFromXml(xml))
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::toXml(void) const
{
  string ret = _fixedValuesSetToXml();
  ret += _genToXml();
  ret += _leadsToXml();
  ret += _threshFieldToXml();
  ret += _fixedFieldToXml();
  ret += _tiling.toXml();
  ret += _thresholdsToXml();
  ret += _thresholdsSetToXml();
  return ret;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::getTiledGrid(int leadTime, 
					double obarThresh, double centerWeight,
					double edgeWeight, int nptSmooth,
					Grid2d &item, bool motherOnly) const
{
  int ltIndex = _leadIndex(leadTime);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }

  if (_thresholdsAtLead[ltIndex].getTiledGrid(_threshField, obarThresh, _tiling,
					      centerWeight, edgeWeight, nptSmooth,
					      item, motherOnly))
  {
    return true;
  }
  else
  {
    LOG(ERROR) << "Setting tiled grids for lead time " << ltIndex;
    return false;
  }
}

//------------------------------------------------------------------
void SpdbGenBasedMetadata::setThresh(int ltSec, int obarThreshIndex,
				     int tileIndex, double value)
{
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Setting tiled grids for lead time " << ltSec;
    return;
  }
  _thresholdsAtLead[i].setThresh(obarThreshIndex, tileIndex, value, false);
}
  
//------------------------------------------------------------------
void SpdbGenBasedMetadata::setThreshColdstart(int ltSec, int obarThreshIndex,
					      int tileIndex)
{
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Setting tiled grids for lead time " << ltSec;
    return;
  }
  _thresholdsAtLead[i].setThresh(obarThreshIndex, tileIndex,
				 _threshColdstartThresh, true);
}

//------------------------------------------------------------------
void
SpdbGenBasedMetadata::printState(const time_t &t, const time_t &twritten,
				 const std::vector<int> &leadSec,
				 const std::vector<int> &tiles,
				 bool verbose) const
{
  if (leadSec.empty() && tiles.empty())
  {
    printState(t, twritten, verbose);
    return;
  }

  printf("---------Threshold/bias information %s ---written %s, for %s----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(twritten).c_str(), 
	 _threshField.c_str());
  printf("\nLeadtimes:");
  vector<int> wantedLt;
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    bool good = false;
    if (leadSec.empty())
    {
      good = true;
    }
    else
    {
      if (find(leadSec.begin(), leadSec.end(), _leadSeconds[i]) != leadSec.end())
      {
	good = true;
      }
    }
    if (good)
    {
      printf(" %d", _leadSeconds[i]);
      wantedLt.push_back(i);
    }
  }
  printf("\n");
  _tiling.print(verbose);

  for (size_t i=0; i<wantedLt.size(); ++i)
  {
    _thresholdsAtLead[wantedLt[i]].print(_leadSeconds[wantedLt[i]],
					 tiles, _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void SpdbGenBasedMetadata::printState(const time_t &t, bool verbose) const
{
  printf("---------Threshold/bias information %s for %s----------\n",
	 DateTime::strn(_genTime).c_str(), _threshField.c_str());
  printf("\nLeadtimes:");
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    printf("%d,", _leadSeconds[i]);
  }
  printf("\n");

  _tiling.print(verbose);

  for (size_t i=0; i<_thresholdsAtLead.size(); ++i)
  {
    _thresholdsAtLead[i].print(_leadSeconds[i], _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void SpdbGenBasedMetadata::showTilingInfo(const time_t &t) const
{
  printf("---------TilingInformation %s ----------\n",
	 DateTime::strn(_genTime).c_str());
  _tiling.print(false);
}

//------------------------------------------------------------------
void SpdbGenBasedMetadata::printState(const time_t &t, const time_t &twritten,
				      bool verbose) const
{
  printf("---------Threshold/bias information %s --- written %s for %s----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(twritten).c_str(),
	 _threshField.c_str());
  printf("\nLeadtimes:");
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    printf("%d,", _leadSeconds[i]);
  }
  printf("\n");

  _tiling.print(verbose);

  for (size_t i=0; i<_thresholdsAtLead.size(); ++i)
  {
    _thresholdsAtLead[i].print(_leadSeconds[i], _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}


//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_fixedValuesSetToXml(void) const
{
  string s = TaXml::writeBoolean("FixedValuesSet", 0, _fixedValuesSet);
  return s;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_thresholdsSetToXml(void) const
{
  string s = TaXml::writeBoolean("ThresholdsSet", 0, _thresholdsSet);
  return s;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_genToXml(void) const
{
  string s = TaXml::writeTime("GenTime", 0, _genTime);
  return s;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_fixedFieldToXml(void) const
{
  string s = TaXml::writeBoolean("HasFixedField", 0, _hasFixedField);
  s += TaXml::writeString("FixedField", 0, _fixedField);
  s += TaXml::writeDouble("FixedThresh", 0, _fixedThresh);
  return s;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_threshFieldToXml(void) const
{
  string s2 = TaXml::writeString("ThreshField", 0, _threshField);
  s2 += TaXml::writeDouble("ThreshColdstartThresh", 0, _threshColdstartThresh);
  return s2;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_leadsToXml(void) const
{
  string s = TaXml::writeStartTag("Lead", 0);
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    s += TaXml::writeInt("lt", 1, _leadSeconds[i], "%08d");
  }
  s += TaXml::writeEndTag("Lead", 0);
  return s;
}

//------------------------------------------------------------------
std::string SpdbGenBasedMetadata::_thresholdsToXml(void) const
{
  string s = "";
  for (size_t i=0; i<_thresholdsAtLead.size(); ++i)
  {
    s += _thresholdsAtLead[i].toXml();
  }
  return s;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_threshFieldFromXml(const std::string &xml)
{
  if (TaXml::readString(xml, "ThreshField", _threshField))
  {
    LOG(ERROR) << "No XML with key ThreshField";
    return false;
  }
  if (TaXml::readDouble(xml, "ThreshColdstartThresh", _threshColdstartThresh))
  {
    LOG(ERROR) << "No XML with key ThreshColdstartThresh";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_fixedFieldFromXml(const std::string &xml)
{
  if (TaXml::readBoolean(xml, "HasFixedField", _hasFixedField))
  {
    LOG(ERROR) << "No XML with key HasFixedField";
    return false;
  }
  if (_hasFixedField)
  {
    if (TaXml::readString(xml, "FixedField", _fixedField))
    {
      LOG(ERROR) << "No XML with key FixedField";
      return false;
    }
    if (TaXml::readDouble(xml, "FixedThresh", _fixedThresh))
    {
      LOG(ERROR) << "No XML with key FixedThresh";
      return false;
    }
  }
  else
  {
    _fixedField = "None";
    _fixedThresh = 0.0;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_leadsFromXml(const std::string &xml)
{
  string str;
  if (TaXml::readString(xml, "Lead", str))
  {
    LOG(ERROR) << "No XML with key Lead";
    return false;
  }

  std::vector<std::string> vstring;
  if (TaXml::readStringArray(str, "lt", vstring))
  {
    LOG(ERROR) << "No XML string array with key lt";
    return false;
  }

  _leadSeconds.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    int lt;
    if (sscanf(vstring[i].c_str(), "%d", &lt) != 1)
    {
      LOG(ERROR) << "Scanning " << vstring[i] << " As an int";
      return false;
    }
    _leadSeconds.push_back(lt);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_tilingFromXml(const std::string &xml)
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
bool SpdbGenBasedMetadata::_thresholdsFromXml(const std::string &xml)
{
  _thresholdsAtLead.clear();
  vector<string> vstring;
  if (TaXml::readStringArray(xml, MultiObarThreshTileThresholds::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag as array " << MultiObarThreshTileThresholds::_tag;
    return false;
  }

  if (vstring.size() != _leadSeconds.size())
  {
    LOG(ERROR) << "Inconsistent tiling "
	       << "input:" << _leadSeconds.size()
	       << " xml:" << vstring.size();
    return false;
  }

  // for every element,
  // parse it
  for (size_t i=0; i<vstring.size(); ++i)
  {
    MultiObarThreshTileThresholds m(vstring[i], _tiling);
    if (!m.isOk())
    {
      return false;
    }
    _thresholdsAtLead.push_back(m);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_fixedValuesSetFromXml(const std::string &xml)
{
  if (TaXml::readBoolean(xml, "FixedValuesSet", _fixedValuesSet))
  {
    LOG(ERROR) << "No XML for FixedValuesSet";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_thresholdsSetFromXml(const std::string &xml)
{
  if (TaXml::readBoolean(xml, "ThresholdsSet", _thresholdsSet))
  {
    LOG(ERROR) << "No XML for ThresholdsSet";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbGenBasedMetadata::_genFromXml(const std::string &xml)
{
  if (TaXml::readTime(xml, "GenTime", _genTime))
  {
    LOG(ERROR) << "No XML for GenTime";
    return false;
  }
  return true;
}

//-------------------------------------------------------------  
int SpdbGenBasedMetadata::_leadIndex(int leadTime) const
{
  vector<int>::const_iterator i;
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
