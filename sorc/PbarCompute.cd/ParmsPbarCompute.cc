/**
 * @file ParmsPbarCompute.cc
 */
#include "ParmsPbarCompute.hh"

//----------------------------------------------------------------------
static bool _passesTest(double value, double thresh, Params::Comparison_t c)
{
  if (c == Params::GREATER_THAN_OR_EQUAL)
  {
    return value >= thresh;
  }
  else
  {
    return thresh >= value;
  }
}

//----------------------------------------------------------------------
int ParmsPbarCompute::index2d(int index1, int index2) const
{
  return index2*_numThresh1 + index1;;
}

//----------------------------------------------------------------------
bool ParmsPbarCompute::passesTests1(double value,
				    double currentThresh) const
{
  return _passesTest(value, currentThresh, _thresholdedFieldComparison1);
}
 
//----------------------------------------------------------------------
bool ParmsPbarCompute::passesTests2(double value1, double value2,
				    double currentThresh1,
				    double currentThresh2) const
{
  return (_passesTest(value1, currentThresh1, _thresholdedFieldComparison1) &&
	  _passesTest(value2, currentThresh2, _thresholdedFieldComparison2));
}
 
//----------------------------------------------------------------------
int ParmsPbarCompute::threshIndex(double thresh, int which) const
{
  double dIndex;
  if (which == 1)
  {
    dIndex = (thresh - _threshMin1)/_threshDelta1;
  }
  else
  {
    dIndex = (thresh - _threshMin2)/_threshDelta2;
  }
  return (int)dIndex;
}

//----------------------------------------------------------------------
double ParmsPbarCompute::thresholdFromIndex(int index, int which) const
{
  if (which == 1)
  {
    return _threshMin1 + ((double)index)*_threshDelta1;
  }
  else
  {    
    return _threshMin2 + ((double)index)*_threshDelta2;
  }
}


