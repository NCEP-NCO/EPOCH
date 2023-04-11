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
 * @file Nbrhd.cc
 */
#include <vector>
#include <ConvWx/Nbrhd.hh>
#include <ConvWx/Merge.hh>
#include <ConvWx/ConvWxFuzzy.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::vector;

//----------------------------------------------------------------
Nbrhd::Nbrhd(int nx, int ny, int maxClumpPts) :
  pValues(),
  pClump(),
  pClumpGrid("clumps", "none", nx, ny, pValues.dBad()),
  pScaleGrid("scale", "none", nx, ny, convWx::FCST_MISSING),
  pHasThresh(false),
  pThresh(0),
  pNx(nx),
  pNy(ny),
  pMaxClumpPts(maxClumpPts),
  pMerge(NULL)
{
  pSetToEmpty();
}

//----------------------------------------------------------------
Nbrhd::Nbrhd(int nx, int ny, double clumpThreshold, int maxClumpPts) :
  pValues(),
  pClump(),
  pClumpGrid("clumps", "none", nx, ny, pValues.dBad()),
  pScaleGrid("scale", "none", nx, ny, convWx::FCST_MISSING),
  pHasThresh(true),
  pThresh(clumpThreshold),
  pNx(nx),
  pNy(ny),
  pMaxClumpPts(maxClumpPts),
  pMerge(NULL)
{
  pSetToEmpty();
}

//----------------------------------------------------------------
Nbrhd::~Nbrhd()
{
}

//----------------------------------------------------------------
void Nbrhd::print(void) const
{
  ILOGF(FORCE, "Clumped reg:(thrsh:%d,%.2f), nextr:(%.2f a:%.2f):",
	pHasThresh, pThresh, pValues.dCurrentClumpValue(), 
	pValues.currentClumpArea());
}

//----------------------------------------------------------------
void Nbrhd::clumpsToImage(Grid &clumpGrid) const
{
  clumpGrid = pClumpGrid;
}

//----------------------------------------------------------------
void Nbrhd::scaleToImage(Grid &scaleGrid) const
{
  scaleGrid = pScaleGrid;
}

//----------------------------------------------------------------
void Nbrhd::buildClumps(const Grid &dataGrid, double minClumpNumPoints)
{
  // build all the clumps.
  pBuildWithoutRemove(dataGrid, minClumpNumPoints);

  // at all points where data is one of the 'remove' values, remove it.
  pClump.remove(pClumpGrid, pValues.dBad());
}

//----------------------------------------------------------------
void Nbrhd::buildClumps(const Grid &dataGrid, double minClumpNumPoints,
			const ConvWxFuzzy &valueToScale)
{
  // build all the clumps.
  pBuildWithoutRemove(dataGrid, minClumpNumPoints);

  // for each 'remove' value, (small clump) go through and set the
  // pScaleGrid value.
  pClump.rescale(valueToScale, pScaleGrid);
}

//----------------------------------------------------------------
void Nbrhd::pSetToEmpty(void)
{
  //sets current clump value to initial value
  pValues.init(pClumpGrid);

  // clear out all clumps.
  pClump.clear();

  // start a clump at the initial value
  pClump.startClump(pValues.dCurrentClumpValue());
}

//----------------------------------------------------------------
void Nbrhd::pBuildWithoutRemove(const Grid &dataGrid, double minClumpNumPoints)
{
  bool hasClumps = false;

  // clear out everything
  pSetToEmpty();

  // build the clumps.
  for (int y=0; y<pNy; ++y)
  {
    for (int x=0; x<pNx; ++x)
    {
      if (pPointChecked(x, y))
      {
	continue;
      }
      if (pBuildClump(dataGrid, x, y))
      {
	// increment clump value and start a new clump at this value.
	pValues.incrementClumpValue();
	pClump.startClump(pValues.dCurrentClumpValue());
	hasClumps = true;
      }
    }
  }

  // if nothing happened done.
  if (!hasClumps)
  {
    ILOG(DEBUG_VERBOSE, "no clumps");
    return;
  }
  else
  {
    // otherwise, merge adjacent clumps 
    pMergeAdjacent(static_cast<int>(minClumpNumPoints));
  }
}

//----------------------------------------------------------------
bool Nbrhd::pBuildClump(const Grid &dataGrid, int ix, int iy)
{
  int ipt = iy*pNx + ix;

  if (!pStatus1(dataGrid, ipt))
  {
    // never want this point, indicate so.
    pValues.markPointNotWanted(ix, iy, pClumpGrid);
    return false;
  }
  else
  {
    // build from here out.
    ILOG(DEBUG_VERBOSE, "grow recursive");
    pGrowRecursive(dataGrid, ix, iy);
    ILOG(DEBUG_VERBOSE, "done grow recursive");
    if (pValues.currentClumpArea() >= pMaxClumpPts)
    {
      ILOGF(DEBUG_VERBOSE, "CLUMP %lf did overflow %lf points",
	    pValues.dCurrentClumpValue(), pValues.currentClumpArea());
    }
    else
    {
      ILOGF(DEBUG_VERBOSE, "CLUMP %lf normal %lf points", 
	    pValues.dCurrentClumpValue(), pValues.currentClumpArea());
    }
    return true;
  }

}

