/**
 * @file PbarAtLeadThresh2.cc
 */

//------------------------------------------------------------------
#include <Epoch/PbarAtLeadThresh2.hh>
#include <Epoch/TileInfo.hh>
#include <euclid/GridAlgs.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

const std::string PbarAtLeadThresh2::_tag = "PbarLeadThresh2";

//------------------------------------------------------------------
PbarAtLeadThresh2::
PbarAtLeadThresh2(const TileInfo &tiling) :
  _ok(false)//, _valuesSet(false)
{
  for (int i=0; i<tiling.numTiles(); ++i)
  {
    _pbar.push_back(-1);
  }
}

//------------------------------------------------------------------
PbarAtLeadThresh2::
PbarAtLeadThresh2(const std::string &xml, const TileInfo &tiling) :
  _ok(true)
{
  // if (TaXml::readBoolean(xml, "ValuesSet", _valuesSet))
  // {
  //   LOG(ERROR) << "Reading tag for ValuesSet";
  //   _ok = false;
  //   return;
  // }
  
  // read in tiles array, and compare to input.
  std::vector<std::string> vstring;
  if (TaXml::readStringArray(xml, "Pbar2", vstring))
  {
    LOG(ERROR) << "Reading tag as array Pbar2";
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
    double v;
    if (sscanf(vstring[i].c_str(), "%lf", &v) != 1)
    {
      LOG(ERROR) << "Scanning as double " << vstring[i];
      _ok = false;
    }
    else
    {
      _pbar.push_back(v);
    }
  }
}

//------------------------------------------------------------------
PbarAtLeadThresh2::~PbarAtLeadThresh2(void)
{
}

//------------------------------------------------------------------
std::vector<double> PbarAtLeadThresh2::pbar(void) const
{
  return _pbar;
}

//------------------------------------------------------------------
std::string PbarAtLeadThresh2::toXml(int indent) const
{
  string s = TaXml::writeStartTag(_tag, indent);
  // s += TaXml::writeBoolean("ValuesSet", 0, _valuesSet);
  for (size_t i=0; i<_pbar.size(); ++i)
  {
    s += TaXml::writeDouble("Pbar2", 1, _pbar[i]);
  }
  s += TaXml::writeEndTag(_tag, indent);
  return s;
}

//------------------------------------------------------------------
bool
PbarAtLeadThresh2::constructTiledGrid(const std::string &fieldName,
				      const TileInfo &tiling,
				      double centerWeight,
				      double edgeWeight,
				      int nptSmooth,
				      Grid2d &grid) const
{
  return tiling.constructWeightedTiledGrid(fieldName, _pbar,
   					   centerWeight, edgeWeight,
   					   nptSmooth, grid);
}

//------------------------------------------------------------------
 void PbarAtLeadThresh2::setPbar(int index, double v)
{
  // _valuesSet = true; // ?
  _pbar[index] = v;
}

//------------------------------------------------------------------
void
PbarAtLeadThresh2::print(int lt, double thresh1, double thresh2, const TileInfo &info,
			bool verbose) const
{
  printf(" --- Pbar at threshold (%5.2lf,%5.2lf) -----\n", thresh1, thresh2);
  for (size_t i=0; i<_pbar.size(); ++i)
  {
    printf("    lt:%08d thr1:%5.2lf thr2:%5.2lf til[%3d(%s)] pbar:%5.2lf\n",
	   lt, thresh1, thresh2, (int)i, info.latlonDebugString(i).c_str(), _pbar[i]);
  }
}

//------------------------------------------------------------------
void
PbarAtLeadThresh2 ::print(int lt, double thresh1, double thresh2,
			  const std::vector<int> &tiles,
			  const TileInfo &info,
			  bool verbose) const
{
  if (tiles.empty())
  {
    print(lt, thresh1, thresh2, info, verbose);
  }
  else
  {
    printf(" --- Pbar at threshold (%5.2lf,%5.2lf) -----\n", thresh1, thresh2);
    {
      for (size_t i=0; i<_pbar.size(); ++i)
      {
	if (find(tiles.begin(), tiles.end(), (int)i) != tiles.end())
	{
	  printf("    lt:%08d thr1:%5.2lf thr2:%5.2lf til[%3d(%s)] pbar:%5.2lf\n",
		 lt, thresh1, thresh2, (int)i, info.latlonDebugString(i).c_str(),
		 _pbar[i]);
	}
      }
    }
  }
}

