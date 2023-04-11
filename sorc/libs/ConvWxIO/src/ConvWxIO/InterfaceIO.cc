// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// � University Corporation for Atmospheric Research (UCAR) 2009-2010. 
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
 * @file InterfaceIO.cc
 * @brief 
 */

//----------------------------------------------------------------
#include <algorithm>

#include <ConvWxIO/InterfaceIO.hh>
#include <ConvWxIO/ILogMsg.hh>
#include <ConvWxIO/EarthRadius.hh>
#include <ConvWxIO/ConvWxThreadMgr.hh>
#include <ConvWx/ConvWxConstants.hh>
#include <ConvWx/ConvWxTime.hh>
#include <ConvWx/TriggerState.hh>
#include <ConvWx/ParmProjection.hh>
#include <ConvWx/ParmFcst.hh>
#include <ConvWx/MetaData.hh>
#include <ConvWx/Grid.hh>
#include <ConvWx/FcstGrid.hh>
#include <ConvWx/MultiGrid.hh>
#include <ConvWx/MultiFcstGrid.hh>

#include <dsdata/DsUrlTrigger.hh>
#include <didss/DsInputPath.hh>
#include <didss/LdataInfo.hh>
#include <Mdv/DsMdvx.hh>
#include <Mdv/MdvxProj.hh>
#include <Mdv/MdvxField.hh>
#include <Mdv/MdvxChunk.hh>
#include <toolsa/pmu.h>
#include <toolsa/DateTime.hh>
#include <toolsa/file_io.h>
#include <toolsa/Path.hh>
#include <rapmath/usort.h>
//using namespace std;

static DsUrlTrigger *sTrigger = NULL;
static TriggerState sTriggerstate;
static bool sVlevelRestricted = false;
static int sVlevelIndex1 = -1;
static int sVlevelIndex2 = -1;
static bool sAllowStoringVerticalLevels = true;
static vector<double> sVlevel;
static DsInputPath *sFileTrigger = NULL;
static DsInputPath *sFileTrigger2 = NULL;
static LdataInfo *sLtrigger = NULL;
static LdataInfo *sLtrigger2 = NULL;
static bool sNoWait = false;
static int sMaxWaitSeconds = 0;
static bool sMaxValidAgeChanged = false;
static int sMaxValidAge = DsUrlTrigger::defaultMaxValidAge();

//------------------------------------------------------------------
static void sTriggeringCheck(const string &url, 
			     const vector<int> &gtSubsample,
			     const bool debug)
{
  if (sTrigger != NULL && sTriggerstate.didChange(url, gtSubsample))
  {
    delete sTrigger;
    sTrigger = NULL;
  }
  if (sTrigger == NULL)
  {
    sTriggerstate.setObsTriggering(url, gtSubsample);
    sTrigger = new DsUrlTrigger(url, DsUrlTrigger::OBS, debug);
    sTrigger->gentimeSubsamplingSet(gtSubsample);
    sTrigger->setMaxValidAge(sMaxValidAge);
    if (sNoWait)
    {
      sTrigger->setNowait();
    }
    sMaxValidAgeChanged = false;
  }
  else
  {
    sTrigger->setDebugging(debug);
    if (sMaxValidAgeChanged)
    {
      sMaxValidAgeChanged = false;
      sTrigger->setMaxValidAge(sMaxValidAge);
    }
  }
}

//------------------------------------------------------------------
static void sTriggeringCheck(const string &url, 
			     const vector<int> &gtSubsample,
			     const time_t &t0, const time_t &t1,
			     const bool debug)
{
  if (sTrigger != NULL && sTriggerstate.didChange(url, gtSubsample, t0, t1))
  {
    delete sTrigger;
    sTrigger = NULL;
  }
  if (sTrigger == NULL)
  {
    sTriggerstate.setObsTriggering(url, gtSubsample, t0, t1);
    sTrigger = new DsUrlTrigger(t0, t1, url, DsUrlTrigger::OBS, debug);
    sTrigger->gentimeSubsamplingSet(gtSubsample);
  }
  else
  {
    sTrigger->setDebugging(debug);
  }
}

//------------------------------------------------------------------
static void sTriggeringCheck(const string &url, const bool isLtSubsample,
			     const int lt0, const int lt1, const int dlt,
			     const vector<int> &gtSubsample,
			     const bool sequence, const bool debug)
{
  if (sTrigger != NULL && 
      sTriggerstate.didChange(url, isLtSubsample, lt0, lt1, dlt, gtSubsample,
			      sequence))
  {
    delete sTrigger;
    sTrigger = NULL;
  }
  if (sTrigger == NULL)
  {
    sTriggerstate.setFcstTriggering(url, isLtSubsample, lt0, lt1, dlt,
				    gtSubsample, sequence);
    if (sequence)
    {
      sTrigger = new DsUrlTrigger(url, DsUrlTrigger::FCST_LEAD, debug);
      if (sNoWait)
      {
	sTrigger->setNowait();
      }
    }
    else
    {
      sTrigger = new DsUrlTrigger(url, DsUrlTrigger::FCST_GEN, debug);
    }
    sTrigger->gentimeSubsamplingSet(gtSubsample);
    if (isLtSubsample)
    {
      // convert from seconds to minutes
      sTrigger->leadtimeSubsamplingActivate(static_cast<double>(lt0)/60.0,
					    static_cast<double>(lt1)/60.0,
					    static_cast<double>(dlt)/60.0);
    }
    sTrigger->setMaxValidAge(sMaxValidAge);
    sMaxValidAgeChanged = false;
  }
  else
  {
    sTrigger->setDebugging(debug);
    if (sMaxValidAgeChanged)
    {
      sMaxValidAgeChanged = false;
      sTrigger->setMaxValidAge(sMaxValidAge);
    }
  }
}

//------------------------------------------------------------------
static void sTriggeringCheck(const string &url, const bool isLtSubsample,
			     const int lt0, const int lt1, const int dlt,
			     const vector<int> &gtSubsample,
			     const bool sequence, const time_t &t0,
			     const time_t &t1, const bool debug)
{
  if (sTrigger != NULL && 
      sTriggerstate.didChange(url, isLtSubsample, lt0, lt1, dlt, gtSubsample,
			      sequence, t0, t1))
  {
    delete sTrigger;
    sTrigger = NULL;
  }
  if (sTrigger == NULL)
  {
    sTriggerstate.setFcstTriggering(url, isLtSubsample, lt0, lt1, dlt,
				    gtSubsample, sequence, t0, t1);
    if (sequence)
    {
      sTrigger = new DsUrlTrigger(t0, t1, url, DsUrlTrigger::FCST_LEAD, debug);
    }
    else
    {
      sTrigger = new DsUrlTrigger(t0, t1, url, DsUrlTrigger::FCST_GEN, debug);
    }
    sTrigger->gentimeSubsamplingSet(gtSubsample);
    if (isLtSubsample)
    {
      // convert from seconds to minutes
      sTrigger->leadtimeSubsamplingActivate(static_cast<double>(lt0)/60.0,
					    static_cast<double>(lt1)/60.0,
					    static_cast<double>(dlt)/60.0);
    }
  }
  else
  {
    sTrigger->setDebugging(debug);
  }
}

//------------------------------------------------------------------
static void sSetProj(const ParmProjection &p, MdvxProj &proj)
{
  proj.setGrid(p.pNx, p.pNy, p.pDx, p.pDy, p.pMinx, p.pMiny);
  switch (p.pProjection)
  {
  case ParmProjection::LAMBERT_CONFORMAL2:
    proj.initLc2(p.pOriginLat, p.pOriginLon, p.pLat1, p.pLat2);
    proj.setOffsetOrigin(p.pOffsetOriginLat, p.pOffsetOriginLon);
    break;
  case ParmProjection::LAMBERT_AZIM_EQUAL_AREA:
    proj.initLambertAzim(p.pOriginLat, p.pOriginLon);
    proj.setOffsetOrigin(p.pOffsetOriginLat, p.pOffsetOriginLon);
    break;
  case ParmProjection::LATLON:
    proj.initLatlon();
    break;
  case ParmProjection::FLAT:
    proj.initFlat(p.pOriginLat, p.pOriginLon, 0);
    break;
  default:
    ILOG(ERROR, "projection is out of synch with ConvWx code");
    // this is wrong, but leave for now
    proj.initFlat(p.pOriginLat, p.pOriginLon, p.pRotation);
  }
}

//------------------------------------------------------------------
static void sSetMasterHdrMetadata(DsMdvx &D, MetaData &metadata,
				  time_t &twritten)
{
  Mdvx::master_header_t mh = D.getMasterHeader();
  string s = mh.data_set_info;
  metadata.setInfo(s);
  s = mh.data_set_name;
  metadata.setName(s);
  s = mh.data_set_source;
  metadata.setSource(s);

  twritten = mh.time_written;
}