//----------------------------------------------------------------
void Nbrhd::pMergeAdjacent(const int minClumpNumPoints)
{
  // create a merge object (pointed to by static pointer above)
  int n = pValues.numClumps();
  if (n <= 1)
  {
    // no clumps or one
    return;
  }

  // prevent memory leak
  if (pMerge != NULL)
  {
    delete pMerge;
  }

  // the clump values are 1,2,3,...,n
  // look at Merge interface, and see you need a slot for index=0 which
  // we don't have, so need one more, allowing 0,1,2,...,n
  pMerge = new Merge(n+1);

  // look for places where two different clump numbers are adjacent and
  // merge them into one value (minimum value)
  for (int y=0; y<pNy; ++y)
  {
    for (int x=0; x<pNx; ++x)
    {
      if (pValues.isInAClump(pClumpGrid, x, y))
      {
	pMergeAdjacent(x, y);
      }
    }
  }

  // now turn the Merge object loose to build up combined sets.
  pMerge->merge();

  // for each set (which is merged or not merged)
  for (int i=0; i<pMerge->numMerged(); ++i)
  {
    // get its ordered list of individual indices (clump values cast to int's)
    vector<int> v = pMerge->getIthMerged(i);

    // merge this ordered list of indices clumps into one.
    pDoMerge(v, minClumpNumPoints);
  }

  delete pMerge;
  pMerge = NULL;
}

//----------------------------------------------------------------
void Nbrhd::pGrowRecursive(const Grid &dataGrid, int x, int y)
{
  int ix, iy, ipt;

  pMarkPoint(dataGrid, x, y);

  // Recursively add neighboring  points
  int delta = 1;
  for ( iy=y-delta; iy<=y+delta; ++iy )
  {
    for ( ix=x-delta; ix<=x+delta; ++ix )
    {
      if (!pGrowOk(ix, iy, x, y))
      {
	continue;
      }
      ipt = iy*pNx + ix;
      if (!pStatus1(dataGrid, ipt))
      {
	pValues.markPointNotWanted(ix, iy, pClumpGrid);
	continue;
      }
      pGrowRecursive(dataGrid, ix, iy);
    }
  }
}

//----------------------------------------------------------------
void Nbrhd::pDoMerge(const vector<int> &clumps, const int minClumpNumPoints)
{
  if (clumps.empty())
  {
    return;
  }

  // clumps is an ordered list from Merge
  // take minimum clump value as the output clump value
  double minv = static_cast<double>(clumps[0]);
  if (minv == 0.0)
  {
    // if minimum is 0 and we have other values, it doesn't make sense,
    // because we have no clumps with value 0.
    if (clumps.size() != 1)
    {
      ILOGF(ERROR, "clump=0, with %d matches",
	    static_cast<int>(clumps.size()));
    }
    return;
  }

  // build the merged clumps
  pClump.merge(clumps, minClumpNumPoints);
}

//----------------------------------------------------------------
// loop through adjacent points looking for ones that have a value
// different than value at ix,iy
void Nbrhd::pMergeAdjacent(const int ix, const int iy)
{
  double minValue;

  if (!pClumpGrid.getValue(ix, iy, minValue))
  {
    ILOG(ERROR, "logic");
    return;
  }
  for (int y=iy-1; y<=iy+1; ++y)
  {
    if (y < 0 || y >= pNy)
    {
      continue;
    }
    for (int x=ix-1; x<=ix+1; ++x)
    {
      if (x < 0 || x >= pNx)
      {
	continue;
      }
      if (x == ix && y == iy)
      {
	continue;
      }
      double v;
      if (pClumpGrid.getValue(x, y, v))
      {
	if (pValues.isInAClump(v))
	{
	  if (v != minValue)
	  {
	    // here is an adjacent point that touches x,y
	    // but has a different clump value, just what is wanted
	    pMerge->update(static_cast<int>(minValue), static_cast<int>(v));
	  }
	}
      }
    }
  }
}

//----------------------------------------------------------------
bool Nbrhd::pStatus1(const Grid &dataGrid, const int ipt)
{
  double v;
  if (dataGrid.getValue(ipt, v))
  {
    return ((pHasThresh && v >= pThresh) || (!pHasThresh));
  }
  else
  {
    return false;
  }
}


