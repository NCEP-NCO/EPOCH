#include <Epoch/ThresholdDatabaseParms.hh>
#include <Epoch/ThresholdDatabaseParmsIO.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>

//--------------------------------------------------------------------------------
MultiThresholdDatabaseParms::MultiThresholdDatabaseParms() : _ok(false)
{
}

//--------------------------------------------------------------------------------
MultiThresholdDatabaseParms::MultiThresholdDatabaseParms(int num,
							 const char **parmfiles) :
  _ok(true)
{
  for (int i=0; i<num; ++i)
  {
    string n = parmfiles[i];
    ThresholdDatabaseParmsIO tio(n);
    if (!tio.isOk())
    {
      printf("Problems loading parameters, database file %s\n", n.c_str());
      _ok = false;
    }
    ThresholdDatabaseParms tp(tio);
    _thresh.push_back(tp);
    _parmFileName.push_back(n);
    _fieldNames.push_back(tp._fieldName);
  }
  if (!_ok)
  {
    return;
  }

  // make sure each threshold param object has outside threshold for
  // all thresholded fields 
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (!_thresh[i].hasAllOutsideThresh(_fieldNames))
    {
      printf("ERROR missing outside thresholds for a field parmfile:%s\n",
	     _fieldNames[i].c_str());
      _ok = false;
    }
  }
  if (!_ok)
  {
    return;
  }
  
  // make sure exactly one field is the 'output obar' field and it has at least
  // obar threshold,
  // and all other fields are NOT output and have 1 obar threshold (spdb) or 0
  // (fixed)
  _outputObarFieldIndex = -1;
  for (size_t i=0; i<_thresh.size(); ++i)
  {
    if (_thresh[i].isGoodOutputObar())
    {
      if (_outputObarFieldIndex == -1)
      {
	_outputObarFieldIndex = i;
      }
      else
      {
   	printf("ERROR more than one output obar field need only one\n");
	printf("%s and %s both are output obar fields\n",
	       _parmFileName[_outputObarFieldIndex].c_str(),
	       _parmFileName[i].c_str());
	_ok = false;
      }
    }
    else
    {
      if (!_thresh[i].isGoodNonoutputObar())
      {
	printf("ERROR %s not set properly as a non obar field\n",
	       _parmFileName[i].c_str());
	_ok = false;
      }
    }
  }
  if (!_ok)
  {
    return;
  }
  if (_outputObarFieldIndex == -1)
  {
    printf("ERROR need one output obar field, got none\n");
    _ok = false;
  }
}

//--------------------------------------------------------------------------------
std::vector<std::string>  MultiThresholdDatabaseParms::fieldNames(void) const
{
  return _fieldNames;
}

//--------------------------------------------------------------------------------
int MultiThresholdDatabaseParms::numOutputObarFields(void) const
{
  return (int)_thresh[_outputObarFieldIndex]._obarThresh.size();
}

//--------------------------------------------------------------------------------
ObarThreshParms MultiThresholdDatabaseParms::ithOutputObarFieldParms(int i) const
{
  return _thresh[_outputObarFieldIndex]._obarThresh[i];
}

//--------------------------------------------------------------------------------
int MultiThresholdDatabaseParms::numFieldParms(void) const
{
  return (int)(_thresh.size());
}

//--------------------------------------------------------------------------------
ThresholdDatabaseParms
MultiThresholdDatabaseParms::ithFieldParms(int i) const
{
  return _thresh[i];
}

//--------------------------------------------------------------------------------
bool ThresholdDatabaseParms::obarMatch(const std::vector<double> &obarThresh) const
{
  if (obarThresh.size() != _obarThresh.size())
  {
    LOG(ERROR) << "DB obar size not same as config";
    return false;
  }
  for (size_t i=0; i<obarThresh.size(); ++i)
  {
    if (obarThresh[i] != _obarThresh[i]._obarThresh)
    {
      LOG(ERROR) << "Obar thresh mismatch";
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------------------
bool ThresholdDatabaseParms::hasAllOutsideThresh(const std::vector<std::string> &fields) const
{
  bool status = true;
  for (size_t i=0; i<_obarThresh.size(); ++i)
  {
    if (!_obarThresh[i].hasAllOutsideThresh(fields))
    {
      status = false;
    }
  }
  return status;
}
  
//--------------------------------------------------------------------------------
bool ThresholdDatabaseParms::isGoodOutputObar(void) const
{
  return _isOutputObar && _fromDatabase && _obarThresh.size() >= 1;
}

//--------------------------------------------------------------------------------
bool ThresholdDatabaseParms::isGoodNonoutputObar(void) const
{
  return (!_isOutputObar && ((_fromDatabase && _obarThresh.size() == 1) ||
			     (!_fromDatabase)));
}


//-------------------------------------------------------------------
bool ThresholdDatabaseParms::threshTest(double val, double thresh) const
{
  return threshTest(val, thresh, _compare);
}

//-------------------------------------------------------------------
bool ThresholdDatabaseParms::threshTest(double val, double thresh,
					ThresholdDatabaseParams::Compare_t c)
{
  bool ret = false;

  switch (c)
  {
  case ThresholdDatabaseParams::LE:
    ret = (val <= thresh);
    break;
  case ThresholdDatabaseParams::EQ:
    ret = (val == thresh);
    break;
  case ThresholdDatabaseParams::GE:
    ret = (val >= thresh);
    break;
  default:
    break;
  }
  return ret;
}

//--------------------------------------------------------------------------------
bool ObarThreshParms::hasAllOutsideThresh(const std::vector<std::string> &fields) const
{
  if (fields.size() != _outsideThresh.size())
  {
    LOG(ERROR) << "uneven number of fields from input compared to parms";
    return false;
  }
  for (size_t i=0; i<_outsideThresh.size(); ++i)
  {
    if (find(fields.begin(), fields.end(), _outsideThresh[i].first) == fields.end())
    {
      LOG(ERROR) << "Missing parameterized field " << _outsideThresh[i].first;
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------------------
bool ObarThreshParms::getOutsideThresh(const std::string &fieldName, double &thresh) const
{
  for (size_t i=0; i<_outsideThresh.size(); ++i)
  {
    if (_outsideThresh[i].first == fieldName)
    {
      thresh = _outsideThresh[i].second;
      return true;
    }
  }
  LOG(ERROR) << "No outside thresh for " << fieldName;
  return false;
}