//------------------------------------------------------------------
static void sSetXmlMetadata(DsMdvx &D, MetaData &metadata)
{
  metadata.xmlClear();
  int nc = D.getNChunks();
  for (int i=0; i<nc; ++i)
  {
    MdvxChunk *c;
    c = new MdvxChunk(*D.getChunkByNum(i));

    // this is a little weak as there could be other CHUNKCOMMENT chunks
    if (c->getId() != Mdvx::CHUNK_COMMENT)
    {
      delete c;
      continue;
    }
    int n = c->getSize();
    const char *d = (const char *)c->getData();
    // make sure it is null terminated by a copy
    char *buf = new char[n+1];
    strncpy(buf, d, n);
    metadata.setXml(buf);
    delete c;
    delete [] buf;
    return;
  }
}

//------------------------------------------------------------------
static vector<double> sSetVlevel(const Mdvx::master_header_t &m,
				 const MdvxField &f)
{
  vector<double> ret;

  Mdvx::field_header_t hdr = f.getFieldHeader();
  
  if (m.vlevel_included == 1)
  {
    Mdvx::vlevel_header_t vhdr = f.getVlevelHeader();
    for (int i=0; i<hdr.nz; ++i)
    {
      ret.push_back(vhdr.level[i]);
    }
  }
  else
  {
    for (int i=0; i<hdr.nz; ++i)
    {
      ret.push_back(hdr.grid_minz + i*hdr.grid_dz);
    }
  }
  return ret;
}

//------------------------------------------------------------------
static bool sLoad(DsMdvx &D, const string &url, const time_t &t,
		  const int lt, const string &field, const bool remap,
		  const ParmProjection &p, FcstGrid &g,
		  bool suppresswarnings)
{
  Grid gr(field, "unknown", p.pNx, p.pNy, p.pNz, -99.0);
  string path = "missing";
  MetaData metadata;
  g = FcstGrid(t, lt, gr, path, metadata);
  D.clearReadFields();
  D.addReadField(field);
  if (remap)
  {
    MdvxProj proj;
    sSetProj(p, proj);
    D.setReadRemap(proj);
  }

  if (sVlevelRestricted)
  {
    vector<Mdvx::vlevel_header_t> vlevels = D.getVlevelHeadersFile();
    if (vlevels.empty())
    {
      ILOG(ERROR, "Cannot subsample vertical levels, no vlevels");
      return false;
    }
    Mdvx::vlevel_header_t v = vlevels[0];
    D.setReadVlevelLimits(v.level[sVlevelIndex1], v.level[sVlevelIndex2]);
    //D.setReadPlaneNumLimits(sVlevelIndex1, sVlevelIndex2);
  }

  if (D.readVolume())
  {
    if (!suppresswarnings)
    {
      ILOGF(ERROR, "error reading mdv volume from %s at %s+%d",
	    url.c_str(), DateTime::strn(t).c_str(), lt);
    }
    return false;
  }
  time_t twritten;
  sSetMasterHdrMetadata(D, metadata, twritten);

  MdvxField *f;
  f = D.getFieldByName(field);
  if (f == NULL)
  {
    if (!suppresswarnings)
    {
      ILOGF(ERROR, "reading field %s from %s at %s",
	    field.c_str(), url.c_str(), DateTime::strn(t).c_str());
    }
    return false;
  }

  f->convertType(Mdvx::ENCODING_FLOAT32, Mdvx::COMPRESSION_NONE);
  Mdvx::field_header_t hdr = f->getFieldHeader();

  int expectedNz = 0;
  if (sVlevelRestricted)
  {
    expectedNz = sVlevelIndex2 - sVlevelIndex1 + 1;
  }
  else
  {
    expectedNz = p.pNz;
  }
  if (hdr.nx != p.pNx || hdr.ny != p.pNy || hdr.nz != expectedNz)
  {
    ILOG(ERROR, "dimensions inconsistent");
    return false;
  }

  if (sAllowStoringVerticalLevels)
  {
    sVlevel = sSetVlevel(D.getMasterHeader(), *f);
  }

  string units = hdr.units;
  fl32 *data = (fl32 *)f->getVol();

  Grid gloc(field, units, hdr.nx, hdr.ny,  hdr.nz, hdr.missing_data_value);
  for (int i=0; i<hdr.nx*hdr.ny*hdr.nz; ++i)
  {
    if (data[i] != hdr.bad_data_value &&
	data[i] != hdr.missing_data_value)
    {
      gloc.setv(i, static_cast<double>(data[i]));
    }
  }
  path = D.getPathInUse();
  sSetXmlMetadata(D, metadata);
  g = FcstGrid(t, lt, gloc, path, metadata);
  g.setTimeWritten(twritten);
  return true;
}

//------------------------------------------------------------------
static bool sLoad(DsMdvx &D, const string &url, const time_t &t,
		  const int lt, const string &field, const bool remap,
		  const ParmProjection &p, FcstGrid &g,
		  std::vector<double> &vlevels)
{
  Grid gr(field, "unknown", p.pNx, p.pNy, p.pNz, -99.0);
  string path = "missing";
  MetaData metadata;
  g = FcstGrid(t, lt, gr, path, metadata);
  D.clearReadFields();
  D.addReadField(field);
  if (remap)
  {
    MdvxProj proj;
    sSetProj(p, proj);
    D.setReadRemap(proj);
  }

  if (sVlevelRestricted)
  {
    vector<Mdvx::vlevel_header_t> vlevels = D.getVlevelHeadersFile();
    if (vlevels.empty())
    {
      ILOG(ERROR, "Cannot subsample vertical levels, no vlevels");
      return false;
    }
    Mdvx::vlevel_header_t v = vlevels[0];
    D.setReadVlevelLimits(v.level[sVlevelIndex1], v.level[sVlevelIndex2]);
    //D.setReadPlaneNumLimits(sVlevelIndex1, sVlevelIndex2);
  }

  if (D.readVolume())
  {
    ILOGF(ERROR, "error reading mdv volume from %s at %s+%d",
	  url.c_str(), DateTime::strn(t).c_str(), lt);
    return false;
  }
  time_t twritten;
  sSetMasterHdrMetadata(D, metadata, twritten);

  MdvxField *f;
  f = D.getFieldByName(field);
  if (f == NULL)
  {
    ILOGF(ERROR, "reading field %s from %s at %s",
	  field.c_str(), url.c_str(), DateTime::strn(t).c_str());
    return false;
  }

  f->convertType(Mdvx::ENCODING_FLOAT32, Mdvx::COMPRESSION_NONE);
  Mdvx::field_header_t hdr = f->getFieldHeader();

  int expectedNz = 0;
  if (sVlevelRestricted)
  {
    expectedNz = sVlevelIndex2 - sVlevelIndex1 + 1;
  }
  else
  {
    expectedNz = p.pNz;
  }
  if (hdr.nx != p.pNx || hdr.ny != p.pNy || hdr.nz != expectedNz)
  {
    ILOG(ERROR, "dimensions inconsistent");
    return false;
  }

  vlevels = sSetVlevel(D.getMasterHeader(), *f);

  string units = hdr.units;
  fl32 *data = (fl32 *)f->getVol();

  Grid gloc(field, units, hdr.nx, hdr.ny,  hdr.nz, hdr.missing_data_value);
  for (int i=0; i<hdr.nx*hdr.ny*hdr.nz; ++i)
  {
    if (data[i] != hdr.bad_data_value &&
	data[i] != hdr.missing_data_value)
    {
      gloc.setv(i, static_cast<double>(data[i]));
    }
  }
  path = D.getPathInUse();
  sSetXmlMetadata(D, metadata);
  g = FcstGrid(t, lt, gloc, path, metadata);
  g.setTimeWritten(twritten);
  return true;
}

