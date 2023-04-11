#include "EnsFcstCombMgr.hh"
#include <ConvWxIO/InterfaceParm.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/Trigger.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWx/InterfaceLL.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/Xml.hh>
#include <algorithm>


using std::vector;
using std::pair;
using std::string;
using std::tuple;

const double EnsFcstCombMgr::ensFcstMissing = -9999.0;

EnsFcstCombMgr::EnsFcstCombMgr(const ParmsEnsFcstCombIO &params,
			       void cleanExit(int)):
  pParams(params),
  pTrigger(NULL)
{
  time_t t = time(0);

  LOG(DEBUG) << "Restarted at " << ConvWxTime::stime(t);

  vector<string> urls;
  
  for (size_t i=0; i<pParams.modelInput.size(); ++i)
  {
    LOG(DEBUG) << pParams.modelInput[i].pUrl;
    urls.push_back(pParams.modelInput[i].pUrl);
  }

  if (params.main.isRealtime())
  {
    pTrigger = new DsEnsembleLeadTrigger(urls, params.leadSeconds);
    if (params.main.pDebugTrigger)
    {
      pTrigger->setDebug();
    }
    pTrigger->setMaximumAgeSeconds(params.maxAgeSeconds);
    pTrigger->setTriggerMaxWaitSeconds(params.triggerFeedbackSeconds);
    pTrigger->setMaxSecondsBeforeTimeout(params.urlTimeoutSeconds);
    pTrigger->setMaxSecondsBeforeDisable(params.urlDisableSeconds);
  }
  else
  {
    pTrigger = new DsEnsembleLeadTrigger(params.main.pArchiveT0,
                                         params.main.pArchiveT1,
                                         urls, params.leadSeconds);
  }

  // create a fake missing data multigrid
  for (size_t i=0; i<params.fieldNames.size(); ++i)
  {
    Grid g(params.fieldNames[i], "none", params.proj.pNx, params.proj.pNy, 0.0);
    pMissingDataGrid.append(g);
  }
}

EnsFcstCombMgr::~EnsFcstCombMgr()
{
  if (pTrigger)
    delete pTrigger;

  time_t t = time(0);

  LOG(DEBUG) << "Terminated at " << ConvWxTime::stime(t);
}

EnsFcstCombMgr::Status_t EnsFcstCombMgr::run(void)
{
  //
  // Register with process mapper 
  // 
  InterfaceLL::doRegister("Running EnsFcstComb algorithm");

  //
  // Forecast generation time
  //
  time_t genTime;
 
  //
  // Forecast generation time
  //
  int leadTime;

  //
  // Subset of urls which updated at genTime and leadTime
  //
  vector<string> url;

  //
  // Flag to indicate that all urls passed into the trigger constructor
  // updated (or not)
  //
  bool complete;

  //
  // Process data while trigger returns valid generation and lead time pair
  //
  while (pTrigger->nextTrigger(genTime, leadTime, url, complete))
  {
    pProcessTrigger(genTime, leadTime, url, complete);
  }

  return ENSFCSTCOMB_SUCCESS;
}

void EnsFcstCombMgr::pProcessTrigger(const time_t &genTime, int leadTime,
				     const std::vector<std::string> &url,
				     bool complete)
{
  //
  // Process generation time if it is in the specified list of hours to process.
  // Fill time struct to access hour of day corresponding to genTime.
  //
  tm *tmPtr = gmtime( &genTime);

  //
  // Get generation time hour of day ( integer in [0,23])
  //
  int genTimeHour = tmPtr->tm_hour;

  if ( find( pParams.genTimeProcessHours.begin(),
	     pParams.genTimeProcessHours.end(), 
	     genTimeHour) ==  pParams.genTimeProcessHours.end() )
  {
    LOG(DEBUG) << "Not processing " << ConvWxTime::stime(genTime)
	       << ". Gen hour is not in list of hours to process.";
    return;
  }

  //
  // pairs of gen/lead times
  //
  vector <tuple<time_t,int,bool> > genTimeLeadTimePairs =
    pSetTimePairs(genTime, leadTime, url, complete);

  InterfaceLL::doRegister("Processing forecast data");
      
  LOG(DEBUG) << "-----Trigger-----" << ConvWxTime::stime(genTime)
	     << "------";
      
  //
  // process the gen/lead times
  //
  pProcess(genTimeLeadTimePairs, genTime, leadTime);
}



