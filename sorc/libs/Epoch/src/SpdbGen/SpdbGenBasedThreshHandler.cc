/**
 * @file SpdbGenBasedThreshHandler.cc
 */
#include <Epoch/SpdbGenBasedThreshHandler.hh>
#include <Epoch/TileThreshInfoGenBased.hh>
#include <Epoch/GenTimeAndOlder.hh>
#include <Epoch/HistGenTime.hh>
#include <Spdb/DsSpdb.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

//------------------------------------------------------------------------
SpdbGenBasedThreshHandler::
SpdbGenBasedThreshHandler(const std::string &spdb) :
  _url(spdb), _chunkValidTime(0), _chunkTimeWritten(0)
{

}

//------------------------------------------------------------------------
SpdbGenBasedThreshHandler::
SpdbGenBasedThreshHandler(const std::string &spdb,
			  const SpdbGenBasedMetadata &md) :
  SpdbGenBasedMetadata(md), _url(spdb), _chunkValidTime(0), _chunkTimeWritten(0)
{
}

//------------------------------------------------------------------------
SpdbGenBasedThreshHandler::~SpdbGenBasedThreshHandler(void)
{
}

//------------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::read(const time_t &genTime)
{
  return _readExisting(genTime, "Exact");
}
		
//------------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::readBestOlder(const time_t &genTime,
					      int maxSecondsBack)
{
  std::vector<time_t>  times = timesInRange(genTime - maxSecondsBack,
					    genTime-1);
  if (times.empty())
  {
    LOG(DEBUG) << "No SPDB data found in data base "
	       << _url << " between " << DateTime::strn(genTime-maxSecondsBack)
	       << " and " << DateTime::strn(genTime-1);
    return false;
  }

  // filter to times with same hour
  DateTime dt(genTime);
  int hour = dt.getHour();
  std::vector<time_t>::iterator i;
  for (i=times.begin(); i!=times.end();)
  {
    dt = DateTime(*i);
    int ithHour = dt.getHour();
    if (ithHour == hour)
    {
      ++i;
    }
    else
    {
      i = times.erase(i);
    }
  }

  int numTimes = (int)times.size();
  for (int i=numTimes-1; i>=0; --i)
  {
    if (_readExisting(times[i], "Older"))
    {
      if (thresholdsSet())
      {
	return true;
      }
    }
  }
  LOG(DEBUG) << "No database entries with thresholds set";
  return false;
}

//------------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::readNewestInRange(const time_t &t0,
						  const time_t &t1)
{
  std::vector<time_t>  times = timesInRange(t0, t1);
  if (times.empty())
  {
    LOG(DEBUG) << "No SPDB data found in data base "
	       << _url << " between " << DateTime::strn(t0)
	       << " and " << DateTime::strn(t1);
    return false;
  }
  time_t tbest = *(times.rbegin());
  return read(tbest);
}


//------------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::readClosestToTargetInRange(const time_t &target,
							   const time_t &t0,
							   const time_t &t1)
{
  std::vector<time_t>  times = timesInRange(t0, t1);
  if (times.empty())
  {
    LOG(DEBUG) << "No SPDB data found in data base "
	       << _url << " between " << DateTime::strn(t0)
	       << " and " << DateTime::strn(t1);
    return false;
  }
  time_t bestT = times[0];
  int bestDelta = times[0] - target;
  if (bestDelta < 0)
  {
    bestDelta = -bestDelta;
  }

  for (size_t i=1; i<times.size(); ++i)
  {
    int delta = times[i] - target;
    if (delta < 0)
    {
      delta = -delta;
    }
    if (delta < bestDelta)
    {
      bestDelta = delta;
      bestT = times[i];
    }
  }
  return read(bestT);
}

//------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::write(void)
{
  return write(_url);
}

//------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::write(const std::string &url)
{
  if (!fixedValuesSet())
  {
    LOG(ERROR) << "Cannot write, values not set, no gen time";
    return false;
  }

  string xml = SpdbGenBasedMetadata::toXml();

  DsSpdb s;
  s.setPutMode(Spdb::putModeOver);
  
  s.clearPutChunks();
  s.clearUrls();
  s.addUrl(url);

  MemBuf mem;
  mem.free();
  
  mem.add(xml.c_str(), xml.size() + 1);
  time_t t = getGenTime();
  if (s.put(SPDB_XML_ID, SPDB_XML_LABEL, 1, t, t, mem.getLen(),
	    (void *)mem.getPtr()))
  {
    LOG(ERROR) << "problems writing out SPDB";
    return false;
  }
  LOG(DEBUG) << "Wrote to " << url << " at gen time " << DateTime::strn(t);
  return true;
}

