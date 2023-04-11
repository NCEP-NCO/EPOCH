/**
 * @file SingleTileThresholdsGenBased.cc
 */

//------------------------------------------------------------------
#include <Epoch/SingleTileThresholdsGenBased.hh>
#include <Epoch/TileThreshInfoGenBased.hh>
#include <Epoch/TileInfo.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>

const std::string SingleTileThresholdsGenBased::_tag = "TileData";

//------------------------------------------------------------------
SingleTileThresholdsGenBased::SingleTileThresholdsGenBased(void) :
  _ok(false), _bias(-99.99), _coldstart(true), _motherTile(false)
{
}

//------------------------------------------------------------------
SingleTileThresholdsGenBased::
SingleTileThresholdsGenBased(const std::string &xml) :
  _ok(true)
{
  if (TaXml::readDouble(xml, "Thresh", _thresh))
  {
    LOG(ERROR) << "Reading tag Thresh";
    _ok = false;
  }
  if (TaXml::readDouble(xml, "Bias", _bias))
  {
    LOG(ERROR) << "Parsing Bias XML as double";
    _ok = false;
  }
  if (TaXml::readBoolean(xml, "coldstart", _coldstart))
  {
    LOG(ERROR) << "no boolean coldstart";
    _ok = false;
  }
  if (TaXml::readBoolean(xml, "motherTile", _motherTile))
  {
    LOG(ERROR) << "no boolean motherTile";
    _ok = false;
  }
}

//------------------------------------------------------------------
SingleTileThresholdsGenBased::
SingleTileThresholdsGenBased(double precipThresh, bool fromMother) :
  _ok(true),
  _thresh(precipThresh),
  _bias(-99.99),
  _coldstart(true),
  _motherTile(fromMother)
{
}

//------------------------------------------------------------------
SingleTileThresholdsGenBased::~SingleTileThresholdsGenBased()
{
}

//------------------------------------------------------------------
bool SingleTileThresholdsGenBased::update(const TileThreshInfoGenBased &item)
{
  _thresh = item.getThresh();
  _coldstart = item.getColdstart();
  _motherTile = item.getMotherTile();
  _bias = item.getBias();
  return true;
}

//------------------------------------------------------------------
std::string SingleTileThresholdsGenBased::toXml(int tileIndex, int indent) const
{
  string s = TaXml::writeStartTag(_tag, indent);
  s += TaXml::writeDouble("Thresh", 0, _thresh, "%010.7lf");
  s += TaXml::writeDouble("Bias", indent+1, _bias, "%010.7lf");
  s += TaXml::writeBoolean("coldstart", indent+1, _coldstart);
  s += TaXml::writeBoolean("motherTile", indent+1, _motherTile);
  s += TaXml::writeEndTag(_tag, indent);
  return s;
}


//------------------------------------------------------------------
void SingleTileThresholdsGenBased::print(int lt, double obar, int tileIndex,
					 const TileInfo &info,bool verbose) const
{
  printf("        lt:%08d obar:%5.2lf tile[%3d(%s)] thr:%5.2lf bias:%10.8lf",
	 lt, obar, tileIndex, info.latlonDebugString(tileIndex).c_str(),
	 _thresh, _bias);
  if (_motherTile)
  {
    printf(" Mother");
  }
  else
  {
    printf("       ");
  }
  if (_coldstart)
  {
    printf(" Coldstart");
  }
  else
  {
    printf("          ");
  }
  if (_motherTile)
  {
    printf(" Mother\n");
  }
  else
  {
    printf("       \n");
  }
}

