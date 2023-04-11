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
 * @file NbrhdClumps.cc
 */
#include <ConvWx/NbrhdClumps.hh>
#include <ConvWx/ConvWxFuzzy.hh>
#include <ConvWx/Grid.hh>
using std::vector;

//----------------------------------------------------------------
NbrhdClumps::NbrhdClumps(void) :
  pClump(),
  pIclump(),
  pRemoveClump()
{
}

//----------------------------------------------------------------
NbrhdClumps::~NbrhdClumps()
{
}

//----------------------------------------------------------------
void NbrhdClumps::print(void) const
{
  vector<NbrhdClump>::const_iterator i;
  for (i=pClump.begin(); i!=pClump.end(); ++i)
  {
    i->print();
  }
}
  
//----------------------------------------------------------------
void NbrhdClumps::clear(void)
{
  pClump.clear();
  pRemoveClump.clear();
}

//----------------------------------------------------------------
void NbrhdClumps::startClump(const double value)
{
  NbrhdClump n(value);
  pClump.push_back(n);
  pIclump = pClump.rbegin();
}

//----------------------------------------------------------------
void NbrhdClumps::markPoint(const int index, const double value)
{
  pIclump->addPoint(index, value);
}

//----------------------------------------------------------------
void NbrhdClumps::merge(const vector<int> &clumps, const int minClumpNumPts)
{
  if (clumps.empty())
  {
    return;
  }

  // build the merged clump
  const NbrhdClump *finalClump = pMergedClumps(clumps);

  // decide whether to put onto remove list based on count
  int count = finalClump->npt();

  if (count < minClumpNumPts)
  {
    ILOGF(DEBUG_VERBOSE, "clump %lf after merge too small %d points",
	  minClumpNumPts, count);
    pRemoveClump.push_back(finalClump);
  }
  else
  {
    ILOGF(DEBUG_VERBOSE, "clump %lf after merge big enough %d points",
	  minClumpNumPts, count);
  }
}

//----------------------------------------------------------------
void NbrhdClumps::remove(Grid &dataGrid, const double missingValue) const
{
  vector<const NbrhdClump *>::const_iterator i;
  for (i=pRemoveClump.begin(); i!=pRemoveClump.end(); ++i)
  {
    for (int j=0; j<(*i)->npt(); ++j)
    {
      dataGrid.setv((*i)->ithGridIndex(j), missingValue);
    }
  }
}

//----------------------------------------------------------------
void NbrhdClumps::rescale(const ConvWxFuzzy &valueToScale,
			  Grid &scaleGrid) const
{
  // initialize scale to 1.0 everywhere
  scaleGrid.setAllToValue(1.0);

  for (int i=0; i<static_cast<int>(pRemoveClump.size()); ++i)
  {
    pRescale(valueToScale, *pRemoveClump[i], scaleGrid);
  }
}

//----------------------------------------------------------------
const NbrhdClump *NbrhdClumps::pMergedClumps(const vector<int> &clumps)
{
  const NbrhdClump *ret;
  if (clumps.size() == 1)
  {
    ret = pMatchingClump(clumps[0]);
  }
  else
  {
    // more than one, merge, create a new one, remove old ones that were
    // merged.
    NbrhdClump nnew(clumps[0]);
    for (int i=0; i<static_cast<int>(clumps.size()); ++i)
    {
      pMergePointsAndRemove(clumps[i], nnew);
    }
    pClump.push_back(nnew);
    pIclump = pClump.rbegin();
    ret = &(*pIclump);
  }
  return ret;
}

//----------------------------------------------------------------
const NbrhdClump *NbrhdClumps::pMatchingClump(const int clumpValue) const
{
  for (int i=0; i<static_cast<int>(pClump.size()); ++i)
  {
    if (pClump[i].clumpValue() == static_cast<double>(clumpValue))
    {
      return &pClump[i];
    }
  }
  ILOGF(ERROR, "never found %d", clumpValue);
  return NULL;
}

//----------------------------------------------------------------
void NbrhdClumps::pMergePointsAndRemove(double value, NbrhdClump &nnew)
{
  vector<NbrhdClump>::iterator i;
  for (i=pClump.begin(); i!=pClump.end(); ++i)
  {
    if (i->clumpValue() == value)
    {
      nnew.mergePoints(*i);
      pClump.erase(i);
      return;
    }
  }
  ILOGF(ERROR, "never found %lf", value);
}

//----------------------------------------------------------------
void NbrhdClumps::pRescale(const ConvWxFuzzy &valueToScale,
			   const NbrhdClump &clump,
			   Grid &scaleGrid) const
{
  // get the minimum data value in the clump
  double minV = clump.minClumpDataValue();

  double v = valueToScale.fuzzyF(minV);
  if (v != 1.0)
  {
    ILOGF(DEBUG_VERBOSE, "reducing clump %lf by %lf",
	  clump.clumpValue(), v);

    // store this value to scale grid which was initialized to 1.0 everywhere
    clump.writeClumpToGridWithValue(scaleGrid, v);
  }
}

