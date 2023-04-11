/**
 * @file DbThresh.cc
 */

#include "DbThresh.hh"
#include <toolsa/DateTime.hh>
#include <toolsa/TaXml.hh>

//-----------------------------------------------------------------------
DbThresh::DbThresh(const ThresholdDatabaseParms &fieldParm,
		   const ParmsEnsLookupGen &parm) :
  _parms(parm),
  _dbParms(fieldParm),
  _spdb(fieldParm._databaseUrl),
  _coldstart(true)
{

}

//-----------------------------------------------------------------------
bool DbThresh::newGenTime(const time_t &genTime, time_t &gt)
{
  if (!_dbParms._fromDatabase)
  {
    // nothing to do, always use the coldstart thresh
    return true;
  }

  if (_parms._main.isRealtime())
  {
    LOG(DEBUG) << "Looking for SPDB data in the range "
	       << DateTime::strn(genTime+_parms._realtimeSpdbMinOffsetSec)
	       << " to "
	       << DateTime::strn(genTime+_parms._realtimeSpdbMaxOffsetSec);
    if (!_spdb.readNewestInRange(genTime+_parms._realtimeSpdbMinOffsetSec,
				 genTime+_parms._realtimeSpdbMaxOffsetSec))
    {
      LOG(WARNING) << "No thresholds, using coldstart values";
      _coldstart = true;
    }
    else
    {
      _coldstart = false;
    }
  }
  else
  {
    if (!_spdb.readClosestToTargetInRange(genTime-_parms._archiveSpdbBestOffsetSec,
					  genTime-_parms._archiveSpdbMinOffsetSec,
					  genTime-_parms._archiveSpdbMaxOffsetSec))
    {
      LOG(WARNING) << "No thresholds, using coldstart values";
      _coldstart = true;
    }
    else
    {
      _coldstart = false;
    }
  }

  if (_coldstart)
  {
    _threshChunkTime = 0;
  }
  else
  {
    _threshChunkTime = _spdb.getChunkValidTime();
    if (_parms._debugSpdb)
    {
#ifdef NOTQUITEYET      
      time_t twritten = _spdb.getChunkTimeWritten();
      vector<int> genHours;
      vector<int> leadSeconds;
      _spdb.printState(_threshChunkTime, twritten, true,
			    genHours, leadSeconds, _parms._debugTiles);
#endif
    }
  }

  if (_coldstart)
  {
    return false;
  }
  else
  {
    gt = _spdb.getGenTime();
    vector<double> obarThresh = _spdb.getObarThresh();
    vector<int> lt = _spdb.getLeadSeconds();
    TileInfo tileInfo = _spdb.getTileInfo();
    return (_dbParms.obarMatch(obarThresh) &&
	    _parms.checkDb(lt, tileInfo));
  }
}

//-----------------------------------------------------------------------
void DbThresh::createThresholdGrid(const time_t &gt, int lt,
				   const ObarThreshParms &oparms,
				   Grid2d &egrid,
				   bool &doOutput) const
{
  Grid2d grid;
  bool fixed = false;
  
  ObarThreshParms oparmsLoc;
  if (!_dbParms._fromDatabase)
  {
    fixed = true;
  }
  else
  {
    if (_dbParms._isOutputObar)
    {
      oparmsLoc = oparms;
    }
    else
    {
      oparmsLoc = _dbParms._obarThresh[0];
    }
    if (_coldstart)
    {
      fixed = true;
    }
    if (!fixed)
    {
      if (!_spdb.getTiledGrid(lt, oparmsLoc._obarThresh,
			      _parms._centerWeightTiledGrid,
			      _parms._edgeWeightTiledGrid,
			      _parms._nptSmoothTiledGrid,
			      grid))
      {
	LOG(ERROR) << "No thresholds at gen/lead/obar, using coldstart values";
	fixed = true;
      }
    }
  }  
  doOutput = _dbParms._doOutputThresholdsGrid;
  double othresh;
  bool gotOthresh;
  
  if (fixed)
  {
    _parms.createFixedTiledGrid(_dbParms._fieldName, oparmsLoc._obarThresh,
				_dbParms._coldstartThreshold, grid);
    gotOthresh = oparms.getOutsideThresh(_dbParms._fieldName, othresh);
  }
  else
  {
    gotOthresh = oparmsLoc.getOutsideThresh(_dbParms._fieldName, othresh);
  }
  
  if (gotOthresh)
  {
    egrid = _expandedGrid(grid, othresh);
  }
  else
  {
    LOG(WARNING) << "No outside thresh for " << _dbParms._fieldName
		 << " Using coldstart outside domain";
    egrid = _expandedGrid(grid,  _dbParms._coldstartThreshold);
  }
}

//----------------------------------------------------------------------
void DbThresh::prepareOutput(std::string &xml) const
{
  string tag = "ChunkTime_";
  tag += _dbParms._fieldName;
  
  if (_threshChunkTime != 0)
  {
    xml += TaXml::writeTime(tag, 0, _threshChunkTime);
  }
  else
  {
    xml += TaXml::writeString(tag, 0, "None");
  }
}

//----------------------------------------------------------------------
Grid2d DbThresh::_expandedGrid(const Grid2d &g, double outsideThresh) const
{
  Grid2d ret(g.getName(), _parms._projExtended.pNx, _parms._projExtended.pNy,
	     g.getMissing());
  for (int y=0; y<ret.getNy(); ++y)
  {
    int ySmall=-1;
    double insideWeight=0;
    bool doModify = false;
    bool isInsideSmaller = _parms._mapper.isInsideSmallerDomain(y, ySmall);
    if (isInsideSmaller)
    {
      int nptIn = -1;
      if (ySmall < _parms._proj.pNy/2)
      {
	if (_parms._mapper.doesExtendSouth())
	{
	  nptIn = ySmall;
	  insideWeight = _parms.insideWeight(nptIn);
	  doModify = true;
	}
      }
      else
      {
	if (_parms._mapper.doesExtendNorth())
	{
	  // do the weighting only if north extension
	  nptIn = _parms._proj.pNy-1-ySmall;
	  insideWeight = _parms.insideWeight(nptIn);
	  doModify = true;
	}
      }
    }
    for (int x=0; x<ret.getNx(); ++x)
    {
      if (isInsideSmaller)
      {
	double v;
	if (g.getValue(x, ySmall, v))
	{
	  if (doModify)
	  {
	    v = v*insideWeight + outsideThresh*(1.0-insideWeight);
	  }
	  ret.setValue(x, y, v);
	}
	else
	{
	  ret.setMissing(x, y);
	}
      }
      else
      {
	ret.setValue(x, y, outsideThresh);
      }
    }
  }
  return ret;
}