//------------------------------------------------------------------
std::vector<time_t> 
SpdbGenBasedThreshHandler::timesInRange(const time_t &t0,
					const time_t &t1) const
{
  DsSpdb D;
  std::vector<time_t> ret;
  if (D.compileTimeList(_url, t0, t1))
  {
    LOG(ERROR) << "Compiling time list";
    return ret;
  }
  ret = D.getTimeList();
  return ret;
}


//------------------------------------------------------------------
std::vector<time_t> 
SpdbGenBasedThreshHandler::timesInRangeWithMatchingHour(const time_t &t0,
							const time_t &t1) const
{
  DsSpdb D;
  std::vector<time_t> ret;
  if (D.compileTimeList(_url, t0, t1))
  {
    return ret;
  }
  ret = D.getTimeList();

  std::vector<time_t>::iterator i;
  DateTime dt(t1);
  int wantedHour = dt.getHour();
  for (i=ret.begin(); i!=ret.end(); )
  {
    dt = DateTime(*i);
    if (dt.getHour() == wantedHour)
    {
      ++i;
    }
    else
    {
      i = ret.erase(i);
    }
  }	
  return ret;
}

//------------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::average(const time_t &recentTime, int daysBack,
					double coldstartThresh,
					SpdbGenBasedMetadata &out)
{
  vector<time_t> times =
    timesInRangeWithMatchingHour(recentTime - daysBack*24*3600, recentTime);

  out = *this;  // the thresholds are all wrong, everything else is ok.

  int nTiles = getNumTiles();
  vector<int> ltimes = getLeadSeconds();
  int numObarThresh = getNumObarThresh();
  HistGenTime hist(recentTime, ltimes, numObarThresh, nTiles);

  if (times.empty())
  {
    LOG(ERROR) << "No history found";
    return false;
  }
  
  for (size_t i=0; i<times.size(); ++i)
  {
    LOG(DEBUG) << "time = " << DateTime::strn(times[i]);
    // read in the data for this time
    if (!read(times[i]))
    {
      LOG(ERROR) << "Expected data at " << DateTime::strn(times[i]);
      return false;
    }

    if (getNumObarThresh()  != numObarThresh)
    {
      LOG(ERROR) << "Obar thresh changed";
      return false;
    }
    
    // for each lead time
    vector<TileThreshInfoGenBased> info;
    for (size_t l=0; l<ltimes.size(); ++l)
    {
      int ltSec = ltimes[l];

      for (int o=0; o<numObarThresh; ++o)
      {
	// pull out the threshold info for this lead time/threshold
	info = getTileThreshInfo(ltSec, o);
	if (!info.empty())
	{
	  hist.update(info, ltSec, o);
	}
      }
    }    
  }

  // now pull out the averages at each lead time/obar thresh and store to the output object
  for (size_t l=0; l<ltimes.size(); ++l)
  {
    int ltSec = ltimes[l];
    for (int o=0; o<numObarThresh; ++o)
    {      
      vector<double> ave = hist.getAverages(ltSec, o);
      for (size_t i=0; i<ave.size(); ++i)
      {
	if (ave[i] >= 0)
	{
	  out.setThresh(ltSec, o, i, ave[i]);
	}
	else
	{
	  out.setThreshColdstart(ltSec, o, i);
	}
      }
    }
  }
  return true;
}

//------------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::_readExisting(const time_t &genTime,
					      const std::string &description)
{
  DsSpdb D;
  if (D.getExact(_url, genTime) != 0)
  {
    return false;
  }
  else
  {
    return _load(D, description);
  }
}

//------------------------------------------------------------------
bool SpdbGenBasedThreshHandler::_load(DsSpdb &s, const std::string &description)
{
  const vector<Spdb::chunk_t> &chunks = s.getChunks();
  int nChunks = static_cast<int>(chunks.size());
  if (nChunks <= 0)
  {
    return false;
  }
  if (nChunks > 1)
  {
    LOG(WARNING) << "Too many chunks " << nChunks << " expected 1";
    return false;
  }
  vector<Spdb::chunk_t> lchunks;
  for (int i=0; i<nChunks; ++i)
  {
    lchunks.push_back(chunks[i]);
  }
  // Process out the results.
  for (int jj = 0; jj < nChunks; jj++)
  {
    const Spdb::chunk_t &chunk = lchunks[jj];
    
    LOG(DEBUG) << "Read " << description << " "
	       << DateTime::strn(chunk.valid_time)
	       << " from " << _url;
    
    void *chunk_data = chunk.data;
    if (s.getProdId() == SPDB_XML_ID)
    {
      string xml((char *)chunk_data);
      _chunkValidTime = chunk.valid_time;
      _chunkTimeWritten = chunk.write_time;
      return SpdbGenBasedMetadata::fromXml(xml);
    }
    else
    { 
      LOG(ERROR) << "spdb data is not XML data, want " << SPDB_XML_ID 
		 << " got " << s.getProdId();
      return false;
    }
  }
  return true;
}
