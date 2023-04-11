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
 * @file BlendingHtmlLine.cc
 */
#include <cstring>
#include <cstdio>
#include <vector>
#include <ConvWxIO/BlendingHtmlLine.hh>
#include <ConvWxIO/FcstState.hh>
#include <ConvWxIO/FcstWithLatencyState.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/LpcState.hh>
#include <ConvWx/PcType.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/UvHtml.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::string;
using std::pair;
using std::vector;

//----------------------------------------------------------------
/**
 * @return HTML for a header column with begin/end tags added
 * @param[in] v  Stuff to put between begin/end tags
 */
static string sAddHeaderColumn(const string &v)
{
  string s = "<TH>";
  s += v;
  s += "</TH>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return HTML for a column with begin/end tags added
 * @param[in] v  Stuff to put between begin/end tags
 */
static string sAddColumn(const string &v)
{
  string s = "<TD>";
  s += v;
  s += "</TD>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return HTML for a column with begin/end tags added
 * @param[in] v  Stuff to put between begin/end tags, with first = string,
 *               second = optional color
 */
static string sAddColumn(const pair<string,string> &v)
{
  string s;
  if (!v.second.empty())
  {
    s = "<TD BGCOLOR=\"";
    s += v.second;
    s += "\">";
  }
  else
  {
    s = "<TD>";
  }
  s += v.first;
  s += "</TD>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return msg, unless that is empty, in which case return rep
 * @param[in] msg  The message
 * @param[in] rep  The replacement message
 */
static string sMsgReplace(const string &msg, const string &rep)
{
  if (msg.empty())
  {
    return rep;
  }
  else
  {
    return msg;
  }
}

//----------------------------------------------------------------
/**
 * @return a string that has the header line for P.C. HTML output
 */
static string sHeaderPc(void)
{
  string s = "";
  for (int i=0; i<blendingHtml::pcNumColumn; ++i)
  {
    s += sAddHeaderColumn(blendingHtml::pcHeader[i]);
  }
  s += "</TR>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return a string that has the header line for Model HTML output
 */
static string sHeaderModel(void)
{
  string s = "";
  for (int i=0; i<blendingHtml::modelNumColumn; ++i)
  {
    s += sAddHeaderColumn(blendingHtml::modelHeader[i]);
  }
  s += "</TR>\n";
  return s;
}

//----------------------------------------------------------------

/**
 * @return header line for mean blending HTML output
 */
static string sHeaderBlendMean(void)
{
  string s = ""; 
  for (int i=0; i<blendingHtml::meanBlendingNumColumn; ++i)
  {
    s += sAddHeaderColumn(blendingHtml::meanBlendingHeader[i]);
  }
  s += "</TR>\n";
  return s;
}

//----------------------------------------------------------------

/**
 * @return header line for rapidmean blending HTML output
 */
static string sHeaderRapidBlendMean(void)
{
  string s = ""; 
  for (int i=0; i<blendingHtml::rapidMeanBlendingNumColumn; ++i)
  {
    s += sAddHeaderColumn(blendingHtml::rapidMeanBlendingHeader[i]);
  }
  s += "</TR>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return header line for median blending HTML output
 */
static string sHeaderBlendMedian(void)
{
  string s = ""; 
  for (int i=0; i<blendingHtml::medianBlendingNumColumn; ++i)
  {
    s += sAddHeaderColumn(blendingHtml::medianBlendingHeader[i]);
  }
  s += "</TR>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return header line for median blending HTML output
 */
static string sHeaderRapidBlendMedian(void)
{
  string s = ""; 
  for (int i=0; i<blendingHtml::rapidMedianBlendingNumColumn; ++i)
  {
    s += sAddHeaderColumn(blendingHtml::rapidMedianBlendingHeader[i]);
  }
  s += "</TR>\n";
  return s;
}

//----------------------------------------------------------------
/**
 * @return a string that has HTML for input help content (one line)
 * @param[in] appHelpHeader first is the brief title, second is the longer
 *                          description
 */
static string sHelpApp0(const pair<string,string> &appHelpHeader)
{
  string s = "<p>\n";
  s += blendingHtml::tableLineBegin;
  s += sAddHeaderColumn(appHelpHeader.first);
  s += sAddHeaderColumn(appHelpHeader.second);
  s += blendingHtml::tableLineEnd;
  return s;
}

//----------------------------------------------------------------
/**
 * @return help content that is common to all BlendingHtml HTML types
 * This is the beginning of a longer line typically
 */
static string sHelp0(void)
{
  string s = "<p>\n";
  s += "<TABLE BORDER ALIGN=\"LEFT\" cellspacing=2 cellpadding=3 style=T1>\n";
  s += blendingHtml::tableLineBegin;
  s += sAddHeaderColumn("Field");
  s += sAddHeaderColumn("Meaning");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("R.T.");
  s += sAddColumn("Real time at which the event occured");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("T.T.");
  s += sAddColumn("Data trigger time (hours:minutes) of incoming data");
  s += blendingHtml::tableLineEnd;
  return s;
}


//----------------------------------------------------------------
/**
 * @return string that ends the line begun by sHelp0
 */
static string sHelp1(void)
{
  string s = "</TABLE>\n</P>\n";
  return s; 
}

//----------------------------------------------------------------
/**
 * @return help columns specific to PC HTML output
 */
static string sHelpPc(void)
{
  string s;
  s = blendingHtml::tableLineBegin;
  s += sAddColumn("Status");
  s += sAddColumn("Status of the Phase Correction run for this T.T.");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Gen");
  s += sAddColumn("Gen time (hours:minutes) of the model data that was used");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Latency");
  s += sAddColumn("Hours offset between Model Gen and T.T.");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Missing");
  s += sAddColumn("list of model lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("UV Gen");
  s += sAddColumn("Gen time (hours:minutes) of UV data used in p.c.");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("UV status");
  s += sAddColumn("phase correction status that generated the UV data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Out Gen");
  s += sAddColumn("Gen time (hours:minutes) of the output data");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Out Missing");
  s += sAddColumn("list of output lead hours that are missing");
  s += blendingHtml::tableLineEnd;
  return s;
}

//----------------------------------------------------------------
/**
 * @return help columns specific to Model HTML output
 */
static string sHelpModel(void)
{
  string s = "";
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Gen");
  s += sAddColumn("Gen time (hours:minutes) of a model run");
  s += blendingHtml::tableLineEnd;
  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Missing");
  s += sAddColumn("list of model lead hours that are missing for that Gen");
  s += blendingHtml::tableLineEnd;
  return s;
}

//----------------------------------------------------------------
/**
 * @return help columns specific to Mean HTML output
 */
static string sHelpBlendMean(void)
{
  string s = ""; 
  s = blendingHtml::tableLineBegin;
  s += sAddColumn("Status");
  s += sAddColumn("Status of the Bending for this T.T.");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the output data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Missing");
  s += sAddColumn("list of input model lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Extrap Missing");
  s += sAddColumn("list of input extrapolation lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Weight G.T.");
  s += sAddColumn("Gen time of the extrapolation/model weights input data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Weight Missing");
  s += sAddColumn("list of input model weights lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Extrap Weight Missing");
  s += sAddColumn("list of input extrapolation weights lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Probability G.T.");
  s += sAddColumn("Gen time of the initiation probability input data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Prob Missing");
  s += sAddColumn("list of input initiation probability lead hours that are missing. This input is optional");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Probability G.T.");
  s += sAddColumn("Gen time of the initiation probability input data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Mask Missing");
  s += sAddColumn("list of input initiation mask lead hours that are missing. This input is optional");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Out Missing");
  s += sAddColumn("list of output lead hours that are set to all missing");
  s += blendingHtml::tableLineEnd;

  return s;
}

//----------------------------------------------------------------
/**
 * @return help columns specific to Mean HTML output
 */
static string sHelpRapidBlendMean(void)
{
  string s = ""; 
  s = blendingHtml::tableLineBegin;
  s += sAddColumn("Status");
  s += sAddColumn("Status of the Rapid Bending for this T.T.");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the output/extrap data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the input model data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Missing");
  s += sAddColumn("list of input model lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Extrap Missing");
  s += sAddColumn("list of input extrapolation lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Weight G.T.");
  s += sAddColumn("Gen time of the extrapolation/model weights input data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Weight Missing");
  s += sAddColumn("list of input model weights lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Extrap Weight Missing");
  s += sAddColumn("list of input extrapolation weights lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Probability G.T.");
  s += sAddColumn("Gen time of the initiation probability input data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Prob Missing");
  s += sAddColumn("list of input initiation probability lead hours that are missing. This input is optional");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Probability G.T.");
  s += sAddColumn("Gen time of the initiation probability input data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Initiation Mask Missing");
  s += sAddColumn("list of input initiation mask lead hours that are missing. This input is optional");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Out Missing");
  s += sAddColumn("list of output lead hours that are set to all missing");
  s += blendingHtml::tableLineEnd;

  return s;
}

//----------------------------------------------------------------
/**
 * @return help columns specific to Median blending HTML output
 */
static string sHelpBlendMedian(void)
{
  string s = ""; 
  s = blendingHtml::tableLineBegin;
  s += sAddColumn("Status");
  s += sAddColumn("Status of the Bending for this T.T.");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the output data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Missing");
  s += sAddColumn("list of input model lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Extrap Missing");
  s += sAddColumn("list of input extrapolation lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Mask Missing");
  s += sAddColumn("list of input mask data lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Out Missing");
  s += sAddColumn("list of output lead hours that are set to all missing");
  s += blendingHtml::tableLineEnd;

  return s;
}

//----------------------------------------------------------------
/**
x * @return help columns specific to Rapid Median blending HTML output
 */
static string sHelpRapidBlendMedian(void)
{
  string s = ""; 
  s = blendingHtml::tableLineBegin;
  s += sAddColumn("Status");
  s += sAddColumn("Status of the Bending for this T.T.");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the output data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Extrap Missing");
  s += sAddColumn("list of input extrapolation lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Mask Missing");
  s += sAddColumn("list of input mask lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += sAddColumn("Model G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the model data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Model Missing");
  s += sAddColumn("list of input model lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += sAddColumn("CI Mask G.T.");
  s += sAddColumn("Gen time (hours:minutes) of the CI Mask data");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("CI Mask Missing");
  s += sAddColumn("list of input CI Mask lead hours that are missing");
  s += blendingHtml::tableLineEnd;

  s += blendingHtml::tableLineBegin;
  s += sAddColumn("Out Missing");
  s += sAddColumn("list of output lead hours that are set to all missing");
  s += blendingHtml::tableLineEnd;

  return s;
}

//----------------------------------------------------------------
/**
 * @return HTML content for description (help) for an app
 * @param[in] appHelpHeader Header line that has 2 strings, for two columns
 * @param[in] appHelp  Any number of lines with 2 strings each (two columns)
 */
static string sHelpAppContent(const pair<string,string> &appHelpHeader,
			      const vector<pair<string,string> > &appHelp)
{
  string s = sHelpApp0(appHelpHeader);

  for (int i=0; i<static_cast<int>(appHelp.size()); ++i)
  {
    s += blendingHtml::tableLineBegin;
    s += sAddColumn(appHelp[i].first);
    s += sAddColumn(appHelp[i].second);
    s += blendingHtml::tableLineEnd;
  }    
  return s;
}

//----------------------------------------------------------------
/**
 * @return complete help content for inputs
 * @param[in] e  A HTML format
 * @param[in] helpHeader Header line that has 2 strings, for two columns
 * @param[in] help  Any number of lines with 2 strings each (two columns)
 */
static string sHelpContent(BlendingHtmlLine::Format_t e,
			   const pair<string,string> &helpHeader,
			   const vector<pair<string,string> > &help)
{
  string s = sHelp0();
  switch (e)
  {
  case BlendingHtmlLine::PC:
    s += sHelpPc();
    break;
  case BlendingHtmlLine::MODEL:
    s += sHelpModel();
    break;
  case BlendingHtmlLine::BLEND_MEAN:
    s += sHelpBlendMean();
    break;
  case BlendingHtmlLine::BLEND_MEDIAN:
    s += sHelpBlendMedian();
    break;
  case BlendingHtmlLine::RAPID_BLEND_MEAN:
    s += sHelpRapidBlendMean();
    break;
  case BlendingHtmlLine::RAPID_BLEND_MEDIAN:
    s += sHelpRapidBlendMedian();
    break;
  default:
    break;
  }
  s += sHelpAppContent(helpHeader, help);
  if (e == BlendingHtmlLine::PC)
  {
    pair<string,string> hdr = pair<string,string>("UV status", "Meaning");
    vector<pair<string,string> > h = PcType::helpDescriptors();
    s += sHelpAppContent(hdr, h);
  }
  s += sHelp1();
  return s;
}

//----------------------------------------------------------------
/**
 * Parse input string, with an assumed format
 * @param[in] vi  String to parse
 * @param[in] first   If true this is the first line
 * @param[in] e  The format
 * @param[in] ret   Parsed lines are returned
 */
static void sParse1(const string &vi, bool first, BlendingHtmlLine::Format_t e,
		    vector<BlendingHtmlLine> &ret) 
{
  // first line should be an array of "TH", next lines "TD"
  string tag;
  if (first)
  {
    tag = "TH";
  }
  else
  {
    tag = "TD";
  }

  vector<pair<string,string> > pairs;
  if (!Xml::readXmlStringArrayWithAtt(vi, tag, "BGCOLOR", pairs))
  {
    ILOG(ERROR, "reading tag buffer array");
    return;
  }
  if (first)
  {
    return;
  }

  pair<string,string> p0(".", "");
  int n = static_cast<int>(pairs.size());
  for (int i=n; i<BlendingHtmlLine::numColumn(e); ++i)
  {
    pairs.push_back(p0);
  }

  BlendingHtmlLine l(pairs, e);
  if (l.isGood())
  {
    ret.push_back(l);
  }
  else
  {
    ILOGF(WARNING, "line not good '%s'", vi.c_str());
  }
}

//----------------------------------------------------------------
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const string &msg,
				   Format_t format) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(format),
  pGood(true)
{
  // set first two columns 
  pInit(t);

  pAddToContent(msg);
  
  // then the rest are empty messages
  for (int i=3; i<pNumColumn(); ++i)
  {
    pAddToContent(".");
  }
}

//----------------------------------------------------------------
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const string &msg,
				   const string &color, Format_t format) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(format),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, color);
  
  // then the rest are empty messages
  for (int i=3; i<pNumColumn(); ++i)
  {
    pAddToContent(".");
  }
}

//----------------------------------------------------------------
// prob no mask
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const FcstState &model,
				   const FcstState &extrap, 
				   const time_t &wtime,
				   const FcstState &mweight,
				   const FcstState &eweight, 	
				   const time_t &itime,
				   const FcstState &iprob,
				   const string &msg,
				   const string &msgColor,
				   const string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::BLEND_MEAN),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);
  
  string gen, missing;
  model.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  extrap.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  string s = ConvWxTime::stime(wtime);
  pAddToContent(s);
  mweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  eweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = ConvWxTime::stime(itime);
  pAddToContent(s);
  iprob.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = ".";
  pAddToContent(s);
  s = ".";
  pAddToContent(s);
  s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}

//----------------------------------------------------------------
// mask no prob
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const time_t &itime,
				   const time_t &wtime,
				   const FcstState &model,
				   const FcstState &extrap, 
				   const FcstState &mweight,
				   const FcstState &eweight, 	
				   const FcstState &imask,
				   const string &msg,
				   const string &msgColor,
				   const string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::BLEND_MEAN),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);
  
  string gen, missing;
  model.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  extrap.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  string s = ConvWxTime::stime(wtime);
  pAddToContent(s);
  mweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  eweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = ".";
  pAddToContent(s);
  s = ".";
  pAddToContent(s);
  s = ConvWxTime::stime(itime);
  pAddToContent(s);
  imask.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}
