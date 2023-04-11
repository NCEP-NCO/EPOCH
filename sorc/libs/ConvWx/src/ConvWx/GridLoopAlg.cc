/**
 * @file GridLoopAlg.cc
 */
#include <ConvWx/GridLoopAlg.hh>
#include <ConvWx/GridData.hh>
#include <cmath>

//-----------------------------------------------------------------
void GridLoopAlgMean::increment(int x, int y, const GridData &g)
{
  double v;
  if (g.getValue(x, y, v))
  {
    if (pIsExclude && v == pExcludeValue)
    {
      ++pNumExclude;
    }
    else
    {
      pSum += v;
      pCount ++;
    }
  }
}

//-----------------------------------------------------------------
void GridLoopAlgMean::decrement(int x, int y, const GridData &g)
{
  double v;
  if (g.getValue(x, y, v))
  {
    if (pIsExclude && v == pExcludeValue)
    {
      --pNumExclude;
    }
    else
    {
      pSum -= v;
      pCount--;
    }
  }
}

//-----------------------------------------------------------------
bool GridLoopAlgMean::getResult(int minGood, const GridData &g,
				int x, int y, double &result) const
{
  if (pIsExclude && pRejectCenterExclude)
  {
    if (g.isEqualAt(x, y, pExcludeValue))
    {
      result = pExcludeValue;
      return true;
    }
  }

  if (pCount > minGood)
  {
    // prevent machine roundoff errors here
    double s = pSum/pCount;
    double f = 1.0;
    if (s < 0)
    {
      f = -1.0;
      s = -s;
    }
    s = s*1000000.0;
    s = floor(s);
    s = s/1000000.0;
    if (s != 0.0 && f != 1.0)
    {
      s = s*f;
    }
    result = s;
    return true;
  }
  else
  {
    if (pIsExclude && !pRejectCenterExclude && pNumExclude > 0)
    {
      result = pExcludeValue;
      return true;
    }
    else
    {
      return false;
    }
  }
}