//------------------------------------------------------------------
static bool sLoad(DsMdvx &D, const string &url, const time_t &t,
		  const vector<string> &field, const bool remap,
		  const ParmProjection &p, bool suppressErrorMessages,
		  MultiGrid &g, string &path, MetaData &metadata)
{
  D.clearReadFields();
  for (int i=0; i<static_cast<int>(field.size()); ++i)
  {
    D.addReadField(field[i]);
  }
  if (remap)
  {
    MdvxProj proj;
    sSetProj(p, proj);
    D.setReadRemap(proj);
  }
  if (sVlevelRestricted)
  {
    vector<Mdvx::vlevel_header_t> vlevels = D.getVlevelHeadersFile();
    if (vlevels.empty())
    {
      ILOG(ERROR, "Cannot subsample vertical levels, no vlevels");
      return false;
    }
    Mdvx::vlevel_header_t v = vlevels[0];
    D.setReadVlevelLimits(v.level[sVlevelIndex1], v.level[sVlevelIndex2]);
    //D.setReadPlaneNumLimits(sVlevelIndex1, sVlevelIndex2);
  }
  if (D.readVolume())
  {
    if (!suppressErrorMessages)
    {
      ILOGF(ERROR, "reading mdv volume from %s at %s",
	    url.c_str(), ConvWxTime::stime(t).c_str());
      for (int i=0; i<static_cast<int>(field.size()); ++i)
      {
	ILOGF(ERROR, "reading volume, field name = %s",
	      field[i].c_str());
      }
    }
    return false;
  }

  int expectedNz = 0;
  if (sVlevelRestricted)
  {
    expectedNz = sVlevelIndex2 - sVlevelIndex1 + 1;
  }
  else
  {
    expectedNz = p.pNz;
  }

  time_t twritten;
  sSetMasterHdrMetadata(D, metadata, twritten);

  MdvxField *f;

  bool atLeastOne = false;
  string firstField = "";

  vector<double> vlevels;
  for (int i=0; i<static_cast<int>(field.size()); ++i)
  {
    Grid gr;
    string units = "unknown";
    f = D.getFieldByName(field[i]);
    if (f == NULL)
    {
      ILOGF(ERROR, "reading field %s from %s at %s",
	    field[i].c_str(), url.c_str(), DateTime::strn(t).c_str());
      gr = Grid(field[i], units, p.pNx, p.pNy,p.pNz, -99.0);
    }
    else
    {
      f->convertType(Mdvx::ENCODING_FLOAT32, Mdvx::COMPRESSION_NONE);
      Mdvx::field_header_t hdr = f->getFieldHeader();
      units = hdr.units;

      if (hdr.nx != p.pNx || hdr.ny != p.pNy || hdr.nz != expectedNz)
      {
	ILOGF(WARNING, "field %s..dimension disagree with expected",
	      field[i].c_str());
	ILOGF(WARNING, "ignore %s data and create empty grid output",
	      field[i].c_str());
	gr = Grid(field[i], units, p.pNx, p.pNy,p.pNz, -99.0);
      }
      else
      {
	if (!atLeastOne)
	{
	  vlevels = sSetVlevel(D.getMasterHeader(), *f);
	  firstField = field[i];
	  atLeastOne = true;
	}
	else
	{
	  vector<double> vlevel2 = sSetVlevel(D.getMasterHeader(), *f);
	  if (!(vlevel2 == vlevels))
	  {
	    ILOGF(WARNING,
		  "%s and %s vertical levels are different, use those in %s",
		  firstField.c_str(), field[i].c_str(), firstField.c_str());
	  }
	}
	fl32 *data = (fl32 *)f->getVol();
	gr = Grid(field[i], units, hdr.nx, hdr.ny, hdr.nz,
		  hdr.missing_data_value);
	for (int j=0; j<hdr.nx*hdr.ny*hdr.nz ; ++j)
	{
	  if (data[j] != hdr.bad_data_value &&
	      data[j] != hdr.missing_data_value)
	  {
	    gr.setv(j, static_cast<double>(data[j]));
	  }
	}
      }
    }
    g.append(gr);
  }

  path = D.getPathInUse();
  sSetXmlMetadata(D, metadata);
  return atLeastOne;
}

//------------------------------------------------------------------
static void sAddChunk(DsMdvx &output, const MetaData &m)
{
  if (!m.hasXml())
  {
    return;
  }
  Mdvx::chunk_header_t h;
  memset(&h, 0, sizeof(Mdvx::chunk_header_t));
  h.chunk_id = Mdvx::CHUNK_COMMENT;
  h.size = static_cast<int>(m.xmlSize())+1; // null termination char at end.
  sprintf(h.info, "MetaData");
  MdvxChunk *c = new MdvxChunk(h, m.xmlCStr());
  output.addChunk(c);
}

//------------------------------------------------------------------
static void sTranslate(const Grid &o, Mdvx::encoding_type_t &mdvE,
		       Mdvx::compression_type_t &mdvC,
		       Mdvx::scaling_type_t &mdvS, double &scale,
		       double &bias)
{
  Grid::Encoding_t e = o.getEncoding();
  Grid::Scaling_t s = o.getScaling();
  o.getScaleBias(scale, bias);
  switch (e)
  {
  case Grid::ENCODING_INT8:
    mdvE = Mdvx::ENCODING_INT8;
    break;
  case Grid::ENCODING_INT16:
    mdvE = Mdvx::ENCODING_INT16;
    break;
  case Grid::ENCODING_FLOAT32:
    mdvE = Mdvx::ENCODING_FLOAT32;
    break;
  default:
    // go with the float encoding
    mdvE = Mdvx::ENCODING_FLOAT32;
    break;
  }

  switch (s)
  {
  case Grid::SCALING_SPECIFIED:
    mdvS = Mdvx::SCALING_SPECIFIED;
    break;
  case Grid::SCALING_DYNAMIC:
    mdvS = Mdvx::SCALING_DYNAMIC;
    break;
  default:
    // use dynamic as the default
    mdvS = Mdvx::SCALING_DYNAMIC;
    break;
  }

  if (o.getCompressionStatus())
  {
    mdvC = Mdvx::COMPRESSION_GZIP;
  }
  else
  {
    mdvC = Mdvx::COMPRESSION_NONE;
  }
}

//------------------------------------------------------------------
static void sConvertType(MdvxField &f, const Grid &o)
{
  Mdvx::encoding_type_t mdvE;
  Mdvx::compression_type_t mdvC;
  Mdvx::scaling_type_t mdvS;
  double scale, bias;
  sTranslate(o, mdvE, mdvC, mdvS, scale, bias);
  f.convertType(mdvE, mdvC, mdvS, scale, bias);
}

//------------------------------------------------------------------
static void sAddField(DsMdvx &output, Mdvx::field_header_t &fieldHdr,
		      Mdvx::vlevel_header_t &vlevelHdr, const Grid &o)
{
  fieldHdr.bad_data_value = o.getMissing();
  fieldHdr.missing_data_value = o.getMissing();
  strncpy(fieldHdr.field_name_long, o.getName().c_str(), MDV_LONG_FIELD_LEN-1);
  fieldHdr.field_name_long[MDV_LONG_FIELD_LEN-1] = '\0';
  strncpy(fieldHdr.field_name, o.getName().c_str(), MDV_SHORT_FIELD_LEN-1);
  fieldHdr.field_name[MDV_SHORT_FIELD_LEN-1] = '\0';
  strncpy(fieldHdr.units, o.getUnits().c_str(), MDV_UNITS_LEN-1);
  fieldHdr.units[MDV_UNITS_LEN-1] = '\0';
  MdvxField *f = new MdvxField(fieldHdr, vlevelHdr, (void *)0,true,false);
  fl32 *fo = (fl32 *)f->getVol();
  if (o.copyFloatFilterNans(fo, o.getName(),
			    fieldHdr.nx*fieldHdr.ny*fieldHdr.nz))
  {
    sConvertType(*f, o);
    output.addField(f);
  }
  else
  {
    // otherwise no field is added.
    delete f;
  }
}

//------------------------------------------------------------------
static void sOutputInit(const time_t &gt, int lt, 
		        DsMdvx &output)
{
  Mdvx::master_header_t m;
  memset(&m, 0, sizeof(m));
  m.time_gen = gt;
  m.time_begin = gt + lt;
  m.time_end = gt + lt;
  m.time_centroid = gt + lt;
  m.time_expire = gt + lt;
  m.data_dimension = 2;
  m.data_collection_type = Mdvx::DATA_FORECAST;
  m.native_vlevel_type = Mdvx::VERT_TYPE_Z;
  m.vlevel_type = Mdvx::VERT_TYPE_Z;;
  m.vlevel_included = 1;
  m.grid_orientation = Mdvx::ORIENT_SN_WE;
  m.data_ordering = Mdvx::ORDER_XYZ;
  m.forecast_time = gt + lt;
  m.forecast_delta = lt;
  output.setMasterHeader(m);
}

