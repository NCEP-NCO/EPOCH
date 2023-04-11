/**
 * @file PbarVector.cc
 */
#include "PbarVector.hh"
#include "ParmsThreshFromObarPbar.hh"
#include <toolsa/LogStream.hh>
#include <cmath>

//-------------------------------------------------------------
PbarVector::PbarVector(double targetBias,
		       const std::vector<double> &thresh) :
  _targetBias(targetBias)
{
  for (size_t i=0; i<thresh.size(); ++i)
  {
    double thr = thresh[i];
    _pBar.push_back(-1.0);
    _threshPbar.push_back(ThreshPbar(thr, i,  -1));
  }
  // leave 'good' empty
}

//-------------------------------------------------------------
PbarVector::~PbarVector()
{
}

//-------------------------------------------------------------
void PbarVector::setValue(int index, double value)
{
  _pBar[index] = value;
  _threshPbar[index]._pbar = value;
  // don't yet update _goodThreshPbar
}

//-------------------------------------------------------------
void PbarVector::setGood(void) 
{
  _goodThreshPbar.clear();
  for (size_t i=0; i<_threshPbar.size(); ++i)
  {
    if (_threshPbar[i]._pbar != -1)
    {
      _goodThreshPbar.push_back(_threshPbar[i]);
    }
  }
}

//-------------------------------------------------------------
bool PbarVector::noGood(void) const
{
  return _goodThreshPbar.empty();
  
}

//----------------------------------------------------------------------------------
bool PbarVector::bestWhenObsZero(bool isGreaterOrEqualTest, double &thresh, double &bias, int &pbarIndex) const
{
  if (_goodThreshPbar.empty())
  {
    return false;
  }
  if (_allZero())
  {
    return false;
  }
  if (isGreaterOrEqualTest)
  {
    // since this is >=, expect pbars to go down with threshold, so 0th good one should
    // be the biggest pbar
    if (_goodThreshPbar[0]._pbar == 0)
    {
      LOG(ERROR) << "Expect 0th pbar to be > 0 based on logic, but it is not";
      return false;
    }
    
    // look higher for either 0 or go all the way up
    for (size_t i=1; i<_goodThreshPbar.size(); ++i)
    {
      if (_goodThreshPbar[i]._pbar == 0)
      {
	// this is it
	thresh = _goodThreshPbar[i]._thresh;
	bias = 0.0;  // pbar=0 and obar=0
	pbarIndex = _goodThreshPbar[i]._index;
	return true;
      }
    }
    // never saw a 0 so use biggest one
    int n = (int)_goodThreshPbar.size();
    thresh = _goodThreshPbar[n-1]._thresh;
    bias = _goodThreshPbar[n-1].bias(0.0);
    pbarIndex = _goodThreshPbar[n-1]._index;
    return true;
  }
  else
  {
    // since this is <=, expect pbars to go up with threshold, so last good one should
    // be the biggest pbar
    int n = (int)_goodThreshPbar.size();
    if (_goodThreshPbar[n-1]._pbar == 0)
    {
      LOG(ERROR) << "Expect highest pbar to be > 0 based on logic, but it is not";
      return false;
    }
    
    // look lower for either 0 or go all the way down
    for (int i=n-1; i>=0; --i)
    {
      if (_goodThreshPbar[i]._pbar == 0)
      {
	// this is it
	thresh = _goodThreshPbar[i]._thresh;
	bias = 0.0;  // pbar=0 and obar=0
	pbarIndex = _goodThreshPbar[i]._index;
	return true;
      }
    }
    // never saw a 0 so use smallest one
    thresh = _goodThreshPbar[0]._thresh;
    bias = _goodThreshPbar[0].bias(0.0);
    pbarIndex = _goodThreshPbar[0]._index;
    return true;
  }
}

//-----------------------------------------------------------------------
bool PbarVector::bestWhenObsNonZero(double obar, double currentThresh,
				    bool isGreaterOrEqualTest,
				    double &thresh, double &bias,
				    int &pbarIndex) const
{  
  // see if the pbar for the current thresh is zero or not
  bool currentIsZero = false;
  int currentIndex = -1;
  for (size_t i=0; i<_goodThreshPbar.size(); ++i)
  {
    if (_goodThreshPbar[i]._thresh == currentThresh)
    {
      if (_goodThreshPbar[i]._pbar == 0.0)
      {
	currentIsZero = true;
	currentIndex = (int)i;
	break;
      }
    }
  }
  
  if (currentIsZero)
  {
    return _bestWhenObsNonZeroAndCurrentBestIsZero(obar, currentIndex,
						   isGreaterOrEqualTest,
						   thresh, bias, pbarIndex);
  }
  else
  {
    return _bestSearchAll(obar, thresh, bias, pbarIndex);
  }
}

