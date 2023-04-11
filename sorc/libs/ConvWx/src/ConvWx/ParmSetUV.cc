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
 * @file ParmSetUV.cc
 * @brief Algorithm parameters all in one place
 */

//----------------------------------------------------------------
#include <string>
#include <cstdio>
#include <cstring>
#include <vector>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/ParmPcFcst.hh>
using std::string;
using std::vector;
using std::pair;

//----------------------------------------------------------------
/**
 * Show (log) value which is input as number of points as a value in km
 * @param[in] npt  Number of grid points
 * @param[in] delta  Km per grid point
 * @param[in] name  Name of the variable
 */
static void sShowKm(const int npt, const double &delta,
		    const string &name)
{
  double v = npt;
  v = v*delta;
  ILOGF(DEBUG_VERBOSE, "\t%s = %.2lf km", name.c_str(), v);
}

//----------------------------------------------------------------
/**
 * Show (log) value which is input as number of points as a value in km,
 * @param[in] lowResNpt  Number of grid points per low res grid point
 * @param[in] npt  Number of grid points (high res)
 * @param[in] delta  Km per low res grid point
 * @param[in] name  Name of the variable
 */
static void sShowLrKm(const int lowResNpt, const int npt,
		      const double &delta, const string &name)
{
  double v = npt*lowResNpt;
  v = v*delta;
  ILOGF(DEBUG_VERBOSE, "\t%s = %.2lf km", name.c_str(), v);
}

//----------------------------------------------------------------
ParmSetUV::ParmSetUV(void)  : 
  ParmConv(),
  pFcst(),
  pFcstInputSmooth(0),
  pVerifInputSmooth(0),
  pLowResNgridpts(0),
  pShiftResNpt(0),
  pMaxShift(0),
  pRefineShiftResNpt(0),
  pRefineMaxShift(0),
  pLowResMotionSmooth(),
  pHighResMotionSmooth(),
  pFcstGapFillSmooth(0),
  pFcstOutputSmooth(),
  pExcludeZeroFromSmoothing(false),
  pHighResMaxExpandNpt(0),
  pGoodScaling(0),
  pGoodDistScaling(0),
  pFractionalAreaMinPcnt(0), 
  pGridAreaMinPcnt(0),
  pFuzzyGridDist(),
  pDxKm(0.0),
  pDyKm(0.0)
{
  pVolSize[0] = 0;
  pVolSize[1] = 0;
  pVolOverlap[0] = 0;
  pVolOverlap[1] = 0;
}

//----------------------------------------------------------------
ParmSetUV::ParmSetUV(const std::vector<ParmPcFcst> &fcst,
		     const int fcstInputSmooth,
		     const int verifInputSmooth,
		     const int lowResNgridpts,
		     const int *volSize,
		     const int *volOverlap,
		     const int shiftResNpt,
		     const int maxShift,
		     const int refineShiftResNpt,
		     const int refineMaxShift,
		     const int nLowResMotionSmooth,
		     const int *lowResMotionSmooth,
		     const int nHighResMotionSmooth,
		     const int *highResMotionSmooth,
		     const int fcstGapFillSmooth,
		     const int nFcstOutputSmooth,
		     const int *fcstOutputSmooth,
		     const bool excludeZeroFromSmoothing,
		     const int highResMaxExpandNpt,
		     const double goodScaling,
		     const double goodDistScaling,
		     const double fractionalAreaMinPcnt,
		     const double gridAreaMinPcnt,
		     const ConvWxFuzzy &fuzzyGridDist) :
  ParmConv(),
  pFcst(fcst),
  pFcstInputSmooth(fcstInputSmooth),
  pVerifInputSmooth(verifInputSmooth),
  pLowResNgridpts(lowResNgridpts),
  pShiftResNpt(shiftResNpt),
  pMaxShift(maxShift),
  pRefineShiftResNpt(refineShiftResNpt),
  pRefineMaxShift(refineMaxShift),
  pLowResMotionSmooth(),
  pHighResMotionSmooth(),
  pFcstGapFillSmooth(fcstGapFillSmooth),
  pFcstOutputSmooth(),
  pExcludeZeroFromSmoothing(excludeZeroFromSmoothing),
  pHighResMaxExpandNpt(highResMaxExpandNpt),
  pGoodScaling(goodScaling),
  pGoodDistScaling(goodDistScaling),
  pFractionalAreaMinPcnt(fractionalAreaMinPcnt),
  pGridAreaMinPcnt(gridAreaMinPcnt),
  pFuzzyGridDist(fuzzyGridDist),
  pDxKm(0),
  pDyKm(0)
{
  for (int i=0; i<nLowResMotionSmooth; ++i)
  {
    pLowResMotionSmooth.push_back(lowResMotionSmooth[i]);
  }
  for (int i=0; i<nHighResMotionSmooth; ++i)
  {
    pHighResMotionSmooth.push_back(highResMotionSmooth[i]);
  }
  for (int i=0; i<nFcstOutputSmooth; ++i)
  {
    pFcstOutputSmooth.push_back(fcstOutputSmooth[i]);
  }
  pVolSize[0] = volSize[0];
  pVolSize[1] = volSize[1];
  pVolOverlap[0] = volOverlap[0];
  pVolOverlap[1] = volOverlap[1];
}

