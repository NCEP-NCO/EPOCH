/**
 * @file SpdbPbarMetadata.cc
 */

//------------------------------------------------------------------
#include <Epoch/SpdbPbarMetadata.hh>
#include <Epoch/PbarAtLead.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//------------------------------------------------------------------
SpdbPbarMetadata::SpdbPbarMetadata(void):  _genTime(0),
					   _pbarSet(false)
{
}

//------------------------------------------------------------------
SpdbPbarMetadata::SpdbPbarMetadata(const time_t &genTime,
					   const std::vector<double> &ltHours,
					   const std::string &threshField,
					   const TileInfo &tiling,
					   const std::vector<double> &thresholds) :
  _genTime(genTime),
  _thresh(thresholds),
  _threshField(threshField),
  _tiling(tiling),
  _pbarSet(false)
{
  for (size_t i=0; i<ltHours.size(); ++i)
  {
    int lts = static_cast<int>(ltHours[i]*3600.0);
    _leadSeconds.push_back(lts);
    PbarAtLead m(_tiling, _thresh);
    _pbarAtLead.push_back(m);
  }
}

//------------------------------------------------------------------
SpdbPbarMetadata::~SpdbPbarMetadata()
{
}

//------------------------------------------------------------------
int SpdbPbarMetadata::getNumThresh(void) const
{
  return (int)_thresh.size();
}

//------------------------------------------------------------------
std::vector<double> SpdbPbarMetadata::getThresh(void) const
{
  return _thresh;
}

//------------------------------------------------------------------
std::vector<double>
SpdbPbarMetadata::getTilePbar(int leadSeconds,
			      int threshIndex) const
{
  vector<double> ret;
  if (!_pbarSet)
  {
    return ret;
  }
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    return ret;
  }
  return _pbarAtLead[ltIndex].getTilePbar(threshIndex);
}

//------------------------------------------------------------------
void SpdbPbarMetadata::copyPbarForAllThresh(int leadSeconds, int fromTile, int toTile)
{
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "lead seconds bad no action";
    return;
  }
  _pbarSet = true;  // hmmmm
  _pbarAtLead[ltIndex].copyPbarForAllThresh(fromTile, toTile);
}

//------------------------------------------------------------------
void SpdbPbarMetadata::setPbarForAllThresh(int leadSeconds, int tileIndex,
					   const std::vector<double> &pbar)
{
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "lead seconds bad no action";
    return;
  }
  _pbarSet = true;  // hmmmm
  _pbarAtLead[ltIndex].setPbarForAllThresh(tileIndex, pbar);
}

//------------------------------------------------------------------
bool SpdbPbarMetadata::fromXml(const std::string &xml)
{
  if (!_genFromXml(xml))
  {
    return false;
  }
  if (!_leadsFromXml(xml))
  {
    return false;
  }
  if (!_threshFromXml(xml))
  {
    return false;
  }
  if (!_threshFieldFromXml(xml))
  {
    return false;
  }
  if (!_tilingFromXml(xml))
  {
    return false;
  }
  if (!_pbarSetFromXml(xml))
  {
    return false;
  }
  if (!_pbarFromXml(xml))
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata::toXml(void) const
{
  string ret = "";
  ret += _genToXml();
  ret += _threshToXml();
  ret += _leadsToXml();
  ret += _threshFieldToXml();
  ret += _tiling.toXml();
  ret += _pbarSetToXml();
  ret += _pbarToXml();
  return ret;
}


//------------------------------------------------------------------
bool SpdbPbarMetadata::getTiledGrid(int leadTime, 
				    double thresh, double centerWeight,
				    double edgeWeight, int nptSmooth,
				    Grid2d &item) const
{
  int ltIndex = _leadIndex(leadTime);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }

  if (_pbarAtLead[ltIndex].getTiledGrid(_threshField, thresh, _tiling,
					centerWeight, edgeWeight, nptSmooth, item))
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
void SpdbPbarMetadata::setPbar(int ltSec, int threshIndex,
			       int tileIndex, double value)
{
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Setting tiled grids for lead time " << ltSec;
    return;
  }
  _pbarSet = true;  // hmmmm
  _pbarAtLead[i].setPbar(threshIndex, tileIndex, value);
}
  
