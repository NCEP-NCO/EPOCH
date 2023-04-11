// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>
/**
 * @file TiledFcstThreshInfo.cc
 */

//----------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <Epoch/TiledFcstThreshInfo.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>

using std::string;

/**
 * Set the value for the fixed XML tag
 */
const string TiledFcstThreshInfo::_tag = "TiledFcstThreshInfo";

//----------------------------------------------------------------
TiledFcstThreshInfo::TiledFcstThreshInfo(void) :
  _ok(false),
  _gentime(),
  _leadtime(0),
  _fieldName()
{
}

//----------------------------------------------------------------
TiledFcstThreshInfo::TiledFcstThreshInfo(const GenTimeAndOlder &gt,
					 const int lt, const std::string &name,
					 const std::vector<double> &thresh) :
  _ok(true),
  _gentime(gt),
  _leadtime(lt),
  _fieldName(name),
  _thresh(thresh)
{
}

//----------------------------------------------------------------
TiledFcstThreshInfo::TiledFcstThreshInfo(const std::string &xml, int numTiles)
{
  _ok = parseXml(xml, numTiles);
}

//----------------------------------------------------------------
TiledFcstThreshInfo::~TiledFcstThreshInfo()
{
}

//----------------------------------------------------------------
bool TiledFcstThreshInfo::operator<(const TiledFcstThreshInfo &f) const
{
  return _gentime < f._gentime || (_gentime == f._gentime && 
				   _leadtime < f._leadtime);
}

//----------------------------------------------------------------
void TiledFcstThreshInfo::print(bool debug) const
{
  printf("%s+%d  %s:[", _gentime.sprint().c_str(), _leadtime,
	 _fieldName.c_str());
  if (debug)
  {
    for (size_t i=0; i<_thresh.size(); ++i)
    {
      printf("%.6lf,", _thresh[i]);
    }
    printf("]\n");
  }
  else
  {
    printf("%d thresholds]\n", (int)_thresh.size());
  }
}

//----------------------------------------------------------------
std::string TiledFcstThreshInfo::getXml(void) const
{
  string s = TaXml::writeString("Name", 0, _fieldName);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    s += TaXml::writeDouble("Thresh", 0, _thresh[i]);
    // remove the tailing '\n', then write the tile index on the same line
    s = s.substr(0, s.size()-1);
    s += TaXml::writeInt("Tile", 0, i);
  }
  s += _gentime.getXml();
  s += TaXml::writeInt("LeadTime", 0, _leadtime);
  return TaXml::writeString(_tag, 0, s);
}

//----------------------------------------------------------------
bool TiledFcstThreshInfo::parseXml(const std::string &xml, int numTiles)
{
  bool status = true;
  if (TaXml::readString(xml, "Name", _fieldName))
  {
    LOG(ERROR) << "reading xml for Name";
    status= false;
  }
  std::vector<string> s;
  _thresh.clear();
  if (TaXml::readStringArray(xml, "Thresh", s))
  {
    LOG(ERROR) << "reading xml for Thresh";
    status = false;
  }
  else
  {
    if (static_cast<int>(s.size()) != numTiles)
    {
      status = false;
    }
    else
    {
      for (size_t i=0; i<s.size(); ++i)
      {
	double j;
	if (TaXml::readDouble(s[i], j))
	{
	  status = false;
	}
	else
	{
	  _thresh.push_back(j);
	}
      }
    }
  }

  if (!_gentime.parseXml(xml))
  {
    status = false;
  }
  
  if (TaXml::readInt(xml, "LeadTime", _leadtime))
  {
    LOG(ERROR) << "Reading xml for LeadTime";
    status = false;
  }
  return status;
}