//----------------------------------------------------------------
ParmSetUV::~ParmSetUV()
{
}

//----------------------------------------------------------------
void ParmSetUV::print(void) const
{    
  printf("\tlowRes:%d  vol[%d,%d], overlap[%d,%d]\n", pLowResNgridpts,
	 pVolSize[0], pVolSize[1], pVolOverlap[0], pVolOverlap[1]);
  printf("\tmaxshift:%d shiftres:%d refinemax:%d refineres:%d\n",
	 pMaxShift, pShiftResNpt, pRefineMaxShift, pRefineShiftResNpt);
  printf("\tfcstinput smooth:%d verifinputsmooth:%d\n", pFcstInputSmooth,
	 pVerifInputSmooth);
  printf("\tgapsmooth:%d scale:%.2lf,%.2lf\n", pFcstGapFillSmooth,
	 pGoodScaling, pGoodDistScaling);
  printf("\tlow res motion smooth:");
  for (int i=0; i<static_cast<int>(pLowResMotionSmooth.size()); ++i)
  {
    printf("%d ", pLowResMotionSmooth[i]);
  }
  printf("\n\thigh res motion smooth:");
  for (int i=0; i<static_cast<int>(pHighResMotionSmooth.size()); ++i)
  {
    printf("%d ", pHighResMotionSmooth[i]);
  }
  printf("\n\toutput smooth:");
  for (int i=0; i<static_cast<int>(pFcstOutputSmooth.size()); ++i)
  {
    printf("%d ", pFcstOutputSmooth[i]);
  }
  printf("\texcludeZeroFromSmoothing:%d, expandIntoZero:%d\n",
	 pExcludeZeroFromSmoothing, pHighResMaxExpandNpt);
  printf("\n\tfractional area min:%lf\n", pFractionalAreaMinPcnt);
  printf("\n\tgrid area min:%lf\n", pGridAreaMinPcnt);
  ParmConv::print();
}