//------------------------------------------------------------------
static void sOutputInit(const time_t &gt, const int lt, 
			const ParmProjection &p, DsMdvx &output,
			const MetaData &metadata, 
			Mdvx::field_header_t &fieldHdr, 
			Mdvx::vlevel_header_t &vlevelHdr)
{
  // create a projection for mdv
  MdvxProj proj;
  sSetProj(p, proj);

  Mdvx::master_header_t m;
  memset(&m, 0, sizeof(m));
  m.time_gen = gt;
  m.time_begin = gt + lt;
  m.time_end = gt + lt;
  m.time_centroid = gt + lt;
  m.time_expire = gt + lt;
  m.data_dimension = 2;
  m.data_collection_type = Mdvx::DATA_FORECAST;
  m.native_vlevel_type = Mdvx::VERT_TYPE_Z;
  m.vlevel_type = Mdvx::VERT_TYPE_Z;;
  m.vlevel_included = 1;
  m.grid_orientation = Mdvx::ORIENT_SN_WE;
  m.data_ordering = Mdvx::ORDER_XYZ;
  m.forecast_time = gt + lt;
  m.forecast_delta = lt;
  if (metadata.hasInfo())
  {
    strncpy(m.data_set_info, metadata.infoCStr(), MDV_INFO_LEN-1);
  }
  if (metadata.hasName())
  {
    strncpy(m.data_set_name, metadata.nameCStr(), MDV_NAME_LEN-1);
  }
  if (metadata.hasSource())
  {
    strncpy(m.data_set_source, metadata.sourceCStr(), MDV_NAME_LEN-1);
  }
  output.setMasterHeader(m);

  memset(&fieldHdr, 0, sizeof(fieldHdr));
  fieldHdr.forecast_delta = lt;
  fieldHdr.forecast_time = gt + lt;
  fieldHdr.encoding_type = Mdvx::ENCODING_FLOAT32;
  fieldHdr.data_element_nbytes = 4;
  fieldHdr.compression_type = Mdvx::COMPRESSION_NONE; // done later
  fieldHdr.transform_type = Mdvx::DATA_TRANSFORM_NONE;
  fieldHdr.scaling_type = Mdvx::SCALING_DYNAMIC;
  fieldHdr.native_vlevel_type = Mdvx::VERT_TYPE_Z;
  fieldHdr.vlevel_type = Mdvx::VERT_TYPE_Z;
  if (p.pNz == 1)
    fieldHdr.dz_constant = 1;
  else
    fieldHdr.dz_constant = 0;
  if (p.pNz == 1)
    fieldHdr.data_dimension = 2;
  else
    fieldHdr.data_dimension = 3;
  fieldHdr.scale = 1.0;
  fieldHdr.bias = 0.0;

  // Fill in the projection information in the field header.  Note
  // that this must be done after dataElementNbytes is set above
  // or the volumeSize value in the header will be wrong.
  proj.syncXyToFieldHdr(fieldHdr);
  
  fieldHdr.nz = p.pNz;
  fieldHdr.grid_minz = p.pMinz;
  fieldHdr.grid_dz = p.pDz;

  fieldHdr.volume_size = fieldHdr.nx *  fieldHdr.ny * 
    fieldHdr.nz * fieldHdr.data_element_nbytes;


  // Create the vlevel header
  memset(&vlevelHdr, 0, sizeof(vlevelHdr));
  for (int z = 0; z < p.pNz; ++z)
  {
    vlevelHdr.type[z] = Mdvx::VERT_TYPE_Z;
    vlevelHdr.level[z] = p.pMinz + z*p.pDz;
  }

}

//------------------------------------------------------------------
static void sOutputInit(const time_t &gt, 
			const ParmProjection &p, DsMdvx &output,
			const MetaData &metadata, 
			Mdvx::field_header_t &fieldHdr, 
			Mdvx::vlevel_header_t &vlevelHdr)
{
  sOutputInit(gt, 0, p, output, metadata, fieldHdr, vlevelHdr);
  output.setDataCollectionType(Mdvx::DATA_MEASURED);
}  

//------------------------------------------------------------------
static void sOutputInit(const time_t &gt, DsMdvx &output)
{
  sOutputInit(gt, 0, output);
  output.setDataCollectionType(Mdvx::DATA_MEASURED);
}  

//------------------------------------------------------------------
static bool sLoadAny(DsMdvx &D, const string &url, const time_t t,
		     int lt, FcstGrid &g)
{
  g = FcstGrid();

  D.clearReadFields();
  D.addReadField(0);
  if (D.readVolume())
  {
    return false;
  }
  MetaData metadata;
  time_t twritten;
  sSetMasterHdrMetadata(D, metadata, twritten);
  MdvxField *f;
  f = D.getFieldByNum(0);
  if (f == NULL)
  {
    ILOGF(ERROR, "reading field 0 from %s at %s", url.c_str(),
	  DateTime::strn(t).c_str());
    return false;
  }

  if (sAllowStoringVerticalLevels)
  {
    sVlevel = sSetVlevel(D.getMasterHeader(), *f);
  }
  f->convertType(Mdvx::ENCODING_FLOAT32, Mdvx::COMPRESSION_NONE);
  Mdvx::field_header_t hdr = f->getFieldHeader();
  string units = hdr.units;
  string field = hdr.field_name;
  fl32 *data = (fl32 *)f->getVol();
  Grid gloc(field, units, hdr.nx, hdr.ny, hdr.nz, hdr.missing_data_value);
  for (int i=0; i<hdr.nx*hdr.ny*hdr.nz; ++i)
  {
    if (data[i] != hdr.bad_data_value &&
	data[i] != hdr.missing_data_value)
    {
      gloc.setv(i, static_cast<double>(data[i]));
    }
  }
  string path = D.getPathInUse();
  sSetXmlMetadata(D, metadata);
  g = FcstGrid(t, lt, gloc, path, metadata);
  g.setTimeWritten(twritten);
  return true;
}

//------------------------------------------------------------------
static time_t sGetTimeWritten(const time_t &gt, int lt, const ParmFcst &parm)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, parm.pUrl, 0, gt, lt);
  if (D.readAllHeaders())
  {
    ILOGF(ERROR, "Reading headers from %s at %s+%d", parm.pUrl.c_str(),
	  ConvWxTime::stime(gt).c_str(), lt);
    return false;
  }
  Mdvx::master_header_t mh = D.getMasterHeaderFile();
  return mh.time_written;
}

//------------------------------------------------------------------
void InterfaceIO::startup(const string &process, const string &instance,
			  const int maxSeconds)
{
  int ms;
  if (maxSeconds < 0)
  {
    ms = 60;
  }
  else
  {
    ms = maxSeconds;
  }
  EarthRadius::init();
  PMU_auto_init(process.c_str(), instance.c_str(), ms);

  const char *valstr = getenv("AUTO_LOGFILES");
  if (valstr != NULL)
  {
    ILOG_LOGFILE_INIT(process, instance, ""); // writes to $LOGFILE_DIR
  }
}

//------------------------------------------------------------------
void InterfaceIO::setCleanExit(void tidyAndExit(int))
{
  PORTsignal(SIGQUIT, tidyAndExit);
  PORTsignal(SIGTERM, tidyAndExit);
  PORTsignal(SIGINT, tidyAndExit);
  PORTsignal(SIGPIPE, (PORTsigfunc)SIG_IGN);
}

//------------------------------------------------------------------
void InterfaceIO::setLogging(const bool debug, const bool debugVerbose,
			     const bool debugTriggering,
			     const bool showRealtime, 
			     const bool showClassAndMethod)
{  
  ILOG_INIT(debug, debugVerbose, showRealtime, showClassAndMethod);
  if (debugTriggering)
  {
    ILOG_ENABLE_SEVERITY(TRIGGER);
  }
}

//------------------------------------------------------------------
void InterfaceIO::doRegister(const string &msg)
{
  PMU_auto_register(msg.c_str());
}

//------------------------------------------------------------------
void InterfaceIO::finish(void)
{
  PMU_auto_unregister();
  if (sTrigger != NULL)
  {
    delete sTrigger;
  }
}

//------------------------------------------------------------------
bool InterfaceIO::triggerObs(const string &url,
			     const vector<int> &gtSubsample,
			     const bool debug, time_t &t)
{
  sTriggeringCheck(url, gtSubsample, debug);
  return sTrigger->nextTime(t);
}

//------------------------------------------------------------------
bool InterfaceIO::triggerObsNoWait(const string &url,
				   const vector<int> &gtSubsample,
				   const bool debug, time_t &t,
				   bool &timedOut)
{
  sTriggeringCheck(url, gtSubsample, debug);
  if (sNoWait)
  {
    time_t t = time(0);
    for (;;)
    {
      if (sTrigger->nextTime(t))
      {
	timedOut = false;
	return true;
      }
      else
      {
	time_t t2 = time(0);
	if (t2 - t > sMaxWaitSeconds)
	{
	  timedOut = true;
	  return false;
	}
	else
	{
	  sleep(1);
	}
      }
    }
  }
  else
  {
    ILOG(ERROR, "not in 'noWait' mode");
    timedOut = false;
    return false;
  }
}

