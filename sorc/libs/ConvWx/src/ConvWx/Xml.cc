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
 * @file Xml.cc
 */

//----------------------------------------------------------------
#include <cstdio>
#include <string>
#include <vector>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::vector;
using std::pair;

// set the default indent spaces per level

/**
 * Chars to indent per 'level'
 */
static const int sIndentPerLevel = 2;

//----------------------------------------------------------------
Xml::Xml()
{
}
  
//----------------------------------------------------------------
Xml::~Xml()
{
}


//------------------------------------------------------------------
bool
Xml::readXmlStringArrayWithAtt(const string &buf, const string &tag,
			       const string &attribute,
			       vector<pair<string,string> > &pairs)
{
  vector<string> elem;
  if (!Xml::readTagBufArray(buf, tag, true, elem))
  {
    ILOG(ERROR, "reading tagbuf array");
    return false;
  }

  // convert each elem based on content
  pairs.clear();
  bool printedBad = false;
  for (int i=0; i<static_cast<int>(elem.size()); ++i)
  {
    string s;
    vector<Xml::Attribute> a;
    if (!Xml::readString(elem[i], tag, true, s, a))
    {
      ILOGF(ERROR, "reading %s", elem[i].c_str());
    }
    else
    {
      if (s.empty() ||
	  ((!s.empty()) &&  s.find_first_not_of(" \n\t") == string::npos))
      {
	ILOGF(SEVERE, "%dth element= '%s' return='%s'", i,
	      elem[i].c_str(), s.c_str());
	if (printedBad == false)
	{
	  printedBad = true;
	  ILOG(SEVERE, "WHERE I DONT WANT TO BE");
	  ILOG(SEVERE, "The entire input looks like this:");
	  ILOG(FORCE, buf);
	}
      }
      if (a.empty())
      {
	pair<string,string> p(s, "");
	pairs.push_back(p);
      }
      else
      {
	string s2;
	if (!Xml::readStringAttr(a, attribute, true, s2))
	{
	  ILOGF(WARNING, "attributes but not %s as expected '%s'",
		attribute.c_str(), elem[i].c_str());
	  pair<string,string> p(s, "");
	  pairs.push_back(p);
	}
	else
	{
	  pair<string,string> p(s, s2);
	  pairs.push_back(p);
	}
      }
    }
  }
  return true;
}

//------------------------------------------------------------------
bool Xml::readXmlStringWithAtt(const string &buf, const string &tag,
			       const bool doPrintErrorMsg, string &retStr)
{
  size_t searchEnd;
  if (!Xml::readTagBuf(buf, tag, doPrintErrorMsg, retStr, 0, &searchEnd))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "reading tag=%s", tag.c_str());
    }
    return false;
  }
  else
  {
    return true;
  }
}

//------------------------------------------------------------------
bool Xml::readXmlString(const string &buf, const string &tag,
			const bool doPrintErrorMsg, string &retStr)
{
  if (!Xml::readString(buf, tag, doPrintErrorMsg, retStr))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "reading tag=%s", tag.c_str());
    }
    return false;
  }
  else
  {
    return true;
  }
}

//------------------------------------------------------------------
bool Xml::readXmlStringArray(const string &buf, const string &tag,
			     vector<string> &retStrs)
{
  if (!Xml::readTagBufArray(buf, tag, true, retStrs))
  {
    ILOGF(ERROR, "reading tagbuf=%s", tag.c_str());
    return false;
  }
  else
  {
    return true;
  }
}

//------------------------------------------------------------------
bool Xml::readXmlStringArrayNoWarnings(const string &buf, const string &tag,
				       vector<string> &retStrs)
{
  return Xml::readTagBufArray(buf, tag, false, retStrs);
}

//------------------------------------------------------------------
bool Xml::readXmlInt(const string &buf, const string &tag,
		     const bool doPrintErrorMsg, int &intV)
{
  if (!Xml::readInt(buf, tag, doPrintErrorMsg, intV))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "reading tag=%s", tag.c_str());
    }
    intV = 0;
    return false;
  }
  else
  {
    return true;
  }
}

