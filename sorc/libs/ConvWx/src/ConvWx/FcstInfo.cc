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
 * @file FcstInfo.cc
 */

//----------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/FcstInfo.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
FcstInfo::FcstInfo(void) :
  pGentime(convWx::BAD_TIME),
  pLeadtime(0),
  pTimeWritten(convWx::BAD_TIME),
  pPath(),
  pMetadata()
{
}

//----------------------------------------------------------------
FcstInfo::FcstInfo(const time_t &gt, const int lt, const string &path,
		   const MetaData &metadata) :
  pGentime(gt),
  pLeadtime(lt),
  pTimeWritten(convWx::BAD_TIME),
  pPath(path),
  pMetadata(metadata)
{
}

//----------------------------------------------------------------
FcstInfo::FcstInfo(const time_t &gt, const int lt) :
  pGentime(gt),
  pLeadtime(lt),
  pTimeWritten(convWx::BAD_TIME),
  pPath("missing"),
  pMetadata()
{
}

//----------------------------------------------------------------
FcstInfo::~FcstInfo()
{
}

//----------------------------------------------------------------
bool FcstInfo::operator==(const FcstInfo &f) const
{
  // note ignores time written
  return (pGentime == f.pGentime &&
	  pLeadtime == f.pLeadtime &&
	  pPath == f.pPath &&
	  pMetadata == f.pMetadata);
}

//----------------------------------------------------------------
bool FcstInfo::similar(const FcstInfo &f) const
{
  return (pGentime == f.pGentime &&
	  pLeadtime == f.pLeadtime &&
	  pMetadata.similar(f.pMetadata));
}

//----------------------------------------------------------------
string FcstInfo::printDifferences(const FcstInfo &f) const
{
  // note ignores time written

  string ret = "";
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  if (pGentime != f.pGentime)
  {
    sprintf(buf, "FcstInfo Gen times unequal %s %s\n",
	    ConvWxTime::stime(pGentime).c_str(), 
	    ConvWxTime::stime(f.pGentime).c_str());
    ret += buf;
  }
  if (pLeadtime != f.pLeadtime)
  {
    sprintf(buf, "FcstInfo Lead times unequal %d %d\n", pLeadtime, f.pLeadtime);
    ret += buf;
  }
  if (pPath != f.pPath)
  {
    sprintf(buf, "FctInfo Paths unequal %s %s\n", pPath.c_str(),
	    f.pPath.c_str());
    ret += buf;
  }
  ret += pMetadata.printDifferences(f.pMetadata);
  return ret;
}

//----------------------------------------------------------------
string FcstInfo::printMajorDifferences(const FcstInfo &f) const
{
  // note ignores time written

  string ret = "";
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  if (pGentime != f.pGentime)
  {
    sprintf(buf, "FcstInfo Gen times unequal %s %s\n",
	    ConvWxTime::stime(pGentime).c_str(), 
	    ConvWxTime::stime(f.pGentime).c_str());
    ret += buf;
  }
  if (pLeadtime != f.pLeadtime)
  {
    sprintf(buf, "FcstInfo Lead times unequal %d %d\n", pLeadtime, f.pLeadtime);
    ret += buf;
  }
  ret += pMetadata.printMajorDifferences(f.pMetadata);
  return ret;
}

//----------------------------------------------------------------
void FcstInfo::copyOut(FcstInfo &f) const
{
  f.pGentime = pGentime;
  f.pLeadtime = pLeadtime;
  f.pTimeWritten = pTimeWritten;
  f.pPath = pPath;
  f.pMetadata = pMetadata;
}

//----------------------------------------------------------------
void FcstInfo::logInfo(void) const
{
  string m = sprintInfo();
  ILOG(FORCE, m);
}

//----------------------------------------------------------------
string FcstInfo::sprintInfo(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "%s+%d path=%s meta:", ConvWxTime::stime(pGentime).c_str(),
	  pLeadtime, pPath.c_str());
  string m = buf;
  m += pMetadata.sprint();
  return m;
}

//----------------------------------------------------------------
string FcstInfo::sprintInfoDebug(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "%s+%d\npath=%s\n", ConvWxTime::stime(pGentime).c_str(),
	  pLeadtime, pPath.c_str());
  string m = buf;
  m += pMetadata.sprintDebug();
  return m;
}

//----------------------------------------------------------------
string FcstInfo::sprintInfoNoMetadata(void) const
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "%s+%d", ConvWxTime::stime(pGentime).c_str(), pLeadtime);
  string m = buf;
  return m;
}

//----------------------------------------------------------------
string FcstInfo::getPathSubset(void) const
{
  string s = pPath;
  InterfaceLL::griddedDataPathFilter(s, pGentime);
  return s;
}
  