//----------------------------------------------------------------
// mask and prob
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const time_t &wtime,
				   const time_t &probtime,
				   const time_t &masktime,
				   const FcstState &model,
				   const FcstState &extrap, 
				   const FcstState &mweight,
				   const FcstState &eweight, 	
				   const FcstState &iprob,
				   const FcstState &imask,
				   const string &msg,
				   const string &msgColor,
				   const string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::BLEND_MEAN),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);
  
  string gen, missing;
  model.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  extrap.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  string s = ConvWxTime::stime(wtime);
  pAddToContent(s);
  mweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  eweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = ConvWxTime::stime(probtime);
  pAddToContent(s);
  iprob.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = ConvWxTime::stime(masktime);
  pAddToContent(s);
  imask.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}

//---------------------------------------------------------------------------
// mask and prob, rapidblend
BlendingHtmlLine::
BlendingHtmlLine(const time_t &t, const FcstState &extrap, const time_t &mt,
		 const FcstState &model, 
		 const time_t &wt, const FcstState &eweight,
		 const FcstState &mweight, const time_t &ipt, 
		 const FcstState &iprob, const time_t &imt,
		 const FcstState &imask,
		 const std::string &msg, const std::string &msgColor,
		 const std::string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::RAPID_BLEND_MEAN),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);
  
  string gen, missing;

  extrap.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);

  string s = ConvWxTime::stime(mt);
  pAddToContent(s);
  model.getLogInfo(gen, missing);
  pAddMissingToContent(missing);

  s = ConvWxTime::stime(wt);
  pAddToContent(s);
  eweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  mweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);

  s = ConvWxTime::stime(ipt);
  pAddToContent(s);
  iprob.getLogInfo(gen, missing);
  pAddMissingToContent(missing);

  s = ConvWxTime::stime(imt);
  pAddToContent(s);
  imask.getLogInfo(gen, missing);
  pAddMissingToContent(missing);

  s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}