//----------------------------------------------------------------
bool ParmSetUV::storeGridDim(const double dxKm, const double dyKm)
{
  pDxKm = dxKm;
  pDyKm = dyKm;

  // here we analyze and show what is going to happen
  double delta = (pDxKm + pDyKm)/2.0;

  sShowKm(pFcstInputSmooth, delta, "forecastInputSmooth");
  sShowKm(pVerifInputSmooth, delta, "verifInputSmooth");

  ILOGF(DEBUG_VERBOSE, "\tlowResNgridpts=%d", pLowResNgridpts);
  sShowKm(pLowResNgridpts, delta, "lowResKm");
  sShowLrKm(pLowResNgridpts, pVolSize[0], delta, "volumeSize[0]");
  sShowLrKm(pLowResNgridpts, pVolSize[1], delta, "volumeSize[1]");
  sShowLrKm(pLowResNgridpts, pVolOverlap[0], delta, "volumeOverlap[0]");
  sShowLrKm(pLowResNgridpts, pVolOverlap[1], delta, "volumeOverlap[1]");
  sShowLrKm(pLowResNgridpts, pShiftResNpt, delta, "phaseShiftResKm");
  sShowLrKm(pLowResNgridpts, pMaxShift*pShiftResNpt, delta, "maxShiftKm");
  sShowLrKm(pLowResNgridpts, pRefineShiftResNpt, delta, "refineShiftResKm");
  sShowLrKm(pLowResNgridpts, pRefineMaxShift*pRefineShiftResNpt, delta,
	    "refineMaxShiftKm");
  for (int i=0; i<static_cast<int>(pLowResMotionSmooth.size()); ++i)
  {
    char buf[convWx::ARRAY_LEN_SHORT];
    sprintf(buf, "lowResMotionSmooth[%d]", i);
    string s = buf;
    sShowLrKm(pLowResNgridpts, pLowResMotionSmooth[i], delta, s);
  }
  for (int i=0; i<static_cast<int>(pHighResMotionSmooth.size()); ++i)
  {
    char buf[convWx::ARRAY_LEN_SHORT];
    sprintf(buf, "highResMotionSmooth[%d]", i);
    string s = buf;
    sShowKm(pHighResMotionSmooth[i], delta, s);
  }
  sShowKm(pFcstGapFillSmooth, delta, "highResGapFill");
  
  for (int i=0; i<static_cast<int>(pFcstOutputSmooth.size()); ++i)
  {
    char buf[convWx::ARRAY_LEN_SHORT];
    sprintf(buf, "highResOutputFcstSmooth[%d]", i);
    string s = buf;
    sShowKm(pFcstOutputSmooth[i], delta, s);
  }
  sShowKm(pHighResMaxExpandNpt, delta, "maxExpand");

  char buf[convWx::ARRAY_LEN_VERY_LONG];

  ILOG(DEBUG_VERBOSE, "\tShifting npt(lowres):");

  sprintf(buf, "\t");
  for (int i=-pMaxShift*pShiftResNpt; i<=pMaxShift*pShiftResNpt; 
       i += pShiftResNpt)
  {
    sprintf(buf + strlen(buf), "%d ", i);
  }
  ILOG(DEBUG_VERBOSE, buf);
  ILOG(DEBUG_VERBOSE, "\tShifting km:");

  sprintf(buf, "\t");
  for (int i=-pMaxShift*pShiftResNpt; i<=pMaxShift*pShiftResNpt; 
       i += pShiftResNpt)
  {
    double km = i*pLowResNgridpts*delta;
    sprintf(buf + strlen(buf), "%4.2lf ", km);
  }
  ILOG(DEBUG_VERBOSE, buf);

  ILOG(DEBUG_VERBOSE, "\tRefinement shifting npt(lowres):");

  sprintf(buf, "\t");
  for (int i=-pRefineMaxShift*pRefineShiftResNpt;
       i<=pRefineMaxShift*pRefineShiftResNpt; 
       i += pRefineShiftResNpt)
  {
    sprintf(buf+strlen(buf), "%d ", i);
  }
  ILOG(DEBUG_VERBOSE, buf);

  ILOG(DEBUG_VERBOSE, "\tShifting km:\t");
  sprintf(buf, "\t");
  for (int i=-pRefineMaxShift*pRefineShiftResNpt;
       i<=pRefineMaxShift*pRefineShiftResNpt; 
       i += pRefineShiftResNpt)
  {
    double km = i*pLowResNgridpts*delta;
    sprintf(buf+strlen(buf), "%4.2lf ", km);
  }
  ILOG(DEBUG_VERBOSE, buf);

  return true;
}

//----------------------------------------------------------------
int ParmSetUV::numScore(void) const
{
  int nscore = 2*pMaxShift + 1;
  return nscore;
}

//----------------------------------------------------------------
int ParmSetUV::ithOffset(const int i) const
{
  return (-pMaxShift+i)*pShiftResNpt;
}

//----------------------------------------------------------------
vector<string> ParmSetUV::getFcstNames(void) const
{
  vector<string> ret;
  for (int i=0; i<static_cast<int>(pFcst.size()); ++i)
  {
    ret.push_back(pFcst[i].pName);
  }
  return ret;
}

//----------------------------------------------------------------
ParmPcFcst ParmSetUV::matchingFcstParms(const string &name) const
{
  for (int i=0; i<static_cast<int>(pFcst.size()); ++i)
  {
    if (name == pFcst[i].pName)
    {
      return pFcst[i];
    }
  }
  ParmPcFcst p;
  return p;
}

//----------------------------------------------------------------
vector<pair<time_t,int> > ParmSetUV::fcstsValidAt(const time_t &vt) const
{
  return pFcst[0].pFcst.fcstsValidAt(vt);
}

