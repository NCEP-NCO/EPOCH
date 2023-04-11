/**
 * @file PrecipAccumCalc.cc
 * @brief Source for PrecipAccumCalc class
 */
#include <unistd.h>
#include "PrecipAccumCalc.hh"
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/Grid.hh>
#include <toolsa/LogStream.hh>

using std::vector;
using std::string;

PrecipAccumCalc::PrecipAccumCalc(const time_t &genTime, 
				 const int &leadTime, 
				 const std::string &url,
				 const ParmsPrecipAccumCalcIO &params):
  pGenTime(genTime),
  pLeadTime(leadTime),
  pUrl(url),
  pParams(params)
{ 
  pEnsembleMem = pUrl.substr( pUrl.length() -5,5);
}

PrecipAccumCalc::~PrecipAccumCalc()
{
 
}

void PrecipAccumCalc::process()
{
  LOG(DEBUG) << "Processing gen " << ConvWxTime::stime(pGenTime)
	     << " lead " << pLeadTime << " at url " << pUrl; 

  // 1) Get the 3hrly accum fields from lead i-1, Append to fcst grids for lead i-1. 
  // 2) Get 6hrly accum fields from lead i, use these and 3 hrly accum fields from lead i-1
  //    to calculate 3 hrly accum fields for lead i. Append to fcst grids for lead i
  // 3) Append 6 hrly accum fields to fcst grids for lead i.
  //
  
  //
  // Load data for 6 hour accumulations at present lead,
  // load data for 3 hour accum from previous lead
  //
  if ( pLoadInputData() ==  CALC_FAILURE)
  {
    LOG(WARNING) << "Data fields not available for gen "
		 << ConvWxTime::stime(pGenTime)
		 << " lead " << pLeadTime << " at url " << pUrl; 
    return;
  }

  //
  // get the apcp3hr grid and create FcstGrid
  // 
  const Grid *hr3AccumField0Ptr =
    pMultiInGrid3hrAccumPrevLead.gridPtr(pParams.hr3AccumName, true);

  FcstGrid apcp3hrPrevLead(pGenTime, pLeadTime - 10800, *hr3AccumField0Ptr);

  //
  // Get the ulwrf grid and create a FcstGrid
  // 
  const Grid *hr3UlwrfPtr =
    pMultiInGrid3hrAccumPrevLead.gridPtr(pParams.hr3UlwrfName, true);
  
  FcstGrid ulwrf3hrPrevLead(pGenTime,pLeadTime - 10800, *hr3UlwrfPtr);

  //
  // set output data type-- default is INT8
  //
  if (pParams.outputDataType == ParmsPrecipAccumCalc::INT16)
  {
    apcp3hrPrevLead.setEncoding(Grid::ENCODING_INT16); 
    ulwrf3hrPrevLead.setEncoding(Grid::ENCODING_INT16); 
    for (int i = 0; i < (int) pParams.extraDataFields.size(); i++)
    {
      const Grid *gridPtr = pMultiInGrid3hrAccumPrevLead.gridPtr( pParams.extraDataFields[i], true);
      FcstGrid fgrid ( pGenTime,pLeadTime - 10800, *gridPtr);
      fgrid.setEncoding(Grid::ENCODING_INT16);
      pMultiOutGrid3hrAccumPrevLead.append(fgrid);
    }

  }
  else if ( pParams.outputDataType == ParmsPrecipAccumCalc::FLOAT32)
  {
    apcp3hrPrevLead.setEncoding(Grid::ENCODING_FLOAT32);
    ulwrf3hrPrevLead.setEncoding(Grid::ENCODING_FLOAT32);
    for (int i = 0; i < (int) pParams.extraDataFields.size(); i++)
    {
      const Grid *gridPtr = pMultiInGrid3hrAccumPrevLead.gridPtr( pParams.extraDataFields[i], true);
      FcstGrid fgrid ( pGenTime,pLeadTime - 10800, *gridPtr);
      fgrid.setEncoding(Grid::ENCODING_FLOAT32);
      pMultiOutGrid3hrAccumPrevLead.append(fgrid);
    }
  }

  //
  // Append 3hr accumulation fields from previous lead time 
  // to multi forecast grid object
  //
  pMultiOutGrid3hrAccumPrevLead.append(apcp3hrPrevLead);
  pMultiOutGrid3hrAccumPrevLead.append(ulwrf3hrPrevLead);
    
  //
  // Get the apcp6hr grid and create a FcstGrid
  //
  const Grid *hr6AccumField0Ptr =
    pMultiInGrid6hrAccum.gridPtr(pParams.hr6AccumName, true);

  FcstGrid apcp6hr(pGenTime,pLeadTime, *hr6AccumField0Ptr);

  //
  // Get the 6 hour ulwrf grid and create a FcstGrid
  // 
  const Grid *hr6UlwrfPtr =
    pMultiInGrid6hrAccum.gridPtr(pParams.hr6UlwrfName, true);
 
  FcstGrid ulwrf6hr(pGenTime, pLeadTime, *hr6UlwrfPtr);

  //
  // Create  apcp3hr accum for the current lead time from pcp6hr accum 
  // by subtracting apcp3hr from the previous lead time.
  // Keep field name consistent with previous lead time.
  // Keep accumulations non negative.
  //
  FcstGrid apcp3hr = apcp6hr;

  apcp3hr.subtract(apcp3hrPrevLead);
  apcp3hr.changeName(apcp3hrPrevLead.getName());
  if (pParams.forceAccumNonNegative)
  {
    apcp3hr.setDataInRangeToValue(-1,0,0);
  }

  //
  // Create the ULWRF3Hr = 2*ULWRF6Hr - ULWRF3Hr from the previous lead. Keep 3hr ULWRF 
  // name consistent with name the previous lead
  //
  FcstGrid ulwrf3hr = ulwrf6hr;
  ulwrf3hr.multiply(2.0);
  ulwrf3hr.subtract(ulwrf3hrPrevLead);
  if (pParams.forceAccumNonNegative)
  {
    ulwrf3hr.setDataInRangeToValue(-1,0,0);
  }
  ulwrf3hr.changeName(pParams.hr3UlwrfName);

  //
  // set output data type-- default is INT8
  //
  if ( pParams.outputDataType == ParmsPrecipAccumCalc::INT16)
  {
    apcp3hr.setEncoding(Grid::ENCODING_INT16); 
    ulwrf3hr.setEncoding(Grid::ENCODING_INT16); 
    for (int i = 0; i < (int) pParams.extraDataFields.size(); i++)
    {
      const Grid *gridPtr = pMultiInGrid6hrAccum.gridPtr( pParams.extraDataFields[i], true);
      FcstGrid fgrid ( pGenTime,pLeadTime, *gridPtr);
      fgrid.setEncoding(Grid::ENCODING_INT16);
      pMultiOutGrid3hrAccum.append(fgrid);
    }
  }
  else if ( pParams.outputDataType == ParmsPrecipAccumCalc::FLOAT32)
  {
    apcp3hr.setEncoding(Grid::ENCODING_FLOAT32);
    ulwrf3hr.setEncoding(Grid::ENCODING_FLOAT32);
    for (int i = 0; i < (int) pParams.extraDataFields.size(); i++)
    {
      const Grid *gridPtr = pMultiInGrid6hrAccum.gridPtr( pParams.extraDataFields[i], true);
      FcstGrid fgrid ( pGenTime,pLeadTime, *gridPtr);
      fgrid.setEncoding(Grid::ENCODING_FLOAT32);
      pMultiOutGrid3hrAccum.append(fgrid);
    }
  }
  pMultiOutGrid3hrAccum.append(apcp3hr);
  pMultiOutGrid3hrAccum.append(ulwrf3hr);	      
}


