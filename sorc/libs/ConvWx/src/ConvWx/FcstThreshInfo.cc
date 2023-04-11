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
 * @file FcstThreshInfo.cc
 */

//----------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/FcstThreshInfo.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/Xml.hh>

using std::string;

/**
 * Set the value for the fixed XML tag
 */
const string FcstThreshInfo::pTag = "FieldThresh";

//----------------------------------------------------------------
FcstThreshInfo::FcstThreshInfo(void) :
  pOk(false),
  pGentime(convWx::BAD_TIME),
  pLeadtime(0),
  pField()
{
}

//----------------------------------------------------------------
FcstThreshInfo::FcstThreshInfo(const time_t &gt, const int lt,
			       const std::string &name,
			       const std::vector<double> &thresh) :
  pOk(true),
  pGentime(gt),
  pLeadtime(lt),
  pField(name),
  pThresh(thresh)
{
}

//----------------------------------------------------------------
FcstThreshInfo::FcstThreshInfo(const std::string &xml, int numTiles)
{
  pOk = parseXml(xml, numTiles);
}

//----------------------------------------------------------------
FcstThreshInfo::~FcstThreshInfo()
{
}

//----------------------------------------------------------------
bool FcstThreshInfo::operator<(const FcstThreshInfo &f) const
{
  return pGentime < f.pGentime || (pGentime == f.pGentime && 
				   pLeadtime < f.pLeadtime);
}

//----------------------------------------------------------------
void FcstThreshInfo::print(void) const
{
  printf("%s+%d  %s:[", ConvWxTime::stime(pGentime).c_str(),
	 pLeadtime, pField.c_str());
  for (size_t i=0; i<pThresh.size(); ++i)
  {
    printf("%.6lf,", pThresh[i]);
  }
  printf("]\n");
}

//----------------------------------------------------------------
std::string FcstThreshInfo::getXml(void) const
{
  string s = Xml::writeString("Name", 0, pField);
  for (size_t i=0; i<pThresh.size(); ++i)
  {
    s += Xml::writeDouble("Thresh", 0, pThresh[i]);
    // remove the tailing '\n', then write the tile index on the same line
    s = s.substr(0, s.size()-1);
    s += Xml::writeInt("Tile", 0, i);
  }
  s += Xml::writeTime("GenTime", 0, pGentime);
  s += Xml::writeInt("LeadTime", 0, pLeadtime);
  return Xml::writeString(pTag, 0, s);
}

//----------------------------------------------------------------
bool FcstThreshInfo::parseXml(const std::string &xml, int numTiles)
{
  bool status = true;
  if (!Xml::readString(xml, "Name", true, pField))
  {
    status= false;
  }
  std::vector<string> s;
  pThresh.clear();
  if (!Xml::readXmlStringArray(xml, "Thresh", s))
  {
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
	if (Xml::readDouble(s[i], "Thresh", true, j))
	{
	  pThresh.push_back(j);
	}
	else
	{
	  status = false;
	}
      }
    }
  }

  if (!Xml::readTime(xml, "GenTime", true, pGentime))
  {
    status = false;
  }
  if (!Xml::readInt(xml, "LeadTime", true, pLeadtime))
  {
    status = false;
  }
  return status;
}
