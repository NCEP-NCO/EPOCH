/**
 * @file SpdbObsHandler.cc
 */
#include <Epoch/SpdbObsHandler.hh>
#include <Spdb/DsSpdb.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

//------------------------------------------------------------------------
SpdbObsHandler::SpdbObsHandler(const std::string &spdb,
			       const std::string &precipField,
			       const TileInfo &tiling,
			       const std::vector<double> &thresh) :
  SpdbObsMetadata(precipField, tiling, thresh),
  _url(spdb),
  _chunkValidTime(0),
  _chunkTimeWritten(0)
{

}

//------------------------------------------------------------------------
SpdbObsHandler::SpdbObsHandler(const std::string &spdb) :

  SpdbObsMetadata(),
  _url(spdb),
  _chunkValidTime(0),
  _chunkTimeWritten(0)
{

}

//------------------------------------------------------------------------
SpdbObsHandler::~SpdbObsHandler(void)
{
}

//------------------------------------------------------------------
bool SpdbObsHandler::write(const time_t &t)
{
  return write(t, _url);
}

//------------------------------------------------------------------
bool SpdbObsHandler::write(const time_t &t, const std::string &url)
{
  string xml = SpdbObsMetadata::toXml();

  DsSpdb s;
  s.setPutMode(Spdb::putModeOver);
  
  s.clearPutChunks();
  s.clearUrls();
  s.addUrl(url);

  MemBuf mem;
  mem.free();
  
  mem.add(xml.c_str(), xml.size() + 1);
  if (s.put(SPDB_XML_ID, SPDB_XML_LABEL, 1, t, t, mem.getLen(),
	    (void *)mem.getPtr()))
  {
    LOG(ERROR) << "problems writing out SPDB";
    return false;
  }
  return true;
}

//------------------------------------------------------------------
std::vector<time_t> 
SpdbObsHandler::timesInRange(const time_t &t0, const time_t &t1) const
{
  DsSpdb D;
  std::vector<time_t> ret;
  if (D.compileTimeList(_url, t0, t1))
  {
    LOG(WARNING) << "Compiling time list for time range "
	       << DateTime::strn(t0)  << " to "
	       << DateTime::strn(t1);
    return ret;
  }
  ret = D.getTimeList();
  return ret;
}

//------------------------------------------------------------------------
bool SpdbObsHandler::read(const time_t &obsTime)
{
  DsSpdb D;
  if (D.getExact(_url, obsTime) != 0)
  {
    return false;
  }
  else
  {
    return _load(D);
  }
}

//------------------------------------------------------------------
bool SpdbObsHandler::_load(DsSpdb &s)
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
    
    LOG(DEBUG) << "Read " << DateTime::strn(chunk.valid_time) << " from " << _url;
    void *chunk_data = chunk.data;
    if (s.getProdId() == SPDB_XML_ID)
    {
      string xml((char *)chunk_data);
      _chunkValidTime = chunk.valid_time;
      _chunkTimeWritten = chunk.write_time;
      return SpdbObsMetadata::fromXml(xml);
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
