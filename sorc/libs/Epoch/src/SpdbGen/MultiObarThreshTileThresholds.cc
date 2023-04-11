/**
 * @file MultiObarThreshTileThresholds.cc
 */

//------------------------------------------------------------------
#include <Epoch/MultiObarThreshTileThresholds.hh>
#include <Epoch/TileThreshInfoGenBased.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

const std::string MultiObarThreshTileThresholds::_tag = "ObarAtThresh";

//------------------------------------------------------------------
MultiObarThreshTileThresholds::MultiObarThreshTileThresholds(void):
  _ok(false),
  _fixedValuesSet(false),
  _thresholdsSet(false)
{
}

//------------------------------------------------------------------
MultiObarThreshTileThresholds::
MultiObarThreshTileThresholds(double threshColdstartThresh,
			      const TileInfo &tiling,
			      const std::vector<double> &obarThresh) :
  _ok(true),
  _fixedValuesSet(true),
  _thresholdsSet(true),
  _obarThresh(obarThresh)
{
  for (size_t i=0; i<obarThresh.size(); ++i)
  {
    MultiTileThresholdsGenBased m(threshColdstartThresh, tiling);
    _thresholdsForObar.push_back(m);
  }
}

//------------------------------------------------------------------
MultiObarThreshTileThresholds::
MultiObarThreshTileThresholds(const std::string &xml, const TileInfo &tiling) :
  _ok(true)
{
  if (TaXml::readBoolean(xml, "ValuesSetLead", _fixedValuesSet))
  {
    LOG(ERROR) << "Reading tag for ValuesSetLead";
    _ok = false;
    return;
  }
  if (TaXml::readBoolean(xml, "ThreshSetLead", _thresholdsSet))
  {
    LOG(ERROR) << "Reading tag for ThreshSetLead";
    _ok = false;
    return;
  }
  vector<string> vstring;
  if (TaXml::readStringArray(xml, "ObarThresh", vstring))
  {
    LOG(ERROR) << "Reading tag ObarThresh";
    _ok = false;
    return;
  }
  for (size_t i=0; i<vstring.size(); ++i)
  {
    double v;
    if (sscanf(vstring[i].c_str(), "%lf", &v) != 1)
    {
      LOG(ERROR) << "Scanning as a double " << vstring[i];
      _ok = false;
      return;
    }
    _obarThresh.push_back(v);
  }
  if (TaXml::readStringArray(xml, MultiTileThresholdsGenBased::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag " << MultiTileThresholdsGenBased::_tag;
    _ok = false;
    return;
  }
  for (size_t i=0; i<vstring.size(); ++i)
  {
    MultiTileThresholdsGenBased m(vstring[i], tiling);
    if (!m.ok())
    {
      LOG(ERROR) << "Bad constructor for multitiles";
      _ok = false;
      return;
    }
    _thresholdsForObar.push_back(m);
  }
}

//------------------------------------------------------------------
MultiObarThreshTileThresholds::~MultiObarThreshTileThresholds()
{
}

//------------------------------------------------------------------
std::vector<double>
MultiObarThreshTileThresholds::getTileThresholds(int obarThreshIndex) const
{
  vector<double> ret;
  if (!_thresholdsSet)
  {
    return ret;
  }
  return _thresholdsForObar[obarThreshIndex].thresholds();
}

//------------------------------------------------------------------
std::vector<TileThreshInfoGenBased>
MultiObarThreshTileThresholds::getTileThreshInfo(const time_t &gen,
						 int leadSeconds,
						 int obarThreshIndex) const
{
  vector<TileThreshInfoGenBased> ret;
  if (!_thresholdsSet)
  {
    return ret;
  }
  return _thresholdsForObar[obarThreshIndex].getTileThreshInfo(gen,
							       leadSeconds);
}


//-----------------------------------------------------------------------
bool MultiObarThreshTileThresholds::update(const TileThreshInfoGenBased &item,
					   int obarThreshIndex)
{
  if (_thresholdsForObar[obarThreshIndex].update(item))
  {
    if (!_thresholdsSet)
    {
      _thresholdsSet = true;
    }
    return true;
  }
  else
  {
    LOG(ERROR) << "Setting thresholds for obar thresh index " << obarThreshIndex;
    return false;
  }
}

//------------------------------------------------------------------
std::string MultiObarThreshTileThresholds::toXml(void) const
{
  string ret = TaXml::writeStartTag(_tag, 0);
  ret += TaXml::writeBoolean("ValuesSetLead", 0, _fixedValuesSet);
  ret += TaXml::writeBoolean("ThreshSetLead", 0, _thresholdsSet);
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    ret += TaXml::writeDouble("ObarThresh", 0, _obarThresh[i]);
  }
  for (size_t i=0; i<_thresholdsForObar.size(); ++i)
  {
    ret += _thresholdsForObar[i].toXml();
  }
  ret += TaXml::writeEndTag(_tag, 0);
  return ret;
}

//------------------------------------------------------------------
void MultiObarThreshTileThresholds::setThresh(int obarThreshIndex,
					      int tileIndex,
					      double value,
					      bool isColdstart)
{
  if (obarThreshIndex < 0 || obarThreshIndex >= (int)_obarThresh.size())
  {
    LOG(ERROR) << "Setting tiled grids for bar obar index " << obarThreshIndex;
    return;
  }
  _thresholdsForObar[obarThreshIndex].setThresh(tileIndex, value, isColdstart);
}
  
//------------------------------------------------------------------
bool MultiObarThreshTileThresholds::getTiledGrid(const std::string &inputName,
						 double obarThresh,
						  const TileInfo &tiling,
						  double centerWeight, double edgeWeight,
						  int nptSmooth,
						 Grid2d &item, bool motherOnly) const
{
  for (size_t i=0; i<_thresholdsForObar.size(); ++i)
  {
    if (_obarThresh[i] == obarThresh)
    {
      char buf[1000];
      sprintf(buf, "_%05.2lf_thresh", obarThresh);
      std::string name = inputName + buf;
      return _thresholdsForObar[i].constructTiledGrid(name, tiling,
       						      centerWeight, edgeWeight,
       						      nptSmooth, item, motherOnly);
    }
  }
  LOG(ERROR) << "No obar thresh in db " << obarThresh;
  return false;
}


//------------------------------------------------------------------
void MultiObarThreshTileThresholds::print(int lt, const TileInfo &info,
					  bool verbose) const
{
  printf("Obar thresholds:");
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    printf(" %5.2lf", _obarThresh[i]);
  }
  printf("\n");
  for (size_t i=0; i<_thresholdsForObar.size(); ++i)
  {
    _thresholdsForObar[i].print(lt, _obarThresh[i], info, verbose);
  }
}

//------------------------------------------------------------------
void
MultiObarThreshTileThresholds::print(int lt, const std::vector<int> &tiles,
				     const TileInfo &info,
				     bool verbose) const
{
  printf("Obar thresholds:");
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    printf(" %5.2lf", _obarThresh[i]);
  }
  printf("\n");
  for (size_t i=0; i<_thresholdsForObar.size(); ++i)
  {
    _thresholdsForObar[i].print(lt, _obarThresh[i], tiles, info, verbose);
  }
}
