/**
 * @file TileObarInfo.cc
 */

#include <Epoch/TileObarInfo.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>

const std::string TileObarInfo::_tag = "ObarTile";

//-----------------------------------------------------------
TileObarInfo::TileObarInfo(const std::string &xml) :
  _ok(true)
{
  if (TaXml::readDouble(xml, "oBar", _obar))
  {
    LOG(ERROR) << "No oBar tag";
    _ok = false;
  }
  if (TaXml::readBoolean(xml, "coldstart", _coldstart))
  {
    LOG(ERROR) << "No coldstart tag";
    _ok = false;
  }
  if (TaXml::readBoolean(xml, "mothertile", _motherTile))
  {
    LOG(ERROR) << "No mothertile tag";
    _ok = false;
  }
  if (TaXml::readInt(xml, "tileIndex", _tileIndex))
  {
    LOG(ERROR) << "No tileIndex tag";
    _ok = false;
  }
}

//-----------------------------------------------------------
std::string TileObarInfo::toXml(int indent) const
{
  string s = TaXml::writeStartTag(_tag, indent);
  s += TaXml::writeDouble("oBar", indent, _obar);
  s += TaXml::writeBoolean("coldstart", indent, _coldstart);
  s += TaXml::writeBoolean("mothertile", indent, _motherTile);
  s += TaXml::writeInt("tileIndex", indent, _tileIndex);
  s += TaXml::writeEndTag(_tag, indent);
  return s;
}

//-----------------------------------------------------------
void TileObarInfo::print(void) const
{
  string cs, mt;
  if (_coldstart)
  {
    cs = "Cold:True";
  }
  else
  {
    cs = "Cold:False";
  }
  if (_motherTile)
  {
    mt = "Mother:True";
  }
  else
  {
    mt = "Mother:False";
  }
  printf("[%03d]:%s %s obar:%.2lf\n", _tileIndex, cs.c_str(), mt.c_str(),
	 _obar);
}

//-----------------------------------------------------------
void TileObarInfo::print(const TileInfo &tiling, bool verbose) const
{
  string cs, mt;
  if (_coldstart)
  {
    cs = "Cold:True";
  }
  else
  {
    cs = "Cold:False";
  }
  if (_motherTile)
  {
    mt = "Mother:True";
  }
  else
  {
    mt = "Mother:False";
  }
  printf("[%03d]:%s %s obar:%.2lf\n", _tileIndex, cs.c_str(), mt.c_str(),
	 _obar);
}

