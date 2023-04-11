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
 * @file PhaseCorrect.cc
 */

//----------------------------------------------------------------
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <ConvWx/Score.hh>
#include <ConvWx/PhaseCorrect.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/UvOutput.hh>
#include <ConvWx/GridsForPc.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/PcVolume.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/MultiGridsForPc.hh>
#include <ConvWx/MetaData.hh>
using std::vector;
using std::string;

/**
 * Just what it looks like
 */
static const double ONE_PERCENT = 0.01;

//----------------------------------------------------------------
PhaseCorrect::PhaseCorrect(const vector<PcInput> &pcInputs,
			   const Grid &weightGrid, const ParmSetUV &parms,
			   const UvUnits &units) :
  pGrids(),
  pWeightGrid(weightGrid),
  pConvThresh(parms),
  pParm(parms),
  pInitialMotion(units),
  pFinalMotion(units)
{
  // build up the local pGrids object from input
  int n = static_cast<int>(pcInputs.size());
  for (int i=0; i<n; ++i)
  {
    GridsForPc g(pcInputs[i], parms);
    if (!pGrids.append(g))
    {
      ILOG(ERROR, "Creating multiGridsForPc");
    }
  }
  if (pGrids.num() == 0)
  {
    ILOG(FATAL, "EXITING due to no grids at all");
    // horrible problem
    exit(convWx::BAD_EXIT);
  }

}

//----------------------------------------------------------------
PhaseCorrect::PhaseCorrect(const PhaseCorrect &original, const int reduction) :
  pGrids(original.pGrids),
  pWeightGrid(original.pWeightGrid),
  pConvThresh(original.pConvThresh),
  pParm(original.pParm),
  pInitialMotion(original.pInitialMotion),
  pFinalMotion(original.pFinalMotion)
{
  pGrids.reduce(reduction);
  pWeightGrid.reduce(reduction);
  pInitialMotion.pcmotionReduce(reduction);
  pFinalMotion.pcmotionReduce(reduction);
}

//----------------------------------------------------------------
PhaseCorrect::~PhaseCorrect()
{
}

//----------------------------------------------------------------
void PhaseCorrect::computePhaseCorrect(const ParmProjection &proj,
				       int lt, 
				       ConvWxMultiThreadMgr *thread)
{
  InterfaceLL::doRegister("pc");
  ILOG(DEBUG_VERBOSE, "begin the computations");

  // create motion grids using local pGrids
  pInitialMotion.init(pGrids);
  pFinalMotion.init(pGrids);

  // see what we have, proceed if any one input has enough stuff
  if (!pGrids.hasEnoughInput(pParm.pGridAreaMinPcnt))
  {
    ILOGF(DEBUG,
	  "NOT ENOUGH DATA above Thresh=%.3lf TO DO PHASE CORRECTION",
	  pParm.pGridAreaMinPcnt);
    pGrids.setNoPc();
    pInitialMotion.setToZero(PcType::NO_PC);
    pFinalMotion.setToZero(PcType::NO_PC);
    return;
  }

  // create lower resolution data from local data.
  ILOG(DEBUG_VERBOSE, "make lower res");
  PhaseCorrect pLowRes(*this, pParm.pLowResNgridpts);
  ParmProjection projLow = ParmProjection(proj, pParm.pLowResNgridpts);

  // compute motion in low resolution world.
  InterfaceLL::doRegister("compute motion");
  if (thread != NULL)
  {
    pLowRes.pComputeMotionThreaded(projLow, lt, thread);
  }
  else
  {
    pLowRes.pComputeMotion(projLow);
  }

  // interpolate back to full grid
  ILOG(DEBUG_VERBOSE, "make higher res");
  pLowRes.interpolate(*this, pParm.pLowResNgridpts);

  InterfaceLL::doRegister("compute motion");

  // apply weights here
  pFinalMotion.pcMultiply(pWeightGrid);

  InterfaceLL::doRegister("compute motion");
  ILOG(DEBUG_VERBOSE, "Before smooth");
  for (int i=0; i<static_cast<int>(pParm.pHighResMotionSmooth.size()); ++i)
  {
    int v = pParm.pHighResMotionSmooth[i];
    ILOGF(DEBUG_VERBOSE, "Smoothing motion by %d", v);
    pFinalMotion.smooth(v, v, pParm.pExcludeZeroFromSmoothing);
    
  }
  ILOG(DEBUG_VERBOSE, "After smooth");

  // expand
  ILOG(DEBUG_VERBOSE, "Before expand");
  pFinalMotion.expand(pParm.pHighResMaxExpandNpt);
  ILOG(DEBUG_VERBOSE, "After expand");

  // now we shift the forecast at each point
  pFinalMotion.phaseShift(pParm.pFcstGapFillSmooth,
			  pParm.pFcstOutputSmooth, pGrids);

  ILOG(DEBUG_VERBOSE, "Phase correction applied..done");
  pInitialMotion.setPhaseCorrect(PcType::PC_NCAR);
  pFinalMotion.setPhaseCorrect(PcType::PC_NCAR);
}

