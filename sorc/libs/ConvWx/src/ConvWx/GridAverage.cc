// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>
/**
 * @file GridAverage.cc
 * @brief GridAverage main class
 */
#include <ConvWx/GridAverage.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ConvWxFuzzy.hh>
#include <vector>

using std::vector;

//----------------------------------------------------------------
GridAverage::GridAverage(void) :
  pNumGrids(0),
  pNumSum(0),
  pBackgroundWeight(0),
  pWeights(false),
  pNoWeights(true)
{
}

//----------------------------------------------------------------
GridAverage::GridAverage(const ParmProjection &p, const int num) :
  pProj(p),
  pNumGrids(num),
  pNumSum(0),
  pBackgroundWeight(0),
  pWeights(false),
  pNoWeights(true)
{
}

//----------------------------------------------------------------
GridAverage::~GridAverage()
{
}

//----------------------------------------------------------------
void GridAverage::accumulateWeightsInfo(const time_t &gt, int lt,
					const ParmFcstIO &input,
					int toleranceSeconds)
{
  if (pNoWeights)
  {
    pNoWeights = false;
    pWtNum = Grid("Num", "none", pProj.pNx, pProj.pNy, -1.0);
    pWtAve = Grid("Ave", "none", pProj.pNx, pProj.pNy, -1.0);
  }

  Grid wtg;
  if (loadWeightsData(gt, lt, input, pProj, toleranceSeconds, wtg, true))
  {
    for (int i=0; i<wtg.getNdata(); ++i)
    {
      double v;
      if (wtg.getValue(i, v))
      {
	pWtNum.incrementValueAtPoint(i, 1.0);
	pWtAve.incrementValueAtPoint(i, v);
      }
    }
  }
}

//----------------------------------------------------------------
void GridAverage::accumulateWeightsInfo(const time_t &gt, const int lt,
					const std::string &url,
					const std::string &field,
					const bool remap)
{
  if (pNoWeights)
  {
    pNoWeights = false;
    pWtNum = Grid("Num", "none", pProj.pNx, pProj.pNy, -1.0);
    pWtAve = Grid("Ave", "none", pProj.pNx, pProj.pNy, -1.0);
  }

  FcstGrid wtg;
  if (InterfaceIO::loadFcst(gt, lt, pProj, url, field, remap, wtg))
  {
    for (int i=0; i<wtg.getNdata(); ++i)
    {
      double v;
      if (wtg.getValue(i, v))
      {
	pWtNum.incrementValueAtPoint(i, 1.0);
	pWtAve.incrementValueAtPoint(i, v);
      }
    }
  }
}

//----------------------------------------------------------------
void GridAverage::finishAccumulateWeightsInfo(void)
{
  pWtAve.divide(pWtNum);
  pWeights = true;
}

//----------------------------------------------------------------
void GridAverage::setBackgroundWeight(double v, const ConvWxFuzzy &f)
{						   
  if (f.num() > 0)
  {
    pBackgroundWeight = f.fuzzyF(v);
  }
  else
  {
    pBackgroundWeight = 1.0;
  }
}

//----------------------------------------------------------------
void GridAverage::setBackgroundWeight(double v)
{						   
  pBackgroundWeight = v;
}

//----------------------------------------------------------------
void GridAverage::init(void)
{
  pNumSum = 0;
  pNum = MultiGrid();
  pSum = MultiGrid();
  pCount = MultiGrid();
}

//----------------------------------------------------------------
bool GridAverage::accumulate(const MultiGrid &grid)
{
  if (pNumSum == 0)
  {
    pInit(grid);
  }

  for (int i=0; i<grid.num(); ++i)
  {
    const Grid *g = grid.ithConstGrid(i);
    Grid *gNum = pNum.ithGrid(i);
    Grid *gSum = pSum.ithGrid(i);
    Grid *gCount = pCount.ithGrid(i);
    pAccumulate(*g, *gCount, *gNum, *gSum);
  }
  pNumSum += 1;
  return true;
}

//----------------------------------------------------------------
bool GridAverage::accumulate(const MultiGrid &grid, const Grid &weights)
{
  if (!pWeights)
  {
    ILOG(ERROR, "Weights info not set");
    return false;
  }

  if (pNumSum == 0)
  {
    pInit(grid);
  }

  for (int i=0; i<grid.num(); ++i)
  {
    // note that these must be lined up, based on the constructor,
    // and on the calling method's way of setting the input grid
    const Grid *g = grid.ithConstGrid(i);
    Grid *gNum = pNum.ithGrid(i);
    Grid *gSum = pSum.ithGrid(i);
    Grid *gCount = pCount.ithGrid(i);
    pAccumulate(*g, weights, *gCount, *gNum, *gSum);
  }
  pNumSum += 1;
  return true;
}

//----------------------------------------------------------------
bool GridAverage::average(const int minFcsts, MultiGrid &mg)
{
  bool ave = (pNumSum >= minFcsts);
  mg = MultiGrid();
  bool ret;
  if (ave)
  {
    ILOGF(DEBUG, "Taking average of %d things for output", pNumSum);
    ret = true;
  }
  else
  {
    ILOGF(WARNING, "%d fields, not enough. Set all output missing",
	  pNumSum);
    ret = false;
  }
  for (int i=0; i< pNum.num(); ++i)
  {
    Grid *gNum = pNum.ithGrid(i);
    Grid *gSum = pSum.ithGrid(i);
    Grid *gCount = pCount.ithGrid(i);
    if (ave)
    {
      pAverage(minFcsts, *gCount, *gNum, *gSum, ret);
    }
    else
    {
      gSum->setAllMissing();
    }
    mg.append(*gSum);
  }
  return ret;
}