//----------------------------------------------------------------
//no mask  no prob
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const FcstState &model,
				   const FcstState &extrap, const time_t &wt,
				   const FcstState &mweight,
				   const FcstState &eweight, const string &msg,
				   const string &msgColor,
				   const string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::BLEND_MEAN),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);
  
  string gen, missing;
  model.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  extrap.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  string s = ConvWxTime::stime(wt);
  pAddToContent(s);
  mweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  eweight.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  s = ".";
  pAddToContent(s);
  pAddToContent(s);
  pAddToContent(s);
  pAddToContent(s);
  s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}

//----------------------------------------------------------------
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const FcstState &model,
				   const FcstState &extrap,
				   const FcstState &mask,
				   const string &msg, const string &msgColor,
				   const string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::BLEND_MEDIAN),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);
  
  string gen, missing;
  model.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  extrap.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  mask.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  string s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}

//----------------------------------------------------------------
// rapid blend median
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const FcstState &extrap,
				   const FcstState &mask,
				   const time_t &modelT,
				   const FcstState &model,
				   const time_t &ciMaskT,
				   const FcstState &ciMask,
				   const string &msg, const string &msgColor,
				   const string &imissing) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::RAPID_BLEND_MEDIAN),
  pGood(true)
{
  string gen, missing;

  pInit(t);
  pAddToContent(msg, msgColor);
  extrap.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  mask.getLogInfo(gen, missing);
  pAddMissingToContent(missing);
  model.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  ciMask.getLogInfo(gen, missing);
  pAddToContent(gen);
  pAddMissingToContent(missing);
  string s = sMsgReplace(imissing, "None");
  pAddMissingToContent(s);
}

