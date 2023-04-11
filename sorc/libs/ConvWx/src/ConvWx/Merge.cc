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
 * @file Merge.cc
 * @brief 
 */
#include <cstdio>
#include <vector>
#include <algorithm>
#include <ConvWx/Merge.hh>

using std::vector;

//----------------------------------------------------------------
/**
 * Insert input into a vector, so that results before and after are
 * increasing values (no duplicates too).
 * @param[in] j  Value to insert
 * @param[in] v   Vector to insert into
 */
static void sInsertElement(const int j, vector<int> &v)
{
  if (v.empty())
  {
    v.push_back(j);
    return;
  }

  if (find(v.begin(), v.end(), j) != v.end())
  {
    return;
  }

  if (j < v[0])
  {
    v.insert(v.begin(), j);
    return;
  }
  int n = static_cast<int>(v.size());
  if (j > v[n-1])
  {
    v.push_back(j);
    return;
  }

  for (int i=1; i<static_cast<int>(v.size()); ++i)
  {
    if (j > v[i-1] && j < v[i])
    {
      v.insert(v.begin() + i, j);
      return;
    }
  }
}

//----------------------------------------------------------------
Merge::Merge() :
  pMatch(NULL),
  pNumMatch(0),
  pMerged()
{
}

//----------------------------------------------------------------
Merge::Merge(const int numItems) :
  pMatch(NULL),
  pNumMatch(numItems),
  pMerged()
{
  pMatch = new bool*[numItems];
  for (int i=0; i<numItems; ++i)
  {
    pMatch[i] = new bool[numItems];
    for (int j=0; j<numItems; ++j)
    {
      pMatch[i][j] = false;
    }
    pMatch[i][i] = true;
  }
}

//----------------------------------------------------------------
Merge::~Merge()
{
  if (pNumMatch > 0)
  {
    for (int i=0; i<pNumMatch; ++i)
    {
      delete [] pMatch[i];
    }
    delete [] pMatch;
  }
  pMatch = NULL;
  pNumMatch = 0;
}

//----------------------------------------------------------------
void Merge::print(void) const
{
  for (int i=0; i<pNumMatch; ++i)
  {
    printf("[%d] = ", i);
    for (int j=0; j<pNumMatch; ++j)
    {
      if (pMatch[i][j])
      {
	printf("%d ", j);
      }
    }
    printf("\n");
  }
}

//----------------------------------------------------------------
void Merge::printMerged(void) const
{
  for (int i=0; i<static_cast<int>(pMerged.size()); ++i)
  {
    for (int j=0; j<static_cast<int>(pMerged[i].size()); ++j)
    {
      printf("%d ", pMerged[i][j]);
    }
    printf("\n");
  }
}

//----------------------------------------------------------------
void Merge::clear(void)
{
  if (pNumMatch > 0)
  {
    for (int i=0; i<pNumMatch; ++i)
    {
      delete [] pMatch[i];
    }
    delete [] pMatch;
  }
  pNumMatch = 0;
  pMatch = NULL;
  pMerged.clear();
}

//----------------------------------------------------------------
void Merge::update(const int index1, const int index2)
{
  pMatch[index1][index2] = true;
  pMatch[index2][index1] = true;
}

//----------------------------------------------------------------
void Merge::merge(void)
{
  pMerged.clear();
  if (pNumMatch <= 0)
  {
    return;
  }
  
  bool *done = new bool[pNumMatch];
  for (int i=0; i<pNumMatch; ++i)
  {
    done[i] = false;
  }

  for (int i=0; i<pNumMatch; ++i)
  {
    vector<int> v;
    pAddRow(i, done, v);
    if (!v.empty())
    {
      pMerged.push_back(v);
    }
  }
  if (done)
  {
    delete [] done;
  }
}

//----------------------------------------------------------------
int Merge::numMerged(void) const
{
  return static_cast<int>(pMerged.size());
}

//----------------------------------------------------------------
vector<int> Merge::getIthMerged(const int i) const 
{
  return pMerged[i];
}

//----------------------------------------------------------------
void Merge::pAddRow(const int i, bool *done, vector<int> &v) const
{
  if (done[i])
  {
    return;
  }

  done[i] = true;

  for (int j=0; j<pNumMatch; ++j)
  {
    if (pMatch[i][j])
    {
      sInsertElement(j, v);
      pAddRow(j, done, v);
    }
  }
}
