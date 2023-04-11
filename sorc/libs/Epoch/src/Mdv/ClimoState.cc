/**
 * @file ClimoState.cc
 * @brief Source for ClimoState class
 */
#include <Epoch/ClimoState.hh>
#include <Epoch/CountSumNames.hh>
#include <Epoch/ClimoStateGrids.hh>
#include <ConvWxIO/ParmFcstIO.hh>
#include <ConvWxIO/InterfaceIO.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
using std::string;

const double ClimoState::_climoMeanMissing = -9999.0;

//-------------------------------------------------------------------------
ClimoState::ClimoState(bool isCloudTop) : _ok(false), _isCloudTop(isCloudTop)
{
}

//-------------------------------------------------------------------------
ClimoState::ClimoState(const time_t &inputGenTime, int inputLeadTime,
		       const MultiFcstGrid &inputData, bool isCloudTop) :
  _ok(true),
  _isCloudTop(isCloudTop),
  _data(inputData)
{
  // for cloud top want everything, for non-cloud top want at least one
  // gen time
  bool needGen=true;
  _metadata = MdvMetadata(_data.getMetadataXml().getFreeformXml(),
			  needGen, _isCloudTop);
}

//-------------------------------------------------------------------------
ClimoState::~ClimoState()
{

}

//-------------------------------------------------------------------------
std::vector<time_t> ClimoState::tooOld(const time_t &genTime,
				       int daysInClimo) const
{
  return _metadata.tooOld(genTime, daysInClimo);
}

//-------------------------------------------------------------------------
void ClimoState::purgeForecast(const time_t &genTime, int leadTime,
			       const MultiFcstGrid &data)
{
  // remove this data from state, decrementing some counters and sums
  _remove(genTime, data);

  // erase gentime from metadata too
  _metadata.erase(genTime);
}

//-------------------------------------------------------------------------
bool ClimoState::addToState(const time_t &genTime,
			    const MultiFcstGrid &mdata,
			    Grid::Encoding_t encoding)
{
  bool isFirst = false;
  if (!_ok)
  {
    // need to initialize using this data, this is the first addition
    _initNew(mdata, encoding);
    isFirst = true;
    _ok = true;
  }
  
  bool didSomething = false;

  // for each field
  for (int i=0; i<mdata.num(); ++i)
  {
    // find the data
    std::string name = mdata.ithGridName(i);
    const Grid *data = mdata.constGridPtr(name);
    if (data != NULL)
    {
      // increment counts/sums for this field in this grid
      if (_increment(name, *data))
      {
	didSomething = true;
      }
    }
  }

  if (didSomething)
  {
    // merge the metadata that was gotten for the new input into
    // pull out specific metadata that defines the MdvMetadata
    bool needGen = false;
    MdvMetadata info(mdata.getMetadataXml().getFreeformXml(),
		     needGen, _isCloudTop);
    GenTimeAndOlder g(genTime, genTime);
    _metadata.merge(g, info, isFirst);
    return true;
  }
  else
  {
    return false;
  }
}

//-------------------------------------------------------------------------
MetaDataXml ClimoState::normalizeAndSetXml(const MultiGrid &inputData,
					   Grid::Encoding_t encoding)
{
  for (int i=0; i<inputData.num(); ++i)
  {
    std::string name = inputData.ithGridName(i);
    const Grid *data = inputData.constGridPtr(name);
    Grid climoGrid(name, "none", data->getNx(), data->getNy(),
		   _climoMeanMissing);
    climoGrid.setEncoding(encoding);

    // load the counts and sums for that field
    ClimoStateGrids state(name, _data);
    if (state.normalize(climoGrid))
    {
      _data.append(climoGrid);
    }
  }  
  MetaDataXml xml;
  xml.addFreeformXml(_metadata.getXml());
  return xml;
}

//-------------------------------------------------------------------------
void ClimoState::_remove(const time_t &genTime, const MultiFcstGrid &data)
{
  LOG(DEBUG) << "Removing data for gen time = " << DateTime::strn(genTime) 
	     << " from state";

  // for each field
  for (int i=0; i<data.num(); ++i)
  {
    std::string name = data.ithGridName(i);
    const Grid *ithdata = data.constGridPtr(name);
    if (ithdata == NULL)
    {
      LOG(ERROR) << "Could not subtract data from state for " << name;
      continue;
    }
    
    // create a temporary ClimoStateGrids object to point to sums and counts
    // for this named field
    ClimoStateGrids s(name, _data);

    // decrement the pointers in s (which point into data) for this field
    // using the data grid pointer ithdata
    s.decrement(*ithdata);
  }
}

//-------------------------------------------------------------------------
void ClimoState::_initNew(const MultiFcstGrid &inputData,
			  Grid::Encoding_t encoding)
{
  // reset data here
  _data = MultiFcstGrid();
  _data.copyInfo(inputData);

  // the input data has the field names,
  // but not the count/sums, which should be added now, because this is
  // a new initialization
  vector<string> countSumNames =
    CountSumNames::countSumNames(inputData.fieldNames());

  const Grid *g = inputData.ithConstGrid(0);
  for (size_t i=0; i<countSumNames.size(); ++i)
  {
    Grid gnew(*g);
    gnew.setEncoding(encoding);
    gnew.changeName(countSumNames[i]);
    gnew.setAllMissing();
    _data.append(gnew);
  }      

  // initialize the state metadata using the inputData metadata
  bool needGen = false;
  _metadata = MdvMetadata(_data.getMetadataXml().getFreeformXml(),
			  needGen, _isCloudTop);
}

//-------------------------------------------------------
bool ClimoState::_increment(const std::string &name, const Grid &inputData)
{
  // set pointers into inputData for this field name
  ClimoStateGrids s(name, _data);

  // increment those pointers
  return s.increment(inputData);
}

