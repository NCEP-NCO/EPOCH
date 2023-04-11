/**
 * @file ThresholdsAtObsTime.cc
 */

//------------------------------------------------------------------
#include <Epoch/SpdbMetadata.hh>
#include <Epoch/TileThreshInfo.hh>
#include <euclid/Grid2d.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>
#include <cstdio>
using std::string;
using std::vector;

//------------------------------------------------------------------
SpdbMetadata::SpdbMetadata(void)
{
}

//------------------------------------------------------------------
SpdbMetadata::
SpdbMetadata(const std::vector<double> &ltHours,
	     const std::vector<std::string> &fields,
	     const std::vector<FieldThresh> &fixedFields,
	     const TileInfo &tiling) :
  _fields(fields), _fixedFields(fixedFields), _tiling(tiling)
{
  for (size_t i=0; i<ltHours.size(); ++i)
  {
    _leadSeconds.push_back(static_cast<int>(ltHours[i]*3600.0));
  }
  // don't know anything about gen times yet
}

//------------------------------------------------------------------
SpdbMetadata::~SpdbMetadata()
{
}

//------------------------------------------------------------------
void SpdbMetadata::clearMapping(void)
{
  _fields.clear();
  _fixedFields.clear();
  _leadSeconds.clear();
  _fcst.clear();
  _tiling = TileInfo();
}

//------------------------------------------------------------------
bool SpdbMetadata::setColdStart(int genFrequencySeconds,
				const std::vector<FieldThresh> &thresh)
{
  if (!_fieldThreshNamesOk(thresh))
  {
    LOG(ERROR) << "Mismatch";
    return false;
  }

  _fcst.clear();
  for (double hms = 0.0; hms < 24.0*3600.0; 
       hms += (double)genFrequencySeconds)
  {
    int hour = static_cast<int>(hms/3600.0);
    int min = static_cast<int>((hms - static_cast<double>(hour)*3600.0)/60.0);
    int sec = static_cast<int>((hms - static_cast<double>(hour)*3600.0 -
				static_cast<double>(min)*60.0)/60.0);
    _fcst.push_back(ThresholdsAtGenHms(hour, min, sec,
				       _leadSeconds, _tiling, thresh));
  }
  return true;
}

//------------------------------------------------------------------
void SpdbMetadata::addLatlonsIfNeeded(const TileInfo &t)
{
  _tiling.addLatlons(t);
}

//------------------------------------------------------------------
bool SpdbMetadata::
replaceValues(const SpdbMetadata &filtMap,
	      const std::vector<std::string> &filterFields,
	      bool exactMatch)
{
  bool ret = true;
  if (_leadSeconds == filtMap._leadSeconds &&
      _tiling == filtMap._tiling &&
      _fcst.size() == filtMap._fcst.size())
  {
    for (size_t i=0; i<filterFields.size(); ++i)
    {
      if (find(_fields.begin(), _fields.end(), filterFields[i]) ==
	  _fields.end())
      {
	LOG(ERROR) << "Field not in local state " << filterFields[i];
	ret = false;
      }
    }
    for (size_t i=0; i<filterFields.size(); ++i)
    {
      if (find(filtMap._fields.begin(), filtMap._fields.end(),
	       filterFields[i]) == filtMap._fields.end())
      {
	LOG(ERROR) << "Field not in input state " << filterFields[i];
	ret = false;
      }
    }
  }
  else
  {
    LOG(ERROR) << "State of input object differs from that of local";
    if (!(_leadSeconds == filtMap._leadSeconds))
    {
      LOG(ERROR) << "LeadSec diff";
    }
    if (!(_tiling == filtMap._tiling))
    {
      LOG(ERROR) << "Tiling diff";
      _tiling.printDiffs(filtMap._tiling);
    }
    if (_fcst.size() != filtMap._fcst.size())
    {
      LOG(ERROR) << "Size diff" << _fcst.size() << " "
		 << filtMap._fcst.size();
    }
    ret = false;
  }
  if (exactMatch)
  {
    if (!(_fields == filtMap._fields &&_fixedFields == filtMap._fixedFields))
    {
      LOG(ERROR) << "State of input object not equal to that of local";
      if (!(_fields == filtMap._fields))
      {
	LOG(ERROR) << "Fields diff";
	ret = false;
      }
      if (!(_fixedFields == filtMap._fixedFields))
      {
	LOG(ERROR) << "Fixed Fields diff";
	ret = false;
      }
    }
  }
  if (!ret)
  {
    return false;
  }

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (!_fcst[i].replaceValues(filtMap._fcst[i], filterFields))
    {
      ret = false;
    }
  }
  return ret;
}