//----------------------------------------------------------------
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const string &msg,
				   const string &msgColor,
				   const FcstWithLatencyState &model, 
				   const UvHtml &uv,
				   double warningLatencyHours,
				   double errorLatencyHours) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::PC),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);

  string modelGt, modelLatency, modelMissing, outGt, outMissing;
  string uvGt, uvLead, uvLatency, uvPc;

  model.getLogInfo(modelGt, modelLatency, modelMissing, outGt, outMissing);
  pAddToContent(modelGt);
  pAddLatency(modelLatency, model.getLatencyHours(), warningLatencyHours,
	      errorLatencyHours);

  pAddMissingToContent(modelMissing);

  uv.getHtmlInfo(uvGt, uvLead, uvLatency, uvPc);
  pAddToContentYellowIfNone(uvGt);
  pAddToContentYellowIfNone(uvLead);
  if (uvLatency == "NONE" || uvLatency == "None" || uvLatency == "none")
  {
    pAddToContent(uvLatency, "yellow");
  }
  else
  {
    pAddLatency(uvLatency, uv.getHtmlLatencyHours(), warningLatencyHours,
		errorLatencyHours);
  }
  pAddToContentYellowIfNone(uvPc);
  pAddToContent(outGt);
  pAddMissingToContent(outMissing);
}

