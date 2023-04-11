/**
 * @file ClimoStateGrids.cc
 * @brief Source for ClimoStateGrids class
 */
#include <Epoch/ClimoStateGrids.hh>
#include <Epoch/CountSumNames.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/Grid.hh>
#include <toolsa/LogStream.hh>
#include <string>


//-------------------------------------------------------------------------
ClimoStateGrids::ClimoStateGrids(const std::string &name, MultiGrid &stateData)
{
  std::string countName = CountSumNames::countsFieldName(name);
  std::string sumName = CountSumNames::sumsFieldName(name);

  _name = name;
  _counts = stateData.gridPtr(countName);
  _sums = stateData.gridPtr(sumName);
  _good = (_sums != NULL && _counts != NULL);
}

//-------------------------------------------------------------------------
ClimoStateGrids::~ClimoStateGrids()
{

}

//-------------------------------------------------------------------------
bool ClimoStateGrids::increment(const Grid &inputData)
{
  if (!_good)
  {
    return false;
  }

  // loop through and update counts and sums
  for (int i=0; i<inputData.getNdata(); ++i)
  {
    double v;
    if (inputData.getValue(i, v))
    {
      _increment(i, v);
    }
  }
  return true;
}

//-------------------------------------------------------------------------
bool ClimoStateGrids::decrement(const Grid &data)
{
  if (!_good)
  {
    return false;
  }

  // loop through and update counts and sums
  for (int i=0; i<data.getNdata(); ++i)
  {
    double v;
    if (data.getValue(i, v))
    {
      _decrement(i, v);
    }
  }
  return true;
}

//-------------------------------------------------------------------------
bool ClimoStateGrids::normalize(Grid &climoGrid) const
{
  if (!_good)
  {
    return false;
  }

  // start out an output grid
  climoGrid.setAllMissing();

  // loop through and update counts and sums
  for (int i=0; i<climoGrid.getNdata(); ++i)
  {
    _normalize(i, climoGrid);
  }
  return true;
}

//-------------------------------------------------------------------------
void ClimoStateGrids::_decrement(int index, double value)
{
  if (!_good)
  {
    return;
  }

  double c, s;
  if (_counts->getValue(index, c) && _sums->getValue(index, s))
  {
    double newC = c - 1.0;
    double newS = s - value;
    if (newC <= 0)
    {
      _counts->setToMissing(index);
      _sums->setToMissing(index);
    }
    else
    {
      _counts->setv(index, newC);
      _sums->setv(index, newS);
    }
  }
  else
  {
    _counts->setToMissing(index);
    _sums->setToMissing(index);
  }
}

//-------------------------------------------------------------------------
void ClimoStateGrids::_increment(int index, double value)
{
  double c, s;
  if (_counts->getValue(index, c) && _sums->getValue(index, s))
  {
    _counts->setv(index, c+1.0);
    _sums->setv(index, s + value);
  }
  else
  {
    _counts->setv(index, 1.0);
    _sums->setv(index, value);
  }
}

//-------------------------------------------------------------------------
void ClimoStateGrids::_normalize(int index, Grid &climoGrid) const
{
  double c, s;
  if (_counts->getValue(index, c) && _sums->getValue(index, s))
  {
    if (c > 0)
    {
      climoGrid.setv(index, s/c);
    }
  }
}
