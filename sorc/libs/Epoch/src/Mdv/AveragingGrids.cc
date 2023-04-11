/**
 * @file AveragingGrids.cc
 */

#include <Epoch/AveragingGrids.hh>
#include <Epoch/CountSumNames.hh>
using std::string;

//-------------------------------------------------------------------------
AveragingGrids::AveragingGrids(const std::string &fieldName,
			       const Grid &templateGrid) :
  _fieldName(fieldName)
{
  string countName = CountSumNames::countsFieldName(fieldName);
  string sumName = CountSumNames::sumsFieldName(fieldName);

  _counts = Grid(templateGrid, countName, "counts");
  _sums = Grid(templateGrid, sumName, templateGrid.getUnits());
  _counts.setAllToValue(0.0);
  _sums.setAllToValue(0.0);
}

//-------------------------------------------------------------------------
AveragingGrids::~AveragingGrids(void)
{
}

//-------------------------------------------------------------------------
void AveragingGrids::initialize(void)
{
  _counts.setAllToValue(0.0);
  _sums.setAllToValue(0.0);
}

//-------------------------------------------------------------------------
void AveragingGrids::incrementCount(int index)
{
  _counts.incrementValueAtPoint(index, 1.0);
}

//-------------------------------------------------------------------------
void AveragingGrids::increment(int index)
{
  _sums.incrementValueAtPoint(index, 1.0);
}

//-------------------------------------------------------------------------
void AveragingGrids::normalize(void)
{
  for (int k = 0; k < _counts.getNdata(); ++k)
  { 
    double v=0.0;
    bool hasCount = _counts.getValue(k, v);
    if (hasCount && v <= 0)
    {
      hasCount = false;
    }
    if (!hasCount)
    {
      _sums.setToMissing(k);
    }
    else
    {
      double val;
      if (_sums.getValue(k, val))
      {
	_sums.setv(k, val/v);
      }
    }
  }
  _sums.changeName(_fieldName);
}