//----------------------------------------------------------------
BlendingHtmlLine::BlendingHtmlLine(const time_t &t, const string &msg,
				   const string &msgColor,
				   const LpcState &model, const UvHtml &uv,
				   bool isRestart,
				   double warningLatencyHours,
				   double errorLatencyHours) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(BlendingHtmlLine::PC),
  pGood(true)
{
  pInit(t);

  pAddToContent(msg, msgColor);

  string modelGt, modelLatency, modelMissing, outGt, outMissing;
  string uvGt, uvPc, uvLead, uvLatency;

  model.getLogInfo(isRestart, modelGt, modelLatency, modelMissing,
		   outGt, outMissing);
  pAddToContent(modelGt);
  pAddLatency(modelLatency, model.getLatencyHours(), warningLatencyHours,
	      errorLatencyHours);
  pAddMissingToContent(modelMissing);
  uv.getHtmlInfo(uvGt, uvLead, uvLatency, uvPc);
  pAddToContentYellowIfNone(uvGt);
  pAddToContentYellowIfNone(uvPc);
  pAddToContent(outGt);
  if (isRestart)
  {
    pAddToContent(outMissing);
  }
  else
  {
    pAddMissingToContent(outMissing);
  }
}

//----------------------------------------------------------------
BlendingHtmlLine::BlendingHtmlLine(const vector<pair<string,string> > &v,
				   Format_t format) :
  pContent(),
  pRealtime(convWx::BAD_TIME),
  pTriggerHour(0),
  pTriggerMinute(0),
  pFormat(format),
  pGood(true)
{
  for (int i=0; i<static_cast<int>(v.size()); ++i)
  {
    pAddToContent(v[i].first, v[i].second);
  }
  if (static_cast<int>(v.size()) != numColumn(format))
  {
    pGood = false;
    return;
  }

  // first is time
  pRealtime = ConvWxTime::utime(v[0].first);

  // second is hour/minute
  if (sscanf(v[1].first.c_str(), "%02d:%02d", &pTriggerHour,
	     &pTriggerMinute) != 2)
  {
    pGood = false;
  }
}

