// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file FcstThreshMetadata.cc
 */

//----------------------------------------------------------------
#include <Epoch/MdvMetadata.hh>
#include <Epoch/TileThreshInfo.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>
#include <algorithm>
using std::string;
using std::vector;
using std::pair;

/**
 * Set the value for the fixed XML tag
 */
const std::string MdvMetadata::_tag = "SpdbThresh";

//----------------------------------------------------------------
MdvMetadata::MdvMetadata(int numTiles)
				       
  : _needGen(false), _needFieldThresh(false),
    _numTiles(numTiles), _averageTiledThreshInfoIsSet(false)
{
}

//----------------------------------------------------------------
MdvMetadata::MdvMetadata(const std::string &xml,
			 bool needGen,
			 bool needFieldThresh) :
  _needGen(needGen), _needFieldThresh(needFieldThresh),
  _numTiles(0), _averageTiledThreshInfoIsSet(false)
{
  if (xml.empty())
  {
    return;
  }

  if (TaXml::readInt(xml, "NumTiles", _numTiles))
  {
    LOG(ERROR) << "No number of tiles in FcstThresh Metadata";
    return;
  }

  string specificXml;
  if (TaXml::readString(xml, _tag, specificXml))
  {
    LOG(ERROR) << "Reading xml for " << _tag;
  }
  else
  {
    vector<string> v;
    if (TaXml::readStringArray(specificXml,
			       TiledFcstThreshInfo::fcstThreshTag(), v))
    {
      if (_needFieldThresh)
      {
	// expect this to be there 
	LOG(ERROR) << "Reading xml array for "
		   << TiledFcstThreshInfo::fcstThreshTag();
      }
    }
    else
    {
      for (size_t i=0; i<v.size(); ++i)
      {
	TiledFcstThreshInfo ii(v[i], _numTiles);
	if (ii.isOk())
	{
	  _tiledThreshInfo.push_back(ii);
	}
	else
	{
	  LOG(WARNING) << "Parsing string as fcstthreshinfo " << v[i];
	}
      }
    }
    if (TaXml::readStringArray(specificXml,
			       FixedFcstThreshInfo::fixedFcstThreshTag(), v))
    {
      LOG(ERROR) << "Reading xml array for "
		 << FixedFcstThreshInfo::fixedFcstThreshTag();
    }
    else
    {
      for (size_t i=0; i<v.size(); ++i)
      {
	FixedFcstThreshInfo ii(v[i]);
	if (ii.isOk())
	{
	  _fixedThreshInfo.push_back(ii);
	}
	else
	{
	  LOG(WARNING) << "Parsing string as fixedfcstthreshinfo " << v[i];
	}
      }
    }
  }
  sort(_tiledThreshInfo.begin(), _tiledThreshInfo.end());

  // set field names
  _setFieldsFromInfo();

  if (TaXml::readString(xml, "genTimes", specificXml))
  {
    LOG(ERROR) << "Reading xml for genTimes";
    return;
  }
  else
  {
    vector<string> v;
    if (TaXml::readStringArray(specificXml, GenTimeAndOlder::_tag, v))
    {
      if (_needGen)
      {
	// expect at least one gen time
	LOG(ERROR) << "No Gen times";
      }
    }
    else
    {
      for (size_t i=0; i<v.size(); ++i)
      {
	GenTimeAndOlder g(v[i]);
	if (g.isOk())
	{
	  _gen.push_back(g);
	}
	else
	{
	  LOG(ERROR) << "Bad XML metadata " << v[i];
	}
      }
    }
  }

  // note averages not set.
}

//----------------------------------------------------------------
MdvMetadata::~MdvMetadata()
{
}

//----------------------------------------------------------------
void MdvMetadata::add(const GenTimeAndOlder &gen, int lead,
		      const std::string &fieldName, 
		      const std::vector<double> &threshold)
{
  if (static_cast<int>(threshold.size()) != _numTiles)
  {
    LOG(ERROR) << "Inconsistent number of tiles " << threshold.size()
	       << " " <<  _numTiles;
    return;
  } 
  TiledFcstThreshInfo info(gen, lead, fieldName, threshold);
  _tiledThreshInfo.push_back(info);

  // sort into time ascending order
  sort(_tiledThreshInfo.begin(), _tiledThreshInfo.end());

  // update the field names
  _setFieldsFromInfo();

  _gen.push_back(gen);
}

