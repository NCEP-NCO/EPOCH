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
 * @file FcstThreshMetadata.cc
 */

//----------------------------------------------------------------
#include <ConvWx/FcstThreshMetadata.hh>
#include <ConvWx/Xml.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <algorithm>
using std::string;
using std::vector;
using std::pair;

/**
 * Set the value for the fixed XML tag
 */
const std::string FcstThreshMetadata::pTag = "SpdbThresh";

//----------------------------------------------------------------
FcstThreshMetadata::FcstThreshMetadata(int numTiles) : pNumTiles(numTiles)
{
}

//----------------------------------------------------------------
FcstThreshMetadata::FcstThreshMetadata(const std::string &xml)
{
  if (xml.empty())
  {
    return;
  }

  if (!Xml::readInt(xml, "NumTiles", true, pNumTiles))
  {
    ILOG(ERROR, "No number of tiles in FcstThresh Metadata");
    return;
  }

  string specificXml;
  if (Xml::readString(xml, pTag, true, specificXml))
  {
    vector<string> v;
    if (Xml::readXmlStringArrayNoWarnings(specificXml,
					  FcstThreshInfo::fcstThreshTag(), v))
    {
      for (size_t i=0; i<v.size(); ++i)
      {
	FcstThreshInfo ii(v[i], pNumTiles);
	if (ii.isOk())
	{
	  pInfo.push_back(ii);
	}
	else
	{
	  ILOGF(WARNING, "Parsing string as fcstthreshinfo %s",
		v[i].c_str());
	}
      }
    }
  }
  sort(pInfo.begin(), pInfo.end());
}

//----------------------------------------------------------------
FcstThreshMetadata::~FcstThreshMetadata()
{
}

//----------------------------------------------------------------
void FcstThreshMetadata::add(const time_t &gen, int lead,
			     const std::string &fieldName, 
			     const std::vector<double> &threshold)
{
  if (static_cast<int>(threshold.size()) != pNumTiles)
  {
    ILOGF(ERROR, "Inconsistent number of tiles %d %d",
	  static_cast<int>(threshold.size()), pNumTiles);
    return;
  } 
  FcstThreshInfo info(gen, lead, fieldName, threshold);
  pInfo.push_back(info);
  sort(pInfo.begin(), pInfo.end());
}

//----------------------------------------------------------------
void FcstThreshMetadata::merge(const FcstThreshMetadata &info)
{
  if (info.pNumTiles != pNumTiles)
  {
    ILOGF(ERROR, "Inconsistent number of tiles %d %d",
	  info.pNumTiles, pNumTiles);
    return;
  } 

  for (size_t i=0; i<info.pInfo.size(); ++i)
  {
    pInfo.push_back(info.pInfo[i]);
  }
  sort(pInfo.begin(), pInfo.end());
}

//----------------------------------------------------------------
std::string FcstThreshMetadata::getXml(void) const
{
  string xml=Xml::writeInt("NumTiles", 0, pNumTiles);
  for (size_t i=0; i<pInfo.size(); ++i)
  {
    xml += pInfo[i].getXml();
  }
  return Xml::writeString(pTag, 0, xml);
}

//----------------------------------------------------------------
void FcstThreshMetadata::erase(const time_t &gt)
{
  vector<FcstThreshInfo>::iterator i;
  for (i=pInfo.begin(); i!=pInfo.end(); )
  {
    if (i->olderThan(gt))
    {
      i = pInfo.erase(i);
    }
    else
    {
      i++;
    }
  }
}

//----------------------------------------------------------------
std::vector<std::pair<std::string,double> > 
FcstThreshMetadata::nameThreshAverage(int tileIndex) const
{
  vector<pair<string,double> > ret;
  vector<string> names;
  for (size_t i=0; i<pInfo.size(); ++i)
  {
    string name = pInfo[i].getName();
    if (find(names.begin(), names.end(), name) == names.end())
    {
      names.push_back(name);
    }
  }

  // for each field, get average threshold
  for (size_t i=0; i<names.size(); ++i)
  {
    string name = names[i];
    double thresh = 0;
    double num = 0;
    for (size_t k=0; k<pInfo.size(); ++k)
    {
      if (pInfo[k].nameMatch(name))
      {
	thresh += pInfo[k].getTileThresh(tileIndex);
	num += 1.0;
      }
    }
    if (num > 0)
    {
      ret.push_back(pair<string,double>(name, thresh/num));
    }
    else
    {
      ILOGF(ERROR, "No matching names to %s for tile %d",
	    name.c_str(), tileIndex);
    }
  }
  return ret;
}

//----------------------------------------------------------------
void FcstThreshMetadata::print(void) const
{
  for (size_t i=0; i<pInfo.size(); ++i)
  {
    pInfo[i].print();
  }
}