//----------------------------------------------------------------
BlendingHtmlLine::~BlendingHtmlLine(void)
{
}

//----------------------------------------------------------------
void BlendingHtmlLine::print(void) const
{
  vector<pair<string,string> >::const_iterator i;
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  buf[0] = 0;
  for (i=pContent.begin(); i!=pContent.end(); ++i)
  {
    if (i->second == "")
    {
      sprintf(buf + strlen(buf), "'%s',", i->first.c_str());
    }
    else
    {
      sprintf(buf + strlen(buf), "'%s[%s]',", i->first.c_str(),
	      i->second.c_str());
    }
  }
  ILOG(FORCE, buf);
}

//----------------------------------------------------------------
string BlendingHtmlLine::getLine(void) const
{
  string s = blendingHtml::tableLineBegin;
  vector<pair<string,string> >::const_iterator i;
  for (i=pContent.begin(); i!=pContent.end(); ++i)
  {
    s += sAddColumn(*i);
  }
  s += blendingHtml::tableLineEnd;
  return s;
}

//----------------------------------------------------------------
string BlendingHtmlLine::dayfileName(const string &path) const
{
  string s = path;
  s += "/";
  s += dayfileName();
  return s;
}

//----------------------------------------------------------------
string BlendingHtmlLine::dayfileName(void) const
{
  string s;
  char buf[convWx::ARRAY_LEN_SHORT];
  int y, m, d, h, min, sec;
  ConvWxTime::expandTime(pRealtime, y, m, d, h, min, sec);
  sprintf(buf, "%04d%02d%02d", y, m, d);
  s = buf;
  s += ".html";
  return s;
}

//----------------------------------------------------------------
void BlendingHtmlLine::dayfileYmd(int &year, int &month, int &day) const
{
  int h, min, sec;
  ConvWxTime::expandTime(pRealtime, year, month, day, h, min, sec);
}

//----------------------------------------------------------------
string BlendingHtmlLine::dayString(const string &name) const
{
  string s = name;
  s += " ";
  s += dayString();
  return s;
}

//----------------------------------------------------------------
string BlendingHtmlLine::dayString(void) const
{
  char buf[convWx::ARRAY_LEN_SHORT];
  int y, m, d, h, min, sec;
  ConvWxTime::expandTime(pRealtime, y, m, d, h, min, sec);
  sprintf(buf, "%04d/%02d/%02d", y, m, d);
  string s = buf;
  return s;
}

