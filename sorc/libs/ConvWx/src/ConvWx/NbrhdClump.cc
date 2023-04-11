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
 * @file NbrhdClump.cc
 */
#include <ConvWx/NbrhdClump.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/ConvWxConstants.hh>

//----------------------------------------------------------------
NbrhdClump::NbrhdClump(const double value) :
  pClumpValue(value),
  pNumPoint(0),
  pExtraGridIndex(),
  pMinDataValue(convWx::FCST_MISSING)
{
  for (int i=0; i<NUM_ARRAY_CLUMP; ++i)
  {
    pGridIndex[i] = convWx::BAD_INDEX;
  }
}

//----------------------------------------------------------------
NbrhdClump::~NbrhdClump()
{
  pNumPoint = 0;
  for (int i=0; i<NUM_ARRAY_CLUMP; ++i)
  {
    pGridIndex[i] = convWx::BAD_INDEX;
  }
}

//----------------------------------------------------------------
void NbrhdClump::print(void) const
{
  printf("clump[%.2lf] minv:%.2lf\n", pClumpValue, pMinDataValue);
  int count = 0;
  for (int i=0; i<pNumPoint; ++i)
  {
    if (i < NUM_ARRAY_CLUMP)
    {
      printf("%d ", pGridIndex[i]);
    }
    else
    {
      printf("%d ", pExtraGridIndex[NUM_ARRAY_CLUMP-i]);
    }
    if (++count > 10)
    {
      printf("\n");
      count = 0;
    }
  }
  if (count > 0)
  {
    printf("\n"); 
  }
}

//----------------------------------------------------------------
void NbrhdClump::addPoint(const int i, const double dataValue)
{
  if (pNumPoint == 0)
  {
    pMinDataValue = dataValue;
  }
  else
  {
    if (dataValue < pMinDataValue)
    {
      pMinDataValue = dataValue;
    }
  }
  pAddPoint(i);
}

//----------------------------------------------------------------
void NbrhdClump::mergePoints(const NbrhdClump &mergeData)
{
  if (pNumPoint > 0)
  {
    if (mergeData.pMinDataValue < pMinDataValue)
    {
      pMinDataValue = mergeData.pMinDataValue;
    }
  }
  else
  {
    pMinDataValue = mergeData.pMinDataValue;
  }

  for (int i=0; i<mergeData.pNumPoint; ++i)
  {
    pAddPoint(mergeData.ithGridIndex(i));
  }
}

//----------------------------------------------------------------
int NbrhdClump::ithGridIndex(const int i) const
{
  if (i >= pNumPoint)
  {
    ILOGF(ERROR, "out of range [0,%d] got %d", pNumPoint-1, i);
    return 0;
  }
  if (i < NUM_ARRAY_CLUMP)
  {
    return pGridIndex[i];
  }
  else
  {
    return pExtraGridIndex[NUM_ARRAY_CLUMP-i];
  }
}

//----------------------------------------------------------------
void NbrhdClump::writeClumpToGridWithValue(Grid &g, const double v) const
{
  for (int i=0; i<pNumPoint; ++i)
  {
    if (i < NUM_ARRAY_CLUMP)
    {
      g.setv(pGridIndex[i], v);
    }
    else
    {
      g.setv(pExtraGridIndex[NUM_ARRAY_CLUMP-i], v);
    }
  }
}

//----------------------------------------------------------------
void NbrhdClump::pAddPoint(const int i)
{
  if (pNumPoint < NUM_ARRAY_CLUMP)
  {
    pGridIndex[pNumPoint++] = i;
  }
  else
  {
    pExtraGridIndex.push_back(i);
    ++pNumPoint;
  }
}

