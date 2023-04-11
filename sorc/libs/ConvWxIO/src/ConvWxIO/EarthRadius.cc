/**
 * @file EarthRadius.cc
 */

#include <ConvWxIO/EarthRadius.hh>
#include <euclid/geometry.h>
#include <toolsa/TaPjg.hh>

void EarthRadius::init(void)
{
  // no op
}

void EarthRadius::setEarthRadiusKm(double earthRadiusKm)
{
  EG_set_earth_radius_km(earthRadiusKm);
  TaPjg::setEarthRadius(earthRadiusKm);
}  