//----------------------------------------------------------------
string BlendingHtmlLine::head(const string &name) 
{
  string s = "<html>\n<head>\n<meta http-equiv=\"Refresh\" content=\"60\">\n";
  s += "<title>";
  s += name;
  s += " Status</title>\n</head>\n<body>\n";
  s += "<!------------------------------------------------------------>\n\n";
  s += "<h3 align=center>";
  s += name;
  s += " Status</h3>\n<P>\n";
  s += "<P>\n<left>\n<a href=\"index.html\">Back to main page</a>\n";
  s += "<TABLE BORDER ALIGN=\"LEFT\" cellspacing=2 cellpadding=3 style=T1>\n";
  return s;
}

//----------------------------------------------------------------
string BlendingHtmlLine::tail(void)
{
  string s = "</TABLE>\n</P>\n<p>\n";
  s += "<!------------- END -------------------->\n";
  s += "</body>\n</html>"; 
  return s;
}


//----------------------------------------------------------------
string BlendingHtmlLine::getHeaderLine(BlendingHtmlLine::Format_t format)
{
  string s = blendingHtml::tableLineBegin;

  switch (format)
  {
  case BlendingHtmlLine::PC:
    s += sHeaderPc();
    break;
  case BlendingHtmlLine::MODEL:
    s += sHeaderModel();
    break;
  case BlendingHtmlLine::BLEND_MEAN:
    s += sHeaderBlendMean();
    break;
  case BlendingHtmlLine::BLEND_MEDIAN:
    s += sHeaderBlendMedian();
    break;
  case BlendingHtmlLine::RAPID_BLEND_MEAN:
    s += sHeaderRapidBlendMean();
    break;
  case BlendingHtmlLine::RAPID_BLEND_MEDIAN:
    s += sHeaderRapidBlendMedian();
    break;
  default:
    break;
  }
  s += blendingHtml::tableLineEnd;
  return s;
}

//----------------------------------------------------------------
string BlendingHtmlLine::getDescriptor(Format_t format)
{
  string s;
  switch (format)
  {
  case BlendingHtmlLine::PC:
    s  = "PC";
    break;
  case BlendingHtmlLine::MODEL:
    s = "MODEL";
    break;
  case BlendingHtmlLine::BLEND_MEAN:
    s = "BLEND_MEAN";
    break;
  case BlendingHtmlLine::BLEND_MEDIAN:
    s = "BLEND_MEDIAN";
    break;
  default:
    s = "UNKNOWNN";
    break;
  }
  return s;
}

//----------------------------------------------------------------
int BlendingHtmlLine::numColumn(Format_t format)
{
  int ret;
  // see SetHeader methods above
  switch (format)
  {
  case BlendingHtmlLine::PC:
    ret = blendingHtml::pcNumColumn;
    break;
  case BlendingHtmlLine::MODEL:
    ret = blendingHtml::modelNumColumn;
    break;
  case BlendingHtmlLine::BLEND_MEAN:
    ret = blendingHtml::meanBlendingNumColumn;
    break;
  case BlendingHtmlLine::BLEND_MEDIAN:
    ret = blendingHtml::medianBlendingNumColumn;
    break;
  case BlendingHtmlLine::RAPID_BLEND_MEAN:
    ret = blendingHtml::rapidMeanBlendingNumColumn;
    break;
  case BlendingHtmlLine::RAPID_BLEND_MEDIAN:
    ret = blendingHtml::rapidMedianBlendingNumColumn;
    break;
  default:
    ret = 0;
    break;
  }
  return ret;
}

//----------------------------------------------------------------
string BlendingHtmlLine::helpFileName(void)
{
  return "help.html";
}

