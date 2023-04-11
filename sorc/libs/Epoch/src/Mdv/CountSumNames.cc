/**
 * @file CountSumNames.cc
 */
#include <Epoch/CountSumNames.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/ParmFcst.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

const std::string CountSumNames::_count = "Counts";
const std::string CountSumNames::_sum = "Sums";
const std::vector<std::string> CountSumNames::_countSum = {"Counts", "Sums"};

//---------------------------------------------------------------------
bool
CountSumNames::loadNonSumCountFcsts(const time_t &genTime,
				    int leadTime,
				    const ParmProjection &proj,
				    const ParmFcst &parms,
				    MultiFcstGrid &fcsts)
{  
  if (InterfaceIO::loadAllFcstExcludeContaining(genTime, leadTime,
						 proj,
						 parms.pUrl,
						 parms.pRemap,
						 _countSum, fcsts))
  {
    return true;
  }
  else
  {
    LOG(ERROR) << "Loading forecasts for " << DateTime::strn(genTime)
	       << "+" << leadTime;
    return false;
  }
}

//---------------------------------------------------------------------
std::string CountSumNames::countsFieldName(const std::string &name)
{
  return name + _count;
}

//---------------------------------------------------------------------
std::string CountSumNames::sumsFieldName(const std::string &name)
{
  return name + _sum;
}

//---------------------------------------------------------------------
std::vector<std::string>
CountSumNames::countSumNames(const std::vector<std::string> &fieldNames)
{
  std::vector<std::string> ret;
  for (size_t i=0; i<fieldNames.size(); ++i)
  {
    string name = countsFieldName(fieldNames[i]);
    ret.push_back(name);
    name = sumsFieldName(fieldNames[i]);
    ret.push_back(name);
  }      
  return ret;
}
