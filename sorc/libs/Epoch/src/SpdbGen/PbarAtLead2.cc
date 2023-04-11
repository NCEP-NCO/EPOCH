/**
 * @file PbarAtLead2.cc
 */

//------------------------------------------------------------------
#include <Epoch/PbarAtLead2.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

const std::string PbarAtLead2::_tag = "PbarAtLead2";

//------------------------------------------------------------------
PbarAtLead2::PbarAtLead2(void):
  _ok(false)//,
  // _fixedValuesSet(false),
  // _thresholdsSet(false)
{
}

//------------------------------------------------------------------
PbarAtLead2::
PbarAtLead2(const TileInfo &tiling,
	    const std::vector<double> &thresh1,
	    const std::vector<double> &thresh2) :
  _ok(true),
  // _fixedValuesSet(true),
  // _thresholdsSet(true),
  _thresh1(thresh1),
  _thresh2(thresh2)
{
  for (size_t i1=0; i1<thresh1.size(); ++i1)
  {
    for (size_t i2=0; i2<thresh2.size(); ++i2)
    {
      PbarAtLeadThresh2  m(tiling);
      _pbarAtThresh.push_back(m);
    }
  }
}

//------------------------------------------------------------------
PbarAtLead2::
PbarAtLead2(const std::string &xml, const TileInfo &tiling) :
  _ok(true)
{
  // if (TaXml::readBoolean(xml, "ValuesSetLead", _fixedValuesSet))
  // {
  //   LOG(ERROR) << "Reading tag for ValuesSetLead";
  //   _ok = false;
  //   return;
  // }
  // if (TaXml::readBoolean(xml, "ThreshSetLead", _thresholdsSet))
  // {
  //   LOG(ERROR) << "Reading tag for ThreshSetLead";
  //   _ok = false;
  //   return;
  // }
  vector<string> vstring;
  if (TaXml::readStringArray(xml, "PbarThresh1", vstring))
  {
    LOG(ERROR) << "Reading tag PbarThresh1";
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
    _thresh1.push_back(v);
  }
  if (TaXml::readStringArray(xml, "PbarThresh2", vstring))
  {
    LOG(ERROR) << "Reading tag PbarThresh2";
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
    _thresh2.push_back(v);
  }

  if (TaXml::readStringArray(xml, PbarAtLeadThresh2::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag " << PbarAtLeadThresh2::_tag;
    _ok = false;
    return;
  }
  for (size_t i=0; i<vstring.size(); ++i)
  {
    PbarAtLeadThresh2 m(vstring[i], tiling);
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
PbarAtLead2::~PbarAtLead2()
{
}

//------------------------------------------------------------------
std::vector<double>
PbarAtLead2::getPbars2ForOneTile(int tileIndex, int index1) const
{
  vector<double> ret;
  // if (!_thresholdsSet)
  // {
  //   return ret;
  // }
  for (size_t i=0; i<_thresh2.size(); ++i)
  {
    int index = _index2d(index1, i);
    ret.push_back(_pbarAtThresh[index].pbarForTile(tileIndex));
  }      
  return ret;
}

//------------------------------------------------------------------
std::vector<double>
PbarAtLead2::getTilePbar(int threshIndex1, int threshIndex2) const
{
  vector<double> ret;
  // if (!_thresholdsSet)
  // {
  //   return ret;
  // }
  int index = _index2d(threshIndex1, threshIndex2);
  return _pbarAtThresh[index].pbar();
}

//------------------------------------------------------------------
void PbarAtLead2::copyPbarForAllThresh(int fromTile, int toTile)
  
{
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    _pbarAtThresh[i].setPbar(toTile, _pbarAtThresh[i].pbarForTile(fromTile));
  }
}

//------------------------------------------------------------------
void PbarAtLead2::setPbarForAllThresh(int tileIndex,
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
std::string PbarAtLead2::toXml(void) const
{
  string ret = TaXml::writeStartTag(_tag, 0);
  // ret += TaXml::writeBoolean("ValuesSetLead", 0, _fixedValuesSet);
  // ret += TaXml::writeBoolean("ThreshSetLead", 0, _thresholdsSet);
  for (size_t i=0; i<_thresh1.size(); ++i)
  {
    ret += TaXml::writeDouble("PbarThresh1", 0, _thresh1[i]);
  }
  for (size_t i=0; i<_thresh2.size(); ++i)
  {
    ret += TaXml::writeDouble("PbarThresh2", 0, _thresh2[i]);
  }
  for (size_t i=0; i<_pbarAtThresh.size(); ++i)
  {
    ret += _pbarAtThresh[i].toXml();
  }
  ret += TaXml::writeEndTag(_tag, 0);
  return ret;
}

//------------------------------------------------------------------
void PbarAtLead2::setPbar(int threshIndex1, int threshIndex2,
			 int tileIndex, double value)
{
  if (threshIndex1 < 0 || threshIndex1 >= (int)_thresh1.size())
  {
    LOG(ERROR) << "Setting pbar threshindex1 out of range " << threshIndex1;
    return;
  }
  if (threshIndex2 < 0 || threshIndex2 >= (int)_thresh2.size())
  {
    LOG(ERROR) << "Setting pbar threshindex1 out of range " << threshIndex2;
    return;
  }
  int index = _index2d(threshIndex1, threshIndex2);
  _pbarAtThresh[index].setPbar(tileIndex, value);
}
  
//------------------------------------------------------------------
bool PbarAtLead2::getTiledGrid(const std::string &inputName,
			       double thresh1, double thresh2,
			      const TileInfo &tiling,
			      double centerWeight, double edgeWeight,
			      int nptSmooth,
			      Grid2d &item) const
{
  int index = _indexFromThresh(thresh1, thresh2);
  if (index < 0)
  {
    LOG(ERROR) << "No thresh in db " << thresh1 << " " << thresh2;
    return false;
  }

  char buf[1000];
  sprintf(buf, "_%05.2lf_%05.2lf_pbar", thresh1, thresh2);
  std::string name = inputName + buf;
  return _pbarAtThresh[index].constructTiledGrid(name, tiling,
						 centerWeight, edgeWeight,
						 nptSmooth, item);
}


//------------------------------------------------------------------
bool PbarAtLead2::getTiledGrid2(const std::string &name1,
				const std::string &name2,
			       double thresh1, double thresh2,
				const TileInfo &tiling,
				double centerWeight, double edgeWeight,
				int nptSmooth,
				Grid2d &item) const
{
  int index = _indexFromThresh(thresh1, thresh2);
  if (index < 0)
  {
    LOG(ERROR) << "No thresh in db " << thresh1 << " " << thresh2;
    return false;
  }

  char buf[1000];
  string n1 = name1.substr(0,2);
  string n2 = name2.substr(0,2);
  sprintf(buf, "%s%05.2lf_%s%05.2lf_pbar", n1.c_str(), thresh1, n2.c_str(), thresh2);
  std::string name = buf;
  return _pbarAtThresh[index].constructTiledGrid(name, tiling,
						 centerWeight, edgeWeight,
						 nptSmooth, item);
}


//------------------------------------------------------------------
void PbarAtLead2::print(int lt, const TileInfo &info,
		       bool verbose) const
{
  printf("Pbar thresh1:");
  for (size_t i=0; i<_thresh1.size(); ++i)
  {
    printf(" %5.2lf", _thresh1[i]);
  }
  printf("\n");
  printf("Pbar thresh2:");
  for (size_t i=0; i<_thresh2.size(); ++i)
  {
    printf(" %5.2lf", _thresh2[i]);
  }
  printf("\n");

  for (size_t i1=0; i1<_thresh1.size(); ++i1)
  {
    for (size_t i2=0; i2<_thresh2.size(); ++i2)
    {
      int index = _index2d(i1, i2);
      _pbarAtThresh[index].print(lt, _thresh1[i1], _thresh2[i2], info, verbose);
    }
  }
}

//------------------------------------------------------------------
void
PbarAtLead2::print(int lt, const std::vector<int> &tiles,
		  const TileInfo &info,
		  bool verbose) const
{
  printf("Pbar thresh1:");
  for (size_t i=0; i<_thresh1.size(); ++i)
  {
    printf(" %5.2lf", _thresh1[i]);
  }
  printf("\n");
  printf("Pbar thresh2:");
  for (size_t i=0; i<_thresh2.size(); ++i)
  {
    printf(" %5.2lf", _thresh2[i]);
  }
  printf("\n");

  for (size_t i1=0; i1<_thresh1.size(); ++i1)
  {
    for (size_t i2=0; i2<_thresh2.size(); ++i2)
    {
      int index = _index2d(i1, i2);
      _pbarAtThresh[index].print(lt, _thresh1[i1], _thresh2[i2],
				 tiles, info, verbose);
    }
  }
}

int PbarAtLead2::_index2d(int i1, int i2) const
{
  return i2*((int)_thresh1.size()) + i1;
}

int PbarAtLead2::_indexFromThresh(double thresh1, double thresh2) const
{
  int i1 = -1;
  int i2 = -1;
  for (size_t i=0; i<_thresh1.size(); ++i)
  {
    if (_thresh1[i] == thresh1)
    {
      i1 = i;
      break;
    }
  }
  if (i1 < 0)
  {
    return -1;
  }
  for (size_t i=0; i<_thresh2.size(); ++i)
  {
    if (_thresh2[i] == thresh2)
    {
      i2 = i;
      break;
    }
  }
  if (i2 < 0)
  {
    return -1;
  }
  return _index2d(i1, i2);
}
