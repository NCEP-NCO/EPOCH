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
 * @file ContingencyTableWindowed.cc
 */

//----------------------------------------------------------------
#include <ConvWx/ContingencyTableWindowed.hh>
#include <ConvWx/ContingencyTable.hh>
#include <ConvWx/Xml.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/MultiFcstGrid.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <cmath>
using std::vector;
using std::string;

//----------------------------------------------------------------
ContingencyTableWindowed::ContingencyTableWindowed(void)
{
}

//----------------------------------------------------------------
ContingencyTableWindowed::ContingencyTableWindowed(const std::string &xml)
{
  if (!pSetFromXml(xml))
  {
    pTable.clear();
  }
}

//----------------------------------------------------------------
ContingencyTableWindowed::ContingencyTableWindowed(const MultiFcstGrid &mg,
						   const bool counts) 
{
  if (!pSetFromXml(mg.getMetadataXml().xmlString()))
  {
    pTable.clear();
    return;
  }

  // Make sure each field name is found in the data:
  bool stat = true;
  for (size_t i=0; i<pTable.size(); ++i)
  {
    vector<string> fields;
    if (counts)
    {
      fields.push_back(pTable[i].hitName());
      fields.push_back(pTable[i].missName());
      fields.push_back(pTable[i].falsepName());
      fields.push_back(pTable[i].noneName());
    }
    else
    {
      fields.push_back(pTable[i].pName);
    }
    for (size_t j=0; j<fields.size(); ++j)
    {
      if (mg.matchingFieldIndex(fields[j]) == convWx::BAD_INDEX)
      {
	ILOGF(ERROR, "field %s not found in data", fields[j].c_str());
	stat = false;
      }
    }
  }
  if (!stat)
  {
    pTable.clear();
  }
}

//----------------------------------------------------------------
ContingencyTableWindowed::~ContingencyTableWindowed()
{
}

//----------------------------------------------------------------
void ContingencyTableWindowed::add(const ContingencyTableWindowedInfo &table)
{
  pTable.push_back(table);
}

//----------------------------------------------------------------
const Grid *ContingencyTableWindowed::ithHitGridPtr(const int index,
						    const MultiGrid &mg) const
{
  return mg.constGridPtr(pTable[index].hitName());
}

//----------------------------------------------------------------
const Grid *ContingencyTableWindowed::ithMissGridPtr(const int index,
						     const MultiGrid &mg) const
{
  return mg.constGridPtr(pTable[index].missName());
}

//----------------------------------------------------------------
const Grid *
ContingencyTableWindowed::ithFalsepGridPtr(const int index,
					   const MultiGrid &mg) const
{
  return mg.constGridPtr(pTable[index].falsepName());
}

//----------------------------------------------------------------
const Grid *ContingencyTableWindowed::ithNoneGridPtr(const int index,
						     const MultiGrid &mg) const
{
  return mg.constGridPtr(pTable[index].noneName());
}


