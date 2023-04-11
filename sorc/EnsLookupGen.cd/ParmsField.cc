/**
 * @file ParmsField.cc
 */

#include "ParmsField.hh"
#include <toolsa/LogStream.hh>

//-------------------------------------------------------------------
ParmsField::ParmsField(const Params::FieldFromDatabaseSpec_t &p,
		       const std::vector<ParmsObar> &op) :
  _fieldName(p.fieldname),
  _comparison(p.compare),
  _isDatabase(true),
  _databaseUrl(p.databaseUrl),
  _obarInfoIndex(p.obarInfoIndex),
  _isOutputObar(p.isOutputObar),
  _doOutputThresholds(p.doOutputThresholdsGrid),
  _obarInfo(op),
  _coldstartThresh(p.coldstartThresh)
{
}

//-------------------------------------------------------------------
ParmsField::ParmsField(const Params::FieldFixedSpec_t &p) :
  _fieldName(p.fieldname),
  _comparison(p.compare),
  _isDatabase(false),
  _obarInfoIndex(-1),
  _isOutputObar(false),
  _doOutputThresholds(false),
  _coldstartThresh(p.threshold)
{
}

//-------------------------------------------------------------------
bool ParmsField::threshTest(double val, double thresh) const
{
  return threshTest(val, thresh, _comparison);
}

//-------------------------------------------------------------------
std::string ParmsField::sprintCompare(Params::Compare_t c)
{
  std::string ret = "";
  switch (c)
  {
  case Params::LE:
    ret = "<=";
    break;
  case Params::EQ:
    ret = "=";
    break;
  case Params::GE:
    ret = ">=";
    break;
  default:
    ret = "?";
    break;
  }
  return ret;
}

//-------------------------------------------------------------------
bool ParmsField::threshTest(double val, double thresh, Params::Compare_t c)
{
  bool ret = false;

  switch (c)
  {
  case Params::LE:
    ret = (val <= thresh);
    break;
  case Params::EQ:
    ret = (val == thresh);
    break;
  case Params::GE:
    ret = (val >= thresh);
    break;
  default:
    break;
  }
  return ret;
}