//------------------------------------------------------------------
bool InterfaceIO::triggerObs(const string &url,
			     const vector<int> &gtSubsample,
			     const bool debug, const time_t &t0,
			     const time_t &t1, time_t &t)
{
  sTriggeringCheck(url, gtSubsample, t0, t1, debug);
  return sTrigger->nextTime(t);
}

//------------------------------------------------------------------
bool InterfaceIO::triggerFcst(const string &url, const bool isLtSubsample,
			      const int lt0, const int lt1, const int dlt,
			      const vector<int> &gtSubsample,
			      const bool debug, time_t &t)
{
  sTriggeringCheck(url, isLtSubsample, lt0, lt1, dlt, gtSubsample, false,
		   debug);
  return sTrigger->nextTime(t);
}

//------------------------------------------------------------------
bool InterfaceIO::triggerFcst(const string &url, const bool isLtSubsample,
			      const int lt0, const int lt1, const int dlt,
			      const vector<int> &gtSubsample, 
			      const bool debug, const time_t &t0, 
			      const time_t &t1, time_t &t)
{
  sTriggeringCheck(url, isLtSubsample, lt0, lt1, dlt, gtSubsample, false,
		   t0, t1, debug);
  return sTrigger->nextTime(t);
}

//------------------------------------------------------------------
bool InterfaceIO::triggerFcstNext(const string &url,
				  const bool isLtSubsample,
				  const int lt0, const int lt1, 
				  const int dlt,
				  const vector<int> &gtSubsample,
				  const bool debug, time_t &t, int &lt)
{
  sTriggeringCheck(url, isLtSubsample, lt0, lt1, dlt, gtSubsample,
		   true, debug);
  return sTrigger->nextTime(t, lt);
}

//------------------------------------------------------------------
bool InterfaceIO::triggerFcstNextNoWait(const string &url,
					const bool isLtSubsample,
					const int lt0, const int lt1, 
					const int dlt,
					const vector<int> &gtSubsample,
					const bool debug, time_t &t, int &lt,
					bool &timedOut)
{
  sTriggeringCheck(url, isLtSubsample, lt0, lt1, dlt, gtSubsample,
		   true, debug);
  if (sNoWait)
  {
    time_t t0 = time(0);
    for (;;)
    {
      if (sTrigger->nextTime(t, lt))
      {
	timedOut = false;
	return true;
      }
      else
      {
	time_t t2 = time(0);
	if (t2 - t0 > sMaxWaitSeconds)
	{
	  timedOut = true;
	  return false;
	}
	else
	{
	  sleep(1);
	}
      }
    }
  }
  else
  {
    ILOG(ERROR, "not in 'noWait' mode");
    timedOut = false;
    return false;
  }
}

//------------------------------------------------------------------
bool InterfaceIO::triggerFcstNextArchive(const string &url, 
					 const bool isLtSubsample,
					 const int lt0, const int lt1,
					 const int dlt,
					 const vector<int> &gtSubsample,
					 const time_t &t0, const time_t &t1,
					 const bool debug,
					 time_t &t, int &lt)
{
  sTriggeringCheck(url, isLtSubsample, lt0, lt1, dlt, gtSubsample, true,
		   t0, t1, debug);
  return sTrigger->nextTime(t, lt);
}

//------------------------------------------------------------------
void InterfaceIO::triggerSetMaxValidAge(const int maxValidAgeSeconds)
{
  sMaxValidAgeChanged = true;
  sMaxValidAge = maxValidAgeSeconds;
}

//------------------------------------------------------------------
bool InterfaceIO::triggerArchiveRewind(void)
{
  if (!sTriggerstate.isArchive())
  {
    ILOG(ERROR, "not in archive mode");
    return false;
  }
  return sTrigger->rewind();
}

/*******************************************************************/
  
bool InterfaceIO::triggerFile(const std::string &inputDir,
			      const bool debug, const time_t &t0,
			      const time_t &t1, 
			      std::string &filepath)
{

  filepath.clear();

  if(sFileTrigger == NULL)
  {
    sFileTrigger = new DsInputPath( "", debug,
				    inputDir.c_str(),
				    t0,t1,true);
    if(sFileTrigger == NULL)
    {
      ILOG(ERROR, "Creation of file trigger failed.");
      return false;
    }
    sFileTrigger->setSearchExt("ascii");
  }
  
  char *path;

  path  = sFileTrigger->next();

  if(path == NULL)
  {
    return false;
  }
  else
  {
    string pathStr(path);
    filepath = pathStr;
    return true;
  }
}

bool InterfaceIO::triggerFiles(const std::string &inputDir1,std::string &filepath1,
                               const std::string &inputDir2,std::string &filepath2,
                               const bool debug, const time_t &t0,
                               const time_t &t1, bool isGenLeadForecastFormat)
{

  if(sFileTrigger == NULL)
  {
    sFileTrigger = new DsInputPath( "", debug,
                                    inputDir1.c_str(),
                                    t0,t1,isGenLeadForecastFormat);
    if(sFileTrigger == NULL)
    {
      ILOG(ERROR, "Creation of file trigger failed.");
      return false;
    }
    sFileTrigger->setSearchExt("ascii");
  }

  if(sFileTrigger2 == NULL)
  {
    
    sFileTrigger2 = new DsInputPath( "", debug,
                                    inputDir2.c_str(),
                                    t0,t1,isGenLeadForecastFormat);
    if(sFileTrigger2 == NULL)
    {
      ILOG(ERROR, "Creation of file trigger failed.");

      return false;
    }
    sFileTrigger2->setSearchExt("ascii");
  }

  char *path  = sFileTrigger->next();

  char *path2 = sFileTrigger2->next();

  if(path == NULL || path2 ==NULL)
  {
    return false;
  }
  else
  {
    string pathStr(path);

    filepath1 = pathStr;

    string pathStr2(path2);

    filepath2 = pathStr2;

    return true;
  }
}

bool InterfaceIO::triggerFilesRealtime(const std::string &inputDir1,
				       std::string &filepath1,
                                       const std::string &inputDir2,
				       std::string &filepath2,
                                       const int lookback, const bool debug,
				       bool isGenLeadForecastFormat)
{
  if (sLtrigger ==NULL)
  {
    ILOGF(DEBUG, "Creating file trigger %s.", inputDir1.c_str());
    sLtrigger = new LdataInfo(inputDir1, false);
  }
  if (sLtrigger2 ==NULL)
  {
    ILOGF(DEBUG, "Creating file trigger %s.", inputDir2.c_str());
    sLtrigger2 = new LdataInfo(inputDir2, false);
  }
  sLtrigger->readBlocking(-1, 1000, PMU_auto_register);
  string rpath1, rpath2;
  rpath1 = sLtrigger->getRelDataPath();
  ILOGF(DEBUG, "GOT data, filePath1 = %s\n", rpath1.c_str());
  int numTries = 0;
  while (sLtrigger2->read() == -1)
  {
    numTries++;
    if (numTries > 20)
    {
      ILOGF(ERROR, "never got data from inputDir2=%s", inputDir2.c_str());
      return false;
    }
    sleep(60);
    ILOG(DEBUG, "Sleeping waiting for second ascii input");
  }
  rpath2 = sLtrigger2->getRelDataPath();
  filepath1 = inputDir1 + "/";
  filepath1 = filepath1 + rpath1;
  filepath2 = inputDir2 + "/";
  filepath2 = filepath2 + rpath2;
  ILOGF(DEBUG, "GOT all data, filePath1 = %s\n", filepath1.c_str());
  ILOGF(DEBUG, "GOT all data, filePath12= %s\n", filepath2.c_str());
  return true;
}

//------------------------------------------------------------------
void InterfaceIO::clearFileTrigger()
{
  if (sFileTrigger)
  {
    delete sFileTrigger;
  }

  sFileTrigger = NULL;

  if(sFileTrigger2)
  {
     delete sFileTrigger;
  }
}

//------------------------------------------------------------------
bool InterfaceIO::newestObsTime(const time_t &t0, const time_t &t1,
				const string &url, time_t &t)
{
  vector<time_t> gt = InterfaceIO::availableObsTimesInARange(url, t0, t1);
  int n = static_cast<int>(gt.size());
  if (n == 0)
  {
    return false;
  }
  else
  {
    t = gt[n-1];
    return true;
  }
}

//------------------------------------------------------------------
bool InterfaceIO::obsExists(const time_t &it, const int toleranceSeconds,
			    const string &url, time_t &tActual)
{
  return InterfaceIO::availableObsDataClosestToTime(url, it, toleranceSeconds,
						    tActual);
}

