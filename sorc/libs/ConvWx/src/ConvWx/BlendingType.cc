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
/**
 * @file BlendingType.cc
 */
//----------------------------------------------------------------
#include <cstring>
#include <vector>
#include <ConvWx/BlendingType.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <cstdio>
using std::string;
using std::pair;
using std::vector;

/**
 * Set the value for the fixed XML tag for blending
 */
const string BlendingType::pBlendingTypeTag = "forecast_type";

//----------------------------------------------------------------
string BlendingType::typeDescrXml(void)
{
  // note this has to agree with Blend_t numerical values, see header file
  string ret = "<forecast_descr>1=blended, 2-extrapOnly, 3-model-only, "
    "0-missing</forecast_descr>\n";
  return ret;
}

//----------------------------------------------------------------
string BlendingType::xml(const Blend_t blendType)
{
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "<%s>%d</%s>\n", pBlendingTypeTag.c_str(),
	  static_cast<int>(blendType),
	  pBlendingTypeTag.c_str());
  string ret = buf;
  return ret;
}

//----------------------------------------------------------------
BlendingType::Blend_t BlendingType::parseXml(const string &xmlString)
{
  int v;
  if (Xml::readXmlInt(xmlString, pBlendingTypeTag, true, v))
  {
    return static_cast<Blend_t>(v);
  }
  else
  {
    return BlendingType::MISSING;
  }
}

//----------------------------------------------------------------
bool BlendingType::parseXmlOptional(const string &xmlString, Blend_t &blendType)
{
  int v;
  if (Xml::readXmlInt(xmlString, pBlendingTypeTag, false, v))
  {
    blendType = static_cast<Blend_t>(v);
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
string BlendingType::descriptor(const BlendingType::Blend_t blendType)
{
  string ret;
  switch (blendType)
  {
  case BlendingType::MISSING:
    ret = "No Blending (missing data)";
    break;
  case BlendingType::BLENDED:
    ret = "Normal blending of 2 inputs";
    break;
  case BlendingType::EXTRAP_ONLY:
    ret = "Output is based on extrap input only";
    break;
  case BlendingType::MODEL_ONLY:
    ret = "Output is based on model input only";
    break;
  case BlendingType::UNKNOWN:
    ret = "Unknown/none";
    break;
  default:
    ret = "Unknown/none";
  }
  return ret;
}

//----------------------------------------------------------------
vector<pair<string,string> > BlendingType::helpDescriptors(void)
{
  vector<pair<string,string> > ret;
  pair<string,string> p0(BlendingType::descriptor(BlendingType::MISSING),
			 "Because of problems, all missing output");
  pair<string,string> p1(BlendingType::descriptor(BlendingType::BLENDED),
			 "Normal blending");
  pair<string,string> p2(BlendingType::descriptor(BlendingType::EXTRAP_ONLY),
			 "Model missing, blending=extrap");
  pair<string,string> p3(BlendingType::descriptor(BlendingType::MODEL_ONLY),
			 "Extrap missing, blending=model");
  ret.push_back(p0);
  ret.push_back(p1);
  ret.push_back(p2);
  ret.push_back(p3);
  return ret;
}

//----------------------------------------------------------------
string BlendingType::blendingDescriptionString(const Blend_t t)
{
  int i = static_cast<int>(t);
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "blending=%d", i);
  string ret = buf;
  return ret;
}

//----------------------------------------------------------------
string BlendingType::blendingDescriptionStringNone(void)
{
  return blendingDescriptionString(BlendingType::UNKNOWN);
}
