/**
 * @file SpdbPbarMetadata2.cc
 */

//------------------------------------------------------------------
#include <Epoch/SpdbPbarMetadata2.hh>
#include <Epoch/PbarAtLead.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//------------------------------------------------------------------
SpdbPbarMetadata2::SpdbPbarMetadata2(void):  _genTime(0)
					     //					   _pbarSet(false)
{
}

//------------------------------------------------------------------
SpdbPbarMetadata2::SpdbPbarMetadata2(const time_t &genTime,
				     const std::vector<double> &ltHours,
				     const TileInfo &tiling,
				     const std::string &threshField1,
				     const std::string &threshField2,
				     const std::vector<double> &thresholds1,
				     const std::vector<double> &thresholds2,
				     bool hasFixedField1,
				     const std::string &fixedField1,
				     double fixedThresh1,
				     bool hasFixedField2,
				     const std::string &fixedField2,
				     double fixedThresh2) :
  _genTime(genTime),
  _tiling(tiling),
  _thresh1(thresholds1),
  _threshField1(threshField1),
  _thresh2(thresholds2),
  _threshField2(threshField2),
  _hasFixedField1(hasFixedField1),
  _fixedField1(fixedField1),
  _fixedThresh1(fixedThresh1),
  _hasFixedField2(hasFixedField2),
  _fixedField2(fixedField2),
  _fixedThresh2(fixedThresh2)
{
  for (size_t i=0; i<ltHours.size(); ++i)
  {
    int lts = static_cast<int>(ltHours[i]*3600.0);
    _leadSeconds.push_back(lts);
    PbarAtLead m(_tiling, _thresh1);
    _pbarAtLead.push_back(m);
    PbarAtLead2 m2(_tiling, _thresh1, _thresh2);
    _pbarAtLead2.push_back(m2);
  }
}

//------------------------------------------------------------------
SpdbPbarMetadata2::~SpdbPbarMetadata2()
{
}

//------------------------------------------------------------------
int SpdbPbarMetadata2::getNumThresh(int which) const
{
  if (which == 1)
  {
    return (int)_thresh1.size();
  }
  else if (which == 2)
  {
    return (int)_thresh2.size();
  }
  else
  {
    return 0;
  }
}

//------------------------------------------------------------------
std::vector<double> SpdbPbarMetadata2::getThresh(int which) const
{
  if (which == 1)
  {
    return _thresh1;
  }
  else if (which == 2)
  {
    return _thresh2;
  }
  else
  {
    vector<double> x;
    return x;
  }
}

//------------------------------------------------------------------
 void SpdbPbarMetadata2::getFixed1(bool &isFixed1, std::string &fixedField1,
				   double &fixedThresh1) const
 {
   isFixed1 = _hasFixedField1;
   fixedField1 = _fixedField1;
   fixedThresh1 = _fixedThresh1;
 }

//------------------------------------------------------------------
void SpdbPbarMetadata2::getFixed2(bool &isFixed2, std::string &fixedField2,
				  double &fixedThresh2) const
{
   isFixed2 = _hasFixedField2;
   fixedField2 = _fixedField2;
   fixedThresh2 = _fixedThresh2;
}

//------------------------------------------------------------------
std::vector<double>
SpdbPbarMetadata2::getTilePbar1(int leadSeconds,
				int threshIndex) const
{
  vector<double> ret;
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    return ret;
  }
  return _pbarAtLead[ltIndex].getTilePbar(threshIndex);
}

//------------------------------------------------------------------
std::vector<double> SpdbPbarMetadata2::getPbar1(int leadSeconds,
						int tileIndex) const
{
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    vector<double> ret;
    return ret;
  }
  return _pbarAtLead[ltIndex].getPbarsForOneTile(tileIndex);  
}

//------------------------------------------------------------------
std::vector<double> SpdbPbarMetadata2::getPbar2(int leadSeconds,
						int tileIndex,
						int index1) const
{
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    vector<double> ret;
    return ret;
  }
  return _pbarAtLead2[ltIndex].getPbars2ForOneTile(tileIndex,
						   index1);  
}


//------------------------------------------------------------------
std::vector<double>
SpdbPbarMetadata2::getTilePbar2(int leadSeconds,
				int threshIndex1,
				int threshIndex2) const
{
  vector<double> ret;
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    return ret;
  }
  return _pbarAtLead2[ltIndex].getTilePbar(threshIndex1, threshIndex2);
}

//------------------------------------------------------------------
void SpdbPbarMetadata2::copyPbarForAllThresh(int leadSeconds, int fromTile, int toTile)
{
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "lead seconds bad no action";
    return;
  }
  _pbarAtLead[ltIndex].copyPbarForAllThresh(fromTile, toTile);
  _pbarAtLead2[ltIndex].copyPbarForAllThresh(fromTile, toTile);
}

