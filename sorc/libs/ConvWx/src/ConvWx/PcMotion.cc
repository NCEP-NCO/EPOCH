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
 * @file PcMotion.cc
 * @brief  class for PcMotion
*/

//----------------------------------------------------------------
#include <vector>
#include <ConvWx/PcMotion.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/MultiGridsForPc.hh>
#include <ConvWx/GridsForPc.hh>
#include <ConvWx/PcVolume.hh>
#include <ConvWx/MetaData.hh>
using std::vector;


//----------------------------------------------------------------
PcMotion::PcMotion(const UvUnits &units) :
  UvOutput(units),
  pCounterGrid(),
  pPcType(PcType::UNKNOWN)
{
}

//----------------------------------------------------------------
PcMotion::PcMotion(const int nx, const int ny, const UvUnits &units) :
  UvOutput("U", "V", "m/s", nx, ny, units),
  pCounterGrid("count", "none", nx, ny, convWx::UV_MISSING),
  pPcType(PcType::UNKNOWN)
{  
  pCounterGrid.setAllToValue(0.0);
}

//----------------------------------------------------------------
PcMotion::~PcMotion()
{
}

//----------------------------------------------------------------
void PcMotion::init(const MultiGridsForPc &templateData)
{    
  pCounterGrid = templateData.initializeGrid("counter", "none", 0.0);
  UvUnits units = (*this);
  UvOutput::operator=(UvOutput(pCounterGrid, "U", "V", "m/s", "U", "V",
			       units));
  pPcType = PcType::UNKNOWN;
}

//----------------------------------------------------------------
bool PcMotion::pcmotionReduce(const int reduction)
{
  bool stat = true;
  if (!UvOutput::reduce(reduction))
  {
    stat = false;
  }
  if (!pCounterGrid.reduce(reduction))
  {
    stat = false;
  }
  return stat;
}

//----------------------------------------------------------------
bool PcMotion::pcmotionInterpolate(const PcMotion &lowresGrids, const int res)
{
  bool stat = true;
  if (!UvOutput::interpolate(lowresGrids, res))
  {
    stat = false;
  }
  if (!pCounterGrid.interpolate(lowresGrids.pCounterGrid, res))
  {
    stat = false;
  }
  return stat;
}

//----------------------------------------------------------------
void PcMotion::addVolume(const PcVolume &volume)
{
  volume.addToMotion(*this, pCounterGrid);
}

//----------------------------------------------------------------
void PcMotion::pcNormalize(void)
{
  UvOutput::normalize(pCounterGrid);
}

//----------------------------------------------------------------
bool PcMotion::pcMultiply(const Grid &weightGrid)
{
  if (!UvOutput::multiply(weightGrid))
  {
    return false;
  }

  // at each point adjust the motion by looking at weight at "to" location
  // if moving to an area with low weight, reduce the motion by that factor.
  double u, v;
  int nx, ny;
  UvOutput::getDim(nx, ny);
  for (int y=0; y<ny; ++y)
  {
    for (int x=0; x<nx; ++x)
    {
      if (!UvOutput::getValuesOrZero(x, y, u, v))
      {
	continue;
      }
      int fx = static_cast<int>(x + u);
      int fy = static_cast<int>(y + v);
      if (weightGrid.inRange(fx, fy))
      {
	double w;
	if (weightGrid.getValue(fx, fy, w))
	{
	  if (w < 1.0)
	  {
	    u = u*w;
	    v = v*w;
	    UvOutput::setValuesAtPoint(x, y, u, v);
	  }
	}
      }
    }
  }
  return true;
}

//----------------------------------------------------------------
void PcMotion::setToZero(const PcType::Pc_t pcType)
{
  UvOutput::setAllZero();
  pPcType = pcType;
}

//----------------------------------------------------------------
void PcMotion::setPhaseCorrect(const PcType::Pc_t pcType)
{
  pPcType = pcType;
}

//----------------------------------------------------------------
MetaData PcMotion::getMetadata(void) const
{
  MetaDataXml xml(pPcType);
  const UvUnits *uv = dynamic_cast<const UvUnits *>(this);
  if (uv == NULL)
  {
    xml.replaceUvUnits(UvUnits());
  }
  else
  {
    xml.replaceUvUnits(*uv);
  }
  MetaData ret("Phase Correction", PcType::correctionString(pPcType),
	       "", xml);
  return ret;
}

//----------------------------------------------------------------
void PcMotion::phaseShift(const int gapFill, const vector<int> &smoothSize,
			  MultiGridsForPc &dataGrids)
{
  for (int i=0; i<dataGrids.num(); ++i)
  {
    InterfaceLL::doRegister("shift");
    dataGrids.ithGrids(i).shiftFcstAndSmooth(*this, gapFill, smoothSize);
  }
}

