/**
 * @file DistStats.cc
 * @brief Source for DistStats class
 */


#include <cstdio>
#include <cmath>
#include <ConvWx/DistStats.hh>
#include <ConvWx/ConvWxTime.hh>
using std::vector;
   
DistStats::DistStats(const time_t genTime, const int leadSecs, 
		     const int subgridNum, const vector <int> obsHistTotals, 
		     const vector <int> fcstHistTotals,const int nObsHists,
		     const int nFcstHists)
  : pGenTime(genTime),
    pLeadSecs(leadSecs),
    pSubgridNum(subgridNum),
    pObsHistTotals(obsHistTotals),
    pFcstHistTotals(fcstHistTotals),
    pNObsHists(nObsHists),
    pNFcstHists(nFcstHists)
{
  //
  // Set time variables of data record
  //
  ConvWxTime::expandTime(genTime, pYear, pMonth, pDay, pGenHour, pGenMin, 
			 pGenSecs);
}

DistStats::DistStats(const time_t genTime, const int leadSecs, 
		     const int subgridNum, const vector <float> obsHistStats, 
		     const vector <float> fcstHistStats,const int nObsHists,
		     const int nFcstHists)
  : pGenTime(genTime),
    pLeadSecs(leadSecs),
    pSubgridNum(subgridNum),
    pObsHistStats(obsHistStats),
    pFcstHistStats(fcstHistStats),
    pNObsHists(nObsHists),
    pNFcstHists(nFcstHists)
{
  //
  // Set time variables of data record
  //
  ConvWxTime::expandTime(genTime, pYear, pMonth, pDay, pGenHour, pGenMin, 
			 pGenSecs);
}

DistStats::~DistStats()
{
  //
  // Clear containers of observation and forecast histogram data
  //
  pObsHistTotals.clear();

  pFcstHistTotals.clear();

  pObsHistStats.clear();

  pFcstHistStats.clear();
}
   
bool DistStats::weightedAve(DistStats *inStats, float inStatsWeight)
{

   //
   // Check that inStats genHour, lead secs, and subgrid are a match
   // with private data members
   //
   if ( (inStats->getSubgridNum() != pSubgridNum) ||
        (inStats->getGenHour() != pGenHour)||
        (inStats->getLeadSecs() != pLeadSecs) )
   {
      return false;
   }

   //
   // Get the average histogram data from input DistStats object
   //
   std::vector < int > inputObsAveHist = inStats->getObsHist();

   std::vector < int > inputFcstAveHist = inStats->getFcstHist();

    if ( ( static_cast<int>(inputObsAveHist.size()) != static_cast<int>(pObsHistTotals.size())) ||
        (static_cast<int>(inputFcstAveHist.size()) != static_cast<int>(pFcstHistTotals.size())) )
   {
      //
      // Histograms have different numbers of bins
      //
      return false;
   }

   //
   // Compute the weighted average and replace pObsHistStats and pFcstHistStats
   // with bin by bin weighted average with histogram stats from input DistStats
   //
   for (int i = 0; i < static_cast<int>(pObsHistTotals.size()); i++)
   {
      pObsHistTotals[i] =  static_cast<int>( round( (inputObsAveHist[i] * inStatsWeight) + (pObsHistTotals[i]* (1 - inStatsWeight)) ) );
      
      pFcstHistTotals[i] = static_cast<int>(round( (inputFcstAveHist[i] * inStatsWeight)  + (pFcstHistTotals[i]* (1 - inStatsWeight)) ) );
   }
   return true;
}
