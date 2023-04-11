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
 * @file UvHtml.hh
 * @brief UV info used in writing HTML status (generation,lead,latency, p.c.
 *        type)
 * @class UvHtml
 * @brief UV info used in writing HTML status (generation,lead,latency, p.c.
 *        type)
 *
 * These are typically strings that show up as content in status HTML output
 * pages
 */

# ifndef    UV_HTML_HH
# define    UV_HTML_HH

#include <string>
#include <ConvWx/PcType.hh>

//----------------------------------------------------------------
class UvHtml 
{
public:
  /**
   * Empty constructor, sets pHasHtml to false
   */
  UvHtml(void);

  /**
   * Set member state to inputs
   *
   * @param[in] gt  Generation time
   * @param[in] lt  Lead seconds
   * @param[in] uvLatencyHours  Latency between generation and current time
   *                            (hours)
   * @param[in] pcType  Phase correction type
   */
  UvHtml(const time_t &gt, const int lt, const double uvLatencyHours,
	 const PcType::Pc_t pcType);

  /**
   * Destructor
   */
  virtual ~UvHtml(void);

  /**
   * Change phase correction type to input
   *
   * @param[in] pcType
   */
  inline void changePc(const PcType::Pc_t pcType) {pPcType = pcType;}

  /**
   * @return A string associated with the phase correction type that can be
   * used in HTML status
   */
  std::string getHtmlCorrectionString(void) const;

  /**
   * @return The latency (hours) or BAD_LATENCY_HOURS
   */
  double getHtmlLatencyHours(void) const;

  /**
   * Return string representation of the local state for use in HTML
   * @param[out] gentimeHourMin  Returned generation time (hour minutes string)
   * @param[out] leadHours  Returned lead hours string
   * @param[out] latencyHours Returned latency hours string
   * @param[out] pcType Returned phase correction type string
   */
  void getHtmlInfo(std::string &gentimeHourMin, std::string &leadHours,
		   std::string &latencyHours, std::string &pcType) const;

  /**
   * Get string pairs to put into help HTML pages.
   * @return pairs of strings where first is a phase correction type,
   *         and second is a description of what it means.
   *
   * @note For the pairs of strings, first = pPcType name.
   */
  static
  std::vector<std::pair<std::string,std::string> > getHelpDescriptors(void);

protected:
private:  

  bool pHasHtml;        /**< true if values valid for HTML output */
  time_t pGenTime;      /**< generation time */
  int pLeadSeconds;     /**< lead seconds */
  double pLatencyHours; /**< latency */
  PcType::Pc_t pPcType; /**< phase correction information */
};

# endif 
