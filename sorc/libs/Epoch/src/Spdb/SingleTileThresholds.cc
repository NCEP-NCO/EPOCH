/**
 * @file SingleTileThresholds.cc
 */

//------------------------------------------------------------------
#include <Epoch/SingleTileThresholds.hh>
#include <Epoch/TileThreshInfo.hh>
#include <Epoch/TileInfo.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/LogStream.hh>
#include <toolsa/DateTime.hh>
#include <cstdio>
#include <algorithm>

const std::string SingleTileThresholds::_tag = "TileData";

//------------------------------------------------------------------
SingleTileThresholds::SingleTileThresholds(void) :
  _ok(false), _bias(-99.99), _coldstart(true), _motherTile(false),
  _generatingTime(0), _obsValue(-99.99), _fcstValue(-99.99)
{
}

//------------------------------------------------------------------
SingleTileThresholds::
SingleTileThresholds(const std::string &xml,
		     const std::vector<std::string> &fields, int &tileIndex) :
  _ok(true)
{
  // read the thresholds array
  vector<string> vstring;
  if (TaXml::readStringArray(xml, FieldThresh::_tag, vstring))
  {
    LOG(ERROR) << "Reading tag as array using " << FieldThresh::_tag;
    _ok = false;
  }
  if (_ok)
  {
    if (vstring.size() != fields.size())
    {
      LOG(ERROR) << "Inconsistent number of fields "
		 << fields.size() << " " << vstring.size();
      _ok = false;
    }
    else
    {
      for (size_t i=0; i<vstring.size(); ++i)
      {
	// read FieldThresh
	FieldThresh f(vstring[i], fields[i]);
	if (!f.ok())
	{
	  _ok = false;
	}
	else
	{
	  _thresh.push_back(f);
	}
      }
    }
  }
  if (TaXml::readDouble(xml, "Bias", _bias))
  {
    LOG(ERROR) << "Parsing Bias XML as double";
    _ok = false;
  }

  if (TaXml::readBoolean(xml, "coldstart", _coldstart))
  {
    LOG(ERROR) << "no boolean coldstart";
    _ok = false;
  }
  if (TaXml::readBoolean(xml, "motherTile", _motherTile))
  {
    LOG(ERROR) << "no boolean motherTile";
    _ok = false;
  }
  if (_coldstart)
  {
    _generatingTime = 0;
    _obsValue = -99.99;
    _fcstValue = -99.99;
  }
  else
  {
    if (TaXml::readTime(xml, "generatingTime", _generatingTime))
    {
      LOG(ERROR) << "Reading generatingTime";
      _ok = false;
    }
    if (TaXml::readDouble(xml, "obsValue", _obsValue))
    {
      LOG(ERROR) << "Parsing obsValue XML as double";
      _ok = false;
    }
    if (TaXml::readDouble(xml, "fcstValue", _fcstValue))
    {
      LOG(ERROR) << "Parsing fcstValue XML as double";
      _ok = false;
    }
  }
  if (TaXml::readInt(xml, "tileIndex", tileIndex))
  {
    LOG(ERROR) << "No tileIndex tag";
    _ok = false;
  }
}

//------------------------------------------------------------------
SingleTileThresholds::
SingleTileThresholds(const std::vector<FieldThresh> &fieldthresh,
		     bool fromMother) :
  _ok(true),
  _thresh(fieldthresh),
  _bias(-99.99),
  _coldstart(true),
  _motherTile(fromMother),
  _generatingTime(0),
  _obsValue(-99.99),
  _fcstValue(-99.99)
{
}

//------------------------------------------------------------------
SingleTileThresholds::
SingleTileThresholds(const std::vector<FieldThresh> &fieldthresh,
		     double bias, const time_t &generatingTime,
		     double obsValue, double fcstValue, bool fromMother) :
  _ok(true),
  _thresh(fieldthresh),
  _bias(bias),
  _coldstart(false),
  _motherTile(fromMother),
  _generatingTime(generatingTime),
  _obsValue(obsValue),
  _fcstValue(fcstValue)
{
}

//------------------------------------------------------------------
SingleTileThresholds::~SingleTileThresholds()
{
}

