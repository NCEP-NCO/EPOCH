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
 * @file FixedFcstThreshInfo.cc
 */

//----------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <Epoch/FixedFcstThreshInfo.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>

using std::string;

/**
 * Set the value for the fixed XML tag
 */
const string FixedFcstThreshInfo::_tag = "FixedThresh";

//----------------------------------------------------------------
FixedFcstThreshInfo::FixedFcstThreshInfo(void) :
  _ok(false),
  _field()
{
}

//----------------------------------------------------------------
FixedFcstThreshInfo::FixedFcstThreshInfo(const std::string &name,
					 double thresh) :
  _ok(true),
  _field(name),
  _thresh(thresh)
{
}

//----------------------------------------------------------------
FixedFcstThreshInfo::FixedFcstThreshInfo(const std::string &xml)
{
  _ok = parseXml(xml);
}

//----------------------------------------------------------------
FixedFcstThreshInfo::~FixedFcstThreshInfo()
{
}

//----------------------------------------------------------------
void FixedFcstThreshInfo::print(void) const
{
  printf("%s:%.6lf\n", _field.c_str(), _thresh);
}

//----------------------------------------------------------------
std::string FixedFcstThreshInfo::getXml(void) const
{
  string s = TaXml::writeString("Name", 0, _field);
  s += TaXml::writeDouble("Thresh", 0, _thresh);
  return TaXml::writeString(_tag, 0, s);
}

//----------------------------------------------------------------
bool FixedFcstThreshInfo::parseXml(const std::string &xml)
{
  bool status = true;
  if (TaXml::readString(xml, "Name", _field))
  {
    LOG(ERROR) << "reading xml for Name";
    status= false;
  }
  if (TaXml::readDouble(xml, "Thresh", _thresh))
  {
    LOG(ERROR) << "reading xml for Thresh";
    status = false;
  }

  return status;
}
