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
 * @file BlendingHtml.cc
 */
#include <algorithm>
#include <list>
#include <vector>
#include <cstring>
#include <cstdio>
#include <ConvWxIO/BlendingHtml.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ParmHtml.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::string;
using std::vector;
using std::list;

//----------------------------------------------------------------
/**
 * @return the beginning part of the index file for inputs
 * @param[in] name title to put into HTML, and header name
 * @param[in] hoursBack  Number of hours back (goes into header string)
 * @param[in] helpFile  Name of help file that can be referred to
 */
static string sSetIndexHeaderBeg(const string &name, int hoursBack,
				 const string &helpFile)
{
  string s = "<html>\n<head>\n<meta http-equiv=\"Refresh\" content=\"60\">\n";
  s += "<title>";
  s += name;
  s += " Status</title>\n</head>\n<body>\n";
  s += "<!------------------------------------------------------------>\n";
  s += "<h3 align=center>";
  s += name;
  s += " Status</h3>\n<P>\n<P>\n<left>\n";
  s += "<a href=\"";
  s += blendingHtml::currentFname;
  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, "%d", hoursBack);
  s += "\">Current (last ";
  s += buf;
  s += " hours)</a>\n";
  s += "<p>\n";
  s += "<a href=\"";
  s += helpFile;
  s += "\">Help</a>\n<p><p>";
  return s;
}

//----------------------------------------------------------------
/**
 * @return The part of index file after the table
 */
static string sSetIndexHeaderEnd(void)
{
  string s = "</P>\n<p>\n<!--------------END---------->\n</body>\n</html>";
  return s;
}

//----------------------------------------------------------------
/**
 * create and return an empty (no archive) index file
 * @param[in] name title to put into HTML, and header name
 * @param[in] hoursBack  Number of hours back (goes into header string)
 * @param[in] helpFile  Name of help file that can be referred to
 * @return index file content
 */
static string sEmptyIndexFile(const string &name, int hoursBack,
			      const string &helpFile)
{
  string ret = sSetIndexHeaderBeg(name, hoursBack, helpFile);
  ret += "\n";
  ret += blendingHtml::tableKey; // for  a table
  ret += blendingHtml::tableTitle;
  ret += "\n";
  ret += blendingHtml::tableEndKey; // for  a table
  ret += sSetIndexHeaderEnd();
  ret += "\n";
  return ret;
}

//----------------------------------------------------------------
/**
 * @return true if input file does not exist
 * @param[in] fname  Name of file to check
 */
static bool sDoesNotExist(const string &fname)
{
  FILE *fp = fopen(fname.c_str(), "r");
  if (fp == NULL)
  {
    return true;
  }
  else
  {
    fclose(fp);
    return false;
  }
}

//----------------------------------------------------------------
/**
 * Create and return a table HTML entry for inputs.
 * @param[in] y  Year
 * @param[in] m  Month
 * @param[in] d  Day
 * @param[in] dayfile Name of matching day file
 * @return table entry
 */
