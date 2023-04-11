#include <Epoch/ThresholdDatabaseParmsIO.hh>
#include <Epoch/ThresholdDatabaseParams.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <toolsa/LogStream.hh>

using std::vector;
using std::string;

//
// failure return value
//
const int ThresholdDatabaseParmsIO::failure = 1;

//-----------------------------------------------------------
ThresholdDatabaseParmsIO::ThresholdDatabaseParmsIO()
{
_ok = false;
}

//-----------------------------------------------------------
ThresholdDatabaseParmsIO::ThresholdDatabaseParmsIO(const std::string &parmfile)
{
  _ok = true;

  ThresholdDatabaseParams params;

  if (params.load(parmfile.c_str(), NULL, true, false) != 0)
  {
    LOG(ERROR) << "Loading file " << parmfile;
    _ok = false;
    return;
  }
  _fromDatabase = params.fromDatabase;
  _fieldName = params.fieldName;
  _compare = params.compare;
  _coldstartThreshold = params.coldstartThresh;

  if (_fromDatabase)
  {
    _databaseUrl = params.databaseUrl;
    _isOutputObar = params.isOutputObar;
    _doOutputThresholdsGrid = params.doOutputThresholdsGrid;
    for (int i=0; i<params.obsSpec_n; ++i)
    {
      ObarThreshParms p;
      p._obarThresh = params._obsSpec[i].obsThresh;
      p._griddedFieldName = params._obsSpec[i].griddedFieldName;
      for (int j=0; j<params.numFieldsWithOutsideThresh; ++j)
      {
	switch (j)
	{
	case 0:
	  p._outsideThresh.push_back(pair<string,double>(params._obsSpec[i].field1,
							 params._obsSpec[i].outsideThresh1));
	  break;
	case 1:
	  p._outsideThresh.push_back(pair<string,double>(params._obsSpec[i].field2,
							 params._obsSpec[i].outsideThresh2));
	  break;
	case 2:
	  p._outsideThresh.push_back(pair<string,double>(params._obsSpec[i].field3,
							 params._obsSpec[i].outsideThresh3));
	  break;
	case 3:
	  p._outsideThresh.push_back(pair<string,double>(params._obsSpec[i].field4,
							 params._obsSpec[i].outsideThresh4));
	  break;
	default:
	  LOG(ERROR) << "Too many fields with outside threshold in config, only allow 4";	
	  _ok = false;
	}
      }
      _obarThresh.push_back(p);
    }
  }
  else
  {
    _databaseUrl = "None";
    _isOutputObar = false;
    _obarThresh.clear();
  }
}

//-----------------------------------------------------------------
ThresholdDatabaseParmsIO::~ThresholdDatabaseParmsIO()
{
}

//-----------------------------------------------------------------
void ThresholdDatabaseParmsIO::printParams(void) const
{
  ThresholdDatabaseParams params;
  params.print(stdout);
}
  
