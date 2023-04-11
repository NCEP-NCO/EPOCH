#include "ParmsPrecipAccumCalc.hh"
#include <ConvWx/MultiGrid.hh>
#include <toolsa/LogStream.hh>

//------------------------------------------------------------------------------
static std::string
_shortToLong(const std::vector<std::pair<std::string,std::string> > &mapping,
	     const std::string &name)
{
  for (size_t i=0; i<mapping.size(); ++i)
  {
    if (mapping[i].first == name)
    {
      return mapping[i].second;
    }
  }
  LOG(ERROR) << "No " << name;
  return "";
}

//------------------------------------------------------------------------------
static std::string
_longToShort(const std::vector<std::pair<std::string,std::string> > &mapping,
	     const std::string &name)
{
  for (size_t i=0; i<mapping.size(); ++i)
  {
    if (mapping[i].second == name)
    {
      return mapping[i].first;
    }
  }
  LOG(ERROR) << "No " << name;
  return "";
}

//------------------------------------------------------------------------------
std::string
ParmsPrecipAccumCalc::shortNameToLongName(int hr,
					  const std::string &shortName) const
{
  if (hr == 3)
  {
    return _shortToLong(mdv3hrShortToLong, shortName);
  }
  else if (hr == 6)
  {
    return _shortToLong(mdv6hrShortToLong, shortName);
  }
  else
  {
    LOG(ERROR) << "Bad hour " << hr;
    return "";
  }
}

//------------------------------------------------------------------------------
std::string
ParmsPrecipAccumCalc::longNameToShortName(int hr,
					  const std::string &longName) const
{
  if (hr == 3)
  {
    return _longToShort(mdv3hrShortToLong, longName);
  }
  else if (hr == 6)
  {
    return _longToShort(mdv6hrShortToLong, longName);
  }
  else
  {
    LOG(ERROR) << "Bad hour " << hr;
    return "";
  }
}


//------------------------------------------------------------------------------
bool ParmsPrecipAccumCalc::longToShort(int hr, MultiGrid &grids) const
{
  for (size_t i=0; i<grids.size(); ++i)
  {
    std::string p = longNameToShortName(hr, grids[i].getName());
    if (p.empty())
    {
      return false;
    }
    grids[i].changeName(p);
  }
  return true;
}     