//------------------------------------------------------------------
std::string SpdbMetadata::toXml(void) const
{
  string ret = _fieldsToXml();
  ret += _fixedFieldsToXml();
  ret += _leadsToXml();
  ret += _tiling.toXml();
  ret += _mappingsToXml();
  return ret;
}

//------------------------------------------------------------------
bool SpdbMetadata::fromXml(const std::string &xml,
			   bool fieldsLeadTilesSet, bool latlonsOptional)
{
  if (!_fieldsFromXml(xml, fieldsLeadTilesSet))
  {
    return false;
  }
  if (!_fixedFieldsFromXml(xml, fieldsLeadTilesSet))
  {
    return false;
  }
  if (!_leadsFromXml(xml, fieldsLeadTilesSet))
  {
    return false;
  }
  if (!_tilingFromXml(xml, fieldsLeadTilesSet, latlonsOptional))
  {
    return false;
  }
  return _mappingsFromXml(xml);
}

//------------------------------------------------------------------
bool SpdbMetadata::
checkColdstart(const time_t &t, int maxSecondsBeforeColdstart,
	       const std::vector<FieldThresh> &coldstartThresh)
{
  bool ret = true;
  for (size_t i=0;i<_fcst.size(); ++i)
  {
    if (!_fcst[i].checkColdstart(t, maxSecondsBeforeColdstart, coldstartThresh))
    {
      ret = false;
    }
  }
  return ret;
}

//--------------------------------------------------------------------
bool SpdbMetadata::update(const TileThreshInfo &item)
{
  if (!item.namesMatch(_fields))
  {
    LOG(ERROR) << "Mismatch";
    return false;
  }

  if (!_leadTimeExists(item.getLeadTime(), "update"))
  {
    return false;
  }

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (item.hmsMatch(_fcst[i]))
    {
      return _fcst[i].update(item);
    }
  }
  
  LOG(ERROR) << "Gen time not found in state hms=" << item.hmsString();
  return false;
}

//------------------------------------------------------------------
bool SpdbMetadata::
setColdStart(const time_t &genTime, int leadTime,  int numTiles,
	     const std::vector<FieldThresh> &fieldThresh)
{
  if (!_fieldThreshNamesOk(fieldThresh))
  {
    LOG(ERROR) << "Mismatch in field names";
    return false;
  }
  if (numTiles != _tiling.numTiles())
  {
    LOG(ERROR) << "Mismatch in tiles";
    return false;
  }

  if (!_leadTimeExists(leadTime, "setColdStart"))
  {
    return false;
  }
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (_fcst[i].hmsMatch(genTime))
    {
      _fcst[i].setColdstart(leadTime, _tiling.numTiles(), fieldThresh);
      return true;
    }
  }

  LOG(ERROR) << "Gen time not in state " << DateTime::strn(genTime);
  return false;
}

