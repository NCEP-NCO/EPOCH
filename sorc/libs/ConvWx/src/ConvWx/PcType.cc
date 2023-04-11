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
 * @file PcType.cc
 */

//----------------------------------------------------------------
#include <cstdio>
#include <string>
#include <vector>
#include <ConvWx/PcType.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::vector;
using std::pair;

//----------------------------------------------------------------
/**
 * Set the tag for phase correction used in metadata XML
 */
const string PcType::pPcTag = "phase_corr_type";

//----------------------------------------------------------------
/**
 * Replace the value between a tag and endtag with a new value in a string
 * @param[in] tag  The start tag
 * @param[in] endTag  The end tag
 * @param[in] newType  The stuff to replace with
 * @param[in,out] s  String
 * @return true if successful
 */
static bool sReplace(const string &tag, const string &endTag,
		     const string &newType, string &s)
{
  // look for start and end tags
  string::size_type l0 = s.find(tag);
  string::size_type l1 = s.find(endTag);
  if (l0 == string::npos || l1 == string::npos)
  {
    return false;
  }
  l1 += endTag.size();
  string::size_type n = l1-l0;
  string t;
  if (s.compare( l1, 1, "\n") == 0)
  {
    string::size_type m = newType.size();
    t = newType.substr(0, m-1);
  }
  else
  {
    t = newType;
  }
  s.replace( l0, n, t);
  return true;
}

//----------------------------------------------------------------
string PcType::typeDescrXml(void)
{
  string ret = "<phase_corr_descr>-2=NO_PC(No UV or Zero UV Data), -1=NO_PC(Low Skill), 0=NO_PC(Algorithm),3=NCAR phase correction, 4=NCAR Lagrangian phase correction, 5=NCAR Eularian fallback from Lagrangian due to no advection data</phase_corr_descr>\n";
  return ret;
}

//----------------------------------------------------------------
string PcType::xml(const Pc_t pcType)
{
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "<%s>%d</%s>\n", pPcTag.c_str(), static_cast<int>(pcType),
	  pPcTag.c_str());
  string ret = buf;
  return ret;
}

//----------------------------------------------------------------
PcType::Pc_t PcType::parseXml(const string &xmlStr)
{
  int v;
  if (Xml::readXmlInt(xmlStr, pPcTag, true, v))
  {
    return static_cast<Pc_t>(v);
  }
  else
  {
    return PcType::UNKNOWN;
  }
}

//----------------------------------------------------------------
bool PcType::parseXmlOptional(const string &xmlStr, Pc_t &pcType)
{
  int v;
  if (Xml::readXmlInt(xmlStr, pPcTag, false, v))
  {
    pcType = static_cast<Pc_t>(v);
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
string PcType::descriptor(const PcType::Pc_t pcType)
{
  string ret;
  switch (pcType)
  {
  case PcType::NO_PC_NO_UV_OR_ZERO_UV_DATA:
    ret = "No P.C. (no U/V data found or U/V all zero)";
    break;
  case PcType::NO_PC_LOW_SKILL:
    ret = "No P.C. (no improvement with pc)";
    break;
  case PcType::NO_PC:
    ret = "No P.C. (determined by alg.)";
    break;
  case PcType::PC_ARMOR_EULARIAN:
    ret = "P.C. Armor (time tapered Eularian)";
    break;
  case PcType::PC_ARMOR_LAGRANGIAN:
    ret = "P.C. Armor (Lagrangian)";
    break;
  case PcType::PC_NCAR:
    ret = "P.C. NCAR";
    break;
  case PcType::PC_NCAR_LAGRANGIAN:
    ret = "P.C. NCAR (Lagrangian)";
    break;
  case PcType::PC_NCAR_EULARIAN_NO_ADVECT_UV:
    ret = "P.C. NCAR (no advection UV so revert to non-lagrangian P.C.NCAR)";
    break;
  case PcType::UNKNOWN:
    ret = "Unknown/none";
    break;
  default:
    ret = "Unknown/none";
    break;
  }
  return ret;
}

//----------------------------------------------------------------
vector<pair<string,string> > PcType::helpDescriptors(void)
{
  vector<pair<string,string> > ret;
  pair<string,string> p0(PcType::descriptor(PcType::NO_PC_NO_UV_OR_ZERO_UV_DATA),
			 "No phase correction possible due to lack of correction vectors or correction vectors are all zero");
  pair<string,string> p1(PcType::descriptor(PcType::NO_PC_LOW_SKILL),
			  "No phase correction because it didn't improve skill");
  pair<string,string> p2(PcType::descriptor(PcType::NO_PC),
			 "Internal alg. decided no phase correction");
  pair<string,string> p3(PcType::descriptor(PcType::PC_NCAR),
			 "Did this phase correcton techique (Brewster)");
  pair<string,string> p4(PcType::descriptor(PcType::PC_NCAR_LAGRANGIAN),
			 "Did this phase correcton techique (Brewster with Lagrangian trending)");
  pair<string,string> p5(PcType::descriptor(PcType::PC_NCAR_EULARIAN_NO_ADVECT_UV),
			 "Did this phase correcton techique (Brewster)");
  ret.push_back(p0);
  ret.push_back(p1);
  ret.push_back(p2);
  ret.push_back(p3);
  ret.push_back(p4);
  ret.push_back(p5);
  return ret;
}

//----------------------------------------------------------------
bool PcType::replaceTypeXml(string &xmlStr, const Pc_t pcType)
{
  string tag = "<";
  tag += pPcTag; 
  tag += ">";
  string endtag = "</";
  endtag += pPcTag; 
  endtag += ">";
  string newType = PcType::xml(pcType);
  return sReplace(tag, endtag, newType, xmlStr);
}

//----------------------------------------------------------------
bool PcType::replaceTypeDescriptorXml(string &xmlStr)
{
  string tag = "<phase_corr_descr>";
  string endtag = "</phase_corr_descr>";
  string newType = PcType::typeDescrXml();
  return sReplace(tag, endtag, newType, xmlStr);
}

//----------------------------------------------------------------
string PcType::correctionString(const Pc_t pcType)
{
  int i = static_cast<int>(pcType);
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "correction=%d", i);
  string ret = buf;
  return ret;
}

//----------------------------------------------------------------
string PcType::correctionStringNone(void)
{
  return correctionString(PcType::UNKNOWN);
}

//----------------------------------------------------------------
bool PcType::doesPc(const Pc_t pcType)
{
  bool ret;
  switch (pcType)
  {
  case PcType::PC_ARMOR_EULARIAN:
  case PcType::PC_ARMOR_LAGRANGIAN:
  case PcType::PC_NCAR:
  case PcType::PC_NCAR_LAGRANGIAN:
  case PcType::PC_NCAR_EULARIAN_NO_ADVECT_UV:
    ret = true;
    break;
  case PcType::NO_PC_NO_UV_OR_ZERO_UV_DATA:
  case PcType::NO_PC_LOW_SKILL:
  case PcType::NO_PC:
  case PcType::UNKNOWN:
  default:
    ret = false;
    break;
  }
  return ret;
}
