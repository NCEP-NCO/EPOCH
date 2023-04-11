/**
 * @file TileThreshInfo.cc
 */
#include <Epoch/TileThreshInfo.hh>
#include <Epoch/SingleTileThresholds.hh>
#include <Epoch/ThresholdsAtGenHms.hh>
#include <Epoch/TileInfo.hh>
#include <Epoch/FieldTiledThresh.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <cstdio>
#include <algorithm>

//-------------------------------------------------------------------
TileThreshInfo::TileThreshInfo(const SingleTileThresholds &mthresh,
			       int genHour, int genMin, int genSec,
			       int leadTime, int tileIndex):
  _ok(true),
  _thresh(mthresh.getThresh()),
  _bias(mthresh.getBias()),
  _coldstart(mthresh.getColdstart()),
  _motherTile(mthresh.getMotherTile()),
  _generatingTime(mthresh.getGeneratingTime()),
  _obsValue(mthresh.getObs()),
  _fcstValue(mthresh.getFcst()),
  _genHour(genHour),  _genMin(genMin), _genSec(genSec), _leadTime(leadTime),
  _tileIndex(tileIndex)
{
}

//-------------------------------------------------------------------
void TileThreshInfo::logDebug(void) const
{
  LOG(DEBUG) << "Gen:" << _genHour << ":" << _genMin << ":" << _genSec;
  LOG(DEBUG) << "   lt:" << _leadTime << " tileIndex:" << _tileIndex;
  std::string s= "";
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
    LOG(DEBUG) << "      ObsValue:" << _obsValue;
    LOG(DEBUG) << "      FcstValue:" <<  _fcstValue;
  }
}

//-------------------------------------------------------------------
std::string TileThreshInfo::hmsString(void) const
{
  char buf[1000];
  sprintf(buf, "%02d %02d %02d", _genHour, _genMin, _genSec);
  std::string s = buf;
  return s;
}

//-------------------------------------------------------------------
void TileThreshInfo::print(const TileInfo &info) const
{
  printf("Gen:%02d:%02d:%02d  ", _genHour, _genMin, _genSec);
  printf("        lt:%08d tile[%3d(%s)] ", _leadTime,
	 _tileIndex, info.latlonDebugString(_tileIndex).c_str());
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
    printf("ObsTime:%s\n",  DateTime::strn(_generatingTime).c_str());
  }
}

//------------------------------------------------------------------
bool TileThreshInfo::namesMatch(const std::vector<std::string> &names,
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

//-------------------------------------------------------------------
bool TileThreshInfo::hmsMatch(const ThresholdsAtGenHms &m) const
{
  return m.hmsMatch(_genHour, _genMin, _genSec);
}

//-------------------------------------------------------------------
void TileThreshInfo::adjust(int tileIndex, double obsMean)
{
  _tileIndex = tileIndex;
  _obsValue = obsMean;
}

//------------------------------------------------------------------
bool TileThreshInfo::get(const std::string &fieldName, FieldThresh &item) const
{
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (_thresh[i].fieldMatch(fieldName))
    {
      item = FieldThresh(_thresh[i]);
      return true;
    }
  }
  return false;
}
  
//------------------------------------------------------------------
void TileThreshInfo::
update(double bias, const time_t &obsTime, double obsValue, double fcstValue,
       const std::vector<FieldTiledThresh> thresholds, int tileIndex)
{
  update(bias, obsTime, obsValue, fcstValue);
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    string name = _thresh[i].getField();
    for (size_t j=0; j<thresholds.size(); ++j)
    {
      if (thresholds[j].nameMatch(name))
      {
	_thresh[i].setThresh(thresholds[j].getThresh(tileIndex));
	break;
      }
    }
  }
}

//------------------------------------------------------------------
void TileThreshInfo::
update(double bias, const time_t &obsTime, double obsValue, double fcstValue)
{
  _bias = bias;
  _coldstart = false;
  _generatingTime = obsTime;
  _obsValue = obsValue;
  _fcstValue = fcstValue;
}

//------------------------------------------------------------------
std::string TileThreshInfo::fieldName(int precision) const
{
  std::string ret = "";
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    ret += _thresh[i].dataFieldName(precision);
  }
  return ret;
}

//------------------------------------------------------------------
std::string
TileThreshInfo::fieldNameLimited(const std::vector<std::string> &ignore,
				  int precision) const
{
  if (ignore.empty())
  {
    return fieldName(precision);
  }

  std::string ret = "";
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    string fullName = _thresh[i].getField();
    if (find(ignore.begin(), ignore.end(), fullName) == ignore.end())
    {
      ret += _thresh[i].dataFieldName(precision);
    }
  }
  return ret;
}

//------------------------------------------------------------------
bool TileThreshInfo::
add(const std::vector<std::pair<std::string,double> > &nameThresh)
{
  bool ret = true;
  for (size_t i=0; i<nameThresh.size(); ++i)
  {
    if (hasField(nameThresh[i].first))
    {
      LOG(ERROR) << "Already have name " << nameThresh[i].first;
      ret = false;
    }
    else
    {
      _thresh.push_back(FieldThresh(nameThresh[i].first,
				    nameThresh[i].second));

    }
  }
  return ret;
}

//------------------------------------------------------------------
bool TileThreshInfo ::hasField(const std::string &name) const
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