//------------------------------------------------------------------
void SpdbMetadata::printState(const time_t &t, bool verbose) const
{
  printf("---------Threshold/bias information %s ----------\n",
	 DateTime::strn(t).c_str());
  printf("Fields:");
  for (size_t i=0; i<_fields.size(); ++i)
  {
    printf("%s ", _fields[i].c_str());
  }
  printf("FixedFields:");
  for (size_t i=0; i<_fixedFields.size(); ++i)
  {
    printf("%s ", _fixedFields[i].sprint().c_str());
  }
  printf("\nLeadtimes:");
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    printf("%d,", _leadSeconds[i]);
  }
  printf("\n");

  _tiling.print(verbose);

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    _fcst[i].print(_tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void SpdbMetadata::printState(const time_t &t, const time_t &twritten,
				     bool verbose) const
{
  printf("---------Threshold/bias information %s --- written %s ----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(twritten).c_str());
  printf("Fields:");
  for (size_t i=0; i<_fields.size(); ++i)
  {
    printf("%s ", _fields[i].c_str());
  }
  printf("FixedFields:");
  for (size_t i=0; i<_fixedFields.size(); ++i)
  {
    printf("%s ", _fixedFields[i].sprint().c_str());
  }
  printf("\nLeadtimes:");
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    printf("%d,", _leadSeconds[i]);
  }
  printf("\n");

  _tiling.print(verbose);

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    _fcst[i].print(_tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void
SpdbMetadata::printState(const time_t &t, bool verbose,
			 const std::vector<int> &genHours,
			 const std::vector<int> &leadSec,
			 const std::vector<int> &tiles) const
{
  if (genHours.empty() && leadSec.empty() && tiles.empty())
  {
    printState(t, verbose);
    return;
  }

  printf("---------Threshold/bias information %s ----------\n",
	 DateTime::strn(t).c_str());
  printf("Fields:");
  for (size_t i=0; i<_fields.size(); ++i)
  {
    printf("%s ", _fields[i].c_str());
  }
  printf("FixedFields:");
  for (size_t i=0; i<_fixedFields.size(); ++i)
  {
    printf("%s ", _fixedFields[i].sprint().c_str());
  }
  printf("\nLeadtimes:");
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    printf("%d,", _leadSeconds[i]);
  }
  printf("\n");
  _tiling.print(verbose);

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    _fcst[i].print(genHours, leadSec, tiles, _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
void
SpdbMetadata::printState(const time_t &t, const time_t &twritten,
			 bool verbose, const std::vector<int> &genHours,
			 const std::vector<int> &leadSec,
			 const std::vector<int> &tiles) const
{
  if (genHours.empty() && leadSec.empty() && tiles.empty())
  {
    printState(t, verbose);
    return;
  }

  printf("---------Threshold/bias information %s ---written %s----------\n",
	 DateTime::strn(t).c_str(), DateTime::strn(twritten).c_str());
  printf("Fields:");
  for (size_t i=0; i<_fields.size(); ++i)
  {
    printf("%s ", _fields[i].c_str());
  }
  printf("FixedFields:");
  for (size_t i=0; i<_fixedFields.size(); ++i)
  {
    printf("%s ", _fixedFields[i].sprint().c_str());
  }
  printf("\nLeadtimes:");
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    printf("%d,", _leadSeconds[i]);
  }
  printf("\n");
  _tiling.print(verbose);

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    _fcst[i].print(genHours, leadSec, tiles, _tiling, verbose);
  }
  printf("-----End Threshold/bias information ------------------\n");
}

//------------------------------------------------------------------
bool SpdbMetadata::getDebugTiledGrids(const time_t &genTime,
				      int leadTime,
				      const std::string &field,
				      double centerWeight,
				      double edgeWeight, int nptSmooth,
				      std::vector<Grid2d> &item) const
{
  if (find(_leadSeconds.begin(), _leadSeconds.end(),
	   leadTime) == _leadSeconds.end())
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (_fcst[i].getDebugTiledGrids(genTime, leadTime, field, _tiling, 
				    centerWeight, edgeWeight, nptSmooth, item))
    {
      return true;
    }
  }
  LOG(ERROR) << "No matching gen time in state " << DateTime::strn(genTime);
  return false;
}

//------------------------------------------------------------------
bool SpdbMetadata::get(const time_t &genTime, int leadTime, int tileInd,
		       TileThreshInfo &item) const
{
  if (find(_leadSeconds.begin(), _leadSeconds.end(),
	   leadTime) == _leadSeconds.end())
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }
  if (!_tiling.inRange(tileInd))
  {
    LOG(ERROR) << "Tile index not in range " << tileInd;
    return false;
  }
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (_fcst[i].get(genTime, leadTime, tileInd, item))
    {
      return true;
    }
  }
  LOG(ERROR) << "No matching gen time in state " << DateTime::strn(genTime);
  return false;
}