//------------------------------------------------------------------
bool Xml::readXmlDouble(const string &buf, const string &tag,
			const bool doPrintErrorMsg, double &doubleV)
{
  if (!Xml::readDouble(buf, tag, doPrintErrorMsg, doubleV))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "reading tag=%s", tag.c_str());
    }
    doubleV = 0.0;
    return false;
  }
  else
  {
    return true;
  }
}

//------------------------------------------------------------------
bool Xml::readXmlBoolean(const string &buf, const string &tag,
			 const bool doPrintErrorMsg, bool &boolV)
{
  if (!Xml::readBoolean(buf, tag, doPrintErrorMsg, boolV))
  {
    if (doPrintErrorMsg) 
    {
      ILOGF(ERROR, "reading tag=%s", tag.c_str());
    }
    boolV = false;
    return false;
  }
  else
  {
    return true;
  }
}

//----------------------------------------------------------------
bool Xml::readTagBufArray(const string &xmlBuf, const string &tag,
			  const bool doPrintErrorMsg,
			  vector<string> &tagBufArray)
{
  tagBufArray.clear();

  size_t searchStart = 0;
  size_t searchEnd = 0;

  string tagBuf;
  while (readTagBuf(xmlBuf, tag, false, tagBuf, searchStart, &searchEnd))
  {
    tagBufArray.push_back(tagBuf);
    searchStart = searchEnd;
  }
  if (tagBufArray.empty())
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "reading tag=%s", tag.c_str());
    }
    return false;
  }
  else
  {
    return true;
  }
}

//----------------------------------------------------------------
bool Xml::readTagBuf(const string &xmlBuf, const string &tag,
		     const bool doPrintErrorMsg, string &tagBuf,
		     size_t searchStart, size_t *searchEnd)
{

  size_t startPos, endPos;
  if (!pFindTagLimits(xmlBuf, tag, searchStart, startPos, endPos))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "finding tag limits for tag=%s", tag.c_str());
    }
    return false;
  }

  // set buffer
  
  tagBuf = xmlBuf.substr(startPos, endPos - startPos);

  if (searchEnd != NULL)
  {
    *searchEnd = endPos;
  }

  return true;
}

//----------------------------------------------------------------
bool Xml::readString(const string &xmlBuf, const string &tag,
		     const bool doPrintErrorMsg, string &val)
{
  
  // compute start and end tokens
  string startTok = "<" + tag + ">";
  string startTokOpen = "<" + tag + " ";
  string endTok = "</" + tag + ">";
  
  // find start token, position startPos immediately after
  // the token
  
  // first look for simple tag
  size_t startPos = xmlBuf.find(startTok, 0);
  if (startPos == string::npos)
  {
    // now look for tag with attributes
    startPos = xmlBuf.find(startTokOpen, 0);
    if (startPos == string::npos)
    {
      if (doPrintErrorMsg)
      {
	ILOGF(ERROR, "finding tag %s", tag.c_str());
      }
      return false;
    }
    startPos = xmlBuf.find(">", startPos);
    if (startPos == string::npos)
    { 
      return false;
    }
    else
    {
      startPos++;
    }
  }
  else
  {
    startPos += startTok.size();
  }
  
  // find end position
  
  size_t endPos = xmlBuf.find(endTok, startPos);
  if (endPos == string::npos)
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "finding endtag %s", tag.c_str());
    }
    return false;
  }
  
  val = xmlBuf.substr(startPos, endPos - startPos);
  return true;
}