//------------------------------------------------------------------
bool InterfaceIO::fcstExists(const time_t &gt, const int lt, const string &url)
{
  vector<int> ltimes = InterfaceIO::availableFcstLeadTimeSecondsAtGt(url, gt);
  return (find(ltimes.begin(), ltimes.end(), lt) != ltimes.end());
}

/*----------------------------------------------------------------*/
bool InterfaceIO::fcstExists(const time_t &igt, const int lt,
			     const int toleranceSeconds,
			     const string &url, time_t &gtActual)
{
  vector<time_t> gt = availableFcstGenTimesInARange(url, 
						    igt-toleranceSeconds,
						    igt+toleranceSeconds);
  if (gt.empty())
  {
    return false;
  }

  int diff = gt[0] - igt;
  if (diff < 0)
  {
    diff = -diff;
  }
  gtActual = gt[0];
  for (int i=1; i<static_cast<int>(gt.size()); ++i)
  {
    int diff_i = gt[i] - igt;
    if (diff_i < 0)
    {
      diff_i = -diff_i;
    }
    if (diff_i < diff)
    {
      diff = diff_i;
      gtActual = gt[i];
    }
  }
  return fcstExists(gtActual, lt, url);
}


//------------------------------------------------------------------
vector<time_t> InterfaceIO::getAvailableValidTimes(const string &url,
						   const time_t &gt)
{
  vector<int> ltimes = InterfaceIO::availableFcstLeadTimeSecondsAtGt(url, gt);
  vector<time_t> ret;
  for (int i=0; i<static_cast<int>(ltimes.size()); ++i)
  {
    time_t vtime = gt + ltimes[i];
    ret.push_back(vtime);
  }
  return ret;
}

//------------------------------------------------------------------
vector<int> InterfaceIO::getAvailableLeadTimes(const time_t &gt, 
					       const string &url,
					       const double ltHour0,
					       const double ltHour1,
					       const double ltDtH)
{
  ILOGF(DEBUG_VERBOSE, "gt: %s, url: %s, ltHour0: %f, ltHour1: %f, ltDtH: %f", ConvWxTime::stime(gt).c_str(), url.c_str(), ltHour0, ltHour1, ltDtH);
  double lt0 = static_cast<double>(static_cast<int>(ltHour0*3600));
  double dt = static_cast<double>(static_cast<int>(ltDtH*3600));
  vector<int> ltimes = InterfaceIO::availableFcstLeadTimeSecondsAtGt(url, gt);
  vector<int> ret;
  ILOGF(DEBUG_VERBOSE, "ltimes.size: %d", ltimes.size());
  for (int i=0; i<static_cast<int>(ltimes.size()); ++i)
  {
    int lt = ltimes[i];
    double i1 = (static_cast<double>(lt) - lt0)/dt;
    int ii = static_cast<int>(i1);
    double i2 = static_cast<double>(ii);
    if (i2 == i1)
    {
      ret.push_back(lt);
    }
    else
    {
      ILOGF(DEBUG_VERBOSE, "reject lead time %d", lt);
    }
  }
  return ret;
}

//------------------------------------------------------------------
vector<time_t> 
InterfaceIO::availableFcstGenTimesInARange(const string &url,
					   const time_t &gt0,
					   const time_t &gt1)
{
  DsMdvx D;
  D.setTimeListModeGen(url, gt0, gt1);
  D.compileTimeList();
  return D.getTimeList();
}

//------------------------------------------------------------------
vector<int> 
InterfaceIO::availableFcstLeadTimeSecondsAtGt(const string &url,
					      const time_t &gt)
{
  DsMdvx D;
  D.setTimeListModeForecast(url, gt);
  D.compileTimeList();
  vector<time_t> vt = D.getValidTimes();
  vector<int> ret;
  for (int i=0; i<static_cast<int>(vt.size()); ++i)
  {
    int lt = vt[i] - gt;
    ret.push_back(lt);
  }
  return ret;
}

//------------------------------------------------------------------
vector<pair<time_t, int>  > 
InterfaceIO::availableFcstGenLeadTimesInARange(const string &url,
					       const time_t &gt0,
					       const time_t &gt1)
{
  vector<pair<time_t, int> > ret;

  DsMdvx D;
  D.setTimeListModeGen(url, gt0, gt1);
  D.compileTimeList();
  vector<time_t> gt = D.getTimeList();
  for (int i=0; i<static_cast<int>(gt.size()); ++i)
  {
    D.setTimeListModeForecast(url, gt[i]);
    D.compileTimeList();
    vector<time_t> vt = D.getValidTimes();
    for (int j=0; j<static_cast<int>(vt.size()); ++j)
    {
      ret.push_back(pair<time_t, int>(gt[i], vt[j]-gt[i]));
    }
  }
  return ret;
}

bool InterfaceIO::newestFcstAtValidTime(const string &url,  
					const time_t &gt0, 
					const time_t &gt1,  
					const time_t &validTime,   
					time_t &gen, 
					int &lead)

{
  vector<pair<time_t, int>  > gtLtPairs;

  gtLtPairs = availableFcstGenLeadTimesInARange(url,gt0,gt1);
  int numGtLtPairs = static_cast<int>(gtLtPairs.size());

  bool gotGenLeadPair = false;
  int count = 0;

  while ( count < static_cast<int>(gtLtPairs.size()) && !gotGenLeadPair)
  {
    if ( gtLtPairs[numGtLtPairs -1 -count].first + gtLtPairs[numGtLtPairs -1 -count].second == validTime)
    {
      gotGenLeadPair = true;
      gen = gtLtPairs[numGtLtPairs -1 -count].first;
      lead = gtLtPairs[numGtLtPairs -1 -count].second;
    }
    else
      count++;
  }
  
  if (gotGenLeadPair)
    return true;
  else
    return false;
}


//------------------------------------------------------------------
bool
InterfaceIO::availableObsDataClosestToTime(const string &url,
					   const time_t &t,
					   const int toleranceSeconds,
					   time_t &tActual)
{
  DsMdvx mdvx;
  mdvx.setTimeListModeClosest(url, t, toleranceSeconds);
  mdvx.compileTimeList();
  vector<time_t> gt = mdvx.getTimeList();
  if (gt.size() != 1)
  {
    return false;
  }
  tActual = gt[0];
  return true;
}

//------------------------------------------------------------------
vector<time_t> 
InterfaceIO::availableObsTimesInARange(const string &url,
				       const time_t &t0,
				       const time_t &t1)
{
  DsMdvx D;
  D.setTimeListModeValid(url, t0, t1);
  D.compileTimeList();
  return D.getTimeList();
}

//------------------------------------------------------------------
bool
InterfaceIO::bestGenTime(const time_t &t, int maxSecondsBack,
			 const ParmFcst &parm, time_t &bestT,
			 bool checkTimeWritten, bool useIncomplete)
{
  // make the assumption that if the last lead time is present, all
  // the earlier ones are too.

  // get all gen times, and for each get all the lead times
  vector<time_t> vgt;
  vgt =
    InterfaceIO::availableFcstGenTimesInARange(parm.pUrl, t-maxSecondsBack, t);
  if (vgt.empty())
  {
    ILOGF(WARNING, "No forecast data back %d seconds from %s",
	maxSecondsBack, ConvWxTime::stime(t).c_str());
    return false;
  }

  int ngt = static_cast<int>(vgt.size());
  time_t candidate = 0;
  for (int i=ngt-1; i>=0; --i)
  {
    std::vector<int> lts = availableFcstLeadTimeSecondsAtGt(parm.pUrl, vgt[i]);
    
    // is the last one there?
    if (!lts.empty())
    {
      int lt = *(lts.rbegin());
      if (parm.isLastLt(lt))
      {
	bool good = true;
	if (checkTimeWritten)
	{
	  time_t twritten = sGetTimeWritten(vgt[i], lt, parm);
	  if (twritten > t)
	  {
	    ILOGF(DEBUG, "forecast gen time = %s is too new",
		  ConvWxTime::stime(vgt[i]).c_str());
	    good = false;
	  }	    
	}
	if (good)
	{
	  bestT = vgt[i];
	  ILOGF(DEBUG, "Best forecast gen time = %s",
		ConvWxTime::stime(bestT).c_str());
	  return true;
	}
      }
      else
      {
	ILOGF(DEBUG, "Gen time %s latest lead=%d not complete",
	      ConvWxTime::stime(vgt[i]).c_str(), lt);
	if (candidate == 0)
	{
	  candidate = vgt[i];
	}
      }
    }
  }
  if (useIncomplete)
  {
    if (candidate != 0)
    {
      bestT = candidate;
      ILOGF(WARNING,
	    "No complete forecast back %d seconds from %s,"
	    " using incomplete forecast at %s",
	    maxSecondsBack, ConvWxTime::stime(t).c_str(),
	    ConvWxTime::stime(bestT).c_str());
      return true;
    }
  }
  ILOGF(ERROR, "No complete forecast back %d seconds from %s",
	maxSecondsBack, ConvWxTime::stime(t).c_str());
  return false;
}