static string sTableEntry(int y, int m, int d, const string &dayfile)
{
  const string sTableLineStart ="<TR ALIGN=\"LEFT\" VALIGN=\"TOP\">\n<TD>";
  const string sTableLineEnd = "</TD>\n</TR>";

  char entry[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(entry, "%s<a href=\"%s\">%04d/%02d/%02d</a>%s",
	  sTableLineStart.c_str(), dayfile.c_str(), y, m, d,
	  sTableLineEnd.c_str());
  string ret = entry;
  return ret;
}

//----------------------------------------------------------------
BlendingHtml::BlendingHtml(void) :
  pPath(),
  pSubdir(),
  pName(),
  pWorkingPath(),
  pIndexFile(),
  pCurrentFile(),
  pHelpFile(),
  pFormat(BlendingHtmlLine::PC),
  pHoursBack(0),
  pWarningLatencyHours(blendingHtml::badLatencyHours),
  pErrorLatencyHours(blendingHtml::badLatencyHours)
{
}

//----------------------------------------------------------------
BlendingHtml::
BlendingHtml(const ParmHtml &parms, const string &name,
	     BlendingHtmlLine::Format_t format,
	     const std::pair<std::string,std::string> &helpHeader,
	     const std::vector<std::pair<std::string,std::string> > &help) :
  pPath(parms.pPath),
  pSubdir(parms.pSubdir),
  pName(name),
  pWorkingPath(parms.pPath + "/" + parms.pSubdir),
  pIndexFile(pWorkingPath + "/" + blendingHtml::indexFname),
  pCurrentFile(pWorkingPath + "/" + blendingHtml::currentFname),
  pHelpFile(),
  pFormat(format),
  pHoursBack(parms.pHoursBack),
  pWarningLatencyHours(blendingHtml::badLatencyHours),
  pErrorLatencyHours(blendingHtml::badLatencyHours)
{
  if (pPath.empty())
  {
    return;
  }

  // if there is an index file, read that in, else create a blank one
  // create sub directory if not there.
  if (!InterfaceLL::makeDirRecurse(pIndexFile))
  {
    ILOGF(ERROR, "forming path recursively %s", pIndexFile.c_str());
    pPath = "";
    return;
  }
  pHelpFile = BlendingHtmlLine::createHelpFile(blendingHtml::indexFname,
					       pWorkingPath,
					       name, format, helpHeader, help);
  //
  // create an empty index file if there isn't one
  //
  FILE *fp = fopen(pIndexFile.c_str(), "r");
  if (fp == NULL)
  {
    string s =sEmptyIndexFile(pName, pHoursBack,
			      BlendingHtmlLine::helpFileName());
    fp = fopen(pIndexFile.c_str(), "w");
    if (fp == NULL)
    {
      ILOGF(ERROR, "opening %s for write", pIndexFile.c_str());
      pPath = "";
      return;
    }
    fprintf(fp, s.c_str());
    fclose(fp);
  }
  else
  {
    fclose(fp);
  }

}
//----------------------------------------------------------------
BlendingHtml::~BlendingHtml(void)
{
}

//----------------------------------------------------------------
void
BlendingHtml::setLatencyThresholds(double warningLatencyHours,
				   double errorLatencyHours)
{
  pWarningLatencyHours = warningLatencyHours;
  pErrorLatencyHours = errorLatencyHours;
}

//----------------------------------------------------------------
void BlendingHtml::log(const time_t &t, const string &msg) const
{
  if (pPath.empty())
  {
    return;
  }
  BlendingHtmlLine l(t, msg, pFormat);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
void BlendingHtml::log(const time_t &t, const string &msg,
		       const string &color) const
{
  if (pPath.empty())
  {
    return;
  }
  BlendingHtmlLine l(t, msg, color, pFormat);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
// initiation prob, no initiation mask
void BlendingHtml::log(const time_t &t, const FcstState &model,
		       const FcstState &extrap, const time_t &wtime,
		       const FcstState &mweight, const FcstState &eweight, 
		       const time_t &itime, const FcstState &iprob,
		       const string &msg, const string &msgColor,
		       const string &imissing) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::BLEND_MEAN)
  {
    ILOG(ERROR, "logging blend into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, model, extrap, wtime, mweight, eweight, itime, iprob,
		     msg, msgColor, imissing);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
// initiation mask, no initiation prob
void BlendingHtml::log(const time_t &t, const FcstState &model,
		       const FcstState &extrap, const time_t &wtime,
		       const time_t &imaskt,
		       const FcstState &mweight, const FcstState &eweight, 
		       const FcstState &imask,
		       const string &msg, const string &msgColor,
		       const string &imissing) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::BLEND_MEAN)
  {
    ILOG(ERROR, "logging blend into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, imaskt, wtime, model, extrap, mweight, eweight,
		     imask, msg, msgColor, imissing);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
// both initiation mask and prob
void BlendingHtml::log(const time_t &t, const FcstState &model,
		       const FcstState &extrap, const time_t &wtime,
		       const FcstState &mweight, const FcstState &eweight, 
		       const time_t &initProbTime, const time_t &initMaskTime,
		       const FcstState &iprob, const FcstState &imask,
		       const string &msg, const string &msgColor,
		       const string &imissing) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::BLEND_MEAN)
  {
    ILOG(ERROR, "logging blend into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, wtime, initProbTime, initMaskTime, model, extrap,
		     mweight, eweight, iprob, imask, msg, msgColor, imissing);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
//rapidblend
void BlendingHtml::log(const time_t &t, const FcstState &extrap,
		       const time_t &mt,
		       const FcstState &model, 
		       const time_t &wt, const FcstState &eweight,
		       const FcstState &mweight, const time_t &ipt, 
		       const FcstState &iprob, const time_t &imt,
		       const FcstState &imask,
		       const std::string &msg, const std::string &msgColor,
		       const std::string &missing) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::RAPID_BLEND_MEAN)
  {
    ILOG(ERROR, "logging blend into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, extrap, mt, model, wt, eweight, mweight, ipt,
		     iprob, imt, imask, msg, msgColor, missing);
  pStoreNewLine(l);
}


//----------------------------------------------------------------
void BlendingHtml::log(const time_t &t, const FcstState &model,
		       const FcstState &extrap,
		       const time_t & wt, const FcstState &mweight,
		       const FcstState &eweight, 
		       const string &msg,
		       const string &msgColor, const string &imissing
		       ) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::BLEND_MEAN)
  {
    ILOG(ERROR, "logging blend into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, model, extrap, wt, mweight, eweight, 
		     msg, msgColor, imissing);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
void BlendingHtml::log(const time_t &t, const FcstState &model,
		       const FcstState &extrap, const FcstState &mask,
		       const string &msg, const string &msgColor,
		       const string &imissing) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::BLEND_MEDIAN)
  {
    ILOG(ERROR, "logging blend with mask into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, model, extrap, mask, msg, msgColor, imissing);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
// rapid blend median
void BlendingHtml::log(const time_t & t, const FcstState &extrap,
		       const FcstState &mask,
		       const time_t & modelGenTime,
		       const FcstState &model,
		       const time_t & ciFlagGenTime,
		       const FcstState &ciMask, 
		       const string &msg, const string &msgColor,
		       const string &imissing) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::RAPID_BLEND_MEDIAN)
  {
    ILOG(ERROR, "logging blend with mask into non-blend file");
    return;
  }
    
  BlendingHtmlLine l(t, extrap, mask, modelGenTime, model, ciFlagGenTime,
		     ciMask, msg, msgColor, imissing);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
void BlendingHtml::log(const time_t & t, const string &msg,
		       const string &msgColor,
		       const FcstWithLatencyState &model,
		       const UvHtml &uv) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::PC)
  {
    ILOG(ERROR, "logging pc into non-pc file");
    return;
  }
  
  BlendingHtmlLine l(t, msg, msgColor, model, uv, pWarningLatencyHours,
		     pErrorLatencyHours);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
void BlendingHtml::log(const time_t & t, const string &msg,
		       const string &msgColor,
		       const LpcState &phaseCorrect, const UvHtml &uv,
		       bool isRestart) const
{
  if (pPath.empty())
  {
    return;
  }
  if (pFormat != BlendingHtmlLine::PC)
  {
    ILOG(ERROR, "logging pc into non-pc file");
    return;
  }
  
  BlendingHtmlLine l(t, msg, msgColor, phaseCorrect, uv, isRestart,
		     pWarningLatencyHours, pErrorLatencyHours);
  pStoreNewLine(l);
}

//----------------------------------------------------------------
void BlendingHtml::pStoreNewLine(const BlendingHtmlLine &line) const
{
  // update the current file.
  if (sDoesNotExist(pCurrentFile))
  {
    // create a new current file.
    pWriteTable(pName, line, pCurrentFile);
  }
  else
  {
    // read current file, break into pieces, write back out append.
    // remove old stuff..
    vector<BlendingHtmlLine> lines =pUpdateContent(pCurrentFile, line, true);
    pWriteTable(pName, lines, pCurrentFile);
  }    

  // update the day file.
  string dayfile = line.dayfileName(pWorkingPath);
  string dname = line.dayString(pName);
  if (sDoesNotExist(dayfile))
  {
    // create a new day file, put that into index file also
    pWriteTable(dname, line, dayfile);
    pUpdateIndexFile(line);
  }
  else
  {
    // read the day file, break into pieces, write back out appending new line
    // don't remove old stuff...
    vector<BlendingHtmlLine> lines =pUpdateContent(dayfile, line, false);
    pWriteTable(dname, lines, dayfile);
  }
}

//----------------------------------------------------------------
void BlendingHtml::pWriteTable(const string &headerName,
			       const vector<BlendingHtmlLine> &lines,
			       const string &fileName) const
{
  string s = BlendingHtmlLine::head(headerName);
  s += BlendingHtmlLine::getHeaderLine(pFormat);

  vector<BlendingHtmlLine>::const_iterator i;
  for (i=lines.begin(); i!=lines.end(); ++i)
  {
    s += i->getLine();
  }

  s += BlendingHtmlLine::tail();
  FILE *fp = fopen(fileName.c_str(), "w");
  if (fp != NULL)
  {
    fprintf(fp, s.c_str());
    fclose(fp);
  }
}

//----------------------------------------------------------------
void BlendingHtml::pWriteTable(const string &headerName,
			       const BlendingHtmlLine &line,
			       const string &fileName) const
{
  string s = BlendingHtmlLine::head(headerName);
  s += BlendingHtmlLine::getHeaderLine(pFormat);
  s += line.getLine();
  s += BlendingHtmlLine::tail();
  FILE *fp = fopen(fileName.c_str(), "w");
  if ( fp != NULL)
  { 
    fprintf(fp, s.c_str());
    fclose(fp);
  }
}

//----------------------------------------------------------------
vector<BlendingHtmlLine>
BlendingHtml::pUpdateContent(const string &fileName,
			     const BlendingHtmlLine &newLine,
			     const bool removeOld) const
{
  vector<BlendingHtmlLine> ret;
  string cfile;
  if (!InterfaceLL::loadFile(fileName, cfile))
  {
    ILOGF(ERROR, "loading file %s", fileName.c_str());
    return ret;
  }

  // break this file into its lines
  vector<BlendingHtmlLine> lines = BlendingHtmlLine::parse(cfile, pFormat);
  // time of new line = t1, cutoff time = pHoursBack earlier
  time_t t1 = newLine.getRealtime();
  time_t t0 = t1 - pHoursBack*convWx::SECS_PER_HOUR;

  vector<BlendingHtmlLine>::iterator vi;
  for (vi=lines.begin(); vi!= lines.end(); ++vi)
  {
    if (removeOld)
    {
      time_t ti = vi->getRealtime();
      if (ti >= t0)
      {
	ret.push_back(*vi);
      }
    }
    else
    {
      ret.push_back(*vi);
    }
  }
  ret.push_back(newLine);
  return ret;
}

//----------------------------------------------------------------
void BlendingHtml::pUpdateIndexFile(const BlendingHtmlLine &line) const
{
  // read in the index file.
  string ifile;
  if (!InterfaceLL::loadFile(pIndexFile, ifile))
  {
    ILOG(ERROR, "with index file");
    return;
  }

  string dayfile = line.dayfileName();
  int year, month, day;
  line.dayfileYmd(year, month, day);

  string tagbuf, c1, tableHeader;
  bool gotHeader = false;
  vector<string> vv;

  if (!Xml::readXmlStringWithAtt(ifile, "TABLE", true, tagbuf))
  {
    ILOG(ERROR, "finding TABLE key in indx file");
    return;
  }
  if (!Xml::readXmlStringArray(tagbuf, "TR", vv))
  {
    ILOG(ERROR, "reading tag buf array key=TR in index file");
    return;
  }
  
  vector<string> entries;
  for (int i=0; i<static_cast<int>(vv.size()); ++i)
  {
    if (Xml::readXmlString(vv[i], "TD", false, c1))
    {
      entries.push_back(vv[i]);
    }
    else if (Xml::readXmlString(vv[i], "TH", false, c1))
    {
      if (!gotHeader)
      {
	tableHeader = vv[i];
	gotHeader = true;
      }
      else
      {
	ILOGF(WARNING, "got more than one header %s %s",
	      tableHeader.c_str(), vv[i].c_str());
      }
    }
    else
    {
      ILOGF(FORCE, "Unexpected entry in index file %s",
	    vv[i].c_str());
    }
  }

  // build a list
  list<string> ll;
  for (int i=0; i<static_cast<int>(entries.size()); ++i)
  {
    ll.push_back(entries[i]);
  }
    
  string newthing =sTableEntry(year, month, day, dayfile);

  // add in the new thing
  if (find(ll.begin(), ll.end(), newthing) == ll.end())
  {
    ll.push_back(newthing);
  }
  else
  {
    ILOGF(WARNING, "dayfile already present %s", dayfile.c_str());
  }

  // sort this list into ascending order
  ll.sort();

  string out =sSetIndexHeaderBeg(pName, pHoursBack, 
				 BlendingHtmlLine::helpFileName());
  out += "\n";
  out +=blendingHtml::tableKey; // for  a table
  out +=blendingHtml::tableTitle;
  out += "\n";

  list<string>::reverse_iterator i;
  for (i=ll.rbegin(); i!= ll.rend(); ++i)
  {
    out += *i;
    out += "\n";
  }
  out += blendingHtml::tableEndKey; // for  a table
  out += sSetIndexHeaderEnd();
  out += "\n";

  FILE *fp = fopen(pIndexFile.c_str(), "w");
  if (fp != NULL )
  {
    fprintf(fp, out.c_str());
    fclose(fp);
  }
}