std::vector<std::tuple<time_t,int,bool> >
EnsFcstCombMgr::pSetTimePairs(const time_t &genTime, int leadTime,
			      const std::vector<std::string> &url,
			      bool complete) const
{
  std::vector<std::tuple<time_t,int,bool> >  genTimeLeadTimePairs;
  if(complete)
  {	
    LOG(DEBUG) << "Triggered " << ConvWxTime::stime(genTime) << "+"
	       << leadTime << "(all complete)" << url.size();
    for (int i = 0; i < (int) pParams.modelInput.size(); i++)
    {
      tuple <time_t, int,bool> gtLt( genTime, leadTime, true);
      genTimeLeadTimePairs.push_back(gtLt);
    }
  }
  else
  {
    LOG(DEBUG) << "Triggered " << ConvWxTime::stime(genTime) << "+"
	       << leadTime << "for " << url.size() << " URLS, incomplete";
    for (int i = 0; i < (int) pParams.modelInput.size(); i++)
    {
      if ( find(url.begin(), url.end(),
		pParams.modelInput[i].pUrl) != url.end() )
      {
	
	genTimeLeadTimePairs.
	  push_back(tuple<time_t,int,bool>( genTime, leadTime, true));
      }
      else
      {
	//
	// Forecast data at genTime/leadTime does not exist for this url.
	// Search previous genTimes (12 hours) for a valid forecast.
	//
	time_t bestAvailGen;
	int bestAvailLead;
	    
	if (InterfaceIO::newestFcstAtValidTime( pParams.modelInput[i].pUrl,
						genTime -12*3600,
						genTime,
						genTime + leadTime,
						bestAvailGen,
						bestAvailLead) )
	{
	  
	  genTimeLeadTimePairs.
	    push_back(tuple<time_t, int, bool>(bestAvailGen, bestAvailLead,
					       true));

	  LOG(DEBUG) << "Best available data valid at "
		     << ConvWxTime::stime(genTime) << "+" << leadTime
		     << " for " << pParams.modelInput[i].pUrl
		     << " is " << ConvWxTime::stime(bestAvailGen)
		     << "+" << bestAvailLead;
	}
	else
	{
	  //
	  // We didnt find a forecast for this valid time. Put in the orginal
	  // gen and lead for which  we were searching. The data will fail to
	  // load in the process step. 
	  //
	  genTimeLeadTimePairs.
	    push_back(tuple<time_t,int,bool>(genTime, leadTime, false));
	}
      }
    }
  }
  return genTimeLeadTimePairs;
}


