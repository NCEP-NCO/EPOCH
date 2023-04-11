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
 * @file Score.cc
 * @brief scoring motion choice
 */

//----------------------------------------------------------------
#include <cmath>
#include <string>
#include <vector>
#include <ConvWx/Score.hh>
#include <ConvWx/PcScore.hh>
#include <ConvWx/MultiGridsForPc.hh>
#include <ConvWx/ParmSetUV.hh>
#include <ConvWx/ConvWxConstants.hh>
using std::string;
using std::vector;

/**
 * Set const bad score here
 */
const double Score::pBadScore = -9999.9;

//----------------------------------------------------------------
/**
 * @return the S(dx, dy) part of the phase correction
 * @param[in] xoff  X offset
 * @param[in] yoff  Y offset
 * @param[in] nxy  measure of distance per grid point
 */
static double sS(const int xoff, const int yoff, const int nxy[2])
{
  double l = static_cast<double>(nxy[0]*nxy[0] + nxy[1]*nxy[1]);
  l = 0.5*sqrt(l);
  double x = static_cast<double>(xoff*xoff + yoff*yoff);
  x = sqrt(x);
  x = x/l;
  x = exp(x)/(1.0 + x);
  return x;
}

//----------------------------------------------------------------
Score::Score() :
  pXoff(0),
  pYoff(0),
  pDistance(0.0),
  pScore(pBadScore)
{
}

//----------------------------------------------------------------
Score::Score(const int xoffset, const int yoffset) : 
  pXoff(xoffset),
  pYoff(yoffset),
  pDistance(sqrt(static_cast<double>(pXoff*pXoff + pYoff*pYoff))),
  pScore(pBadScore)
{
}

//----------------------------------------------------------------
Score::~Score()
{
}

//----------------------------------------------------------------
bool Score::operator==(const Score &s) const
{
  return (pXoff == s.pXoff &&
	  pYoff == s.pYoff &&
	  pDistance == s.pDistance &&
	  pScore == s.pScore);
}

//----------------------------------------------------------------
string Score::sprint(void) const
{    
  char buf[convWx::ARRAY_LEN_VERY_LONG];
  sprintf(buf, "x,y:%d,%d  d:%lf  s:%lf", pXoff, pYoff, pDistance, pScore);
  string s = buf;
  return s;
}

//----------------------------------------------------------------
double Score::setScoreFromSmoothed(const int x0, const int y0, 
				   const MultiGridsForPc &dataGrids,
				   const ParmSetUV &parms, string &debugMsg)
{
  debugMsg = "";

  vector<PcScore> pcscore;

  // build a vector of PcScore objects for this volume (one per data type)
  if (!dataGrids.buildScores(x0, y0, pXoff, pYoff, parms.pVolSize, pcscore))
  {
    debugMsg = "..No score at this offset";
    pScore = pBadScore;
    return pScore;
  }

  // build up sums needed to compute score from this vector
  double sumAlpha = 0;
  double sumScore = 0;
  bool scoreSet = false;
  for (int i=0; i<static_cast<int>(pcscore.size()); ++i)
  {
    if (pcscore[i].incrementScoreVars(sumScore, sumAlpha, debugMsg))
    {
      scoreSet = true;
    }
  }
  if (!scoreSet)
  {
    pScore = pBadScore;
  }
  else
  {
    // compute the actual score
    pScore = sS(pXoff, pYoff, parms.pVolSize)*sumScore/sumAlpha;
  }

  char buf[convWx::ARRAY_LEN_SHORT];
  sprintf(buf, " score=%lf", pScore);
  debugMsg += buf;
  return pScore;
}

//----------------------------------------------------------------
double Score::refineFromSmoothed(const int x0, const int y0,
				 const MultiGridsForPc &dataGrids,
				 const ParmSetUV &parms)
{
  ILOGF(DEBUG_VERBOSE, "Refining:%s", sprint().c_str());

  // keep track of best score as found on input
  Score sbest(*this);
  double scoreBest = pScore;
  bool didChange = false;

  // look in refinement window around the initial offsets
  int min = -parms.pRefineMaxShift*parms.pRefineShiftResNpt;
  int max  = parms.pRefineMaxShift*parms.pRefineShiftResNpt;
  int delta = parms.pRefineShiftResNpt;
  for (int y= min; y<=max; y += delta)
  {
    for (int x= min; x<=max; x+= delta)
    {
      // build a score at this refinement point
      Score stest(pXoff+x, pYoff+y);
      string debugMsg;
      double sxy = stest.setScoreFromSmoothed(x0, y0, dataGrids, parms,
					      debugMsg);
      ILOGF(DEBUG_VERBOSE, "testing refinement=%d,%d   p.c.=%d,%d %s",
	    x, y, pXoff+x, pYoff+y, debugMsg.c_str());
      if (stest.isBadScore(sxy))
      {
	continue;
      }

      // if its a smaller score, than we switch to that one.
      if (sxy < scoreBest)
      {
	ILOGF(DEBUG_VERBOSE, " Did refine:%s", stest.sprint().c_str());
	sbest = stest;
	scoreBest = sxy;
	didChange = true;
      }
    }
  }
  if (didChange)
  {
    // overwrite with refined best score object
    *this = sbest;
  }
  // return the score
  return pScore;
}

