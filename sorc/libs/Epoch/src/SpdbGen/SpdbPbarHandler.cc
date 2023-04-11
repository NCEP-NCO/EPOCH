/**
 * @file SpdbPbarHandler.cc
 */
#include <Epoch/SpdbPbarHandler.hh>
#include <Spdb/DsSpdb.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

//------------------------------------------------------------------------
SpdbPbarHandler::
SpdbPbarHandler(const std::string &spdb) :
  _url(spdb), _chunkValidTime(0), _chunkTimeWritten(0)
{

}

//------------------------------------------------------------------------
SpdbPbarHandler::
SpdbPbarHandler(const std::string &spdb,
		const SpdbPbarMetadata &md) :
  SpdbPbarMetadata(md), _url(spdb), _chunkValidTime(0), _chunkTimeWritten(0)
{
}

//------------------------------------------------------------------------
SpdbPbarHandler::~SpdbPbarHandler(void)
{
}

//------------------------------------------------------------------------
bool SpdbPbarHandler::read(const time_t &genTime)
{
  return _readExisting(genTime, "Exact");
}
		
//------------------------------------------------------------------------
bool SpdbPbarHandler::readBestOlder(const time_t &genTime,
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
      if (pbarSet())
      {
	return true;
      }
    }
  }
  LOG(DEBUG) << "No database entries with thresholds set";
  return false;
}

//------------------------------------------------------------------------
bool SpdbPbarHandler::readNewestInRange(const time_t &t0,
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
bool SpdbPbarHandler::readClosestToTargetInRange(const time_t &target,
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
bool SpdbPbarHandler::write(void)
{
  return write(_url);
}

//------------------------------------------------------------------
bool SpdbPbarHandler::write(const std::string &url)
{
  string xml = SpdbPbarMetadata::toXml();

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
SpdbPbarHandler::timesInRange(const time_t &t0,
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
SpdbPbarHandler::timesInRangeWithMatchingHour(const time_t &t0,
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
bool SpdbPbarHandler::_readExisting(const time_t &genTime,
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
bool SpdbPbarHandler::_load(DsSpdb &s, const std::string &description)
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
      return SpdbPbarMetadata::fromXml(xml);
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