void
EnsFcstCombMgr::
pProcess(const vector<tuple<time_t, int,bool> > &genTimeLeadTimePairs,
	 const time_t &genTime, const int &leadTime)
{
  LOG(DEBUG) << "time="<< ConvWxTime::stime(genTime) << "+" << leadTime;

  InterfaceLL::doRegister("Processing forecast data");

  if (genTimeLeadTimePairs.size() != pParams.modelInput.size())
  {
    LOG(ERROR) << "Mismatch in vector length no output "
	       << genTimeLeadTimePairs.size() << " "
	       << pParams.modelInput.size();
    return;
  }
  
  //
  // create a multiGrid with one Grid per input named field (based on params)
  // and initialize to all zero
  //
  MultiGrid mGrid;
  for (size_t i=0; i<pParams.fieldNames.size(); ++i)
  {
    LOG(DEBUG) <<"Creating and initializing output grid "
	       << pParams.fieldNames[i];
    Grid cGrid(pParams.fieldNames[i], "none", pParams.proj.pNx,
	       pParams.proj.pNy, pParams.proj.pNz, ensFcstMissing);
    cGrid.setAllToValue(0.0);
    mGrid.append(cGrid);
  }

  //
  // Multi grid container for output
  //
  MultiGrid mOutGrids;

  //
  // The inputs for each field, for each model, i.e. everything coming in
  // mInGrids[i] = all the inputs for i'th model input, set to a pair
  // where first is the MultiGrid, second is true when data is all missing
  //
  std::vector<MultiGrid> mInGrids;
  
  //
  // Loop through models and load data
  //
  for (int i = 0; i < (int) pParams.modelInput.size(); i++)
  {
    
    InterfaceLL::doRegister("processing models at given lead");

    MultiGrid mInGrid;

    if (std::get<2>(genTimeLeadTimePairs[i]))
    {
      //
      // Load all input fields for this model input
      //
      if (pLoadInputData(i, std::get<0>(genTimeLeadTimePairs[i]),
			 std::get<1>(genTimeLeadTimePairs[i]),
			 mInGrid) == ENSFCSTCOMB_SUCCESS)
      {  
	//  this should have data for each input. Process each one of them
	if (mInGrid.size() == pParams.fieldNames.size())
	{
	  mInGrids.push_back(mInGrid);
	}
	else
	{
	  LOG(ERROR) <<	"Logic error in setting up read, input "
		     << i << ", no action";
	  return;
	}
      }
      else
      {
	LOG(ERROR) << "Should have been able to access data,no action";
	return;
      }
    }
    else
    {
      LOG(WARNING) << "All missing data for " << pParams.modelInput[i].pUrl;
      mInGrids.push_back(pMissingDataGrid);
    }
  }
  
  // now loop through each field and process that input from each model
  for (size_t i=0; i<pParams.fieldNames.size(); ++i)
  {
    // loop through each model (j) and pull out the i'th input field
    vector<Grid> inputGrids;
    for (size_t j=0; j<mInGrids.size(); ++j)
    {
      Grid gridCopy = (mInGrids[j])[i];
      // make a unique output name, because these fields are output
      // for debugging.  Use the pName which should be unique per model,
      // and the fieldName
      gridCopy.changeName(pParams.modelInput[j].pName + "_"
			  + pParams.fieldNames[i]);
      inputGrids.push_back(gridCopy);
    }

    // process this set of input fields to produce a combined output grid
    Grid combGrid;
    if (!pProcessInputField(inputGrids, pParams.fieldNames[i], combGrid))
    {
      return;
    }

    // add that result to the outputs, and add in the renamed inputs
    mOutGrids.append(combGrid);
    for (int i = 0; i < (int) inputGrids.size(); i++)
    {
      mOutGrids.append(inputGrids[i]);
    }	
  }
  
  //
  // Output everything
  //
  pOutputData( genTime, leadTime, genTimeLeadTimePairs, mOutGrids);
}

void EnsFcstCombMgr::
pOutputData(const time_t &genTime, const int &leadTime,
	    const vector<tuple<time_t,int,bool> > &genTimeLeadTimePairs,
	    MultiGrid &mOutGrids)
{

  InterfaceLL::doRegister("Writing data");

  LOG(DEBUG) << "Writing data to " << pParams.modelOut.pUrl
	     << " for " << ConvWxTime::stime(genTime) << "+" << leadTime;
  
  //
  // Meta data object (empty)
  //
  MetaData metaData;
  if (genTimeLeadTimePairs.size() != pParams.modelInput.size())
  {
    LOG(ERROR) << "Mismatch in URLs/gen/lead lengths, no metadata";
  }	  
  else
  {
    std::string s = "";
    for (size_t i=0; i<genTimeLeadTimePairs.size(); ++i)
    {    
      string si = Xml::writeString("URL", 1, pParams.modelInput[i].pUrl);
      if (std::get<2>(genTimeLeadTimePairs[i]))
      {
	si += Xml::writeTime("GenTime", 1,std::get<0>(genTimeLeadTimePairs[i]));
	si += Xml::writeInt("LeadTime", 1,std::get<1>(genTimeLeadTimePairs[i]));
	si += Xml::writeBoolean("AllMissing", 1, false);
      }
      else
      {
	si += Xml::writeBoolean("AllMissing", 1, true);
      }
      s += Xml::writeString("Fcst", 0, si);
    }
    metaData.xmlAddFreeform(s);
  }
  
  //
  // Output data
  //
  ParmFcstIO parm(pParams.modelOut);

  mOutGrids.setEncoding( pParams.outputEncodingType);

  parm.write(genTime, leadTime, pParams.proj, mOutGrids, metaData);
}