//----------------------------------------------------------------
bool Xml::readString(const string &xmlBuf, const string &tag,
		     const bool doPrintErrorMsg, string &val,
		     vector<Attribute> &attributes)
{

  // compute start and end tokens
  
  string startTok = "<" + tag + ">";
  string startTokOpen = "<" + tag + " ";
  string endTok = "</" + tag + ">";
  bool hasVal;

  // find start token, position startPos immediately after
  // the token
  
  // first look for simple tag
  string attrStr;
  size_t startPos = xmlBuf.find(startTok, 0);
  if (startPos == string::npos)
  {
    size_t closePos = 0;

    // now look for tag with attributes
    startPos = xmlBuf.find(startTokOpen, 0);
    if (startPos == string::npos)
    {
      if (doPrintErrorMsg)
      {
	ILOGF(ERROR, "finding tag %s", tag.c_str());
      }
      return false;
    }

    closePos = xmlBuf.find(">", startPos);
    if (closePos == string::npos)
    {
      if (doPrintErrorMsg)
      {
	ILOGF(ERROR, "finding endtag %s", tag.c_str());
      }
      return false;
    }
    else
    {
      size_t attrStart = startPos + startTokOpen.size();
      int attrLen = closePos - attrStart;
      attrStr = xmlBuf.substr(attrStart, attrLen);
      startPos = closePos + 1;
    }

    // check for <  /> style (no value, only attributes)
    hasVal = (xmlBuf[closePos - 1] != '/');

  }
  else
  {
    startPos += startTok.size();
    hasVal = true;
  }

  if (!hasVal)
  {
    // no value, only attributes
    val = "";
  }
  else
  {
    // find end position
    
    size_t endPos = xmlBuf.find(endTok, startPos);
    if (endPos == string::npos)
    {
      if (doPrintErrorMsg)
      {
	ILOGF(ERROR, "finding endtag %s", endTok.c_str());
      }
      return false;
    }
    
    // set string
    
    val = xmlBuf.substr(startPos, endPos - startPos);

  }
  
  // decode attributes into vector
  
  pAttrDecode(attrStr, attributes);

  return true;
}

//----------------------------------------------------------------
bool Xml::readInt(const string &xmlBuf, const string &tag,
		  const bool doPrintErrorMsg, int &val)
{
  string valStr;
  if (!readString(xmlBuf, tag, doPrintErrorMsg, valStr))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "finding tag %s", tag.c_str());
    }
    return false;
  }
  if (!pReadInt(valStr, val))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "interpreting %s as int", valStr.c_str());
    }
    return false;
  }
  return true;
}

//----------------------------------------------------------------
bool Xml::readDouble(const string &xmlBuf, const string &tag,
		     const bool doPrintErrorMsg, double &val)
{
  string valStr;
  if (!readString(xmlBuf, tag, doPrintErrorMsg, valStr))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "finding tag %s", tag.c_str());
    }
    return false;
  }
  if (!pReadDouble(valStr, val))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "interpreting %s as double", valStr.c_str());
    }
    return false;
  }
  return true;
}

//----------------------------------------------------------------
bool Xml::readBoolean(const string &xmlBuf, const string &tag,
		      const bool doPrintErrorMsg, bool &val)
{
  string valStr;
  if (!readString(xmlBuf, tag, doPrintErrorMsg, valStr))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "finding tag %s", tag.c_str());
    }
    return false;
  }
  if (!pReadBoolean(valStr, val))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "interpreting %s as bool", valStr.c_str());
    }
    return false;
  }
  return true;
}

//----------------------------------------------------------------
bool Xml::readTime(const string &xmlBuf, const string &tag,
		   const bool doPrintErrorMsg, time_t &val)
{
  string valStr;
  if (!readString(xmlBuf, tag, doPrintErrorMsg, valStr))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "finding tag %s", tag.c_str());
    }
    return false;
  }
  if (!readTime(valStr, val))
  {
    if (doPrintErrorMsg)
    {
      ILOGF(ERROR, "interpreting %s as time", valStr.c_str());
    }
    return false;
  }
  return true;
}


//----------------------------------------------------------------
bool Xml::readTime(const string &valStr, time_t &val)
{
  int year, month, day, hour, min, sec;
  char c1, c2, c3, c4, c5;
  if (sscanf(valStr.c_str(), "%4d%1c%2d%1c%2d%1c%2d%1c%2d%1c%2d",
             &year, &c1, &month, &c2, &day, &c3,
             &hour, &c4, &min, &c5, &sec) == 11) {
    val = ConvWxTime::setTime(year, month, day, hour, min, sec);
    return true;
  }
  ILOGF(ERROR, "Cannot decode string as time: %s", valStr.c_str());
  return false;
}

