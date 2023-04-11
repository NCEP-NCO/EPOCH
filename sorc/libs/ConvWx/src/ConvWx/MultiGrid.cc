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
 * @file MultiGrid.cc
 */

//----------------------------------------------------------------
#include <string>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;

//----------------------------------------------------------------
MultiGrid::MultiGrid() :
  pGrids()
{
}

//----------------------------------------------------------------
MultiGrid::MultiGrid(const Grid &g) :
  pGrids()
{
  pGrids.push_back(g);
}

//----------------------------------------------------------------
MultiGrid::~MultiGrid()
{
}

//----------------------------------------------------------------
bool MultiGrid::dimensionsOk(const int nx, const int ny) const
{
  bool stat = true;
  for (int i=0; i<static_cast<int>(pGrids.size()); ++i)
  {
    int inx, iny;
    pGrids[i].getDim(inx, iny);
    if (nx != inx || ny != iny)
    {
      ILOGF(ERROR, "%d,%d vs %d,%d", nx, ny, inx, iny);
      stat = false;
    }
  }
  return stat;
}

//----------------------------------------------------------------
bool MultiGrid::fieldsMatch(const MultiGrid &g) const
{
  if (g.size() != size())
  {
    return false;
  }
  for (size_t i=0; i<size(); ++i)
  {
    if (!(g.pGrids[i].getName() == pGrids[i].getName()))
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------
void MultiGrid::appendUnlessNoName(const Grid &g, const string &name)
{
  if (name.empty())
  {
    return;
  }
  if (g.getName() == name)
  {
    pGrids.push_back(g);
  }
  else
  {
    Grid g2(g);
    g2.changeName(name);
    pGrids.push_back(g2);
  }
}

//----------------------------------------------------------------
void MultiGrid::copyOut(MultiGrid &m) const
{
  m.pGrids = pGrids;
}

//----------------------------------------------------------------
std::vector<std::string> MultiGrid::fieldNames(void) const
{
  std::vector<string> ret;
  for (size_t i=0; i<pGrids.size(); ++i)
  {
    ret.push_back(pGrids[i].getName());
  }
  return ret;
}

//----------------------------------------------------------------
std::string MultiGrid::ithGridName(const int i) const
{
  return pGrids[i].getName();
}

//----------------------------------------------------------------
int MultiGrid::matchingFieldIndex(const string &field) const
{
  for (int i=0; i< static_cast<int>(pGrids.size()); ++i)
  {
    if (pGrids[i].getName() == field)
    {
      return i;
    }
  }
  return convWx::BAD_INDEX;
}

//----------------------------------------------------------------
const Grid *MultiGrid::constGridPtr(const string &fieldname,
				    const bool debugWarn) const
{
  // find index to grid with this name
  int ind = matchingFieldIndex(fieldname);
  if (ind == convWx::BAD_INDEX)
  {
    if (debugWarn)
    {
      ILOGF(ERROR, "no matching field name %s", fieldname.c_str());
    }
    return NULL;
  }
  else
  {
    return &pGrids[ind];
  }
}

//----------------------------------------------------------------
Grid *MultiGrid::gridPtr(const string &fieldname, const bool debugWarn)
{
  // find index to grid with this name
  int ind = matchingFieldIndex(fieldname);
  if (ind == convWx::BAD_INDEX)
  {
    if (debugWarn)
    {
      ILOGF(ERROR, "no matching field name %s", fieldname.c_str());
    }
    return NULL;
  }
  else
  {
    return &pGrids[ind];
  }
}

//----------------------------------------------------------------
bool MultiGrid::replaceData(const string &fieldFrom,
			    const string &fieldTo)
{
  int fromInd = matchingFieldIndex(fieldFrom);
  int toInd = matchingFieldIndex(fieldTo);
  if (fromInd == convWx::BAD_INDEX || toInd == convWx::BAD_INDEX)
  {
    ILOGF(ERROR, "matching field names %s %s", fieldFrom.c_str(),
	  fieldTo.c_str());
    return false;
  }

  return ithGrid(toInd)->copyValues(*ithConstGrid(fromInd));
}

//----------------------------------------------------------------
void MultiGrid::smooth(const int sx, const int sy)
{
  for (int i=0; i< static_cast<int>(pGrids.size()); ++i)
  {
    pGrids[i].fastsmooth(sx, sy);
  }
}

//----------------------------------------------------------------
bool MultiGrid::subtract(const MultiGrid &sub)
{
  bool stat = true;
  for (int i=0; i< static_cast<int>(pGrids.size()); ++i)
  {
    string n = pGrids[i].getName();
    const Grid *subI = sub.constGridPtr(n);
    if (subI == NULL)
    {
      stat = false;
    }
    else
    {
      if (!pGrids[i].subtract(*subI))
      {
	ILOGF(ERROR, "Subtracting %s", n.c_str());
	stat = false;
      }
    }
  }
  return stat;
}

//----------------------------------------------------------------
bool MultiGrid::add(const MultiGrid &a)
{
  bool stat = true;
  for (int i=0; i< static_cast<int>(pGrids.size()); ++i)
  {
    string n = pGrids[i].getName();
    const Grid *aI = a.constGridPtr(n);
    if (aI == NULL)
    {
      stat = false;
    }
    else
    {
      if (!pGrids[i].subtract(*aI))
      {
	ILOGF(ERROR, "Adding %s", n.c_str());
	stat = false;
      }
    }
  }
  return stat;
}

//----------------------------------------------------------------
void MultiGrid::monotonicDecreasing(const MultiGrid &g)
{
  if (!fieldsMatch(g))
  {
    ILOG(ERROR, "Fields not one to one");
    return;
  }
  for (size_t i=0; i<size(); ++i)
  {
    pGrids[i].monotonicDecreasing(g[i]);
  }
}

//----------------------------------------------------------------
void MultiGrid::setEncoding(const Grid::Encoding_t encoding)
{
  for (size_t i=0; i<pGrids.size(); ++i)
  {
    pGrids[i].setEncoding(encoding);
  }
}

//----------------------------------------------------------------
void MultiGrid::setScalingSpecified(const double scale, const double bias)
{
  for (size_t i=0; i<pGrids.size(); ++i)
  {
    pGrids[i].setScalingSpecified(scale, bias);
  }
}

//----------------------------------------------------------------
Grid& MultiGrid::operator[](const std::string &s)
{
  for (size_t i=0; i<pGrids.size(); ++i)
  {
    if (pGrids[i].getName() == s)
    {
      return pGrids[i];
    }
  }
  return *end();
}

//----------------------------------------------------------------
const Grid& MultiGrid::operator[](const std::string &s) const
{
  for (size_t i=0; i<pGrids.size(); ++i)
  {
    if (pGrids[i].getName() == s)
    {
      return pGrids[i];
    }
  }
  return *end();
}

//----------------------------------------------------------------
MultiGrid::iterator MultiGrid::find(const Grid &grid)
{
  for (iterator i = pGrids.begin(); i != pGrids.end(); ++i)
  {
    if (grid == *i)
    {
      return i;
    }
  }
  return end();
}

//----------------------------------------------------------------
MultiGrid::const_iterator MultiGrid::find(const Grid &grid) const
{
  for (const_iterator i = pGrids.begin(); i != pGrids.end(); ++i)
  {
    if (grid == *i)
    {
      return i;
    }
  }
  return end();
}

//----------------------------------------------------------------
void MultiGrid::push_back(const Grid &grid)
{
  int index = matchingFieldIndex(grid.getName());
  if (index == convWx::BAD_INDEX)
  {
    pGrids.push_back(grid);
  }
  else
  {
    ILOGF(WARNING, "Cannot push grid %s, already there",
	  grid.getName().c_str());
  }
}

//----------------------------------------------------------------
MultiGrid::iterator MultiGrid::erase(MultiGrid::iterator pos)
{
  return pGrids.erase(pos);
}