//----------------------------------------------------------------
void MdvMetadata::setAverage(void)
{
  // here is where averages get set
  // loop over fields
  _averageTiledThreshInfo.clear();
  for (size_t i=0; i<_fieldNameTiledThresh.size(); ++i)
  {
    string s = _fieldNameTiledThresh[i];
    _averageTiledThreshInfo.push_back(_getAverageThresh(s));
  }
  _averageTiledThreshInfoIsSet = !_averageTiledThreshInfo.empty();
}

//----------------------------------------------------------------
void MdvMetadata::add(const std::string &fieldName,  double threshold)
{
  FixedFcstThreshInfo info(fieldName, threshold);
  _fixedThreshInfo.push_back(info);
}

//----------------------------------------------------------------
std::string MdvMetadata::getXml(void) const
{
  string xml=TaXml::writeInt("NumTiles", 0, _numTiles);
  for (size_t i=0; i<_tiledThreshInfo.size(); ++i)
  {
    xml += _tiledThreshInfo[i].getXml();
  }
  for (size_t i=0; i<_fixedThreshInfo.size(); ++i)
  {
    xml += _fixedThreshInfo[i].getXml();
  }
  string ret = TaXml::writeString(_tag, 0, xml);
  xml = "";
  for (size_t i=0; i<_gen.size(); ++i)
  {
    xml += _gen[i].getXml();
  }
  ret += TaXml::writeString("genTimes", 0, xml);
  return ret;
}

//----------------------------------------------------------------
void MdvMetadata::merge(const GenTimeAndOlder &genTime,
			const MdvMetadata &info,
			bool isFirst)
{
  if (info._numTiles != _numTiles)
  {
    LOG(ERROR) << "Inconsistent number of tiles " << info._numTiles
	       << " " <<  _numTiles;
    return;
  } 

  if (find(_gen.begin(), _gen.end(), genTime) != _gen.end())
  {
    if (isFirst)
    {
      // as expected..set averages now
      setAverage();
      return;
    }
    LOG(WARNING) << "Gen time already present, this is unexpected"
		 << genTime.sprint();
  }
  
  // add this gen time
  _gen.push_back(genTime);

  // put in the tiled thresholds
  for (size_t i=0; i<info._tiledThreshInfo.size(); ++i)
  {
    _tiledThreshInfo.push_back(info._tiledThreshInfo[i]);
  }
  sort(_tiledThreshInfo.begin(), _tiledThreshInfo.end());
  
  // set averages
  setAverage();

  // add any fixed thresholds as needed
  for (size_t i=0; i<info._fixedThreshInfo.size(); ++i)
  {
    bool match = false;
    for (size_t j=0; j<_fixedThreshInfo.size(); ++j)
    {
      if (_fixedThreshInfo[j].
	  nameMatchThreshDoesntMatch(info._fixedThreshInfo[i]))
      {
	LOG(ERROR) << "Same field with two thresholds, field:"
		   << _fixedThreshInfo[j].getName() << " thresholds: "
		   << _fixedThreshInfo[j].getThresh() << " "
		   << info._fixedThreshInfo[i].getThresh();
	//don't add new one
	match = true;
	break;
      }
      if (_fixedThreshInfo[j] == info._fixedThreshInfo[i])
      {
	match = true;
	break;
      }
    }
    if (!match)
    {
      _fixedThreshInfo.push_back(info._fixedThreshInfo[i]);
    }
  }
}

//----------------------------------------------------------------
void MdvMetadata::erase(const time_t &gt)
{
  vector<GenTimeAndOlder>::iterator it;

  for (it=_gen.begin(); it!=_gen.end(); )
  {
    if (it->genTimeEqual(gt))
    {
      it = _gen.erase(it);
    }
    else
    {
      ++it;
    }
  }

  vector<TiledFcstThreshInfo>::iterator i;
  for (i=_tiledThreshInfo.begin(); i!=_tiledThreshInfo.end(); )
  {
    if (i->olderThan(gt))
    {
      i = _tiledThreshInfo.erase(i);
    }
    else
    {
      i++;
    }
  }
  _averageTiledThreshInfoIsSet = false;
}

