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
 * @file UvUnits.cc
 */

//----------------------------------------------------------------
#include <cstdio>
#include <string>
#include <ConvWx/UvUnits.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
/**
 * The tag for UV units used in metadata XML
 */
const string UvUnits::pUvUnitsTag = "uv_units";

/**
 * The tag for UV delta seconds used in metadata XML
 */
const string UvUnits::pUvUnitsDtTag = "uv_dt_seconds";


//----------------------------------------------------------------
UvUnits::UvUnits() :
  pUnits(UvUnits::UNKNOWN),
  // set to -1 to indicate not used
  pDtSeconds(-1)
{
}

//----------------------------------------------------------------
UvUnits::UvUnits(const int dtSeconds) :
  pUnits(UvUnits::GRIDPTS_PER_DT_SECONDS),
  pDtSeconds(dtSeconds)
{
}

//----------------------------------------------------------------
UvUnits::UvUnits(const UvUnits_t &type) :
  pUnits(type),
    // set to -1 to indicate not used
  pDtSeconds(-1)
{
  if (type == UvUnits::GRIDPTS_PER_DT_SECONDS)
  {
    // set to unknown because don't know dt seconds
    pUnits = UvUnits::UNKNOWN;
  }
}

//----------------------------------------------------------------
UvUnits::~UvUnits()
{
}

//----------------------------------------------------------------
bool UvUnits::operator==(const UvUnits &u) const
{
  return (pUnits == u.pUnits &&
	  pDtSeconds == u.pDtSeconds);
}

//----------------------------------------------------------------
bool UvUnits::getUnits(UvUnits_t &units) const
{
  units = pUnits;
  return (pUnits != UvUnits::UNKNOWN);
}

//----------------------------------------------------------------
string UvUnits::xml(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "<%s>%s</%s>", pUvUnitsTag.c_str(), enumName(pUnits).c_str(),
	  pUvUnitsTag.c_str());
  string ret = buf;
  sprintf(buf, "<%s>%d</%s>\n", pUvUnitsDtTag.c_str(), pDtSeconds,
	  pUvUnitsDtTag.c_str());
  ret += buf;
  return ret;
}

//----------------------------------------------------------------
string UvUnits::briefDescr(void) const
{
  string ret = enumName(pUnits);
  if (pUnits == UvUnits::GRIDPTS_PER_DT_SECONDS)
  {
    char buf[100];
    sprintf(buf, ",dtSec=%d", pDtSeconds);
    ret += buf;
  }
  return ret;
}

//----------------------------------------------------------------
string UvUnits::enumName(const UvUnits_t type)
{
  string ret;
  switch (type)
  {
  case UvUnits::PURE_PC_GRIDPTS:
    ret = "PURE_PC_GRIDPTS";
    break;
  case UvUnits::GRIDPTS_PER_DT_SECONDS:
    ret = "GRIDPTS_PER_DT_SECONDS";
    break;
  case UvUnits::METERS_PER_SECOND:
    ret = "METERS_PER_SECOND";
    break;
  default:
    ret = "UNKNOWN";
    break;
  }
  return ret;
}

//----------------------------------------------------------------
UvUnits::UvUnits_t UvUnits::enumValue(const string &str)
{
  if (str == "PURE_PC_GRIDPTS")
  {
    return UvUnits::PURE_PC_GRIDPTS;
  }
  else if (str == "GRIDPTS_PER_DT_SECONDS")
  {
    return UvUnits::GRIDPTS_PER_DT_SECONDS;
  }
  else if (str == "METERS_PER_SECOND")
  {
    return UvUnits::METERS_PER_SECOND;
  }
  else
  {
    return UvUnits::UNKNOWN;
  }
}

//----------------------------------------------------------------
string UvUnits::xml(const UvUnits &units)
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "<%s>%s</%s>", pUvUnitsTag.c_str(),
	  enumName(units.pUnits).c_str(),  pUvUnitsTag.c_str());
  string ret = buf;
  sprintf(buf, "<%s>%d</%s>\n", pUvUnitsDtTag.c_str(), units.pDtSeconds,
	  pUvUnitsDtTag.c_str());
  ret += buf;
  return ret;
}

//----------------------------------------------------------------
UvUnits UvUnits::parseXml(const string &xmlStr)
{
  string s;
  if (!Xml::readXmlString(xmlStr, pUvUnitsTag, true, s))
  {
    return UvUnits();
  }
  UvUnits::UvUnits_t u = enumValue(s);
  if (u == UvUnits::UNKNOWN)
  {
    return UvUnits();
  }
  else if (u == UvUnits::GRIDPTS_PER_DT_SECONDS)
  {
    int dtSeconds;
    if (!Xml::readXmlInt(xmlStr, pUvUnitsDtTag, true, dtSeconds))
    {
      return UvUnits();
    }
    else
    {
      return UvUnits(dtSeconds);
    }
  }
  else
  {
    return UvUnits(u);
  }
}

//----------------------------------------------------------------
bool UvUnits::parseXmlOptional(const string &xmlStr, UvUnits &units)
{
  string s;
  if (!Xml::readXmlString(xmlStr, pUvUnitsTag, false, s))
  {
    return false;
  }
  UvUnits::UvUnits_t type = enumValue(s);
  if (type == UvUnits::UNKNOWN)
  {
    return false;
  }
  else if (type == UvUnits::GRIDPTS_PER_DT_SECONDS)
  {
    int dtSeconds;
    if (!Xml::readXmlInt(xmlStr, pUvUnitsDtTag, false, dtSeconds))
    {
      return false;
    }
    else
    {
      units = UvUnits(dtSeconds);
      return true;
    }
  }
  else
  {
    units = UvUnits(type);
    return true;
  }
}

//----------------------------------------------------------------
string UvUnits::descriptor(const UvUnits &units)
{
  string ret;
  switch (units.pUnits)
  {
  case UvUnits::PURE_PC_GRIDPTS:
    ret = "Pure phase correction in number of gridpoints, no time dimension";
    break;
  case UvUnits::GRIDPTS_PER_DT_SECONDS:
    ret = "Number of gridpoints moved over dtSeconds seconds";
    break;
  case UvUnits::METERS_PER_SECOND:
    ret = "external units meters per second";
    break;
  case UvUnits::UNKNOWN:
    ret = "Unknown/none";
    break;
  default:
    ret = "Unknown/none";
  }
  return ret;
}