//-----------------------------------------------------------------------
bool PbarVector::
_bestWhenObsNonZeroAndCurrentBestIsZero(double oBar,  int startingInd,
					bool isGreaterOrEqualTest, 
					double &thresh, double &bias,
					int &pbarIndex) const
{
  if (isGreaterOrEqualTest)
  {
    if (_allAreZeroBelow(startingInd))
    {
      // use smallest one
      thresh = _goodThreshPbar[0]._thresh;
      bias = _goodThreshPbar[0].bias(oBar);
      pbarIndex = _goodThreshPbar[0]._index;
    }
    else
    {
      // use one that minimizes bias for all thresholds <= starting
      // note that pbar at 0 should be nonzero for sure.
      thresh = _goodThreshPbar[0]._thresh;
      bias = _goodThreshPbar[0].bias(oBar);
      pbarIndex = _goodThreshPbar[0]._index;
      for (int i=1; i<= startingInd; ++i)
      {
	double pbar = _goodThreshPbar[i]._pbar;
	if (pbar > 0)
	{
	  double biasi = pbar - oBar;
	  if (fabs(biasi - _targetBias) < fabs(bias - _targetBias))
	  {
	    thresh = _goodThreshPbar[i]._thresh;
	    bias = biasi;
	    pbarIndex = _goodThreshPbar[i]._index;
	  }
	}
      }
    }
  }
  else
  {
    int n = (int)_goodThreshPbar.size();
    if (_allAreZeroAbove(startingInd))
    {
      // use largest one
      thresh = _goodThreshPbar[n-1]._thresh;
      bias = _goodThreshPbar[n-1].bias(oBar);
      pbarIndex = _goodThreshPbar[n-1]._index;
    }
    else
    {
      // use the one that minimizes bias for all thresholds >= starting
      // note that pbar at highest threshold should be nonzero for sure
      thresh = _goodThreshPbar[n-1]._thresh;
      bias = _goodThreshPbar[n-1].bias(oBar);
      pbarIndex = _goodThreshPbar[n-1]._index;
      for (int i=n-2; i>=startingInd; --i)
      {
	double pbar = _goodThreshPbar[i]._pbar;
	if (pbar > 0)
	{
	  double biasi = pbar - oBar;
	  if (fabs(biasi - _targetBias) < fabs(bias - _targetBias))
	  {
	    thresh = _goodThreshPbar[i]._thresh;
	    pbarIndex = _goodThreshPbar[i]._index;
	    bias = biasi;
	  }
	}
      }
    }
  }
  return true;
}

//-----------------------------------------------------------------------
bool PbarVector::
_bestSearchAll(double oBar, double &thresh, double &bias,
	       int &pbarIndex) const
{
  if (_goodThreshPbar.empty())
  {
    return false;
  }
  bias = _goodThreshPbar[0].bias(oBar);
  thresh = _goodThreshPbar[0]._thresh;
  pbarIndex = _goodThreshPbar[0]._index;
  for (size_t i=1; i<_goodThreshPbar.size(); ++i)
  {
    double biasi = _goodThreshPbar[i].bias(oBar);
    if (fabs(biasi - _targetBias) < fabs(bias - _targetBias))
    {
      bias = biasi;
      thresh = _goodThreshPbar[i]._thresh;
      pbarIndex = _goodThreshPbar[i]._index;
    }
  }
  return true;
}


//-------------------------------------------------------------
bool PbarVector::_allAreZeroBelow(int ind) const
{
  int n = _goodThreshPbar.size();
  for (int i=0; i<ind && i < n; ++i)
  {
    if (_goodThreshPbar[i]._pbar != 0)
    {
      return false;
    }
  }
  return true;
}

//-------------------------------------------------------------
bool PbarVector::_allAreZeroAbove(int ind) const
{
  int n = (int)_goodThreshPbar.size();
  for (int i=n-1; i>ind; --i)
  {
    if (_goodThreshPbar[i]._pbar != 0)
    {
      return false;
    }
  }
  return true;
}

//-------------------------------------------------------------
bool PbarVector::_allZero(void) const
{
  for (size_t i=0; i<_goodThreshPbar.size(); ++i)
  {
    if (_goodThreshPbar[i]._pbar != 0)
    {
      return false;
    }
  }
  return !_goodThreshPbar.empty();
}