//----------------------------------------------------------------
bool GridAverage::loadWeightsData(const time_t &gt, int lt,
				  const ParmFcstIO &input,
				  const ParmProjection &proj,
				  int toleranceSeconds,
				  Grid &wtg, bool debug)
{
  time_t t0 = gt - toleranceSeconds;
  vector<time_t> vgt =
    InterfaceIO::availableFcstGenTimesInARange(input.pUrl, t0, gt);
  if (vgt.empty())
  {
    ILOG(WARNING, "No Weights data within tolerance of trigger time");
    return false;
  }

  time_t wt = *(vgt.rbegin());
  FcstGrid fg;
  if (input.loadFcstData(wt, lt, proj, fg))
  {
    if (debug)
    {
      ILOGF(DEBUG, "Weights data loaded, %s+%d", 
	    ConvWxTime::stime(wt).c_str(), lt);
    }
    wtg = fg;
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------
bool GridAverage::findBestWeightsGenTime(const time_t &gt, int lt,
					 const std::string &url, 
					 int toleranceSeconds,
					 time_t &weightsGt)
{
  time_t t0 = gt - toleranceSeconds;
  vector<time_t> vgt =
    InterfaceIO::availableFcstGenTimesInARange(url, t0, gt);
  if (vgt.empty())
  {
    ILOG(WARNING, "No Weights data within tolerance of trigger time");
    return false;
  }
  // use the time with the most lead times available
  int max=0;
  for (size_t i=0; i<vgt.size(); ++i)
  {
    vector<time_t> valt = InterfaceIO::getAvailableValidTimes(url, vgt[i]);

    int nlead = static_cast<int>(valt.size());
    if (nlead >= max)  // >= so that get latest gen time
    {
      max = nlead;
      weightsGt = vgt[i];
    }
  }
  ILOGF(DEBUG, "Weights gen time set to %s",
	ConvWxTime::stime(weightsGt).c_str());
  return true;

}

//----------------------------------------------------------------
void GridAverage::pAccumulate(const Grid &g, Grid &count, Grid &num, Grid &sum)
{
  for (int i=0; i<g.getNdata(); ++i)
  {
    double v;
    if (g.getValue(i, v))
    {
      count.incrementValueAtPoint(i, 1.0);
      num.incrementValueAtPoint(i, pBackgroundWeight);
      sum.incrementValueAtPoint(i, v*pBackgroundWeight);
    }
  }
}


//----------------------------------------------------------------
void GridAverage::pAccumulate(const Grid &g, const Grid &weight, 
			      Grid &count, Grid &num, Grid &sum)
{
  for (int i=0; i<g.getNdata(); ++i)
  {
    double w = pDetermineWeight(i, weight);
    if (w >= 0.0)
    {
      double v;
      if (g.getValue(i, v))
      {
	count.incrementValueAtPoint(i, 1.0);
	num.incrementValueAtPoint(i, w);
	sum.incrementValueAtPoint(i, v*w);
      }
    }
  }
}

//----------------------------------------------------------------
void GridAverage::pInit(const MultiGrid &grid)
{
  pNum = MultiGrid();
  pSum = MultiGrid();
  pCount = MultiGrid();
  for (int i=0; i<grid.num(); ++i)
  {
    pNum.append(Grid(grid.ithGridName(i) + "_Num", "none", pProj.pNx, pProj.pNy,
		     -1.0));
    pSum.append(Grid(grid.ithGridName(i), "none",  pProj.pNx, pProj.pNy, -1.0));
    pCount.append(Grid(grid.ithGridName(i) + "_Count", "none", pProj.pNx,
		       pProj.pNy, -1.0));
  }
}

//----------------------------------------------------------------
double GridAverage::pDetermineWeight(const int i, const Grid &weight) const
{
  double n;
  if (!pWtNum.getValue(i, n))
  {
    // use background weight
    return pBackgroundWeight;
  }

  if (n == pNumGrids)
  {
    // expect all weights grids to have data
    double w;
    if (weight.getValue(i, w))
    {
      return w;
    }
    else
    {
      ILOGF(ERROR, "Expected all weights at %d", i);
      return -1.0;
    }
  }
  else if (n > 0 && n < pNumGrids)
  {
    // use this weight if its there, otherwise use average
    double w;
    if (weight.getValue(i, w))
    {
      return w;
    }
    else
    {
      if (pWtAve.getValue(i, w))
      {
	return w;
      }
      else
      {
	ILOGF(ERROR, "Expected non-missing average at %d", i);
	return -1.0;
      }
    }
  }
  else
  {
    // use background weight
    return pBackgroundWeight;
  }
}

//----------------------------------------------------------------
void GridAverage::pAverage(const int minForecasts, const Grid &count,
			   const Grid &num, Grid &sum, bool &valset)
{
  for (int i=0; i<num.getNdata(); ++i)
  {
    double v, c, n;
    if (count.getValue(i, c) && sum.getValue(i, v) && num.getValue(i, n))
    {
      if (static_cast<int>(c) >= minForecasts && fabs(n) > 1.0e-6)
      {
	sum.setv(i, v/n);
	valset = true;
      }
      else
      {
	sum.setToMissing(i);
      }
    }
    else
    {
      sum.setToMissing(i);
    }
  }
}