//----------------------------------------------------------------
string
BlendingHtmlLine::createHelpFile(const string &indexFile,
				 const string &filePath,
				 const string &name,
				 Format_t format,
				 const pair<string,string> &helpHeader,
				 const vector<pair<string,string> > 
				 &helpContent)
{
  string fname = filePath;
  fname += "/";
  fname += helpFileName();
  FILE *fp = fopen(fname.c_str(), "w");
  if (fp == NULL)
  {
    ILOGF(ERROR, "opening %s", fname.c_str());
    return "";
  }

  string s = "<html><head><title>Help for ";
  s += name;
  s += "</title>\n</head>\n<body>\n<!--------------->\n";
  s += "<h3 align=center>Help for ";
  s += name;
  s += "</h3>\n<P>\n<a href=\"";
  s += indexFile;
  s += "\">Back to index</a>\n<P>\n";
  s += sHelpContent(format, helpHeader, helpContent);
  s += "<p>\n<!-----------END---------->\n</body></html>\n";
  fprintf(fp, s.c_str());
  fclose(fp);
  return fname;
}

//----------------------------------------------------------------
vector<BlendingHtmlLine> BlendingHtmlLine::parse(const string &parseString,
						 Format_t format)
{
  vector<BlendingHtmlLine> ret;

  // parse from <TABLE > to </TABLE> within that arrays of:
  // <TR > to </TR> within that an array of strings
  // first such string should have this: <TH>v</TH>
  // remaining strings should have this <TD>v</TD>
  string tag = "TABLE";
  string s0;
  if (!Xml::readXmlString(parseString, tag, true, s0))
  {
    ILOG(ERROR, "reading xml string");
    return ret;
  }

  vector<string> v;
  tag = "TR";
  if (!Xml::readXmlStringArray(s0, tag, v))
  {
    ILOG(ERROR, "reading xml string array");
    return ret;
  }

  vector<string>::iterator vi;
  for (vi=v.begin(); vi!=v.end(); ++vi)
  {
    string si;
    if (!Xml::readXmlString(*vi, tag, true, si))
    {
      ILOG(ERROR, "reading xml string");
      continue;
    }
    sParse1(si, vi == v.begin(), format, ret);
  }
  return ret;
}

//----------------------------------------------------------------
void BlendingHtmlLine::pInit(const time_t &t)
{
  pContent.clear();

  // set the hour and minute
  int y, m, d, sec;
  ConvWxTime::expandTime(t, y, m, d, pTriggerHour, pTriggerMinute, sec);

  // 0 = the date
  pRealtime = time(0);
  string s = ConvWxTime::stime(pRealtime);
  pAddToContent(s);

  // 1  = the hour minute
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "%02d:%02d", pTriggerHour, pTriggerMinute);
  s = buf;
  pAddToContent(s);
}

//----------------------------------------------------------------
int BlendingHtmlLine::pNumColumn(void) const
{
  return numColumn(pFormat);
}

//----------------------------------------------------------------
void BlendingHtmlLine::pAddLatency(const string &msg,
				   double latencyHours,
				   double warningLatencyHours, 
				   double errorLatencyHours)
{
  if (warningLatencyHours > 0 && errorLatencyHours > 0)
  {
    if (latencyHours > warningLatencyHours && latencyHours <= errorLatencyHours)
    {
      pAddToContent(msg, "yellow");
    }
    else if (latencyHours > errorLatencyHours)
    {
      pAddToContent(msg, "red");
    }
    else
    {
      pAddToContent(msg);
    }
  }
  else
  {
    pAddToContent(msg);
  }
}

//----------------------------------------------------------------
void BlendingHtmlLine::pAddMissingToContent(const string &missingString)
{
  if (missingString == "none" || missingString == "None" || 
      missingString == "NONE")
  {
    pAddToContent(missingString);
  }
  else if (missingString == "all" || missingString == "All" ||
	   missingString == "ALL")
  {
    pAddToContent(missingString, "red");
  }
  else
  {
    pAddToContent(missingString, "yellow");
  }
}

//----------------------------------------------------------------
void BlendingHtmlLine::pAddToContentYellowIfNone(const string &msg)
{
  if (msg == "none" || msg == "None" || msg == "NONE")
  {
    pAddToContent(msg, "yellow");
  }
  else
  {
    pAddToContent(msg);
  }
}  

//----------------------------------------------------------------
void BlendingHtmlLine::pAddToContent(const string &msg) 
{
  pair<string, string> p(msg, "");
  pContent.push_back(p);
}

//----------------------------------------------------------------
void BlendingHtmlLine::pAddToContent(const string &msg, const string &color) 
{
  pair<string, string> p(msg, color);
  pContent.push_back(p);
}

