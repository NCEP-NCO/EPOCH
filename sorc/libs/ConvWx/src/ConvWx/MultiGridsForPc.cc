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
 * @file MultiGridsForPc.cc
 * @brief A phase correction volume box
 */

//----------------------------------------------------------------
#include <string>
#include <vector>
#include <ConvWx/MultiGridsForPc.hh>
#include <ConvWx/GridsForPc.hh>
#include <ConvWx/PcScore.hh>
using std::string;
using std::vector;

//----------------------------------------------------------------
MultiGridsForPc::MultiGridsForPc() :
  pGrids()
{
}

//----------------------------------------------------------------
MultiGridsForPc::~MultiGridsForPc()
{
}

//----------------------------------------------------------------
bool MultiGridsForPc::append(const GridsForPc &g)
{
  if (pGrids.empty())
  {
    pGrids.push_back(g);
  }
  else
  {
    if (!pGrids[0].dimensionsConsistent(g))
    {
      ILOG(ERROR, "dimensions inconsistent");
      return false;
    }
    pGrids.push_back(g);
  }
  return true;
}
  
//----------------------------------------------------------------
bool
MultiGridsForPc::hasEnoughInput(const double minPercentAboveThreshold) const
{
  for (int i=0; i<static_cast<int>(pGrids.size()); ++i)
  {
    if (pGrids[i].hasEnoughInput(minPercentAboveThreshold))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------
void MultiGridsForPc::setNoPc(void)
{
  for (int i=0; i<static_cast<int>(pGrids.size()); ++i)
  {
    pGrids[i].setNoPc();
  }
}

//----------------------------------------------------------------
bool MultiGridsForPc::reduce(const int res)
{
  bool ret = true;
  for (int i=0; i<static_cast<int>(pGrids.size()); ++i)
  {
    if (!pGrids[i].reduce(res))
    {
      ret = false;
    }
  }
  return ret;
}

//----------------------------------------------------------------
bool MultiGridsForPc::fractionalAreaTest(const int x0, const int y0,
					 const int *volSize, 
					 const double minPcnt) const
{
  for (int i=0; i<static_cast<int>(pGrids.size()); ++i)
  {
    if (!pGrids[i].fractionalAreaTest(x0, y0, volSize, minPcnt))
    {
      ILOGF(DEBUG_VERBOSE, 
	    "Fractional test indicated no p.c. for box at %d,%d", x0, y0);
      return false;
    }
  }
  ILOGF(DEBUG_VERBOSE,
	"Fractional tests indicate perform P.C. for box at %d,%d", x0, y0);
  return true;
}

//----------------------------------------------------------------
bool MultiGridsForPc::buildScores(const int x0, const int y0,
				  const int xoff, const int yoff,
				  const int *volSize,
				  vector<PcScore> &pcscore) const
{
  pcscore.clear();
  bool hasScore = false;
  for (int i=0; i<static_cast<int>(pGrids.size()); ++i)
  {
    PcScore pcs;
    if (pGrids[i].buildScore(x0, y0, xoff, yoff, volSize, pcs))
    {
      hasScore = true;
      pcscore.push_back(pcs);
    }
  }
  return hasScore;
}

//----------------------------------------------------------------
Grid MultiGridsForPc::initializeGrid(const string &name, const string &units,
				     const double value) const
{
  if (pGrids.empty())
  {
    ILOG(ERROR, "no grids for template");
    return Grid(name, units, 0, 0, -1);
  }
  else
  {
    return pGrids[0].initializeGrid(name, units, value);
  }
}