//----------------------------------------------------------------
bool ContingencyTableWindowed::ithBias(const int index, const MultiGrid &mg,
				       Grid &biasGrid) const
{
  const Grid *h, *f, *m;
  h = ithHitGridPtr(index, mg);
  f = ithFalsepGridPtr(index, mg);
  m = ithMissGridPtr(index, mg);
  if (h == NULL || f == NULL || m == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  biasGrid = Grid(pTable[index].biasName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hitCount, falsepCount, missCount;
    if (h->getValue(i, hitCount) && f->getValue(i, falsepCount) && 
	m->getValue(i, missCount))
    {
      double bias = 0.0;
      if (hitCount + missCount > 0)
      {
	bias = (hitCount + falsepCount)/(hitCount + missCount);
	biasGrid.setv(i, bias);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithCSI(const int index, const MultiGrid &mg,
				      Grid &csiGrid) const
{
  const Grid *h, *f, *m;
  h = ithHitGridPtr(index, mg);
  f = ithFalsepGridPtr(index, mg);
  m = ithMissGridPtr(index, mg);
  if (h == NULL || f == NULL || m == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  csiGrid = Grid(pTable[index].csiName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hitCount, falsepCount, missCount;
    if (h->getValue(i, hitCount) && f->getValue(i, falsepCount) && 
	m->getValue(i, missCount))
    {
      double csi = 0.0;
      if (hitCount + missCount + falsepCount > 0)
      {
	csi = hitCount/(hitCount + missCount + falsepCount);
	csiGrid.setv(i, csi);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithHSS(const int index, const MultiGrid &mg,
				      Grid &hssGrid) const
{
  const Grid *h, *f, *m, *n;
  h = ithHitGridPtr(index, mg);
  f = ithFalsepGridPtr(index, mg);
  m = ithMissGridPtr(index, mg);
  n = ithNoneGridPtr(index, mg);
  if (h == NULL || f == NULL || m == NULL || n == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  hssGrid = Grid(pTable[index].hssName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hss = 0.0;

    double hitCount, falsepCount, missCount, noneCount;
    if (h->getValue(i, hitCount) && f->getValue(i, falsepCount) && 
	m->getValue(i, missCount) && n->getValue(i, noneCount))
    {
      double a = (hitCount + falsepCount)*(hitCount + missCount) + 
	(missCount + noneCount)*(falsepCount + noneCount);
      double b = hitCount + missCount + falsepCount + noneCount;
      if (b > 0.0)
      {
	double c = a/b;
	double d = hitCount + noneCount;
	if (fabs(b - c) > 0.00001)
	{
	  hss = (d - c)/(b - c);
	  hssGrid.setv(i, hss);
	}
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithFAR(const int index, const MultiGrid &mg,
				      Grid &farGrid) const
{
  const Grid *h, *f;
  h = ithHitGridPtr(index, mg);
  f = ithFalsepGridPtr(index, mg);
  if (h == NULL || f == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  farGrid = Grid(pTable[index].farName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hitCount, falsepCount;
    if (h->getValue(i, hitCount) && f->getValue(i, falsepCount))
    {
      double far = 0.0;
      if (hitCount + falsepCount > 0)
      {
	far = falsepCount/(hitCount+falsepCount);
	farGrid.setv(i, far);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithPODno(const int index, const MultiGrid &mg,
					Grid &podNoGrid) const
{
  const Grid *f, *n;
  f = ithFalsepGridPtr(index, mg);
  n = ithNoneGridPtr(index, mg);
  if (f == NULL || n == NULL)
  {
    return false;
  }

  int nx, ny;
  f->getDim(nx, ny);
  podNoGrid = Grid(pTable[index].podNoName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double podno = 0.0;

    double falsepCount, noneCount;
    if (f->getValue(i, falsepCount) && n->getValue(i, noneCount))
    {
      if (falsepCount + noneCount > 0)
      {
	podno = noneCount / (falsepCount + noneCount);
	podNoGrid.setv(i, podno);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithPOD(const int index, const MultiGrid &mg,
				      Grid &podGrid) const
{
  const Grid *h, *m;
  h = ithHitGridPtr(index, mg);
  m = ithMissGridPtr(index, mg);
  if (h == NULL || m == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  podGrid = Grid(pTable[index].podName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hitCount, missCount;
    if (h->getValue(i, hitCount) && m->getValue(i, missCount))
    {
      double pod = 0.0;
      if (hitCount + missCount > 0)
      {
	pod = hitCount/(hitCount+missCount);
	podGrid.setv(i, pod);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithObsFreq(const int index, const MultiGrid &mg,
					  Grid &obsFreqGrid) const
{
  const Grid *h, *m, *f, *n;
  h = ithHitGridPtr(index, mg);
  m = ithMissGridPtr(index, mg);
  f = ithFalsepGridPtr(index, mg);
  n = ithNoneGridPtr(index, mg);
  if (h == NULL || m == NULL || f == NULL || n == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  obsFreqGrid = Grid(pTable[index].obsFreqName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hitCount, missCount, falsepCount, noneCount;
    if (h->getValue(i, hitCount) && m->getValue(i, missCount) &&
	f->getValue(i, falsepCount) && n->getValue(i, noneCount))
    {
      double obsFreq = 0.0;
      if (hitCount + missCount + falsepCount + noneCount > 0)
      {
	obsFreq = (hitCount + missCount)/
	  (hitCount + missCount + falsepCount + noneCount);
	obsFreqGrid.setv(i, obsFreq);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::ithFcstFreq(const int index, const MultiGrid &mg,
					  Grid &fcstFreqGrid) const
{
  const Grid *h, *m, *f, *n;
  h = ithHitGridPtr(index, mg);
  m = ithMissGridPtr(index, mg);
  f = ithFalsepGridPtr(index, mg);
  n = ithNoneGridPtr(index, mg);
  if (h == NULL || m == NULL || f == NULL || n == NULL)
  {
    return false;
  }

  int nx, ny;
  h->getDim(nx, ny);
  fcstFreqGrid = Grid(pTable[index].fcstFreqName(), "none", nx, ny, -1);
  for (int i=0; i<nx*ny; ++i)
  {
    double hitCount, missCount, falsepCount, noneCount;
    if (h->getValue(i, hitCount) && m->getValue(i, missCount) &&
	f->getValue(i, falsepCount) && n->getValue(i, noneCount))
    {
      double fcstFreq = 0.0;
      if (hitCount + missCount + falsepCount + noneCount > 0)
      {
	fcstFreq = (hitCount + falsepCount)/
	  (hitCount + missCount + falsepCount + noneCount);
	fcstFreqGrid.setv(i, fcstFreq);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
std::string ContingencyTableWindowed::metadata(void) const
{
  string s = "";
  for (size_t i=0; i<pTable.size(); ++i)
  {
    string t = "";
    t += Xml::writeString("name", 0, pTable[i].pName);
    t += Xml::writeDouble("obsThresh", 0, pTable[i].pObsThresh, "%.5lf");
    t += Xml::writeDouble("fcstThresh", 0, pTable[i].pFcstThresh, "%.5lf");
    t += Xml::writeInt("obsMinutesBefore", 0, pTable[i].pObsSecondsBefore/60);
    t += Xml::writeInt("obsMinutesAfter", 0, pTable[i].pObsSecondsAfter/60);
    t += Xml::writeInt("fcstMinutesBefore", 0, pTable[i].pFcstSecondsBefore/60);
    t += Xml::writeInt("fcstMinutesAfter", 0, pTable[i].pFcstSecondsAfter/60);
    s += Xml::writeString("ContingencyTable", 0, t);
  }
  return s;
}

//----------------------------------------------------------------
MultiGrid ContingencyTableWindowed::createCounterGrids(const int nx,
						       const int ny) const
{
  MultiGrid ret;

  for (size_t i=0; i<pTable.size(); ++i)
  {
    Grid g(pTable[i].hitName(), "counts", nx, ny, -1.0);
    g.setAllToValue(0.0);
    ret.append(g);
    g = Grid(pTable[i].missName(), "counts", nx, ny, -1.0);
    g.setAllToValue(0.0);
    ret.append(g);
    g = Grid(pTable[i].falsepName(), "counts", nx, ny, -1.0);
    g.setAllToValue(0.0);
    ret.append(g);
    g = Grid(pTable[i].noneName(), "counts", nx, ny, -1.0);
    g.setAllToValue(0.0);
    ret.append(g);
  }
  return ret;
}

//------------------------------------------------------------------
void ContingencyTableWindowed::accumulate(const MultiGrid &mg,
					  MultiGrid &sum) const
{
  for (size_t i=0; i<pTable.size(); ++i)
  {
    const Grid *g = mg.constGridPtr(pTable[i].pName);
    if (g == NULL)
    {
      continue;
    }
    int nx, ny;
    g->getDim(nx, ny);

    Grid *hits = sum.gridPtr(pTable[i].hitName());
    Grid *misses = sum.gridPtr(pTable[i].missName());
    Grid *falsep = sum.gridPtr(pTable[i].falsepName());
    Grid *none = sum.gridPtr(pTable[i].noneName());
    for (int j=0; j<nx*ny; ++j)
    {
      double v;
      if (g->getValue(j, v))
      {
	ContingencyTable::Type_t iv = static_cast<ContingencyTable::Type_t>(v);
	switch (iv)
	{
	case ContingencyTable::NONE:
	  none->incrementValueAtPoint(j, 1.0);
	  break;
	case ContingencyTable::FALSEP:
	  falsep->incrementValueAtPoint(j, 1.0);
	  break;
	case ContingencyTable::MISS:
	  misses->incrementValueAtPoint(j, 1.0);
	  break;
	case ContingencyTable::HIT:
	  hits->incrementValueAtPoint(j, 1.0);
	  break;
	default:
	  break;
	}
      }
    }
  }
}

//------------------------------------------------------------------
void ContingencyTableWindowed::accumulateWeighted(const MultiGrid &mg,
						  double weight,
						  MultiGrid &sum,
						  MultiGrid &counts) const
{
  for (size_t i=0; i<pTable.size(); ++i)
  {
    const Grid *g = mg.constGridPtr(pTable[i].pName);
    if (g == NULL)
    {
      continue;
    }
    int nx, ny;
    g->getDim(nx, ny);

    Grid *hits = sum.gridPtr(pTable[i].hitName());
    Grid *misses = sum.gridPtr(pTable[i].missName());
    Grid *falsep = sum.gridPtr(pTable[i].falsepName());
    Grid *none = sum.gridPtr(pTable[i].noneName());

    Grid *chits = counts.gridPtr(pTable[i].hitName());
    Grid *cmisses = counts.gridPtr(pTable[i].missName());
    Grid *cfalsep = counts.gridPtr(pTable[i].falsepName());
    Grid *cnone = counts.gridPtr(pTable[i].noneName());
    for (int j=0; j<nx*ny; ++j)
    {
      double v;
      if (g->getValue(j, v))
      {
	ContingencyTable::Type_t iv = static_cast<ContingencyTable::Type_t>(v);
	switch (iv)
	{
	case ContingencyTable::NONE:
	  none->incrementValueAtPoint(j, weight);
	  cnone->incrementValueAtPoint(j, 1.0);
	  break;
	case ContingencyTable::FALSEP:
	  falsep->incrementValueAtPoint(j, weight);
	  cfalsep->incrementValueAtPoint(j, 1.0);
	  break;
	case ContingencyTable::MISS:
	  misses->incrementValueAtPoint(j, weight);
	  cmisses->incrementValueAtPoint(j, 1.0);
	  break;
	case ContingencyTable::HIT:
	  hits->incrementValueAtPoint(j, weight);
	  chits->incrementValueAtPoint(j, 1.0);
	  break;
	default:
	  break;
	}
      }
    }
  }
}

//----------------------------------------------------------------
void ContingencyTableWindowed::filter(const int num, const double minp,
				      MultiGrid &sum) const
{
  const Grid *g = sum.ithConstGrid(0);
  int nx, ny;
  g->getDim(nx, ny);

  for (size_t i=0; i<pTable.size(); ++i)
  {
    Grid *hits = sum.gridPtr(pTable[i].hitName());
    Grid *misses = sum.gridPtr(pTable[i].missName());
    Grid *falsep = sum.gridPtr(pTable[i].falsepName());
    Grid *none = sum.gridPtr(pTable[i].noneName());
    for (int j=0; j<nx*ny; ++j)
    {
      double count = 0;
      double v;
      if (hits->getValue(j, v))
      {
	count += v;
      }
      if (misses->getValue(j, v))
      {
	count += v;
      }
      if (falsep->getValue(j, v))
      {
	count += v;
      }
      if (none->getValue(j, v))
      {
	count += v;
      }
      if (count/static_cast<double>(num) < minp)
      {
	hits->setToMissing(j);
	misses->setToMissing(j);
	falsep->setToMissing(j);
	none->setToMissing(j);
      }
      else
      {
	ILOGF(DEBUG_VERBOSE, "Ok at %d", j);
      }
    }
  }
}

//----------------------------------------------------------------
void ContingencyTableWindowed::filterWeighted(const int num,
					      const double minp,
					      const MultiGrid &counts,
					      MultiGrid &sum) const
{
  const Grid *g = sum.ithConstGrid(0);
  int nx, ny;
  g->getDim(nx, ny);

  for (size_t i=0; i<pTable.size(); ++i)
  {
    Grid *hits = sum.gridPtr(pTable[i].hitName());
    Grid *misses = sum.gridPtr(pTable[i].missName());
    Grid *falsep = sum.gridPtr(pTable[i].falsepName());
    Grid *none = sum.gridPtr(pTable[i].noneName());
    const Grid *chits = counts.constGridPtr(pTable[i].hitName());
    const Grid *cmisses = counts.constGridPtr(pTable[i].missName());
    const Grid *cfalsep = counts.constGridPtr(pTable[i].falsepName());
    const Grid *cnone = counts.constGridPtr(pTable[i].noneName());
    for (int j=0; j<nx*ny; ++j)
    {
      double count = 0;
      double v;
      if (chits->getValue(j, v))
      {
	count += v;
      }
      if (cmisses->getValue(j, v))
      {
	count += v;
      }
      if (cfalsep->getValue(j, v))
      {
	count += v;
      }
      if (cnone->getValue(j, v))
      {
	count += v;
      }
      if (count/static_cast<double>(num) < minp)
      {
	hits->setToMissing(j);
	misses->setToMissing(j);
	falsep->setToMissing(j);
	none->setToMissing(j);
      }
      else
      {
	ILOGF(DEBUG_VERBOSE, "Ok at %d", j);
      }
    }
  }
}

//----------------------------------------------------------------
bool ContingencyTableWindowed::pSetFromXml(const std::string &xml)
{
  pTable.clear();

  vector<string> tables;
  if (!Xml::readXmlStringArray(xml, "ContingencyTable", tables))
  {
    return false;
  }

  bool ret = true;
  for (size_t i=0; i<tables.size(); ++i)
  {
    bool stat = true;
    string name;
    double ot, ft;
    int obsSecBefore=0, obsSecAfter=0;
    int fcstSecBefore=0, fcstSecAfter=0;
    if (!Xml::readString(tables[i], "name", true, name))
    {
      stat = false;
    }
    if (!Xml::readDouble(tables[i], "obsThresh", true, ot))
    {
      stat = false;
    }
    if (!Xml::readDouble(tables[i], "fcstThresh", true, ft))
    {
      stat = false;
    }

    // old has same window for obs and forecasts, new has individual
    // windows, both are supported on read

    int m0, m1;
    if (Xml::readInt(tables[i], "minutesBefore", false, m0) &&
	Xml::readInt(tables[i], "minutesAfter", false, m1))
    {
      obsSecBefore = fcstSecBefore = m0*60;
      obsSecAfter = fcstSecAfter = m1*60;
    }
    else
    {
      int o0, o1, f0, f1;
      if (Xml::readInt(tables[i], "obsMinutesBefore", true, o0) &&
	  Xml::readInt(tables[i], "obsMinutesAfter", true, o1) &&
	  Xml::readInt(tables[i], "fcstMinutesBefore", true, f0) &&
	  Xml::readInt(tables[i], "fcstMinutesAfter", true, f1))
      {
	obsSecBefore = o0*60;
	obsSecAfter = o1*60;
	fcstSecBefore = f0*60;
	fcstSecAfter = f1*60;
      }
      else
      {
	stat = false;
      }
    }

    if (stat)
    {
      ContingencyTableWindowedInfo info(ot, ft, obsSecBefore, obsSecAfter, 
					fcstSecBefore, fcstSecAfter, name);
      pTable.push_back(info);
    }
    else
    {
      ret = false;
    }
  }
  return ret;
}

