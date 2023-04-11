/**
 * @file OutputToThreshProj.cc
 */
#include "OutputToThreshProj.hh"
#include <ConvWx/ConvWxProjection.hh>
#include <toolsa/LogStream.hh>

//------------------------------------------------------------------------
OutputToThreshProj::OutputToThreshProj(const ParmProjection &threshParms,
				     const ParmProjection &outputParms) :
  _ok(true)
{
  ConvWxProjection threshProj, outputProj;

  // make sure things are ok to proceed
  if (!outputParms.isSuperset(threshParms))
  {
    LOG(ERROR) << "Projections are inconsistent";
    _ok = false;
    return;
  }
  if (outputParms.pNx != threshParms.pNx)
  {
    LOG(ERROR) << "projection extent east/west must be consistent and is not";
    _ok = false;
    return;
  }
  
  double maxThreshY = threshParms.pMiny +
    (double)(threshParms.pNy-1)*threshParms.pDy;
  double maxThreshYOutput = outputParms.pMiny +
    (double)(outputParms.pNy-1)*outputParms.pDy;

  _doesExtendSouth = threshParms.pMiny > outputParms.pMiny;
  _doesExtendNorth = maxThreshY < maxThreshYOutput;

  _minThreshYInOutputIndex=-1;
  _maxThreshYInOutputIndex=-1;
 for (int iy=0; iy<outputParms.pNy; ++iy)
 {
   double y = outputParms.pMiny + (double)iy*outputParms.pDy;
   if (y == threshParms.pMiny)
   {
     _minThreshYInOutputIndex = iy;
   }
   else if (y == maxThreshY)
   {
     _maxThreshYInOutputIndex = iy;
     break;
   }
 }

 if (_minThreshYInOutputIndex == -1 || _maxThreshYInOutputIndex == -1)
 {
   LOG(ERROR) << "Projections are out of alignment";
   _ok = false;
 }

}

//------------------------------------------------------------------------
OutputToThreshProj::~OutputToThreshProj(void)
{
}

//------------------------------------------------------------------------
bool OutputToThreshProj::isInsideSmallerDomain(int y, int &ySmall) const
{
  if (y < _minThreshYInOutputIndex)
  {
    return false;
  }
  else if (y > _maxThreshYInOutputIndex)
  {
    return false;
  }
  else
  {
    if (y == _minThreshYInOutputIndex)
    {
      if (_doesExtendSouth)
      {
	return false;
      }
      else
      {
	ySmall = 0;
	return true;
      }
    }
    else if (y == _maxThreshYInOutputIndex)
    {
      if (_doesExtendNorth)
      {
	return false;
      }
      else
      {
	ySmall = y - _minThreshYInOutputIndex;
	return true;
      }
    }
    else
    {
      ySmall = y - _minThreshYInOutputIndex;
      return true;
    }
  }
}

