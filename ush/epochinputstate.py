#!/usr/bin/env python3
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2017 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2017/10/12 15:20:37 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import configparser
import os
#import errno
#import time
#from time import gmtime, strftime
import datetime

def chunker(seq, size):
  return (seq[pos:pos+size] for pos in xrange(0, len(seq), size))

#----------------------------------------------------------------------------
def getWithDefault(config, section, name, default):
  if config.has_option(section,name):
    return config.get(section,name)
  else:
    return default

#----------------------------------------------------------------------------
class EpochInputState:
  def __init__(self):
    self._statefile = "not set"
    self._ok = False
    self._LIR = []
    self._GFS = []
    self._CMORPH = []
    self._RAW_CMORPH = []
    
  def readOrCreate(self, statefile):
    self._statefile = statefile
    self._ok = True
    if os.path.exists(statefile):
      parser = configparser.ConfigParser(os.environ,strict=False)
      parser.read(self._statefile)

      l = getWithDefault(parser, 'inputs', 'GFS', [])
      #l = parser.get('inputs', 'GFS')
      if l:
        self._GFS = l.split()
      else:
        self._GFS = []

      l = getWithDefault(parser, 'inputs', 'CMORPH', [])
      #l = parser.get('inputs', 'CMORPH')
      if l:
        self._CMORPH = l.split()
      else:
        self._CMORPH = []

      l = getWithDefault(parser, 'inputs', 'RAW_CMORPH', [])
      #l = parser.get('inputs', 'CMORPH')
      if l:
        self._RAW_CMORPH = l.split()
      else:
        self._RAW_CMORPH = []

      l = getWithDefault(parser, 'inputs', 'LIR', [])
      #l = parser.get('inputs', 'LIR')
      if l:
        self._LIR = l.split()
      else:
        self._LIR = []
      self._GFS.sort()
      self._LIR.sort()
      self._CMORPH.sort()
      self._RAW_CMORPH.sort()
    else:
      print("----State file does not exist, initialize to empty state----" + self._statefile)
      self._LIR = []
      self._GFS = []
      self._CMORPH = []
      self._RAW_CMORPH = []
    
  def updateGFS(self, ymdh):
    if ymdh not in str(self._GFS):
      self._GFS.append(ymdh)
      self._GFS.sort()

  def updateCMORPH(self, ymdhm):
    if ymdhm not in str(self._CMORPH):
      self._CMORPH.append(ymdhm)
      self._CMORPH.sort()

  def updateRawCMORPH(self, ymdhm):
    if ymdhm not in str(self._RAW_CMORPH):
      self._RAW_CMORPH.append(ymdhm)
      self._RAW_CMORPH.sort()

  def updateLIR(self, ymdh):
    if ymdh not in str(self._LIR):
      self._LIR.append(ymdh)
      self._LIR.sort()

  def write(self, statefile, debug=False):
    f = open(statefile, "w")
    f.write("[inputs]\n")
    f.write("GFS=")
    for name in self._GFS:
      f.write(" " + name +"\n")
    f.write("\nLIR=")
    for name in self._LIR:
      f.write(" " + name +"\n")
    f.write("\nCMORPH=")
    for name in self._CMORPH:
      f.write(" " + name +"\n")
    f.write("\nRAW_CMORPH=")
    for name in self._RAW_CMORPH:
      f.write(" " + name +"\n")
    f.close()
    if debug:
      print("Wrote to ", statefile)
      
  def hasGfs(self, ymdh):
    return ymdh in str(self._GFS)
  def hasLir(self, ymdh):
    return ymdh in str(self._LIR)
  def hasCmorph(self, ymdhm):
    return ymdhm in str(self._CMORPH)
  def hasRawCmorph(self, ymdhm):
    return ymdhm in str(self._RAW_CMORPH)
    
  def removeTooOld(self, tmin, debug):
    cmorph = []
    for ymdhm in self._CMORPH:
      t = datetime.datetime.strptime(ymdhm + '00', '%Y%m%d%H%M%S')
      if (t >= tmin):
        cmorph.append(ymdhm)
      else:
        if debug:
          print("Aging off cmorph from input state file ", ymdhm)
    self._CMORPH = cmorph
    raw_cmorph = []
    for ymdhm in self._RAW_CMORPH:
      t = datetime.datetime.strptime(ymdhm + '00', '%Y%m%d%H%M%S')
      if (t >= tmin):
        raw_cmorph.append(ymdhm)
      else:
        if debug:
          print("Aging off raw cmorph from input state file ", ymdhm)
    self._RAW_CMORPH = raw_cmorph
    lir = []
    for ymdh in self._LIR:
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if (t >= tmin):
        lir.append(ymdh)
      else:
        if debug:
          print("Aging off LIR from input state file ", ymdh)
    self._LIR = lir
    gfs = []
    for ymdh in self._GFS:
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if (t >= tmin):
        gfs.append(ymdh)
      else:
        if debug:
          print("Aging off GFS from input state file ", ymdh)
    self._GFS = gfs
