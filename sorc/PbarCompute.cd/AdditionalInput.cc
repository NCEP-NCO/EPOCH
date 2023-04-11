/**
 * @file AdditionalInput.cc
 */
#include "AdditionalInput.hh"
#include <ConvWx/MultiGrid.hh>

//----------------------------------------------------------------------------------------
AdditionalInput::AdditionalInput()
{
}

//----------------------------------------------------------------------------------------
AdditionalInput::AdditionalInput(const std::string &ensembleFieldName,
				 double fixedThreshold,
				 Params::Comparison_t comparisonType) :
  _ensembleFieldName(ensembleFieldName),
  _comparisonType(comparisonType),
  _ensembleGridPtr(NULL),
  _threshold(fixedThreshold),
  _currentEnsembleValueSet(false),
  _currentEnsembleValue(-1)
{
}

//----------------------------------------------------------------------------------------
bool AdditionalInput::setEnsembleGridPtr(const MultiGrid &ensembleInputs)
{
  _ensembleGridPtr = ensembleInputs.constGridPtr(_ensembleFieldName);
  return _ensembleGridPtr != NULL;
}

//----------------------------------------------------------------------------------------
bool AdditionalInput::getAndStoreEnsembleValueAtPoint(int gridIndex)
{
  _currentEnsembleValueSet = false;
  if (_ensembleGridPtr == NULL)
  {
    return false;
  }
  else
  {
    if (!_ensembleGridPtr->getValue(gridIndex, _currentEnsembleValue))
    {
      return false;
    }	
    _currentEnsembleValueSet = true;
    return true;
  }
}

//----------------------------------------------------------------------------------------
bool AdditionalInput::passesTest(void) const
{
  if (_currentEnsembleValueSet)
  {
    if (_comparisonType == Params::GREATER_THAN_OR_EQUAL)
    {
      return _currentEnsembleValue >= _threshold;
    }
    else
    {
      return _threshold >= _currentEnsembleValue;
    }
  }
  else
  {
    return false;
  }
}

