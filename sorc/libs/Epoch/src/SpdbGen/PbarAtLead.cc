/**
 * @file PbarAtLead.cc
 */

//------------------------------------------------------------------
#include <Epoch/PbarAtLead.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

const std::string PbarAtLead::_tag = "PbarAtLead";

//------------------------------------------------------------------
PbarAtLead::PbarAtLead(void):
  _ok(false),
  _fixedValuesSet(false),
  _thresholdsSet(false)
{
}

//------------------------------------------------------------------
PbarAtLead::
PbarAtLead(const TileInfo &tiling,
	   const std::vector<double> &thresh) :
  _ok(true),
  _fixedValuesSet(true),
  _thresholdsSet(true),
  _thresh(thresh)
{
  for (size_t i=0; i<thresh.size(); ++i)
  {
    PbarAtLeadThresh  m(tiling);
    _pbarAtThresh.push_back(m);
  }
}

//------------------------------------------------------------------
PbarAtLead::
PbarAtLead(const std::string &xml, const TileInfo &tiling) :
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
  if (TaXml::readStringArray(xml, "PbarThresh", vstring))
  {
    LOG(ERROR) << "Reading tag PbarThresh";
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
    _thresh.push_back(v);
  }
  if (TaXml::readStringArray(xml, PbarAtLeadThresh::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag " << PbarAtLeadThresh::_tag;
    _ok = false;
    return;
  }
  for (size_t i=0; i<vstring.size(); ++i)
  {
    PbarAtLeadThresh m(vstring[i], tiling);
    if (!m.ok())
    {
      LOG(ERROR) << "Bad constructor";
      _ok = false;
      return;
    }
    _pbarAtThresh.push_back(m);
  }
}

//------------------------------------------------------------------
PbarAtLead::~PbarAtLead()
{
}

//------------------------------------------------------------------
std::vector<double>
PbarAtLead::getTilePbar(int threshIndex) const
{
  vector<double> ret;
  if (!_thresholdsSet)
  {
    return ret;
  }
  return _pbarAtThresh[threshIndex].pbar();
}

//------------------------------------------------------------------
std::vector<double>
PbarAtLead::getPbarsForOneTile(int tileIndex) const
{
  vector<double> ret;
  if (!_thresholdsSet)
  {
    return ret;
  }

  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    ret.push_back(_pbarAtThresh[i].pbarForTile(tileIndex));
  }
  return ret;
}

//------------------------------------------------------------------
void PbarAtLead::copyPbarForAllThresh(int fromTile, int toTile)
  
{
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    _pbarAtThresh[i].setPbar(toTile, _pbarAtThresh[i].pbarForTile(fromTile));
  }
}

//------------------------------------------------------------------
void PbarAtLead::setPbarForAllThresh(int tileIndex,
				     const std::vector<double> &pbar)
{
  if (pbar.size() != _pbarAtThresh.size())
  {
    LOG(ERROR) << "Mismatch in number of thresholds "
	       << pbar.size() << " " << _pbarAtThresh.size();
    return;
  }
  for (size_t i=0;i<pbar.size(); ++i)
  {
    _pbarAtThresh[i].setPbar(tileIndex, pbar[i]);
  }
}

//------------------------------------------------------------------
std::string PbarAtLead::toXml(void) const
{
  string ret = TaXml::writeStartTag(_tag, 0);
  ret += TaXml::writeBoolean("ValuesSetLead", 0, _fixedValuesSet);
  ret += TaXml::writeBoolean("ThreshSetLead", 0, _thresholdsSet);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    ret += TaXml::writeDouble("PbarThresh", 0, _thresh[i]);
  }
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    ret += _pbarAtThresh[i].toXml();
  }
  ret += TaXml::writeEndTag(_tag, 0);
  return ret;
}

//------------------------------------------------------------------
void PbarAtLead::setPbar(int threshIndex,
			 int tileIndex,
			 double value)
{
  if (threshIndex < 0 || threshIndex >= (int)_thresh.size())
  {
    LOG(ERROR) << "Setting tiled grids for bar obar index " << threshIndex;
    return;
  }
  _pbarAtThresh[threshIndex].setPbar(tileIndex, value);
}
  
//------------------------------------------------------------------
bool PbarAtLead::getTiledGrid(const std::string &inputName,
			      double thresh,
			      const TileInfo &tiling,
			      double centerWeight, double edgeWeight,
			      int nptSmooth,
			      Grid2d &item) const
{
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    if (_thresh[i] == thresh)
    {
      char buf[1000];
      sprintf(buf, "_%05.2lf_pbar", thresh);
      std::string name = inputName + buf;
      return _pbarAtThresh[i].constructTiledGrid(name, tiling,
						 centerWeight, edgeWeight,
						 nptSmooth, item);
    }
  }
  LOG(ERROR) << "No thresh in db " << thresh;
  return false;
}


//------------------------------------------------------------------
void PbarAtLead::print(int lt, const TileInfo &info,
		       bool verbose) const
{
  printf("Pbar thresh:");
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    printf(" %5.2lf", _thresh[i]);
  }
  printf("\n");
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    _pbarAtThresh[i].print(lt, _thresh[i], info, verbose);
  }
}

//------------------------------------------------------------------
void
PbarAtLead::print(int lt, const std::vector<int> &tiles,
		  const TileInfo &info,
		  bool verbose) const
{
  printf("Pbar thresholds:");
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    printf(" %5.2lf", _thresh[i]);
  }
  printf("\n");
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    _pbarAtThresh[i].print(lt, _thresh[i], tiles, info, verbose);
  }
}