//------------------------------------------------------------------
void
SpdbPbarMetadata::printState(const time_t &t, const time_t &twritten,
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
    _pbarAtLead[wantedLt[i]].print(_leadSeconds[wantedLt[i]],
				   tiles, _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void SpdbPbarMetadata::printState(const time_t &t, bool verbose) const
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

  for (size_t i=0; i<_pbarAtLead.size(); ++i)
  {
    _pbarAtLead[i].print(_leadSeconds[i], _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void SpdbPbarMetadata::printState(const time_t &t, const time_t &twritten,
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

  for (size_t i=0; i<_pbarAtLead.size(); ++i)
  {
    _pbarAtLead[i].print(_leadSeconds[i], _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata::_pbarSetToXml(void) const
{
  string s = TaXml::writeBoolean("PbarSet", 0, _pbarSet);
  return s;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata::_genToXml(void) const
{
  string s = TaXml::writeTime("GenTime", 0, _genTime);
  return s;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata::_threshFieldToXml(void) const
{
  string s2 = TaXml::writeString("ThreshField", 0, _threshField);
  return s2;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata::_leadsToXml(void) const
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
std::string SpdbPbarMetadata::_threshToXml(void) const
{
  string s = TaXml::writeStartTag("DataThresholds", 0);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    s += TaXml::writeDouble("thr", 1, _thresh[i]);
  }
  s += TaXml::writeEndTag("DataThresholds", 0);
  return s;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata::_pbarToXml(void) const
{
  string s = "";
  for (size_t i=0; i<_pbarAtLead.size(); ++i)
  {
    s += _pbarAtLead[i].toXml();
  }
  return s;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata::_threshFieldFromXml(const std::string &xml)
{
  if (TaXml::readString(xml, "ThreshField", _threshField))
  {
    LOG(ERROR) << "No XML with key ThreshField";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata::_leadsFromXml(const std::string &xml)
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
bool SpdbPbarMetadata::_threshFromXml(const std::string &xml)
{
  string str;
  if (TaXml::readString(xml, "DataThresholds", str))
  {
    LOG(ERROR) << "No XML with key DataThresholds";
    return false;
  }

  std::vector<std::string> vstring;
  if (TaXml::readStringArray(str, "thr", vstring))
  {
    LOG(ERROR) << "No XML string array with key thr";
    return false;
  }

  _thresh.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    double thr;
    if (sscanf(vstring[i].c_str(), "%lf", &thr) != 1)
    {
      LOG(ERROR) << "Scanning " << vstring[i] << " As a double";
      return false;
    }
    _thresh.push_back(thr);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata::_tilingFromXml(const std::string &xml)
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
bool SpdbPbarMetadata::_pbarFromXml(const std::string &xml)
{
  _pbarAtLead.clear();
  vector<string> vstring;
  if (TaXml::readStringArray(xml, PbarAtLead::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag as array " << PbarAtLead::_tag;
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
    PbarAtLead m(vstring[i], _tiling);
    if (!m.isOk())
    {
      return false;
    }
    _pbarAtLead.push_back(m);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata::_pbarSetFromXml(const std::string &xml)
{
  if (TaXml::readBoolean(xml, "PbarSet", _pbarSet))
  {
    LOG(ERROR) << "No XML for PbarSet";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata::_genFromXml(const std::string &xml)
{
  if (TaXml::readTime(xml, "GenTime", _genTime))
  {
    LOG(ERROR) << "No XML for GenTime";
    return false;
  }
  return true;
}

//-------------------------------------------------------------  
int SpdbPbarMetadata::_leadIndex(int leadTime) const
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
