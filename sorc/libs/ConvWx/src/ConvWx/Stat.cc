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
 * @file Stat.cc
 * @brief Stat main class
 */
#include <string>
#include <ConvWx/Xml.hh>
#include <ConvWx/Stat.hh>
using std::string;

//----------------------------------------------------------------
Stat::Stat(const double threshold, const double value, const double weight,
	   const bool isPc, const Stat_t statType) :
  pThresh(threshold),
  pValue(value),
  pWeight(weight),
  pIsPc(isPc),
  pType(statType)
{
}

//----------------------------------------------------------------
Stat::~Stat()
{
}

//----------------------------------------------------------------
string Stat::metadata(void) const
{
  string middle=Xml::writeDouble("thresh", 2, pThresh, "%.2lf");
  middle += Xml::writeDouble("score", 2, pValue, "%.2lf");
  middle += Xml::writeDouble("weight", 2, pWeight, "%.2lf");
  middle += Xml::writeBoolean("isPc", 2, pIsPc);
  string ret = Xml::writeString("Score1", 0, middle);
  return ret;
}

//----------------------------------------------------------------
string Stat::typeDescr(const Stat_t statType)
{
  string ret;
  switch (statType)
  {
  case Stat::CSI:
    ret = "CSI";
    break;
  case Stat::CORRELATION:
    ret = "CORRELATION";
    break;
  case Stat::UNKNOWN:
    ret = "UNKNOWN";
    break;
  default:
    ret = "UNKNOWN";
  }
  return ret;
}