PrecipAccumCalc::Status_t PrecipAccumCalc::pLoadInputData()
{
  //
  // Object to hold multiple gridded data fields corresponding
  // to the same generation and lead time
  //
  LOG(DEBUG) << "Loading data for gen " << ConvWxTime::stime(pGenTime)
	     << " lead " << pLeadTime << " at url " << pUrl; 
  
  InterfaceLL::doRegister("Loading data"); 
  
  if (pLeadTime % 21600 != 0)
  {
    return CALC_SUCCESS;
  }
  if (!InterfaceIO::loadMultiFcst(pGenTime, pLeadTime, pParams.proj, 
				  pUrl,
				  pParams.longNames6hr,
				  false, 
				  pMultiInGrid6hrAccum))
  {
    LOG(ERROR) << "Failure to load data for gen "
	       << ConvWxTime::stime(pGenTime)
	       << " lead " << pLeadTime << " at url " << pUrl; 
    return CALC_FAILURE;
  }
  else
  {
    LOG(DEBUG) << "6hr accum and ulwrf data loaded";
    if (!pParams.longToShort(6, pMultiInGrid6hrAccum))
    {
      return CALC_FAILURE;
    }
  }

  if (!InterfaceIO::loadMultiFcst(pGenTime, pLeadTime-10800, pParams.proj, 
				  pUrl,
				  pParams.longNames3hr,
				  false, 
				  pMultiInGrid3hrAccumPrevLead))
  {
    LOG(ERROR) << "Failure to load fcst data for gen "
	       << ConvWxTime::stime(pGenTime)
	       << " lead " << pLeadTime << " at url " << pUrl; 
    return CALC_FAILURE;
  }
  else
  {
    LOG(DEBUG) << "3hr accum and ulwrf data loaded";
    if (!pParams.longToShort(3, pMultiInGrid3hrAccumPrevLead))
    {
      return CALC_FAILURE;
    }
  }
  return CALC_SUCCESS;
}

void PrecipAccumCalc::pWriteOutput(const time_t gtOut, const int ltOut, 
				      const MultiFcstGrid &mOutGrid, 
				      const string &outputDirTail) 
{
  //
  // Set output metadata with number of days in climatology
  //
  MetaData md;
  
  MetaDataXml xml;
  
  md.setXml(xml);
  
  LOG(DEBUG) << "Writing data for gen "
	     << ConvWxTime::stime(pGenTime)
	     << " lead " << pLeadTime << " to url "
	     << pParams.modelOut.pUrl;

  //
  // Write data
  //
  string url =  pParams.modelOut.pUrl + "/" + outputDirTail + "/" + pEnsembleMem ;

  InterfaceIO::write(gtOut, ltOut, url, pParams.proj, mOutGrid, md);  
}


void PrecipAccumCalc::write()
{
  //
  // Write output for 3 hour accumulations for previous lead time
  //

  pWriteOutput(pGenTime, pLeadTime - 10800,  pMultiOutGrid3hrAccumPrevLead,  
	       pParams.hr3AccumOutputDirTail);
  
  //
  // Write output for 3hr accumulations for current lead time
  //
  pWriteOutput(pGenTime, pLeadTime,  pMultiOutGrid3hrAccum,  
	       pParams.hr3AccumOutputDirTail);

}
