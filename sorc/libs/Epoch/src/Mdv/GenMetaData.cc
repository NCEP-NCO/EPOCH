/**
 * @file GenMetaData.cc
 * @brief Source for GenMetaData class
 */
#include <Epoch/GenMetaData.hh>
#include <ConvWx/FcstInfo.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/ConvWxTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>
using std::vector;
using std::string;

//----------------------------------------------------------------
GenMetaData::GenMetaData(void)
{
}

//----------------------------------------------------------------
GenMetaData::GenMetaData(const std::string &xml)
{
  string content;
  if (!Xml::readXmlString(xml, "genTimes", true, content))
  {
    return;
  }

  vector<string> v;
  if (!Xml::readXmlStringArray(content, "genTime", v))
  {
    LOG(WARNING) << "No Gen times";
  }
  for (size_t i=0; i<v.size(); ++i)
  {
    time_t t;
    if (Xml::readTime(v[i], "genTime", true, t))
    {
      _gen.push_back(t);
    }
  }
}

//----------------------------------------------------------------
GenMetaData::~GenMetaData()
{
}

//----------------------------------------------------------------
void GenMetaData::print(void) const
{
  LOG(PRINT) << "GenMetaData:";
  for (size_t i=0; i<_gen.size(); ++i)
  {
    LOG(PRINT) << ConvWxTime::stime(_gen[i]);
  }
}

//----------------------------------------------------------------
bool GenMetaData::contains(const time_t &gt) const
{
  return find(_gen.begin(), _gen.end(), gt) != _gen.end();
}

//----------------------------------------------------------------
std::string GenMetaData::xml(void) const
{
  string v = "";
  for (size_t i=0; i<_gen.size(); ++i)
  {
    v += Xml::writeTime("genTime", 0, _gen[i]);
  }
  return Xml::writeString("genTimes", 0, v);
}

//----------------------------------------------------------------
void GenMetaData::erase(const time_t &t)
{
  vector<time_t>::iterator i;
  i = find(_gen.begin(), _gen.end(), t);
  if (i != _gen.end())
  {
    _gen.erase(i);
  }
}