//----------------------------------------------------------------
bool Xml::readStringAttr(const vector<Attribute> &attributes,
			 const string &name, const bool doPrintErrorMsg, 
			 string &val)
{

  for (int ii = 0; ii < static_cast<int>(attributes.size()); ii++)
  {
    if (name == attributes[ii].getName())
    {
      val = attributes[ii].getVal();
      return true;
    }
  }

  if (doPrintErrorMsg)
  {
    ILOGF(WARNING, "didn't find attribute %s", name.c_str());
  }
  return false;
}

//----------------------------------------------------------------
string Xml::writeString(const string &tag, int level, const string &val)
{

  string str;
  for (int ii = 0; ii < level * sIndentPerLevel; ii++)
  {
    str += " ";
  }
  str += "<";
  str += tag;
  str += ">";
  str += val;
  str += "</";
  str += tag;
  str += ">\n";
  return str;
}

//----------------------------------------------------------------
string Xml::writeBoolean(const string &tag, int level, bool val)
{
  string str;
  for (int ii = 0; ii < level * sIndentPerLevel; ii++)
  {
    str += " ";
  }
  str += "<";
  str += tag;
  str += ">";
  str += pWriteBoolean(val);
  str += "</";
  str += tag;
  str += ">\n";
  return str;
}
  
//----------------------------------------------------------------
string Xml::writeInt(const string &tag, int level, int val,
		     const char *format)
{
  string str;
  for (int ii = 0; ii < level * sIndentPerLevel; ii++)
  {
    str += " ";
  }
  str += "<";
  str += tag;
  str += ">";
  str += pWriteInt(val, format);
  str += "</";
  str += tag;
  str += ">\n";
  return str;
}
  
//----------------------------------------------------------------
string Xml::writeDouble(const string &tag, int level, double val,
			const char *format)
{
  string str;
  for (int ii = 0; ii < level * sIndentPerLevel; ii++)
  {
    str += " ";
  }
  str += "<";
  str += tag;
  str += ">";
  str += pWriteDouble(val, format);
  str += "</";
  str += tag;
  str += ">\n";
  return str;
}
  
//----------------------------------------------------------------
string Xml::writeTime(const string &tag, int level, time_t val)
{
  
  string str;
  for (int ii = 0; ii < level * sIndentPerLevel; ii++)
  {
    str += " ";
  }
  str += "<";
  str += tag;
  str += ">";
  str += pWriteTime(val);
  str += "</";
  str += tag;
  str += ">\n";
  return str;

}

//----------------------------------------------------------------
///////////////////////////////////
// Find tag limits
// Search starts at searchStart.
// StartPos is set to beginning of start tag.
// Endpos is set 1 beyond the end of the closing tag
//

bool Xml::pFindTagLimits(const string &xmlBuf,
                         const string &tag,
                         size_t searchStart,
                         size_t &startPos,
                         size_t &endPos)
  
{
  
  // compute start and end tokens
  
  string startTok = "<" + tag;
  string endTok = "</" + tag + ">";

  // find start tag
  bool startFound = false;
  while (!startFound)
  {
    size_t openPos = xmlBuf.find(startTok, searchStart);
    if (openPos == string::npos)
    {
      return false;
    }
    size_t nextPos = openPos + startTok.size();
    if (xmlBuf[nextPos] == '>' || xmlBuf[nextPos] == ' ')
    {
      startPos = openPos;
      startFound = true;
    }
  }
  
  // get close of start tag
  
  size_t startClosePos = xmlBuf.find(">", startPos);
  if (startClosePos == string::npos)
  {
    return false;
  }

  // position past the '>'

  startClosePos++;

  // set endPos
  
  // check for <  /> style
  
  if (xmlBuf[startClosePos - 2] == '/')
  {
    // no value, only attributes
    endPos = startClosePos;
  }
  else
  {
    // find end position
    endPos = xmlBuf.find(endTok, startClosePos);
    if (endPos == string::npos)
    {
      return false;
    }

    // position past the close tag
    endPos += endTok.size();
  }
  return true;
}

