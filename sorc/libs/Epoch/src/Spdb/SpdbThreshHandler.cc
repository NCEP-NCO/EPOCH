/**
 * @file SpdbThreshHandler.cc
 */
#include <Epoch/SpdbThreshHandler.hh>

//------------------------------------------------------------------------
SpdbThreshHandler::
SpdbThreshHandler(const std::string &spdb,
		  const std::vector<double> &ltHours,
		  const std::vector<std::string> &fields,
		  const std::vector<FieldThresh> &fixedfields,
		  int genFrequencySeconds, const TileInfo &tiling,
		  int maxSecondsBack, int maxSecondsBeforeColdstart,
		  const std::vector<FieldThresh> &coldstartThresh,
		  bool latLonsOptional) :
  SpdbThresholds(spdb, ltHours, fields, fixedfields, tiling, latLonsOptional),
  _genFrequencySeconds(genFrequencySeconds),
  _tiling(tiling),
  _maxSecondsBack(maxSecondsBack),
  _maxSecondsBeforeColdstart(maxSecondsBeforeColdstart),
  _coldstartThresh(coldstartThresh)
{

}

//------------------------------------------------------------------------
SpdbThreshHandler::~SpdbThreshHandler(void)
{
}

//------------------------------------------------------------------------
void SpdbThreshHandler::readFirstBeforeOrSetColdstart(const time_t &t)
{
  if (!readFirstBefore(t, _maxSecondsBack, _maxSecondsBeforeColdstart,
		       _coldstartThresh))
  {
    setColdStart(_genFrequencySeconds, _coldstartThresh);
  }
  addLatlonsIfNeeded(_tiling);
}

//------------------------------------------------------------------------
bool SpdbThreshHandler::setForecastToColdstart(const time_t &genTime,
						int leadTime)
{
  return setColdStart(genTime, leadTime, _tiling.numTiles(), _coldstartThresh);
}