//----------------------------------------------------------------
void PhaseCorrect::setNoMotion(void)
{
  pInitialMotion.init(pGrids);
  pInitialMotion.setToZero(PcType::NO_PC);
  pFinalMotion.init(pGrids);
  pFinalMotion.setToZero(PcType::NO_PC);
}

//----------------------------------------------------------------
void PhaseCorrect::interpolate(PhaseCorrect &highresData, int resolution) const
{
  // local object is low res
  if (!highresData.pInitialMotion.pcmotionInterpolate(pInitialMotion,
						      resolution))
  {
    exit(convWx::BAD_EXIT);
  }
  if (!highresData.pFinalMotion.pcmotionInterpolate(pFinalMotion, resolution))
  {
    exit(convWx::BAD_EXIT);
  }
  if (!highresData.pWeightGrid.interpolate(pWeightGrid, resolution))
  {
    exit(convWx::BAD_EXIT);
  }

  // re-scale motion units back to larger grid resolution
  highresData.pInitialMotion.multiply(
				      static_cast<double>(pParm.pLowResNgridpts));
  highresData.pFinalMotion.multiply(static_cast<double>(pParm.pLowResNgridpts));

  // copy over stats as is
  highresData.pStats = pStats;
}
 
//----------------------------------------------------------------
void PhaseCorrect::getOutput(const ParmProjection &proj, Grid &uGrid,
			     Grid &vGrid, MetaData &metadata,
			     PcStats &stats) const
{
  pFinalMotion.getOutput(proj.pDx, proj.pDy, uGrid, vGrid);
  metadata = pFinalMotion.getMetadata();
  // replace metadata UvUnits with external (meters per seconds),
  // because getOutput method has done that conversion.
  MetaDataXml xml = metadata.xmlMetadata();
  xml.replaceUvUnits(UvUnits(UvUnits::METERS_PER_SECOND));
  metadata.setXml(xml);

  stats = pStats;
}
 
//----------------------------------------------------------------
UvOutput PhaseCorrect::getFinalMotion(void) const
{
  return pFinalMotion;
}

//----------------------------------------------------------------
void PhaseCorrect::compute(void *inputInfo)
{
  PhaseCorrectInfo *info = static_cast<PhaseCorrectInfo *>(inputInfo);
  ILOG(DEBUG_VERBOSE, "In thread now");
  info->pBox->getPcOfSmoothed(info->pAlg->pGrids);
  delete info;
}

//----------------------------------------------------------------
std::string PhaseCorrect::leadThreadKey(int lt)
{
  char buf[convWx::ARRAY_LEN_LONG];
  sprintf(buf, "PhaseCorrect_%d", lt);
  string s = buf;
  return s;
}

//----------------------------------------------------------------
void PhaseCorrect::pComputeMotion(const ParmProjection &proj)
{
  ILOG(DEBUG_VERBOSE, "start computing motion");
  vector<PcVolume> vol;
  int nx, ny, dx, dy;

  // get dimensions and grid spacing
  nx = proj.pNx;
  ny = proj.pNy;
  dx = pParm.pVolSize[0] - pParm.pVolOverlap[0];
  dy = pParm.pVolSize[1] - pParm.pVolOverlap[1];

  ILOGF(DEBUG_VERBOSE, "Step size for boxes:%d,%d", dx, dy);

  // build all the volumes and get a shift for each one
  // this is the slow thing!!!!
  for (int y=0; y<ny; y+=dy)
  {
    for (int x=0; x<nx; x+=dx)
    {
      PcVolume box(x, y, pParm);
      box.getPcOfSmoothed(pGrids);
      box.updateStats(pStats);
      vol.push_back(box);
    }
  }

  // make sure start with 0 everywhere
  pInitialMotion.setToZero(PcType::NO_PC);

  // now average these Volume results into pMotion grid, accumulating as go
  vector<PcVolume>::iterator iv;
  for (iv=vol.begin(); iv!=vol.end(); ++iv)
  {
    InterfaceLL::doRegister("box");
    pInitialMotion.addVolume(*iv);
  }

  // normalize when done accumulating
  pInitialMotion.pcNormalize();

  // smooth these results
  InterfaceLL::doRegister("smooth");
  for (int i=0; i<static_cast<int>(pParm.pLowResMotionSmooth.size()); ++i)
  {
    int sm = pParm.pLowResMotionSmooth[i];
    pInitialMotion.smooth(sm, sm, pParm.pExcludeZeroFromSmoothing);
  }

  // now apply the convergence thresholding algorithm.
  pConvThresh.apply(pInitialMotion, proj, pFinalMotion);
}

