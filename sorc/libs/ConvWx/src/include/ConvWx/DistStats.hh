/**
 * @file DistStats.hh
 * @brief DistStats holds average data distribution information for observation 
 *        and valid forecast data. Time information of data record is also 
 *        stored.  
 * @class DistStats
 * @brief DistStats holds average data distribution information for observation 
 *        and valid forecast data. Time information of data record is also 
 *        stored. 
 */
#ifndef DISTSTATS_HH
#define DISTSTATS_HH

#include <string>
#include <vector>

class DistStats
 {
  
public:

   /**
    * Constructor
    * @param[in] genTime  Generation time of data record
    * @param[in] leadSecs  Lead time in seconds of data record
    * @param[in] subgridNum  Sub-grid of forecast domain of data record
    * @param[in] obsHistTotals  Histogram bin totals of observation data
    * @param[in] fcstHistTotals Histogram bin totals of forecast data
    * @param[in] nObsHists  Number of histograms in average recorded in
    *                       obsHistTotals
    * @param[in] nFcstHist  Number of histograms in average recorded in
    *                       fcstHistTotals
    */
   DistStats(const time_t genTime, const int leadSecs, 
	     const int subgridNum, const std::vector <int> obsHistTotals, 
	     const std::vector <int> fcstHistTotals, const int nObsHists = 0, 
	     const int nFcstHist = 0);
   /**
    * Constructor
    * @param[in] genTime  Generation time of data record
    * @param[in] leadSecs  Lead time in seconds of data record
    * @param[in] subgridNum  Sub-grid of forecast domain of data record
    * @param[in] obsHistStats  Histogram bin statistics of observation data
    * @param[in] fcstHistStats Histogram bin statistics of forecast data
    * @param[in] nObsHists  Number of histograms in average recorded in
    *                       obsHistTotals
    * @param[in] nFcstHist  Number of histograms in average recorded in
    *                       fcstHistTotals
    */
   DistStats(const time_t genTime, const int leadSecs, 
	     const int subgridNum, const std::vector <float> obsHistStats, 
	     const std::vector <float> fcstHistStats, const int nObsHists = 0, 
	     const int nFcstHist = 0);

   /**
    *  Destructor
    */
   virtual ~DistStats(void);
   

   /**
    *  @return Sub-grid number of data record
    */
   inline int getSubgridNum(void) const { return pSubgridNum;}
   
   /**
    *  @return Generation time of data record
    */ 
   inline time_t getGenTime(void) const {return pGenTime;}

   /**
    *  @return Year of data record 
    */ 
   inline int getYear(void) const {return pYear;}
   
   /**
    *  @return Month of data record in [1,12]
    */ 
   inline int getMonth(void) const {return pMonth;}

   /**
    *  @return Day of data record in [1,31]
    */ 
   inline int getDay(void) const {return pDay;}

   /**
    *  @return  Generation time hour of day
    */ 
   inline int getGenHour(void) const {return pGenHour;}

   /**
    *  @return Generation time minute of hour
    */ 
   inline int getGenMin(void) const {return pGenMin;}

   /**
    *  @return Lead time of data record in seconds
    */ 
   inline int getLeadSecs(void) const {return pLeadSecs;}

   /**
    *  @return Vector containing  histogram bin totals
    */ 
   inline const std::vector < int > getObsHist(void) const
   {
     return pObsHistTotals;
   }

   /**
    *  @return Vector containing  histogram bin totals
    */ 
   inline const std::vector < int > getFcstHist(void) const
   {
     return pFcstHistTotals;
   }

   /**
    *  @return Vector containing  histogram bin stats
    */ 
   inline const std::vector < float > getObsHistStats(void) const
   {
     return pObsHistStats;
   }

   /**
    *  @return Vector containing  histogram bin stats 
    */ 
   inline const std::vector < float > getFcstHistStats(void) const
   {
     return pFcstHistStats;
   }

   /**
    *  Perform a weighted average of forecast and observation histogram
    *  members with the histograms members of inStats. For each bin, weight the
    *  inStats histogram bin total values by "inStatsWeight" and the private
    *  member histogram bin total values by (1-inStatsWeight).
    *  Replace the histogram bin totals with this weighted average.  
    *  @param[in] inStats  Pointer to DistStats object
    *  @param[in] inStatsWeight  Weight assigned to input histogram bin totals
    *                            (1-inStatsWeight) will be applied to private 
    *                             members.
    *  @return True if weighted average was performed and False if it could
    *          not be performed.
    */
   bool weightedAve(DistStats *inStats, float inStatsWeight);

   /**
    *  @return Number of histograms averaged and represented in  pObsHistStats
    */ 
   inline int getNObsHists(void) const {return pNObsHists;}

   /**
    *  @return Number of histograms averaged and represented in  pFcstHistStats
    */ 
   inline int getNFcstHists(void) const {return pNFcstHists;}

   
   /**
    * Set the sub-grid number for data record
    * @param[in] num  Sub-Grid integer id
    */
   inline void setSubgridNum( int num) { pSubgridNum = num;}
   
   /**
    * Set the lead seconds for data record
    * @param[in] secs  Seconds representing forecast lead time
    */
   inline void setLeadSecs(int secs) { pLeadSecs = secs; }

   /**
    * Set the generation hour of data record
    * @param[in] hour  Integer in [0,23] representing generation hour of day
    */
   inline void setGenHour (int hour) { pGenHour = hour; }

   /**
    * Set the generation time of data record
    * @param[in] gtime  Unix time of data record.
    */
   inline void setGenTime (time_t gtime) { pGenTime = gtime;}
   
   
protected:
  
private:

   /**
    * Year of data record
    */
   int pYear;

   /**
    * Month of data record [1,12]
    */
   int pMonth;

   /**
    *  Day of data record [1,31]
    */   
   int pDay;

   /**
    *  Generation hour of forecast data record
    */
   int pGenHour;

   /**
    * Generation minute of hour of forecast data record
    */
   int pGenMin;
   
   /**
    * Generation seconds of minute of forecast data record
    */
   int pGenSecs;   
   
   /**
    * Unix time of data record
    */
   time_t pGenTime;

   /**
    * Lead seconds of data record
    */
   int pLeadSecs; 
   
   /**
    * Sub-Grid id of data record
    */
   int pSubgridNum;
   

   /**
    * Container to hold integer observation histogram bin totals
    */
   std::vector < int > pObsHistTotals;
   
   /**
    * Container to hold integer forecast histograms bin totals
    */
   std::vector < int > pFcstHistTotals;

   /**
    * Container to hold average bin totals of multiple observation histograms
    */
   std::vector < float > pObsHistStats;
   
   /**
    * Container to hold average bin totals of multiple forecast histograms
    */
   std::vector < float > pFcstHistStats;
   
   /**
    * Number of observation elements in each average of observation  histogram 
    * bin totals
    */
   int pNObsHists;

   /**
    * Number of forecast elements in each average of forecast histogram bin
    * totals
    */
   int pNFcstHists;
 
};

#endif
