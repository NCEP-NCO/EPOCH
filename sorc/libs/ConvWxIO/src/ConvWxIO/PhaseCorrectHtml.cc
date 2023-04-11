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
 * @file PhaseCorrectHtml.cc
 */

//----------------------------------------------------------------
#include <string>
#include <vector>
#include <ConvWxIO/PhaseCorrectHtml.hh>
#include <ConvWx/UvHtml.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::pair;
using std::vector;

/**
 * Termination string
 */
static const string sTerminate = "Program terminated at this real time";

/**
 * Restart string
 */
static const string sRestart = "Restarted at this real time";

/**
 * No output because of no model data string
 */
static const string sNoOutNoModel = "No P.C. output, no model data for time";

/**
 * No output because of no phase correction input string
 */
static const string sNoOutNoPc = "No P.C. output, no input UV data for time";

//----------------------------------------------------------------
/**
 * @return header for help with two columns
 */
static pair<string,string> sGetHeader(void)
{
  return pair<string,string>("Status", "Meaning");
}

//----------------------------------------------------------------
/**
 * @return all the help entries, two columns each
 */
static vector<pair<string,string> > sGetHelp(void)
{
  pair<string,string> p0(sRestart, "program started");
  pair<string,string> p1(sTerminate, "program stopped");
  pair<string,string> p2(sNoOutNoModel, 
			 "no output because there is no model data to p.c.");
  pair<string,string> p3(sNoOutNoPc, 
			 "no output because there is no phase correction vector data.");

  vector<pair<string,string> > help;
  help.push_back(p0);
  help.push_back(p1);
  help.push_back(p2);
  help.push_back(p3);

  // because one of the log methods logs UvHtml do this
  vector<pair<string,string> > uvh = UvHtml::getHelpDescriptors();
  for (int i=0; i<static_cast<int>(uvh.size()); ++i)
  {
    help.push_back(uvh[i]);
  }
  return help;
}

//----------------------------------------------------------------
PhaseCorrectHtml::PhaseCorrectHtml(const ParmHtml &html) : 
  BlendingHtml(html, "PhaseCorrection", BlendingHtmlLine::PC, sGetHeader(),
	       sGetHelp())
{
  BlendingHtml::setLatencyThresholds(convWx::WARNING_LATENCY_HOURS,
				     convWx::ERROR_LATENCY_HOURS);
}

//----------------------------------------------------------------
PhaseCorrectHtml::~PhaseCorrectHtml()
{
}

//----------------------------------------------------------------
void PhaseCorrectHtml::logRed(const time_t &t, const NoPc_t &type) const
{
  string s = pMsg(type);
  BlendingHtml::log(t, s, "red");
}

//----------------------------------------------------------------
void PhaseCorrectHtml::log(const time_t &t, const NoPc_t &type) const
{
  string s = pMsg(type);
  BlendingHtml::log(t, s);
}

//----------------------------------------------------------------
void PhaseCorrectHtml::log(const time_t & t, const string &s,
			   const string &scolor, const FcstWithLatencyState &f,
			   const UvHtml &uv) const
{
  BlendingHtml::log(t, s, scolor, f, uv);
}

//----------------------------------------------------------------
void PhaseCorrectHtml::log(const time_t & t, const string &s,
			   const string &scolor, const LpcState &state,
			   const UvHtml &uv, const bool isRestart) const
{
  BlendingHtml::log(t, s, scolor, state, uv, isRestart);
}

//----------------------------------------------------------------
string PhaseCorrectHtml::pMsg(const NoPc_t &type) const
{
  string s;
  switch (type)
  {
  case PhaseCorrectHtml::TERMINATE:
    s = sTerminate;
    break;
  case PhaseCorrectHtml::RESTART:
    s = sRestart;
    break;
  case PhaseCorrectHtml::NO_PC_NO_MODEL:
    s = sNoOutNoModel;
    break;
  case PhaseCorrectHtml::NO_PC_NO_INPUT_PC:
    s = sNoOutNoPc;
    break;
  default:
    s = "Unknown";
    break;
  }
  return s;
}
