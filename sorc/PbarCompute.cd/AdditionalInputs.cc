/**
 * @file AdditionalInputs.cc
 */

#include "AdditionalInputs.hh"
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/ConvWxTime.hh>

//----------------------------------------------------------------------------------------
AdditionalInputs::AdditionalInputs(const ParmsPbarCompute &parms) :
  _parms(parms), _hasInput1(false), _hasInput2(false)
{
}

//----------------------------------------------------------------------------------------
void AdditionalInputs::reset(void)
{
  if (_parms._hasFixedField1)
  {
    _hasInput1 = true;
    _input1 = AdditionalInput(_parms._inputFixedField1,
			      _parms._fixedFieldThresh1,
			      _parms._fixedFieldComparison1);
  }
  if (_parms._hasFixedField2)
  {
    _hasInput2 = true;
    _input2 = AdditionalInput(_parms._inputFixedField2,
			      _parms._fixedFieldThresh2,
			      _parms._fixedFieldComparison2);
  }
}

//----------------------------------------------------------------------------------------
bool AdditionalInputs::setGridPointers(const MultiGrid &mg)
{
  bool status = true;
  if (_hasInput1)
  {
    if (!_input1.setEnsembleGridPtr(mg))
    {
      LOG(ERROR) << "Setting pointer to ensemble grid data";
      status = false;
    }
  }
  if (_hasInput2)
  {
    if (!_input2.setEnsembleGridPtr(mg))
    {
      LOG(ERROR) << "Setting pointer to ensemble grid data";
      status = false;
    }
  }
  return status;
}

//----------------------------------------------------------------------------------------
bool AdditionalInputs::setAllValues(int gridIndex, int which)
{
  if (which == 1)
  {
    if (_hasInput1)
    {
      if (!_input1.getAndStoreEnsembleValueAtPoint(gridIndex))
      {
	return false;
      }
    }
  }
  if (which == 2)
  {
    if (_hasInput2)
    {
      if (!_input2.getAndStoreEnsembleValueAtPoint(gridIndex))
      {
	return false;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------
bool AdditionalInputs::passesTests(int which) const
{
  if (which == 1)
  {
    if (_hasInput1)
    {
      if (!_input1.passesTest())
      {
	return false;
      }
    }
  }
  if (which == 2)
  {
    if (_hasInput2)
    {
      if (!_input2.passesTest())
      {
	return false;
      }
    }
  }
  return true;
}