//------------------------------------------------------------------
bool
SpdbMetadata::getFixedFieldThresh(const std::string fieldName,
				  FieldThresh &fieldthresh) const
{
  for (size_t i=0; i<_fixedFields.size(); ++i)
  {
    if (_fixedFields[i].fieldMatch(fieldName))
    {
      fieldthresh = _fixedFields[i];
      return true;
    }
  }
  return false;
}  

//------------------------------------------------------------------
bool SpdbMetadata::getTiledGrids(const time_t &genTime,int leadTime, 
				 double centerWeight, double edgeWeight,
				 int nptSmooth,
				 std::vector<Grid2d> &item) const
{
  if (find(_leadSeconds.begin(), _leadSeconds.end(),
	   leadTime) == _leadSeconds.end())
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }
  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (_fcst[i].getTiledGrids(genTime, leadTime, _tiling, centerWeight,
			       edgeWeight, nptSmooth, item))
    {
      if (item.size() > 0)
      {
	Grid2d temp(item[0]);
	
	// add in the fixed fields/thresholds
	for (size_t j=0; j<_fixedFields.size(); ++j)
	{
	  temp.setName(_fixedFields[j].getField());
	  temp.setAllToValue(_fixedFields[j].getThresh());
	  item.push_back(temp);
	}
      }
      return true;
    }
  }
  LOG(ERROR) << "No matching gen time in state " << DateTime::strn(genTime);
  return false;
}


//------------------------------------------------------------------
bool SpdbMetadata::get(const time_t &genTime, int leadTime,
		       int numTiles, const std::string &fieldName,
		       std::vector<FieldThresh> &item) const
{
  if (numTiles != _tiling.numTiles())
  {
    LOG(ERROR) << "Mismatch in number of tiles " << numTiles << " " 
	       << _tiling.numTiles();
    return false;
  }

  bool status = true;
  for (int i=0; i<numTiles; ++i)
  {
    TileThreshInfo m;
    if (get(genTime, leadTime, i, m))
    {
      FieldThresh itemxy;
      if (m.get(fieldName, itemxy))
      {
	item.push_back(itemxy);
      }
      else
      {
	LOG(ERROR) << "No field " << fieldName << " at " 
		   << DateTime::strn(genTime) << " + " << leadTime 
		   << " for tile " << i;
	status = false;
      }
    }
    else
    {
      LOG(ERROR) << "No data at " << DateTime::strn(genTime) 
		 << " + " << leadTime 
		 << " for tile " << i;
      status = false;
    }
  }
  return status;
}

//------------------------------------------------------------------
bool SpdbMetadata::get(const time_t &genTime, int leadTime,
		       int numTiles, const std::string &fieldName,
		       std::vector<double> &item) const
{
  if (numTiles != _tiling.numTiles())
  {
    LOG(ERROR) << "Mismatch in number of tiles " << numTiles << " " 
	       << _tiling.numTiles();
    return false;
  }

  bool status = true;
  for (int i=0; i<numTiles; ++i)
  {
    TileThreshInfo m;
    if (get(genTime, leadTime, i, m))
    {
      FieldThresh itemxy;
      if (m.get(fieldName, itemxy))
      {
	item.push_back(itemxy.getThresh());
      }
      else
      {
	LOG(ERROR) << "No field " << fieldName << " at " 
		   << DateTime::strn(genTime) << " + " << leadTime 
		   << " for tile " << i;
	status = false;
      }
    }
    else
    {
      LOG(ERROR) << "No data at " << DateTime::strn(genTime) 
		 << " + " << leadTime 
		 << " for tile " << i;
      status = false;
    }
  }
  return status;
}

