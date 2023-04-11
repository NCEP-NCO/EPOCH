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
 * @file MetaData.cc
 */

//----------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <string>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/MetaData.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
MetaData::MetaData() :
  pName(),
  pInfo(),
  pSource(),
  pXml()
{
}

//----------------------------------------------------------------
MetaData::MetaData(const string &name, const string &info,
		   const string &source, const MetaDataXml &xml) :
  pName(name),
  pInfo(info),
  pSource(source),
  pXml(xml)
{
}

//----------------------------------------------------------------
MetaData::MetaData(const string &name, const string &info,
		   const string &source, const string &xml) :
  pName(name),
  pInfo(info),
  pSource(source),
  pXml(xml)
{
}

//----------------------------------------------------------------
MetaData::~MetaData()
{
}

//----------------------------------------------------------------
bool MetaData::operator==(const MetaData &m) const
{
  return (pName == m.pName &&
	  pInfo == m.pInfo &&
	  pSource == m.pSource &&
	  pXml == m.pXml);
}

//----------------------------------------------------------------
bool MetaData::similar(const MetaData &m) const
{
  return pXml.similar(m.pXml);
}

//----------------------------------------------------------------
string MetaData::printDifferences(const MetaData &m) const
{
  string ret = "";
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  if (pName != m.pName)
  {
    sprintf(buf, "MetaData name unequal %s %s\n",
	    pName.c_str(), m.pName.c_str());
    ret += buf;
  }
  if (pInfo != m.pInfo)
  {
    sprintf(buf, "MetaData info unequal %s %s\n",
	    pInfo.c_str(), m.pInfo.c_str());
    ret += buf;
  }
  if (pSource != m.pSource)
  {
    sprintf(buf, "MetaData source unequal %s %s\n",
	    pSource.c_str(), m.pSource.c_str());
    ret += buf;
  }
  ret += pXml.printDifferences(m.pXml);
  return ret;
}

//----------------------------------------------------------------
string MetaData::printMajorDifferences(const MetaData &m) const
{
  return pXml.printMajorDifferences(m.pXml);
}

//----------------------------------------------------------------
string MetaData::sprint(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];

  // nice ASCII:
  sprintf(buf, "Metadata:name=%s  ", pName.c_str());
  sprintf(buf+strlen(buf), "info=%s  ", pInfo.c_str());
  sprintf(buf+strlen(buf), "source=%s  ", pSource.c_str());
  string ret = buf;

  // append similar from pXml object
  ret += pXml.oneLineString();

  return ret;
}

//----------------------------------------------------------------
string MetaData::sprintDebug(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];

  sprintf(buf, "Metadata:name=%s\n", pName.c_str());
  sprintf(buf+strlen(buf), "Metadata:info=%s\n", pInfo.c_str());
  sprintf(buf+strlen(buf), "Metadata:source=%s\n", pSource.c_str());
  string ret = buf;
  ret += "Metadata:XML:\n";
  ret += pXml.xmlString();
  return ret;
}

//----------------------------------------------------------------
void MetaData::logMetadata(void) const
{
  string p = sprint();
  ILOG(FORCE, p);
}

//----------------------------------------------------------------
MetaData MetaData::merge(const MetaData &mergeData) const
{
  MetaData ret = *this;

  // concatenate the info/name and source
  ret.pInfo += " ";
  ret.pInfo += mergeData.pInfo;

  ret.pName += " ";
  ret.pName += mergeData.pName;

  ret.pSource += " ";
  ret.pSource += mergeData.pSource;

  // merge the metadata XML's 
  ret.pXml.merge(mergeData.pXml);
  return ret;
}

//----------------------------------------------------------------
void MetaData::printXmlString(void) const
{
  printf("%s\n", xmlString().c_str());
}