//------------------------------------------------------------------
bool InterfaceIO::bestGenTimeWithLatency(const time_t &t, int maxSecondsBack,
					 int minSecondsBack,
					 const ParmFcst &parm, time_t &bestT)
{
  // make the assumption that if the last lead time is present, all
  // the earlier ones are too.

  // get all gen times, and for each get all the lead times
  vector<time_t> vgt;
  vgt =
    InterfaceIO::availableFcstGenTimesInARange(parm.pUrl, t-maxSecondsBack,
					       t-minSecondsBack);
  if (vgt.empty())
  {
    ILOGF(ERROR, "No forecast data back [%d,%d] seconds from %s",
	  maxSecondsBack, minSecondsBack, ConvWxTime::stime(t).c_str());
    return false;
  }

  int ngt = static_cast<int>(vgt.size());
  for (int i=ngt-1; i>=0; --i)
  {
    std::vector<int> lts = availableFcstLeadTimeSecondsAtGt(parm.pUrl, vgt[i]);
    
    // is the last one there?
    if (!lts.empty())
    {
      int lt = *(lts.rbegin());
      if (parm.isLastLt(lt))
      {
	bestT = vgt[i];
	ILOGF(DEBUG, "Best forecast gen time = %s",
	      ConvWxTime::stime(vgt[i]).c_str());
	return true;
      }
      else
      {
	ILOGF(DEBUG, "Gen time %s latest lead=%d not complete",
	      ConvWxTime::stime(vgt[i]).c_str(), lt);
      }
    }
  }
  ILOGF(ERROR, "No complete forecast back [%d,%d] seconds from %s",
	maxSecondsBack, minSecondsBack, ConvWxTime::stime(t).c_str());
  return false;
}


//------------------------------------------------------------------
void InterfaceIO::restrictVerticalLevel(const int index)
{
  sVlevelRestricted = true;
  sVlevelIndex1 = index;
  sVlevelIndex2 = index;
}

//------------------------------------------------------------------
void InterfaceIO::restrictVerticalLevel(const int index1, const int index2)
{
  sVlevelRestricted = true;
  sVlevelIndex1 = index1;
  sVlevelIndex2 = index2;
}

//------------------------------------------------------------------
void InterfaceIO::unrestrictVerticalLevel(void)
{
  sVlevelRestricted = false;
  sVlevelIndex1  = -1;
  sVlevelIndex2 = -1;
}

//------------------------------------------------------------------
void InterfaceIO::disallowStoringVerticalLevels(void)
{
  sAllowStoringVerticalLevels = false;
  sVlevel.clear();
}

//------------------------------------------------------------------
std::vector<double> InterfaceIO::getVerticalLevels(void)
{
  if (sAllowStoringVerticalLevels)
  {
    ILOG(ERROR, 
	 "Cannot retrieve vertical levels. State has been set to disallow");
  }
  return sVlevel;
}

//------------------------------------------------------------------
bool InterfaceIO::loadObs(const time_t &t, const ParmProjection &p,
			  const string &url, const string &field,
			  const bool remap, FcstGrid &g, bool suppresswarnings)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_FIRST_BEFORE, url, 0, t);
  return sLoad(D, url, t, 0, field, remap, p, g, suppresswarnings);
}

//------------------------------------------------------------------
bool InterfaceIO::loadObs(const time_t &t, const ParmProjection &p,
                          const string &url, const string &field,
                          const bool remap, FcstGrid &g,
			  vector <double> &vLevels)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_FIRST_BEFORE, url, 0, t);
  bool ret = sLoad(D, url, t, 0, field, remap, p, g, vLevels);
  if (sAllowStoringVerticalLevels)
  {
    for (int i = 0; i < (int)sVlevel.size(); i++)
      vLevels.push_back( sVlevel[i]);
  }
  else
  {
    ILOG(ERROR, 
	 "Cannot retrieve vertical levels. State has been set to disallow");
    ret = false;
  }
  return ret; 
}

//------------------------------------------------------------------
bool InterfaceIO::loadObs(const time_t &t, const ParmProjection &p,
			  const string &url, const string &field,
			  const bool remap, Grid &g, bool suppresswarnings)
{
  FcstGrid fg;
  bool stat = InterfaceIO::loadObs(t, p, url, field, remap, fg,
				   suppresswarnings);
  g = fg;
  return stat;
}

//------------------------------------------------------------------
bool InterfaceIO::loadObs(const time_t &t, const ParmProjection &p,
                          const string &url, const string &field,
                          const bool remap, Grid &g, vector <double> &vLevels)
{
  FcstGrid fg;
  bool stat = InterfaceIO::loadObs(t, p, url, field, remap, fg, vLevels);
  g = fg;
  return stat;
}

//------------------------------------------------------------------
bool InterfaceIO::loadMultiObs(const time_t &t, const ParmProjection &p,
			       const string &url, const vector<string> &field,
			       const bool remap, MultiGrid &g)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_FIRST_BEFORE, url, 0, t);
  string path;
  MetaData metadata;
  bool suppressErrorMessages = false;
  return sLoad(D, url, t, field, remap, p, suppressErrorMessages, g, path,
	       metadata);
}

//------------------------------------------------------------------
bool InterfaceIO::loadMultiObsWMetaData(const time_t &t, const ParmProjection &p,
                                        const std::string &url,
                                        const std::vector<std::string> &field,
                                        const bool remap, MultiGrid &g,
                                        MetaData &metadata)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_FIRST_BEFORE, url, 0, t);
  string path;
  bool suppressErrorMessages = false;
  return sLoad(D, url, t, field, remap, p, suppressErrorMessages,
	       g, path, metadata);
}

//------------------------------------------------------------------
bool InterfaceIO::loadFcst(const time_t &gt, const int lt, 
			   const ParmProjection &p, const string &url,
			   const string &field, const bool remap,
			   FcstGrid &g)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  return sLoad(D, url, gt, lt, field, remap, p, g, false);
}

//------------------------------------------------------------------
bool InterfaceIO::loadAnyFcst(const time_t &gt, const string &url,
			      FcstGrid &g)
{
  vector<int> ltimes = 
    InterfaceIO::availableFcstLeadTimeSecondsAtGt(url, gt);
  if (ltimes.empty())
  {
    return false;
  }
  int lt = ltimes[0];
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  return sLoadAny(D, url, gt, lt, g);
}

//------------------------------------------------------------------
bool InterfaceIO::loadMultiFcst(const time_t gt, const int lt, 
				const ParmProjection &p, const string &url,
				const vector<string> &field, const bool remap,
				MultiFcstGrid &g, bool suppressErrorMessages)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  MultiGrid gr;
  string path;
  MetaData metadata;
  bool stat = sLoad(D, url, gt, field, remap, p, suppressErrorMessages, gr,
		    path, metadata);
  if (stat)
  {
    g.init(gr, gt, lt, path, metadata);
  }
  return stat;
}

//------------------------------------------------------------------
bool InterfaceIO::
loadAllFcstExcludeContaining(const time_t gt, const int lt, 
			     const ParmProjection &p,
			     const std::string &url,
			     const bool remap,
			     const std::vector<std::string> &excludes, 
			     MultiFcstGrid &g)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  D.readAllHeaders();
  int nfields = D.getNFieldsFile();
  vector<string> fnames;
  for (int i=0; i<nfields; ++i)
  {
    string s = D.getFieldHeaderFile(i).field_name_long;
    fnames.push_back(s);
  }

  if (fnames.empty())
  {
    ILOGF(ERROR, "No fields found at %s+%d for %s", 
	  ConvWxTime::stime(gt).c_str(), lt, url.c_str());
    return false;
  }

  vector<string>::iterator i;
  for (i=fnames.begin(); i!=fnames.end(); )
  {
    bool remove = false;
    for (size_t e=0; e<excludes.size(); ++e)
    {
      if (i->find(excludes[e]) != string::npos)
      {
	// this string contains one of the excludes
	remove = true;
	break;
      }
    }
    if (remove)
    {
      i = fnames.erase(i);
    }
    else
    {
      i++;
    }
  }
  if (fnames.empty())
  {
    ILOGF(ERROR, "No fields found after removes at %s+%d for %s", 
	  ConvWxTime::stime(gt).c_str(), lt, url.c_str());
    return false;
  }

  return loadMultiFcst(gt, lt, p, url, fnames, remap, g);
}