//------------------------------------------------------------------
bool
SpdbMetadata::constructTiledGrid(const std::string &fieldName,
				 const time_t &genTime,  int leadTime,
				 double centerWeight, double edgeWeight,
				 int nptSmooth, Grid2d &grid) const
{
  if (find(_fields.begin(), _fields.end(), fieldName) == _fields.end())
  {
    LOG(ERROR) << "Field not found " << fieldName;
    return false;
  }
  if (find(_leadSeconds.begin(), _leadSeconds.end(),
	   leadTime) == _leadSeconds.end())
  {
    LOG(ERROR) << "Lead time not found in state " << leadTime;
    return false;
  }

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    if (_fcst[i].hmsMatch(genTime))
    {
      return _fcst[i].constructTiledGrid(fieldName, leadTime, _tiling, 
					 centerWeight, edgeWeight, 
					 nptSmooth, grid);
    }
  }
  LOG(ERROR) << "Gen time not found in state " << DateTime::strn(genTime);
  return false;
}

//------------------------------------------------------------------
std::string SpdbMetadata::_fieldsToXml(void) const
{
  string s = TaXml::writeStartTag("Fields", 0);
  for (size_t i=0; i<_fields.size(); ++i)
  {
    s += TaXml::writeString("field", 1, _fields[i]);
  }
  s += TaXml::writeEndTag("Fields", 0);
  return s;
}

//------------------------------------------------------------------
std::string SpdbMetadata::_fixedFieldsToXml(void) const
{
  string s2 = TaXml::writeStartTag("FixedFields", 0);
  for (size_t i=0; i<_fixedFields.size(); ++i)
  {
    s2 += _fixedFields[i].toXml(1);
  }
  s2 += TaXml::writeEndTag("FixedFields", 0);
  return s2;
}