//------------------------------------------------------------------
bool SingleTileThresholds::update(const TileThreshInfo &item)
{
  if (_thresh.size() != item.getThresh().size())
  {
    LOG(ERROR) << "Sizes don't match " << item.getThresh().size() << " "
	       << _thresh.size();
    return false;
  }
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (!_thresh[i].fieldMatch(item.getIthName(i), true))
    {
      return false;
    }
    _thresh[i].setThresh(item.getIthThresh(i));
  }

  _coldstart = item.getColdstart();
  _motherTile = item.getMotherTile();
  _generatingTime = item.getGeneratingTime();
  _bias = item.getBias();
  _obsValue = item.getObs();
  _fcstValue = item.getFcst();
  return true;
}


//------------------------------------------------------------------
bool
SingleTileThresholds::filterFields(const std::vector<std::string> &fieldNames)
{
  vector<FieldThresh> newThresh;

  for (size_t i=0; i<fieldNames.size(); ++i)
  {
    int k = getThresholdIndex(fieldNames[i]);
    if (k < 0)
    {
      LOG(ERROR) << "Field Not found, cannot filter " << fieldNames[i];
      return false;
    }
    else
    {
      newThresh.push_back(_thresh[k]);
    }
  }
  _thresh = newThresh;
  return true;
}

//------------------------------------------------------------------
bool SingleTileThresholds::
replaceValues(const SingleTileThresholds &filtMap,
	      const std::vector<std::string> &filterFields)
{
  bool ret = true;
  for (size_t i=0; i<filterFields.size(); ++i)
  {
    if (!_replaceValue(filterFields[i], filtMap))
    {
      ret = false;
    }
  }
  return ret;
}

//------------------------------------------------------------------
std::string SingleTileThresholds::toXml(int tileIndex, int indent) const
{
  string s = TaXml::writeStartTag(_tag, indent);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    s += _thresh[i].toXml(indent+1);
  }    
  s += TaXml::writeDouble("Bias", indent+1, _bias, "%010.7lf");
  s += TaXml::writeBoolean("coldstart", indent+1, _coldstart);
  s += TaXml::writeBoolean("motherTile", indent+1, _motherTile);
  s += TaXml::writeTime("generatingTime", indent+1, _generatingTime);
  s += TaXml::writeDouble("obsValue", indent+1, _obsValue, "%010.7lf");
  s += TaXml::writeDouble("fcstValue", indent+1, _fcstValue, "%010.7lf");
  s += TaXml::writeInt("tileIndex", indent+1, tileIndex);
  s += TaXml::writeEndTag(_tag, indent);
  return s;
}

//------------------------------------------------------------------
bool SingleTileThresholds::
checkColdstart(const time_t &t, int maxSecondsBeforeColdstart,
	       const std::vector<FieldThresh> &coldstartThresh)
{
  if (_coldstart)
  {
    // already set
    return true;
  }

  if (t - _generatingTime <= maxSecondsBeforeColdstart)
  {
    // not too old
    return true;
  }
  if (_thresh.size() != coldstartThresh.size())
  {
    LOG(ERROR) << "Sizes don't match " 
	       << coldstartThresh.size() << " "
	       << _thresh.size();
    return false;
  }
  bool good = true;
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (!_thresh[i].fieldMatch(coldstartThresh[i], true))
    {
      good = false;
    }
  }
  if (!good)
  {
    return false;
  }
  _bias = -99.99;
  _coldstart = true;
  // Note: mother tile is not changed here
  _generatingTime = 0;
  _thresh = coldstartThresh;
  _obsValue = -99.99;
  _fcstValue = -99.99;
  return true;
}

//------------------------------------------------------------------
void SingleTileThresholds::print(int leadTime, int tileIndex,
				 const TileInfo &info,bool verbose) const
{
  printf("        lt:%08d tile[%3d(%s)] ", leadTime,
	 tileIndex, info.latlonDebugString(tileIndex).c_str());
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    printf("%s ", _thresh[i].sprint().c_str());
  }
  printf("bias:%10.8lf ", _bias);
  if (_motherTile)
  {
    printf("Mother ");
  }
  else
  {
    printf("       ");
  }
  if (_coldstart)
  {
    printf("Coldstart\n");
  }
  else
  {
    printf("ObsTime:%s",  DateTime::strn(_generatingTime).c_str());
    if (verbose)
    {
      printf(" ObsValue:%.8lf", _obsValue);
      printf(" FcstValue:%.8lf\n", _fcstValue);
    }
    else
    {
      printf("\n");
    }
  }
}

