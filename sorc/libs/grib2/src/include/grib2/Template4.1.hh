// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/**
 * @file Template4.1.hh
 * @author Jason Craig
 * @date   Aug 2006
 */

#ifndef _GRIB2_TEMPLATE_4_PT_1 
#define _GRIB2_TEMPLATE_4_PT_1

#include <dataport/port_types.h>
#include <grib2/ProdDefTemp.hh>
#include <grib2/constants.h>

using namespace std;

namespace Grib2 {

/** 
 * @class Template4_pt_1
 *
 * A derived Product Definition Template class for 
 * Individual ensemble forecast, control and perturbed, at a
 * horizontal level or in a horizontal layer at a point in time
 *
 * See http://www.nco.ncep.noaa.gov/pmb/docs/grib2/grib2_temp4-1.shtml
 *
 * @note The pack and unpack routines are static methods in Grib2::GribSection
 */
class Template4_pt_1: public ProdDefTemp {

public:
  /**
   * @brief Default constructor for use in passing object to GribFile.addField()
   *
   * @note After creation.. all public values variables below should be set
   * before passing this object to GribFile. 
   */
  Template4_pt_1();

  /**
   * @brief This constructor is used internally to the grib2 library when reading
   * a grib2 file.
   */
  Template4_pt_1(Grib2Record::Grib2Sections_t sectionsPtr);

  virtual ~Template4_pt_1();

  /** @brief Unpack a Product Definition Template 
   *  @param[in] projPtr Pointer to start of template
   *  @return Either GRIB_SUCCESS or GRIB_FAILURE */
  int unpack (ui08 *projPtr);

  /** @brief Pack up this Product Definition Template
   *  @param[in] projPtr Pointer to start of location to pack template
   *  @return Either GRIB_SUCCESS or GRIB_FAILURE */
  int pack (ui08 *projPtr);

  /** @brief Print to stream/file all information for this template */
  void print (FILE *) const;

  /** @brief Get the record summary for this grib record 
   *  @param[out] summary Record summary struct */
  void getRecSummary (Grib2Record::rec_summary_t *summary);

  /** @brief Get the forecast time for this grib record 
   *  @return Forecast lead time in seconds */
  long int getForecastTime() const;

  /** @brief Get the size of the packed derived template class. 
   *  @return Size of packed template in bytes */ 
  virtual si32 getTemplateSize() { return TEMPLATE4_PT_1_SIZE; };

  /** @brief Type of generating process */
  si32 _processType;             
  /** @brief Background generating process identifier */
  si32 _backgrdProcessId;        
  /** @brief Hours of observational data cutoff after reference time */
  si32 _hoursObsDataCutoff;      
  /** @brief Minutes of observational data cutoff after reference time */
  si32 _minutesObsDataCutoff;    
  /** @brief Indicator of unit of time range  */
  si32 _timeRangeUnit;           
  /** @brief In units defined by _timeUintRange */
  si32 _forecastTime;            
  /** @brief Type of first fixed surface */
  si32 _firstSurfaceType;       
  /** @brief Scale factor of first fixed surface */
  si32 _scaleFactorFirstSurface; 
  /** @brief Scale value of first fixed surface */
  si32 _scaleValFirstSurface;    
  /** @brief Type of second fixed surface */
  si32 _secondSurfaceType;      
  /** @brief Scale factor of second fixed surface */
  si32 _scaleFactorSecondSurface;
  /** @brief Scale value of second fixed surface */
  si32 _scaleValSecondSurface;   
  /** @brief Type of ensemble forecast (see Code table 4.6) */
  si32 _ensembleType;            
  /** @brief Perturbation number */
  si32 _perturbationNum;         
  /** @brief Number of forecasts in ensemble */
  si32 _numForecasts;            

protected:

private: 
  
  static const si32 TEMPLATE4_PT_1_SIZE;

};

} // namespace Grib2

#endif