//------------------------------------------------------------------
bool SpdbMetadata::_fieldsFromXml(const std::string &xml,
				  bool fieldsLeadTilesSet)
{
  std::vector<std::string> fields;
  std::string str;

  if (TaXml::readString(xml, "Fields", str))
  {
    LOG(ERROR) << "No XML with key Fields";
    return false;
  }
  if (TaXml::readStringArray(str, "field", fields))
  {
    LOG(ERROR) << "No XML string array with key field";
    return false;
  }
  if (fieldsLeadTilesSet)
  {
    if (fields != _fields)
    {
      LOG(ERROR) << "Fields in XML does not match local state";
      return false;
    }
  }
  else
  {
    _fields = fields;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbMetadata::_fixedFieldsFromXml(const std::string &xml,
				       bool fieldsLeadTilesSet)
{
  string str;
  if (TaXml::readString(xml, "FixedFields", str))
  {
    LOG(ERROR) << "No XML with key FixedFields";
    return false;
  }
  
  std::vector<std::string> fields;
  if (TaXml::readStringArray(str, FieldThresh::_tag, fields))
  {
    LOG(ERROR) << "No XML with key" << FieldThresh::_tag;
    return false;
  }
  std::vector<FieldThresh> fixedFields;
  for (size_t i=0; i<fields.size(); ++i)
  {
    FieldThresh f = FieldThresh::fromXml(fields[i]);
    if (f.ok())
    {
      fixedFields.push_back(f);
    }
    else
    {
      return false;
    }
  }

  if (fieldsLeadTilesSet)
  {
    if (fixedFields != _fixedFields)
    {
      LOG(ERROR) << "Fields in XML does not match local state";
      return false;
    }
  }
  else
  {
    _fixedFields = fixedFields;
  }
  return true;
}

//------------------------------------------------------------------
std::string SpdbMetadata::_leadsToXml(void) const
{
  string s = TaXml::writeStartTag("Lead", 0);
  for (size_t i=0; i<_leadSeconds.size(); ++i)
  {
    s += TaXml::writeInt("lt", 1, _leadSeconds[i], "%08d");
  }
  s += TaXml::writeEndTag("Lead", 0);
  return s;
}

//------------------------------------------------------------------
bool SpdbMetadata::_leadsFromXml(const std::string &xml,
				 bool fieldsLeadTilesSet)
{
  string str;
  if (TaXml::readString(xml, "Lead", str))
  {
    LOG(ERROR) << "No XML with key Lead";
    return false;
  }

  std::vector<std::string> vstring;
  if (TaXml::readStringArray(str, "lt", vstring))
  {
    LOG(ERROR) << "No XML string array with key lt";
    return false;
  }

  std::vector<int> vlt;
  for (size_t i=0; i<vstring.size(); ++i)
  {
    int lt;
    if (sscanf(vstring[i].c_str(), "%d", &lt) != 1)
    {
      LOG(ERROR) << "Scanning " << vstring[i] << " As an int";
      return false;
    }
    vlt.push_back(lt);
  }
  
  if (fieldsLeadTilesSet)
  {
    if (vlt != _leadSeconds)
    {
      LOG(ERROR) << "Lead Times in XML does not match local state";
      return false;
    }
  }
  else
  {
    _leadSeconds = vlt;
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbMetadata::_tilingFromXml(const std::string &xml,
				  bool fieldsLeadTilesSet,
				  bool latlonsOptional)
{
  TileInfo tinfo(xml);
  if (!tinfo.isOk())
  {
    return false;
  }
  if (fieldsLeadTilesSet)
  {
    bool status = true;
    if (latlonsOptional)
    {
      status = tinfo.equalExceptLatlons(_tiling);
    }
    else
    {
      status = (tinfo == _tiling);
    }
    if (!status)
    {
      LOG(ERROR) << "Tiling does not match local state";
      return false;
    }
  }
  else
  {
    _tiling = tinfo;
  }
  return true;
}

//------------------------------------------------------------------
std::string SpdbMetadata::_mappingsToXml(void) const
{
  string s = "";

  for (size_t i=0; i<_fcst.size(); ++i)
  {
    s += _fcst[i].toXml(0);
  }
  return s;
}

//------------------------------------------------------------------
bool SpdbMetadata::_mappingsFromXml(const std::string &xml)
{
  vector<string> vstring;
  if (TaXml::readStringArray(xml, ThresholdsAtGenHms::_tag, vstring))
  {
    LOG(ERROR) << "String array tag missing, " 
	       << ThresholdsAtGenHms::_tag;
    return false;
  }
  
  _fcst.clear();
  for (size_t i=0; i<vstring.size(); ++i)
  {
    ThresholdsAtGenHms m(vstring[i], _fields, _leadSeconds, _tiling);
    if (m.ok())
    {
      _fcst.push_back(m);
    }
    else
    {
      _fcst.clear();
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------
bool SpdbMetadata::
_fieldThreshNamesOk(const std::vector<FieldThresh> &fieldThresh) const
{
  if (fieldThresh.size() != _fields.size())
  {
    LOG(ERROR) << "Uneven sizes " << fieldThresh.size() << " " 
               << _fields.size();
    return false;
  }
  for (size_t i=0; i<fieldThresh.size(); ++i)
  {
    if (fieldThresh[i].getField() != _fields[i])
    {
      LOG(ERROR) << "Name mismatch " << fieldThresh[i].getField() 
                 << " " << _fields[i];
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------
bool
SpdbMetadata::_leadTimeExists(int leadTime,
			      const std::string &debugName) const
{
  vector<int>::const_iterator index;
  index = find(_leadSeconds.begin(), _leadSeconds.end(), leadTime);
  if (index == _leadSeconds.end())
  {
    LOG(ERROR) << debugName << " Lead time not on list " << leadTime;
    return false;
  }
  else
  {
    return true;
  }
}