//------------------------------------------------------------------
bool InterfaceIO::loadAllFcst(const time_t gt, const int lt, 
			      const ParmProjection &p, const string &url,
			      const bool remap,	MultiFcstGrid &g)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  D.readAllHeaders();
  int nfields = D.getNFieldsFile();
  vector<string> fnames;
  for (int i=0; i<nfields; ++i)
  {
    string s = D.getFieldHeaderFile(i).field_name_long;
    fnames.push_back(s);
  }

  if (fnames.empty())
  {
    ILOGF(ERROR, "No fields found at %s+%d for %s", 
	  ConvWxTime::stime(gt).c_str(), lt, url.c_str());
    return false;
  }
  
  return loadMultiFcst(gt, lt, p, url, fnames, remap, g);
}

//------------------------------------------------------------------
void InterfaceIO::write(const time_t &t, const string &url,
			const ParmProjection &p, const MultiGrid &o,
			const MetaData &metadata)
{
  if (!o.dimensionsOk(p.pNx, p.pNy))
  {
    return;
  }

  DsMdvx output;
  Mdvx::field_header_t fieldHdr;
  Mdvx::vlevel_header_t vlevelHdr;

  sOutputInit(t, p, output, metadata, fieldHdr, vlevelHdr);
  for (int i=0; i<static_cast<int>(o.num()); ++i)
  {
    sAddField(output, fieldHdr, vlevelHdr, *o.ithConstGrid(i));
  }
  sAddChunk(output, metadata);
  output.setWriteLdataInfo();
  if (output.writeToDir(url.c_str()))
  {
    ILOG(ERROR, "Unable to write mdv");
  }
}

//------------------------------------------------------------------
void InterfaceIO::write(const time_t &gt, const int lt, const string &url,
			const ParmProjection &p, const MultiGrid &o,
			const MetaData &metadata)
{
  if (!o.dimensionsOk(p.pNx, p.pNy))
  {
    return;
  }

  DsMdvx output;
  Mdvx::field_header_t fieldHdr;
  Mdvx::vlevel_header_t vlevelHdr;

  sOutputInit(gt, lt, p, output, metadata, fieldHdr, vlevelHdr);
  for (int i=0; i<o.num(); ++i)
  {
    sAddField(output, fieldHdr, vlevelHdr, *o.ithConstGrid(i));
  }

  sAddChunk(output, metadata);

  output.setWriteLdataInfo();
  output.setWriteAsForecast();
  if (output.writeToDir(url.c_str()))
  {
    ILOG(ERROR, "Unable to write mdv");
  }
}

//------------------------------------------------------------------
void InterfaceIO::write(const time_t &gt, const int lt, const string &url,
			const ParmProjection &p, const MultiGrid &o,
			const MetaData &metadata,
			ConvWxThreadMgr &threads)
{
  if (!o.dimensionsOk(p.pNx, p.pNy))
  {
    return;
  }

  DsMdvx output;
  Mdvx::field_header_t fieldHdr;
  Mdvx::vlevel_header_t vlevelHdr;

  sOutputInit(gt, lt, p, output, metadata, fieldHdr, vlevelHdr);
  for (int i=0; i<o.num(); ++i)
  {
    sAddField(output, fieldHdr, vlevelHdr, *o.ithConstGrid(i));
  }

  sAddChunk(output, metadata);

  output.setWriteLdataInfo();
  output.setWriteAsForecast();
  threads.lock();
  if (output.writeToDir(url.c_str()))
  {
    ILOG(ERROR, "Unable to write mdv");
  }
  threads.unlock();
}

//------------------------------------------------------------------
void InterfaceIO::clearData(const time_t &t, const string &url)
{
  DsMdvx output;

  sOutputInit(t, output);
  output.setWriteLdataInfo();
  if (output.writeToDir(url.c_str()))
  {
    ILOG(ERROR, "Unable to write mdv");
  }
}

//------------------------------------------------------------------
void InterfaceIO::append(const time_t &t, const string &url,
			 const ParmProjection &p, const MultiGrid &o)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_FIRST_BEFORE, url, 0, t);
  D.readAllHeaders();
  int nfields = D.getNFieldsFile();
  vector<string> fnames;
  for (int i=0; i<nfields; ++i)
  {
    string s = D.getFieldHeaderFile(i).field_name_long;
    fnames.push_back(s);
  }

  if (fnames.empty())
  {
    // do the normal write here
    MetaData md;
    write(t, url, p, o, md);
  }
  else
  {
    MultiGrid mg;
    string path;
    MetaData metadata;
    bool suppressErrorMessages = false;
    if (sLoad(D, url, t, fnames, false, p, suppressErrorMessages,
	      mg, path, metadata))
    {
      // append the input multigrid to the local multigrid and write
      // back out
      for (int i=0; i<o.num(); ++i)
      {
	mg.append(*o.ithConstGrid(i));
      }
      write(t, url, p, mg, metadata);
    }
  }
}

//------------------------------------------------------------------
void InterfaceIO::clearData(const time_t &gt, int lt, const string &url)
{
  DsMdvx output;

  sOutputInit(gt, lt, output);
  output.setWriteLdataInfo();
  output.setWriteAsForecast();
  if (output.writeToDir(url.c_str()))
  {
    ILOG(ERROR, "Unable to write mdv");
  }
}

//------------------------------------------------------------------
void InterfaceIO::append(const time_t &gt, int lt, const string &url,
			 const ParmProjection &p, const MultiGrid &o)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  D.readAllHeaders();
  int nfields = D.getNFieldsFile();
  vector<string> fnames;
  for (int i=0; i<nfields; ++i)
  {
    string s = D.getFieldHeaderFile(i).field_name_long;
    fnames.push_back(s);
  }

  if (fnames.empty())
  {
    // do the normal write here
    MetaData md;
    write(gt, lt, url, p, o, md);
  }
  else
  {
    MultiGrid mg;
    string path;
    MetaData metadata;
    bool suppressErrorMessages = false;
    if (sLoad(D, url, gt, fnames, false, p, suppressErrorMessages,
	      mg, path, metadata))
    {
      // append the input multigrid to the local multigrid and write
      // back out
      for (int i=0; i<o.num(); ++i)
      {
	mg.append(*o.ithConstGrid(i));
      }
      write(gt, lt, url, p, mg, metadata);
    }
  }
}

//------------------------------------------------------------------
void InterfaceIO::append(const time_t &gt, int lt, const string &url,
			 const ParmProjection &p, const MultiGrid &o,
			 const MetaData &md)
{
  DsMdvx D;
  D.setReadTime(Mdvx::READ_SPECIFIED_FORECAST, url, 0, gt, lt);
  D.readAllHeaders();
  int nfields = D.getNFieldsFile();
  vector<string> fnames;
  for (int i=0; i<nfields; ++i)
  {
    string s = D.getFieldHeaderFile(i).field_name_long;
    fnames.push_back(s);
  }

  if (fnames.empty())
  {
    // do the normal write here
    write(gt, lt, url, p, o, md);
  }
  else
  {
    MultiGrid mg;
    string path;
    MetaData metadata;
    bool suppressErrorMessages = false;
    if (sLoad(D, url, gt, fnames, false, p, suppressErrorMessages, mg, path,
	      metadata))
    {
      // append the input multigrid to the local multigrid, merge input metadata
      // with local metadata,  and write back out
      for (int i=0; i<o.num(); ++i)
      {
	mg.append(*o.ithConstGrid(i));
      }
      metadata = metadata.merge(md);
      write(gt, lt, url, p, mg, metadata);
    }
  }
}

//------------------------------------------------------------------
bool InterfaceIO::getArchiveCmdargRange(int argc, char **argv,
					time_t &t0, time_t &t1)
{
  bool archive, error;
  if (DsUrlTrigger::checkArgs(argc, argv, t0, t1, archive, error))
  {
    return archive;
  }
  else
  {
    return false;
  }
}

//------------------------------------------------------------------
void InterfaceIO::writeLatestDataInfo(const std::string &path,
				      const time_t &time)
{
  LdataInfo L;
  L.setDir(path);
  L.write(time);
}

//------------------------------------------------------------------
void InterfaceIO::writeLatestDataInfo(const std::string &dir,
				      const std::string &relPath,
				      const time_t &time)
{
  LdataInfo L;
  L.setDir(dir);
  L.setRelDataPath(relPath);
  L.write(time);
}

//------------------------------------------------------------------
void InterfaceIO::setNoWaitTriggering(int maxWaitSeconds)
{
  sNoWait = true;
  sMaxWaitSeconds = maxWaitSeconds;
  if (sTrigger != NULL)
  {
    ILOG(FATAL, "Setting no wait after creating trigger object");
    exit(-1);
  }
}
