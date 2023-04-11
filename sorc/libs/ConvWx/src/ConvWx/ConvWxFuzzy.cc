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
 * @file ConvWxFuzzy.cc
 */
//----------------------------------------------------------------
#include <ConvWx/ConvWxFuzzy.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdlib>

using std::vector;
using std::pair;
using std::string;

/**
 * set the bad value
 */
const double ConvWxFuzzy::pBadValue = -999.999;

//----------------------------------------------------------------
/**
 * @return fuzzy result using interpolation
 * @param[in] v  Value to interpolate to
 * @param[in] v0  Lower interpolation value
 * @param[in] f0  Lower interpolation function value
 * @param[in] v1  Upper interpolation value
 * @param[in] f1  Upper interpolation function value
 * @param[in] badv  bad data value to use if can't do this
 */
static double sFuzzy1(const double v, const double v0, const double f0,
		      const double v1, const double f1,
		      const double badv)
{
  // get range of x 
  double ax = fabs(v1-v0);
  if (ax < convWx::EPSILON)
  {
    // no range to speak of.. not good, can't have two x with same y
    ILOGF(ERROR, "Unexpected range of values %lf %lf", v0, v1);
    return badv;
  }

  double slope, yint;

  // do the interpolation
  slope = (f1-f0)/(v1-v0);
  yint = f0 - slope*v0;
  return (slope*v + yint);
}

//----------------------------------------------------------------
ConvWxFuzzy::ConvWxFuzzy() : pMapping()
{
}
  
//----------------------------------------------------------------
ConvWxFuzzy::ConvWxFuzzy(const vector<pair<double,double> > &f) : pMapping(f)
{
  if (pMapping.empty())
  {
    ILOG(WARNING, "Empty fuzzy function");
    return;
  }
  if (pMapping.size() == 1)
  {
    ILOG(WARNING, "Singleton fuzzy function");
    return;
  }

  bool bad = false;

  // see if things are either strictly increasing or strictly decreasing
  if (pMapping[1].first == pMapping[0].first)
  {
    ILOGF(ERROR, "succesive input x values cannot be equal, %lf",
	  pMapping[1].first);
    bad = true;
  }
  bool increasing = pMapping[1].first > pMapping[0].first;

  for (size_t i=2; i<pMapping.size(); ++i)
  {
    if (pMapping[i].first == pMapping[i-1].first)
    {
      ILOGF(ERROR, "succesive input x values cannot be equal, %lf",
	    pMapping[i].first);
      bad = true;
    }
    bool increasingi = pMapping[i] > pMapping[i-1];
    if ((increasingi && !increasing) || (increasing && !increasingi))
    {
      ILOG(ERROR, "X values not strictly increasing or decreasing");
      bad = true;
    }
  }
  if (bad)
  {
    exit(1);
  }
  if (!increasing)
  {
    // reverse the order so the remaining methods will work
    std::vector<std::pair<double,double> > mapping;
    std::vector<std::pair<double,double> >::reverse_iterator i;
    for (i=pMapping.rbegin(); i!=pMapping.rend(); ++i)
    {
      mapping.push_back(*i);
    }
    pMapping = mapping;
  }
}

//----------------------------------------------------------------
ConvWxFuzzy::~ConvWxFuzzy()
{
}

//----------------------------------------------------------------
void ConvWxFuzzy::log(void)
{
  ILOG(FORCE, "fuzzy function:");
  for (int i=0; i<static_cast<int>(pMapping.size()); ++i)
  {
    ILOGF(FORCE, "f[%d]=(%12.7lf,%12.7lf)", i, pMapping[i].first,
	  pMapping[i].second);
  }
}

//----------------------------------------------------------------
double ConvWxFuzzy::fuzzyF(const double v) const
{
  int n = static_cast<int>(pMapping.size());
  if (n == 0)
  {
    // no points
    return pBadValue;
  }

  if (v <= pMapping[0].first)
  {
    // v < smallest x.. return smallest y
    return pMapping[0].second;
  }

  else if (v >= pMapping[n-1].first)
  {
    // v > largest x, return largest y
    return pMapping[n-1].second;
  }
  
  // find slot for input
  for (int i=0; i<n-1; ++i)
  {
    if (v >= pMapping[i].first && v <= pMapping[i+1].first)
    {
      // interpolate here.
      return sFuzzy1(v, pMapping[i].first, pMapping[i].second,
		     pMapping[i+1].first, pMapping[i+1].second, pBadValue);
    }
  }

  // just in case
  return pMapping[n-1].second;
}

//----------------------------------------------------------------
double ConvWxFuzzy::maxX(void) const
{
  double maxv = -99.99;
  for (int i=0; i<static_cast<int>(pMapping.size()); ++i)
  {
    if (i == 0)
    {
      maxv = pMapping[i].first;
    }
    else
    {
      if (pMapping[i].first > maxv)
      {
	maxv = pMapping[i].first;
      }
    }
  }
  return maxv;
}

//----------------------------------------------------------------
double ConvWxFuzzy::minX(void) const
{
  double minv = -99.99;
  for (int i=0; i<static_cast<int>(pMapping.size()); ++i)
  {
    if (i == 0)
    {
      minv = pMapping[i].first;
    }
    else
    {
      if (pMapping[i].first < minv)
      {
	minv = pMapping[i].first;
      }
    }
  }
  return minv;
}

//----------------------------------------------------------------
string ConvWxFuzzy::xmlContent(const string &name) const
{
  string ret0, ret;

  // for each pair
  for (int i=0; i<static_cast<int>(pMapping.size()); ++i)
  {
    // write x value as XML
    string s = Xml::writeDouble("FzX", 0, pMapping[i].first, "%.2lf");

    // remove the \n if there is one.
    string::size_type ind = s.find("\n");
    if (ind != string::npos)
    {
      s = s.substr(0, ind);
    }
    
    // write y value as XML
    s += Xml::writeDouble("FzY", 0, pMapping[i].second, "%.2lf");

    // remove \n if any
    ind = s.find("\n");
    if (ind != string::npos)
    {
      s = s.substr(0, ind);
    }

    // write all of this out as an XML string
    ret0 += Xml::writeString("Fz1", 0, s);
  }

  // we now have an array of XML strings, each with two doubles.
  // write all of that out as the named string
  ret = Xml::writeString(name, 0, ret0);
  return ret;
}

//----------------------------------------------------------------
bool ConvWxFuzzy::readXml(const string &s, const string &name)
{
  // initialize local state
  pMapping.clear();

  // try to get the string that has input name as the tag
  string content;
  if (!Xml::readXmlString(s, name, true, content))
  {
    // didn't find it
    return false;
  }

  // now expect an array of strings
  vector<string> v;
  if (!Xml::readXmlStringArray(content, "Fz1", v))
  {
    // nope
    return false;
  }
  // each string should have two XML doubles
  bool stat = true;
  for (int i=0; i<static_cast<int>(v.size()); ++i)
  {
    double x, y;
    if (!Xml::readXmlDouble(v[i], "FzX", true, x))
    {
      stat = false;
      continue;
    }
    if (!Xml::readXmlDouble(v[i], "FzY", true, y))
    {
      stat = false;
      continue;
    }
    pair<double,double> p(x, y);
    pMapping.push_back(p);
  }
  return stat;
}