//----------------------------------------------------------------
//----------------------------------------------------------------
void PhaseCorrect::pComputeMotionThreaded(const ParmProjection &proj,
					  int lt, 
					  ConvWxMultiThreadMgr *threads)
{
  ILOG(DEBUG_VERBOSE, "start computing motion");
  vector<PcVolume *> vol;
  int nx, ny, dx, dy;

  // get dimensions and grid spacing
  nx = proj.pNx;
  ny = proj.pNy;
  dx = pParm.pVolSize[0] - pParm.pVolOverlap[0];
  dy = pParm.pVolSize[1] - pParm.pVolOverlap[1];

  ILOGF(DEBUG_VERBOSE, "Step size for boxes:%d,%d", dx, dy);

  // build all the volumes and get a shift for each one
  // this is the slow thing!!!!
  for (int y=0; y<ny; y+=dy)
  {
    for (int x=0; x<nx; x+=dx)
    {
      PcVolume *box = new PcVolume(x, y, pParm);
      // box.getPcOfSmoothed(pGrids);
      vol.push_back(box);
    }
  }
  ILOGF(DEBUG, "%d total boxes", static_cast<int>(vol.size()));

  std::string key = leadThreadKey(lt);
  ILOGF(DEBUG, "Begin threading %s", key.c_str());

  // here introduce threading
  for (size_t k=0; k<vol.size(); ++k)
  {
    PhaseCorrectInfo *info = new PhaseCorrectInfo(vol[k], lt, this);
    threads->thread(key, k, static_cast<void *>(info));
  }
  ILOGF(DEBUG_VERBOSE, "Waiting for phase correct threading %s", key.c_str());
  threads->waitForThreads(key);
  ILOGF(DEBUG_VERBOSE, "Done Waiting for phase correct threading, %s",
	key.c_str());

  // make sure start with 0 everywhere
  pInitialMotion.setToZero(PcType::NO_PC);

  // double numCorrected = 0.0, numFixed=0.0;

  // now average these Volume results into pMotion grid, accumulating as go
  for (size_t i=0; i<vol.size(); ++i)
  {
    InterfaceLL::doRegister("box");
    // if (iv->printStats())
    // {
    //   ++numCorrected;
    // }
    // else
    // {
    //   ++numFixed;
    // }
    vol[i]->updateStats(pStats);
    pInitialMotion.addVolume(*vol[i]);
    delete vol[i];
  }
  
  ILOGF(DEBUG_VERBOSE,
       "Done averaging, now have deleted all phasecorrect thread related stuff, %s",
	key.c_str());
  // printf("%d boxes %d were corrected, %.4lf percent\n",
  // 	 static_cast<int>(numCorrected+numFixed), static_cast<int>(numCorrected),
  // 	 numCorrected/(numCorrected+numFixed));

  // normalize when done accumulating
  pInitialMotion.pcNormalize();

  // smooth these results
  InterfaceLL::doRegister("smooth");
  for (int i=0; i<static_cast<int>(pParm.pLowResMotionSmooth.size()); ++i)
  {
    int sm = pParm.pLowResMotionSmooth[i];
    pInitialMotion.smooth(sm, sm, pParm.pExcludeZeroFromSmoothing);
  }

  // now apply the convergence thresholding algorithm.
  pConvThresh.apply(pInitialMotion, proj, pFinalMotion);

  ILOGF(DEBUG, "DONE with phasecorrect, %s", key.c_str());
}