//------------------------------------------------------------------
void SpdbPbarMetadata2::setPbarForAllThresh(int leadSeconds, int tileIndex,
					    const std::vector<double> &pbar,
					    int which)
{
  int ltIndex = _leadIndex(leadSeconds);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "lead seconds bad no action";
    return;
  }
  if (which == 1)
  {
    _pbarAtLead[ltIndex].setPbarForAllThresh(tileIndex, pbar);
  }
  else
  {
    _pbarAtLead2[ltIndex].setPbarForAllThresh(tileIndex, pbar);
  }
}

//------------------------------------------------------------------
bool SpdbPbarMetadata2::fromXml(const std::string &xml)
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
  if (!_fixedFromXml(xml))
  {
    return false;
  }

  // if (!_pbarSetFromXml(xml))
  // {
  //   return false;
  // }
  if (!_pbarFromXml(xml))
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata2::toXml(void) const
{
  string ret = "";
  ret += _genToXml();
  ret += _threshToXml();
  ret += _leadsToXml();
  ret += _threshFieldToXml();
  ret += _tiling.toXml();
  ret += _fixedToXml();
  //  ret += _pbarSetToXml();
  ret += _pbarToXml();
  return ret;
}


//------------------------------------------------------------------
bool SpdbPbarMetadata2::getTiledGrid1(int leadTime, 
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

  if (_pbarAtLead[ltIndex].getTiledGrid(_threshField1, thresh, _tiling,
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
bool SpdbPbarMetadata2::getTiledGrid2(int leadTime, 
				      double thresh1, double thresh2,
				      double centerWeight,
				      double edgeWeight, int nptSmooth,
				      Grid2d &item) const
{
  int ltIndex = _leadIndex(leadTime);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }

  if (_pbarAtLead2[ltIndex].getTiledGrid(_threshField2, thresh1, thresh2, _tiling,
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
bool SpdbPbarMetadata2::getTiledGrid2_2(int leadTime, 
				      double thresh1, double thresh2,
				      double centerWeight,
				      double edgeWeight, int nptSmooth,
				      Grid2d &item) const
{
  int ltIndex = _leadIndex(leadTime);
  if (ltIndex < 0)
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }

  if (_pbarAtLead2[ltIndex].getTiledGrid2(_threshField1, _threshField2,
					  thresh1, thresh2, _tiling,
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
void SpdbPbarMetadata2::setPbar1(int ltSec, int threshIndex,
				 int tileIndex, double value)
{
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Setting tiled grids for lead time " << ltSec;
    return;
  }
  _pbarAtLead[i].setPbar(threshIndex, tileIndex, value);
}
  
//------------------------------------------------------------------
void SpdbPbarMetadata2::setPbar2(int ltSec, int threshIndex1,
				 int threshIndex2, 
				 int tileIndex, double value)
{
  int i = _leadIndex(ltSec);
  if (i < 0)
  {
    LOG(ERROR) << "Setting tiled grids for lead time " << ltSec;
    return;
  }
  _pbarAtLead2[i].setPbar(threshIndex1, threshIndex2, tileIndex, value);
}
  
//------------------------------------------------------------------
void
SpdbPbarMetadata2::printState(const time_t &t, const time_t &twritten,
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
	 _threshField1.c_str());
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
void SpdbPbarMetadata2::printState(const time_t &t, bool verbose) const
{
  printf("---------Threshold/bias information %s for %s----------\n",
	 DateTime::strn(_genTime).c_str(), _threshField1.c_str());
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
void SpdbPbarMetadata2::printState(const time_t &t, const time_t &twritten,
				      bool verbose) const
{
  printf("---------Threshold/bias information %s --- written %s for %s----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(twritten).c_str(),
	 _threshField1.c_str());
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

// //------------------------------------------------------------------
// std::string SpdbPbarMetadata2::_pbarSetToXml(void) const
// {
//   string s = TaXml::writeBoolean("PbarSet", 0, _pbarSet);
//   return s;
// }

//------------------------------------------------------------------
std::string SpdbPbarMetadata2::_genToXml(void) const
{
  string s = TaXml::writeTime("GenTime", 0, _genTime);
  return s;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata2::_threshFieldToXml(void) const
{
  string s2 = TaXml::writeString("ThreshField1", 0, _threshField1);
  s2 += TaXml::writeString("ThreshField2", 0, _threshField2);
  return s2;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata2::_fixedToXml(void) const
{
  string s = TaXml::writeBoolean("HasFixedField1", 0, _hasFixedField1);
  s += TaXml::writeString("FixedField1", 0, _fixedField1);
  s += TaXml::writeDouble("FixedThresh1", 0, _fixedThresh1);
  s += TaXml::writeBoolean("HasFixedField2", 0, _hasFixedField2);
  s += TaXml::writeString("FixedField2", 0, _fixedField2);
  s += TaXml::writeDouble("FixedThresh2", 0, _fixedThresh2);
  return s;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata2::_leadsToXml(void) const
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
std::string SpdbPbarMetadata2::_threshToXml(void) const
{
  string s = TaXml::writeStartTag("DataThresholds1", 0);
  for (size_t i=0; i<_thresh1.size(); ++i)
  {
    s += TaXml::writeDouble("thr1", 1, _thresh1[i]);
  }
  s += TaXml::writeEndTag("DataThresholds1", 0);

  s += TaXml::writeStartTag("DataThresholds2", 0);
  for (size_t i=0; i<_thresh2.size(); ++i)
  {
    s += TaXml::writeDouble("thr2", 1, _thresh2[i]);
  }
  s += TaXml::writeEndTag("DataThresholds2", 0);
  return s;
}

//------------------------------------------------------------------
std::string SpdbPbarMetadata2::_pbarToXml(void) const
{
  string s = "";
  for (size_t i=0; i<_pbarAtLead.size(); ++i)
  {
    s += _pbarAtLead[i].toXml();
  }
  for (size_t i=0; i<_pbarAtLead2.size(); ++i)
  {
    s += _pbarAtLead2[i].toXml();
  }
  return s;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata2::_fixedFromXml(const std::string &xml)
{
  if (TaXml::readBoolean(xml, "HasFixedField1", _hasFixedField1))
  {
    LOG(ERROR) << "No XML with key HasFixedField1";
    return false;
  }
  if (_hasFixedField1)
  {
    if (TaXml::readString(xml, "FixedField1", _fixedField1))
    {
      LOG(ERROR) << "No XML with key FixedField1";
      return false;
    }
    if (TaXml::readDouble(xml, "FixedThresh1", _fixedThresh1))
    {
      LOG(ERROR) << "No XML with key FixedThresh1";
      return false;
    }
  }
  else
  {
    _fixedField1 = "None";
    _fixedThresh1 = 0.0;
  }

  if (TaXml::readBoolean(xml, "HasFixedField2", _hasFixedField2))
  {
    LOG(ERROR) << "No XML with key HasFixedField2";
    return false;
  }
  if (_hasFixedField2)
  {
    if (TaXml::readString(xml, "FixedField2", _fixedField2))
    {
      LOG(ERROR) << "No XML with key FixedField2";
      return false;
    }
    if (TaXml::readDouble(xml, "FixedThresh2", _fixedThresh2))
    {
      LOG(ERROR) << "No XML with key FixedThresh2";
      return false;
    }
  }
  else
  {
    _fixedField2 = "None";
    _fixedThresh2 = 0.0;
  }

  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata2::_threshFieldFromXml(const std::string &xml)
{
  if (TaXml::readString(xml, "ThreshField1", _threshField1))
  {
    LOG(ERROR) << "No XML with key ThreshField1";
    return false;
  }
  if (TaXml::readString(xml, "ThreshField2", _threshField2))
  {
    LOG(ERROR) << "No XML with key ThreshField2";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata2::_leadsFromXml(const std::string &xml)
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
bool SpdbPbarMetadata2::_threshFromXml(const std::string &xml)
{
  string str;
  if (TaXml::readString(xml, "DataThresholds1", str))
  {
    LOG(ERROR) << "No XML with key DataThresholds1";
    return false;
  }

  std::vector<std::string> vstring;
  if (TaXml::readStringArray(str, "thr1", vstring))
  {
    LOG(ERROR) << "No XML string array with key thr1";
    return false;
  }

  _thresh1.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    double thr;
    if (sscanf(vstring[i].c_str(), "%lf", &thr) != 1)
    {
      LOG(ERROR) << "Scanning " << vstring[i] << " As a double";
      return false;
    }
    _thresh1.push_back(thr);
  }

  if (TaXml::readString(xml, "DataThresholds2", str))
  {
    LOG(ERROR) << "No XML with key DataThresholds2";
    return false;
  }

  vstring.clear();
  if (TaXml::readStringArray(str, "thr2", vstring))
  {
    LOG(ERROR) << "No XML string array with key thr2";
    return false;
  }

  _thresh2.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    double thr;
    if (sscanf(vstring[i].c_str(), "%lf", &thr) != 1)
    {
      LOG(ERROR) << "Scanning " << vstring[i] << " As a double";
      return false;
    }
    _thresh2.push_back(thr);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata2::_tilingFromXml(const std::string &xml)
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
bool SpdbPbarMetadata2::_pbarFromXml(const std::string &xml)
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

  _pbarAtLead2.clear();
  vstring.clear();
  if (TaXml::readStringArray(xml, PbarAtLead2::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag as array " << PbarAtLead2::_tag;
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
    PbarAtLead2 m(vstring[i], _tiling);
    if (!m.isOk())
    {
      return false;
    }
    _pbarAtLead2.push_back(m);
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbPbarMetadata2::_genFromXml(const std::string &xml)
{
  if (TaXml::readTime(xml, "GenTime", _genTime))
  {
    LOG(ERROR) << "No XML for GenTime";
    return false;
  }
  return true;
}

//-------------------------------------------------------------  
int SpdbPbarMetadata2::_leadIndex(int leadTime) const
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
