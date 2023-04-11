/**
 * @file FieldThresh.cc
 */

#include <Epoch/FieldThresh.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>
#include <cstdio>

const std::string FieldThresh::_tag = "FldThr";

//-------------------------------------------------------------------------
FieldThresh::FieldThresh(const std::string &xml, const std::string &fname) :
  _ok(true),
  _fieldName("Unknown"),
  _thresh(-99.99)
{
  if (TaXml::readString(xml, "F", _fieldName))
  {
    LOG(ERROR) << "No tag F in string";
    _ok = false;
  }
  if (TaXml::readDouble(xml, "T", _thresh))
  {
    LOG(ERROR) << "No tag T in string";
    _ok = false;
  }
  if (_fieldName != fname)
  {
    LOG(ERROR) << "Names don't match " << _fieldName << " " << fname;
    _ok = false;
  }
}

//-------------------------------------------------------------------------
std::string FieldThresh::dataFieldName(int precision) const
{
  return dataFieldName(_fieldName, _thresh, precision);
}

//-------------------------------------------------------------------------
std::string FieldThresh::dataFieldName(const std::string &fieldName,
				       double thresh, int precision)
{
  char buf[1000];
  switch (precision)
  {
  case 0:
    sprintf(buf, "%s%.0lf", fieldName.c_str(), thresh);
    break;
  case 1:
    sprintf(buf, "%s%.1lf", fieldName.c_str(), thresh);
    break;
  case 2:
    sprintf(buf, "%s%.2lf", fieldName.c_str(), thresh);
    break;
  case 3:
    sprintf(buf, "%s%.3lf", fieldName.c_str(), thresh);
    break;
  case 4:
    sprintf(buf, "%s%.4lf", fieldName.c_str(), thresh);
    break;
  case 5:
    sprintf(buf, "%s%.5lf", fieldName.c_str(), thresh);
    break;
  default:
    LOG(ERROR) << "Maximum digits=5, had " << precision;
    sprintf(buf, "ERROR");
    break;
  }
  std::string ret = buf;
  return ret;
}

//-------------------------------------------------------------------------
FieldThresh FieldThresh::fromXml(const std::string &xml)
{
  string fieldName;
  if (TaXml::readString(xml, "F", fieldName))
  {
    LOG(ERROR) << "No tag F in string";
    return FieldThresh();
  }
  FieldThresh f(xml, fieldName);
  return f;
}

//-------------------------------------------------------------------------
std::string FieldThresh::toXml(int indent) const
{
  std::string ret = TaXml::writeStartTag(_tag, indent);
  ret += toXmlContent(indent+1);
  ret += TaXml::writeEndTag(_tag, indent);
  return ret;
}

//-------------------------------------------------------------------------
std::string FieldThresh::toXmlContent(int indent) const
{
  std::string ret = TaXml::writeString("F", indent, _fieldName);
  ret += TaXml::writeDouble("T", indent, _thresh, "%.10lf");
  return ret;
}

//-------------------------------------------------------------------------
std::string FieldThresh::sprint(void) const
{
  char buf[1000];
  sprintf(buf, "%s:%05.2f", _fieldName.c_str(), _thresh);
  string s = buf;
  return s;
}

//-------------------------------------------------------------
void FieldThresh::print(void) const
{
  printf("%s[%lf]\n", _fieldName.c_str(), _thresh);
}

//-------------------------------------------------------------
void FieldThresh::printNoNewline(void) const
{
  printf("%s[%lf] ", _fieldName.c_str(), _thresh);
}

//-------------------------------------------------------------
bool FieldThresh::fieldMatch(const FieldThresh &f, bool warnings) const
{
  if (_fieldName != f._fieldName)
  {
    if (warnings)
    {
      LOG(ERROR) << "Names do not match " << _fieldName << ","
		 << f._fieldName;
    }
    return false;
  }
  return true;
}

//-------------------------------------------------------------
void FieldThresh::
replaceThresh(const std::vector<std::pair<std::string,double> > &nameThresh)
{
  for (size_t i=0; i<nameThresh.size(); ++i)
  {
    if (nameThresh[i].first == _fieldName)
    {
      _thresh = nameThresh[i].second;
    }
  }
}
