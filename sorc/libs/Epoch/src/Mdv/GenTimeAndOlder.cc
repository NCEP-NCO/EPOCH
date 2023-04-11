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
 * @file GenTimeAndOlder.cc
 */

//----------------------------------------------------------------
#include <Epoch/GenTimeAndOlder.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

/**
 * Set the value for the fixed XML tag
 */
const std::string GenTimeAndOlder::_tag = "GenTimeFromSpdb";

//---------------------------------------------------------------
GenTimeAndOlder::GenTimeAndOlder(const std::string &xml) :
  _ok(true), _genTime(0), _olderGenTime(0), _hasOlderGenTime(false)
{
  parseXml(xml);
}

//---------------------------------------------------------------
bool GenTimeAndOlder::operator<(const GenTimeAndOlder &f) const
{
  // keep it simple for now
  return _genTime < f._genTime;
}

//---------------------------------------------------------------
bool GenTimeAndOlder::operator==(const GenTimeAndOlder &f) const
{
  if ((!_ok) || (!f._ok))
  {
    return false;
  }
  if (_genTime != f._genTime)
  {
    return false;
  }
  if (_hasOlderGenTime != f._hasOlderGenTime)
  {
    return false;
  }
  if (_hasOlderGenTime)
  {
    return _olderGenTime == f._olderGenTime;
  }
  else
  {
    return true;
  }
}

//---------------------------------------------------------------
std::string GenTimeAndOlder::sprint(void) const
{
  string ret = "NOT SET";
  if (!_ok)
  {
    return ret;
  }

  ret = DateTime::strn(_genTime);
  
  if (_hasOlderGenTime)
  {
    ret += "[";
    ret += DateTime::strn(_olderGenTime);
    ret += "]";
  }
  return ret;
}

//---------------------------------------------------------------
std::string GenTimeAndOlder::getXml(void) const
{
  // assume it is ok at this point
  string ret = "";
  string ret0 = "";
  ret0 += TaXml::writeTime("genData", 0, _genTime);
  ret0 += TaXml::writeBoolean("HasOlderGen", 0, _hasOlderGenTime);
  if (_hasOlderGenTime)
  {
    ret0 += TaXml::writeTime("genOlder", 0, _olderGenTime);
  }
  ret = TaXml::writeString(_tag, 0, ret0);
  return ret;
}

//---------------------------------------------------------------
bool GenTimeAndOlder::parseXml(const std::string &xml)
{
  _ok = true;
  if (TaXml::readTime(xml, "genData", _genTime))
  {
    LOG(WARNING) << "No genData tag found";
    _ok = false;
  }
  else
  {
    if (TaXml::readBoolean(xml, "HasOlderGen", _hasOlderGenTime))
    {
      LOG(WARNING) << "No HasOlderGen tag";
      _ok = false;
    }
    else
    {
      if (_hasOlderGenTime)
      {
	if (TaXml::readTime(xml, "genOlder", _olderGenTime))
	{
	  LOG(WARNING) << "No genOlder found";
	  _ok = false;
	}
      }
    }
  }
  return _ok;
}

