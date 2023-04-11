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
 * @file ConvWxPjgLatlonMath.cc
 */

#include <ConvWx/ConvWxPjgLatlonMath.hh>
using std::ostream;
using std::endl;


//------------------------------------------------------------------
ConvWxPjgLatlonMath::ConvWxPjgLatlonMath(double earthRadiusKm,
					 double originLon) :
  ConvWxPjgMath(earthRadiusKm)
{
  pProjType = ParmProjection::LATLON;
  pOriginLon = originLon;
}

//------------------------------------------------------------------
ConvWxPjgLatlonMath::~ConvWxPjgLatlonMath(void)
{
}
					  
//------------------------------------------------------------------
void ConvWxPjgLatlonMath::setOffsetOrigin(double offsetLat, double offsetLon)
{

}

//------------------------------------------------------------------
void ConvWxPjgLatlonMath::setOffsetCoords(double falseNorthing,
					  double falseEasting)
{

}
  
//------------------------------------------------------------------
void ConvWxPjgLatlonMath::print(ostream &out) const
{
  out << "  Projection: LATLON " << endl;
  if (pOriginLon != 0.0)
  {
    out << "  origin_lon (deg): " << pOriginLon << endl;
  }
}

//------------------------------------------------------------------
void ConvWxPjgLatlonMath::printDetails(ostream &out) const
{
  print(out);
}

//------------------------------------------------------------------
void ConvWxPjgLatlonMath::latlon2xy(double lat, double lon,
                              double &x, double &y) const
{
  y = lat;
  x = lon;
}

//------------------------------------------------------------------
void ConvWxPjgLatlonMath::xy2latlon(double x, double y,
                              double &lat, double &lon) const
{
  lat = y;
  lon = x;
}
     
