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
/** 
 * @file DigitalVilConverter.cc
 * @brief Source for DigitalVilConverter class 
 */

#include<cmath>

#include <ConvWx/DigitalVilConverter.hh>

const double DigitalVilConverter::minVil = 0;

const double DigitalVilConverter::maxVil = 253;

DigitalVilConverter::DigitalVilConverter(const double lowThresh, 
					 const double highThresh, 
					 const double a0, const double a1, 
					 const double b0, const double b1, 
					 const double b2, const double cc,
					 const double ee):
  pLowThresh(lowThresh),
  pHighThresh(highThresh),
  pA0(a0), 
  pA1(a1),
  pB0(b0),
  pB1(b1),
  pB2(b2),
  pCc(cc),
  pEe(ee)
{   
}

DigitalVilConverter::~DigitalVilConverter()
{
}

void DigitalVilConverter::runAlg(GridData &grid) const
{ 
  //
  // Number of points in the grid
  //
  int npts = grid.getNdata();
  
  for (int i = 0; i < npts; i++)
  {
    //
    // If vil <= lowYhresh then digVil = minVil
    // If lowThresh < vil < highThresh then digVil =  a0 + a1 * vil
    // If vil >= highThresh then digVil = cc * (b0 + b1 * log (b2 * vil^ee ))
    //
    double vil;

    //
    // Scaled VIL
    //
    double digVil;
    
    if (grid.getValue(i, vil))
    {
      if (vil <= pLowThresh)
      {
	digVil = minVil;
      }
      else if ( pLowThresh < vil && vil < pHighThresh)
      {
	digVil =  pA0 +  pA1 * vil;
      }
      else
      {
	digVil = pCc * (pB0 + pB1 * (log (pB2)  + pEe * log (vil)));
      }

      if ( digVil > maxVil )
      {
	digVil = maxVil;
      }
      
      grid.setv(i,digVil);
    }
  } 
}

