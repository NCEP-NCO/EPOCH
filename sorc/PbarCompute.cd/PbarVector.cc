/**
 * @file PbarVector.cc
 */
#include "PbarVector.hh"
#include "ParmsPbarCompute.hh"
#include <toolsa/LogStream.hh>

//-------------------------------------------------------------
PbarVector::PbarVector(const ParmsPbarCompute &parm, int index) :
  _index(index)
{
  if (index == 1)
  {
    for (double thr = parm._threshMin1; thr <= parm._threshMax1; 
	 thr += parm._threshDelta1)
    {
      _pBar.push_back(-1.0);
      _threshPbar1.push_back(ThreshPbar1(thr, -1));
    }
  }
  else if (index == 2)
  {
    for (double thr1 = parm._threshMin1; thr1 <= parm._threshMax1; 
	 thr1 += parm._threshDelta1)
    {
      for (double thr2 = parm._threshMin2; thr2 <= parm._threshMax2; 
	 thr2 += parm._threshDelta2)
      {
	_pBar.push_back(-1.0);
	_threshPbar2.push_back(ThreshPbar2(thr1, thr2, -1));
      }
    }
  }
  // don't yet update _goodThreshPbar1 and 2
}

//-------------------------------------------------------------
PbarVector::~PbarVector()
{
}

//-------------------------------------------------------------
void PbarVector::setValue(int index, double value, int which)
{
  if (which == 1)
  {
    _pBar[index] = value;
    _threshPbar1[index]._pbar = value;
  }
  else
  {
    _pBar[index] = value;
    _threshPbar2[index]._pbar = value;
  }
  // don't yet update _goodThreshPbar1 and 2
}

//-------------------------------------------------------------
void PbarVector::setGood(int which) 
{
  if (which == 1)
  {
    _goodThreshPbar1.clear();
    for (size_t i=0; i<_threshPbar1.size(); ++i)
    {
      if (_threshPbar1[i]._pbar != -1)
      {
	_goodThreshPbar1.push_back(_threshPbar1[i]);
      }
    }
  }
  else
  {
    _goodThreshPbar2.clear();
    for (size_t i=0; i<_threshPbar2.size(); ++i)
    {
      if (_threshPbar2[i]._pbar != -1)
      {
	_goodThreshPbar2.push_back(_threshPbar2[i]);
      }
    }
  }
}