//------------------------------------------------------------------
void SingleTileThresholds::logDebug(int leadTime, int tileIndex,
				    bool verbose) const
{
  LOG(DEBUG) << "   lt:" << leadTime << " tileIndex:" << tileIndex;
  string s= "";
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    s += _thresh[i].sprint().c_str();
  }
  LOG(DEBUG) << "     " << s;
  LOG(DEBUG) << "      bias:" << _bias;
  if (_motherTile)
  {
    LOG(DEBUG) << "    fromMotherTile";
  }
  if (_coldstart)
  {
    LOG(DEBUG) << "      Coldstart";
  }
  else
  {
    LOG(DEBUG) << "      ObsTime: " <<  DateTime::strn(_generatingTime);
    if (verbose)
    {
      LOG(DEBUG) << "      ObsValue:" << _obsValue;
      LOG(DEBUG) << "      FcstValue:" <<  _fcstValue;
    }
  }
}

//------------------------------------------------------------------
std::string SingleTileThresholds::sprint(int leadTime, int tileIndex,
					 const TileInfo &info,
					 bool verbose) const
{
  char buf[10000];
  sprintf(buf, "lt:%08d tile[%3d(%s)] ", leadTime,
	  tileIndex, info.latlonDebugString(tileIndex).c_str());
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    sprintf(buf+strlen(buf), "%s ", _thresh[i].sprint().c_str());
  }
  sprintf(buf+strlen(buf), "bias:%10.8lf ", _bias);
  if (_motherTile)
  {
    sprintf(buf + strlen(buf), "fromMother ");
  }
  else
  {
    sprintf(buf + strlen(buf), "          ");
  }
  if (_coldstart)
  {
    sprintf(buf+strlen(buf), "Coldstart");
  }
  else
  {
    sprintf(buf+strlen(buf), "ObsTime:%s",
	    DateTime::strn(_generatingTime).c_str());
    if (verbose)
    {
      sprintf(buf+strlen(buf), " ObsValue:%.8lf", _obsValue);
      sprintf(buf+strlen(buf), " FcstValue:%.8lf", _fcstValue);
    }
  }

  string s = buf;
  return s;
}

//------------------------------------------------------------------
bool SingleTileThresholds::namesOk(const std::vector<std::string> &names,
				   bool printErrors) const
{
  if (names.size() != _thresh.size())
  {
    if (printErrors)
    {
      LOG(ERROR) << "Uneven sizes " << names.size() << " " 
		 << _thresh.size();
    }
    return false;
  }
  for (size_t i=0; i<names.size(); ++i)
  {
    if (_thresh[i].getField() != names[i])
    {
      if (printErrors)
      {
	LOG(ERROR) << "Name mismatch " << _thresh[i].getField() 
		   << " " << names[i];
      }
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------
bool SingleTileThresholds::hasField(const std::string &name) const
{
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (_thresh[i].fieldMatch(name))
    {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------
int SingleTileThresholds::getThresholdIndex(const std::string &fieldName) const
{
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (_thresh[i].fieldMatch(fieldName))
    {
      return static_cast<int>(i);
    }
  }
  LOG(ERROR) << "Name not found " << fieldName;
  return -1;
}

//------------------------------------------------------------------
bool SingleTileThresholds::getIthThreshold(int i, double &thresh) const
{
  if (i >= 0 && i < static_cast<int>(_thresh.size()))
  {
    thresh = _thresh[i].getThresh();
    return true;
  }
  else
  {
    return false;
  }
}

//------------------------------------------------------------------
bool SingleTileThresholds::getIthName(int i, std::string &name) const
{
  if (i >= 0 && i < static_cast<int>(_thresh.size()))
  {
    name = _thresh[i].getField();
    return true;
  }
  else
  {
    return false;
  }
}

//------------------------------------------------------------------
bool SingleTileThresholds::_replaceValue(const std::string &fieldName,
					 const SingleTileThresholds &filtMap)
{
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (_thresh[i].fieldMatch(fieldName))
    {
      for (size_t j=0; j<filtMap._thresh.size(); ++j)
      {
	if (filtMap._thresh[j].fieldMatch(fieldName))
	{
	  _thresh[i].setThresh(filtMap._thresh[j].getThresh());
	  return true;
	}
      }
    }
  }
  LOG(ERROR) << "Field not found in state " << fieldName;
  return false;
}