//----------------------------------------------------------------
void MdvMetadata::changeThreshIfMatch(int tileIndex,
				      const std::string &fullname,
				      double &matchThresh) const
{
  if (!_averageTiledThreshInfoIsSet)
  {
    //LOG(ERROR) << "Averages not set";
    return;
  }

  // find the name
  for (size_t i=0; i<_averageTiledThreshInfo.size(); ++i)
  {
    if (_averageTiledThreshInfo[i].nameMatch(fullname))
    {
      matchThresh = _averageTiledThreshInfo[i].getThresh(tileIndex);
      return;
    }
  }
}

//----------------------------------------------------------------
void MdvMetadata::updateBest(int tileIndex, double bias,
			     const time_t &obsTime, double obsMean,
			     double fcstMean, TileThreshInfo &best) const
{
  if (_averageTiledThreshInfoIsSet)
  {
    // NEED TO REWORK THIS, last arg has changed
    best.update(bias, obsTime, obsMean, fcstMean, _averageTiledThreshInfo,
		tileIndex);
  }
  else
  {
    best.update(bias, obsTime, obsMean, fcstMean);
  }
}


//-------------------------------------------------------------------------
std::vector<time_t> MdvMetadata::tooOld(const time_t &genTime,
					int daysInClimo) const
{
  std::vector<time_t> times;

  for (size_t i=0; i<_gen.size(); ++i)
  {
    if (_gen[i].olderThan(genTime - daysInClimo*convWx::SECS_PER_DAY))
    {
      times.push_back(_gen[i].getGenTime());
    }
  }
  return times;
}

//-------------------------------------------------------------------------
bool MdvMetadata::contains(const time_t &gentime) const
{
  for (size_t i=0; i<_gen.size(); ++i)
  {
    if (_gen[i].genTimeEqual(gentime))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------
void MdvMetadata::print(bool verbose) const
{
  printf("Numtiles:%d\n", _numTiles);
  for (size_t i=0; i<_tiledThreshInfo.size(); ++i)
  {
    printf("TileThresh:\n");
    _tiledThreshInfo[i].print(verbose);
  }
  for (size_t i=0; i<_fixedThreshInfo.size(); ++i)
  {
    printf("FixedThresh:");
    _fixedThreshInfo[i].print();
  }
  if (verbose)
  {
    if (_averageTiledThreshInfoIsSet)
    {
      for (size_t i=0; i<_averageTiledThreshInfo.size(); ++i)
      {
	printf("AverageThresh:\n");
	_averageTiledThreshInfo[i].print(verbose);
      }
    }
    else
    {
      printf("AverageThresh:NOT SET\n");
    }
  }

  printf("Gen Times:\n");
  for (size_t i=0; i<_gen.size(); ++i)
  {
    printf("%s\n", _gen[i].sprint().c_str());
  }
}


//----------------------------------------------------------------
FieldTiledThresh
MdvMetadata::_getAverageThresh(const std::string &name) const
{

  // set up a count/sum for each tile, initialized to 0
  std::vector<double> counts, sums;
  for (int i=0; i<_numTiles; ++i)
  {
    counts.push_back(0);
    sums.push_back(0);
  }

  for (size_t i=0; i<_tiledThreshInfo.size(); ++i)
  {
    if (_tiledThreshInfo[i].nameMatch(name))
    {
      for (int t=0; t<_numTiles; ++t)
      {
	counts[t]++;
	sums[t] += _tiledThreshInfo[i].getTileThresh(t);
      }
    }
  }
  for (int t=0; t<_numTiles; ++t)
  {
    if (counts[t] > 0)
    {
      sums[t] /= counts[t];
    }
  }
  
  FieldTiledThresh ret(name, sums);
  return ret;
}

//---------------------------------------------------------------------------
void MdvMetadata::_setFieldsFromInfo(void)
{
  _fieldNameTiledThresh.clear();
  for (size_t i=0; i<_tiledThreshInfo.size(); ++i)
  {
    string name = _tiledThreshInfo[i].getName();
    if (find(_fieldNameTiledThresh.begin(), _fieldNameTiledThresh.end(),
	     name) == _fieldNameTiledThresh.end())
    {
      _fieldNameTiledThresh.push_back(name);
    }
  }
}