//----------------------------------------------------------------
//////////////////////////////////////////////////////////
// decode attributes from attribute string into a vector

void Xml::pAttrDecode(const string &attrStr, vector<Attribute> &attributes)

{
  size_t nameStart = 0;
  while (nameStart < attrStr.size() && nameStart != string::npos)
  {
    // move past white space before name
    char next = attrStr[nameStart];
    if (isspace(next))
    {
      nameStart++;
      continue;
    }

    // find '='
    size_t equalsPos = attrStr.find('=', nameStart);
    if (equalsPos == string::npos)
    {
      return;
    }

    // back up past any white space
    size_t nameEnd = equalsPos - 1;
    for (size_t pos = nameEnd; pos > nameStart; pos--)
    {
      if (!isspace(attrStr[nameEnd]))
      {
        nameEnd = pos + 1;
        break;
      }
    }

    // set name
    int nameLen = nameEnd - nameStart;
    if (nameLen < 1)
    {
      return;
    }
    string name = attrStr.substr(nameStart, nameLen);

    // find first quote
    size_t startQuotePos = attrStr.find_first_of("\'\"", equalsPos + 1);
    if (startQuotePos == string::npos)
    {
      return;
    }

    // find next quote
    char quote = attrStr[startQuotePos];
    size_t endQuotePos = attrStr.find(quote, startQuotePos + 1);
    if (endQuotePos == string::npos)
    {
      return;
    }

    // set value

    int valueLen = endQuotePos - startQuotePos - 1;
    if (valueLen < 1)
    {
      return;
    }
    string val = attrStr.substr(startQuotePos + 1, valueLen);

    // add attribute
    attributes.push_back(Attribute(name, val));

    // jump forward
    nameStart = endQuotePos + 1;

  } // while
}

//----------------------------------------------------------------
bool Xml::pReadInt(const string &valStr, int &val)
  
{
  int ival;
  if (sscanf(valStr.c_str(), "%d", &ival) != 1)
  {
    ILOGF(ERROR, "Cannot decode string into int: %s", valStr.c_str());
    return false;
  }
  val = ival;
  return true;
}

//----------------------------------------------------------------
bool Xml::pReadBoolean(const string &valStr, bool &val)
  
{
  if (valStr == "true" || valStr == "TRUE")
  {
    val = true;
  }
  else if (valStr == "false" || valStr == "FALSE")
  {
    val = false;
  }
  else
  {
    ILOGF(ERROR, "Cannot decode string into bool: %s", valStr.c_str());
    return false;
  }
  return true;
}

//----------------------------------------------------------------
// double from a string

bool Xml::pReadDouble(const string &valStr, double &val)
{
  double dval;
  if (sscanf(valStr.c_str(), "%lg", &dval) != 1)
  {
    ILOGF(ERROR, "Cannot decode string into double: %s",
	  valStr.c_str());
    return false;
  }
  val = dval;
  return true;
}

string Xml::pWriteBoolean(bool val)
{
  string str;
  if (val)
  {
    str = "true";
  }
  else
  {
    str = "false";
  }
  return str;
}
  
//----------------------------------------------------------------
string Xml::pWriteInt(int val, const char *format)

{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  if (format == NULL)
  {
    sprintf(buf, "%d", val);
  }
  else
  {
    sprintf(buf, format, val);
  }
  string str = buf;
  return str;
}
  
//----------------------------------------------------------------
string Xml::pWriteDouble(double val, const char *format)

{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  if (format == NULL)
  {
    sprintf(buf, "%g", val);
  }
  else
  {
    sprintf(buf, format, val);
  }
  string str = buf;
  return str;
}
  

//----------------------------------------------------------------
string Xml::pWriteTime(time_t val)
{
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  int y, m, d, h, min, s;
  ConvWxTime::expandTime(val, y, m, d, h, min, s);
  sprintf(buf, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d", y, m, d, h, min, s);
  string str = buf;
  return str;
}

