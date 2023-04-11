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
 * @file Stats.cc
 * @brief Stats main class
 */
#include <vector>
#include <string>
#include <ConvWx/Stats.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/Grid.hh>
using std::vector;
using std::pair;
using std::string;

/**
 * @return CSI score for inputs (summed over entire grid)
 * @param[in] fcst  Prediction
 * @param[in] data  Verification
 * @param[in] thresh  Threshold value
 */
static double sCsi(const Grid &fcst, const Grid &data, const double thresh);

//----------------------------------------------------------------
Stats::Stats(void) :
  pThreshWeight(),
  pType(Stat::UNKNOWN),
  pStat(),
  pPcScore(0.0),
  pScore(0.0),
  pKeepPc(false)
{
}

//----------------------------------------------------------------
Stats::~Stats()
{
}

//----------------------------------------------------------------
void Stats::init(const vector<pair<double, double> > &threshWeightPairs,
		 const Stat::Stat_t type)
{

  pThreshWeight = threshWeightPairs;
  pType = type;
  pStat.clear();
  pPcScore = pScore = 0.0;
  pKeepPc = false;
}

//----------------------------------------------------------------
void Stats::compute(const Grid &noPcGrid, const Grid &yesPcGrid,
		    const Grid &obsGrid)
{
  // clear pStat vector
  pStat.clear();
  pStat.reserve(pThreshWeight.size()*2);

  // build up and store each individual stat into stats vector
  for (int i=0; i<static_cast<int>(pThreshWeight.size()); ++i)
  {
    double f0 = pComputeStat(noPcGrid, obsGrid, pThreshWeight[i].first);
    Stat s(pThreshWeight[i].first, f0, pThreshWeight[i].second, false, pType);
    pStat.push_back(s);

    f0 = pComputeStat(yesPcGrid, obsGrid, pThreshWeight[i].first);
    s = Stat(pThreshWeight[i].first, f0, pThreshWeight[i].second, true, pType);
    pStat.push_back(s);
  }

  // now compute the overall stats from the individual results
  pPcScore = pScore = 0.0;

  for (int i=0; i<static_cast<int>(pStat.size()); ++i)
  {
    if (pStat[i].isPc())
    {
      pPcScore += pStat[i].weightedScore();
    }
    else
    {
      pScore += pStat[i].weightedScore();
    }
  }
  pKeepPc = pPcScore > pScore;
}

//----------------------------------------------------------------
void Stats::clear(void)
{
  pStat.clear();
  pPcScore = pScore = 0.0;
  pKeepPc = false;
}

//----------------------------------------------------------------
string Stats::metadata(void) const
{
  string stype = Stat::typeDescr(pType);
  string ret = Xml::writeString("StatType", 0, stype);
  ret += Xml::writeBoolean("KeepPc", 0, pKeepPc);
  ret += Xml::writeDouble("PcScore", 0, pPcScore, "%.2lf");
  ret += Xml::writeDouble("Score", 0, pScore, "%.2lf");
  for (int i=0; i<static_cast<int>(pStat.size()); ++i)
  {
    ret += pStat[i].metadata();
  }
  return ret;
}

//----------------------------------------------------------------
double Stats::pComputeStat(const Grid &fcstGrid, const Grid &obsGrid,
			   const double thresh)
{
  double ret;
  switch (pType)
  {
  case Stat::CSI:
    ret = sCsi(fcstGrid, obsGrid, thresh);
    break;
  case Stat::CORRELATION:
    ILOG(SEVERE, "Correlation not impelemented");
    ret = 0.0;
    break;
  default:
    ILOGF(SEVERE, " bad stats type %d", pType);
    ret = 0.0;
    break;
  }
  return ret;
}    

//----------------------------------------------------------------
static double sCsi(const Grid &fcstGrid, const Grid &obsGrid,
		   const double thresh)
{
  double fdata, odata;
  bool fyes, oyes;

  double hit = 0;
  double miss = 0;
  double far = 0;
  double none = 0;

  for (int i=0; i<fcstGrid.getNdata(); ++i)
  {
    if (fcstGrid.getValue(i, fdata))
    {
      fyes = fdata > thresh;
    }
    else
    {
      fyes = false;
    }
    if (obsGrid.getValue(i, odata))
    {
      oyes = odata > thresh;
    }
    else
    {
      oyes = false;
    }

    if (oyes && fyes)
    {
      ++hit;
    }
    else if (oyes && !fyes)
    {
      ++miss;
    }
    else if (fyes && !oyes)
    {
      ++far;
    }
    else
    {
      ++none;
    }
  }
  double v = hit + miss + far;
  if (v == 0.0)
  {
    return 0.0;
  }
  else
  {
    return hit/v;
  }
}