EnsFcstCombMgr::Status_t 
EnsFcstCombMgr::pLoadInputData(const int fcstNum,
			       const time_t &genTime,
			       const int &leadTime,
			       MultiGrid &mInGrid)
{
  //
  // Object to hold multiple gridded data fields corresponding
  // to the same generation and lead time
  //
  LOG(DEBUG) << "Loading data for "
	     << ConvWxTime::stime(genTime)
	     << "+" << leadTime << " at url "
	     << pParams.modelInput[ fcstNum].pUrl;

  InterfaceLL::doRegister("Loading data");

  MultiFcstGrid mfg;
  if (!InterfaceIO::loadMultiFcst(genTime, leadTime, pParams.proj,
                                  pParams.modelInput[fcstNum].pUrl,
				  pParams.fieldNames,
                                  pParams.modelInput[fcstNum].pRemap, mfg))
  {
    LOG(WARNING) << "Failure to load data for "
	     << ConvWxTime::stime(genTime)
	     << "+" << leadTime << " at url "
	     << pParams.modelInput[ fcstNum].pUrl;
    return ENSFCSTCOMB_FAILURE;
  }
  else
  {
    LOG(DEBUG) << "Data loaded for "
	       << ConvWxTime::stime(genTime)
	       << "+" << leadTime << " at url "
	       << pParams.modelInput[ fcstNum].pUrl;
    mInGrid = mfg;
    return ENSFCSTCOMB_SUCCESS;
  }
}

bool EnsFcstCombMgr::pProcessInputField(const std::vector<Grid> &inputGrids,
					const std::string &outputFieldName,
					Grid &combProbGrid)
{      
  if (inputGrids.empty())
  {
    LOG(ERROR) << "No input data";
    return false;
  }
  combProbGrid = inputGrids[0];
  combProbGrid.setAllToValue(0.0);
  combProbGrid.changeName(outputFieldName);
  Grid sumWgtsDenom(combProbGrid);
  for (size_t i=0; i<inputGrids.size(); ++i)
  {    
    for (int j = 0; j< inputGrids[i].getNdata(); j++)
    {
      double val; 
      if (  inputGrids[i].getValue(j, val) )
      {
	//
	// calculate running sum of the weighted values
	//
	double runningSumNumerator;

	combProbGrid.getValue(j, runningSumNumerator);
	combProbGrid.setv(j, runningSumNumerator + val * pParams.weights[i]);

	//
	// calculate running sum of the weights
	//
	double runningSumDenominator;

	sumWgtsDenom.getValue(j,runningSumDenominator);
	sumWgtsDenom.setv(j, runningSumDenominator +  pParams.weights[i]);
      }
    }
  } 
  // normalize the results
  for (int j = 0; j< combProbGrid.getNdata(); j++)
  {
    double numerator;
    double denominator;
    if (combProbGrid.getValue(j, numerator) &&
	sumWgtsDenom.getValue(j, denominator) )
    {
      if (denominator > 0)
      {
	combProbGrid.setv(j,numerator/denominator);
      }
      else
      {
	combProbGrid.setv(j, ensFcstMissing);
      }
    }
  }
  return true;
}
