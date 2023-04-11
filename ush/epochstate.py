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
import errno
#import time
import datetime
#from time import gmtime, strftime

#----------------------------------------------------------------------------
def getWithDefault(config, section, name, default):
  if config.has_option(section,name):
    return config.get(section,name)
  else:
    return default

#----------------------------------------------------------------------------
class EpochState:
  def __init__(self):
    self._statefile = "not set"
    self._ok = False
    self._gefs = []
    self._cmce = []
    self._threshGefs = []
    self._threshCmce = []

    self._currentPartial = ""
    self._inProgress = ""
    self._lastCompleted = ""

    self._gfsPartial = ""
    self._gfsLastDone = ""
    self._gfsaLastGrib2toMdv = ""
    self._gfsbLastGrib2toMdv = ""

    self._lirPartial = ""
    self._lirLastDone = ""

    self._cmcePartial = ""
    self._cmceLastDone = ""

    self._gefsPartial = ""
    self._gefsLastDone = ""

    
  def readOrCreate(self, statefile):
    self._statefile = statefile
    self._ok = True
    if os.path.exists(statefile):
      parser = configparser.ConfigParser(os.environ,strict=False)
      parser.read(self._statefile)
      lst = getWithDefault(parser, 'proj', 'GEFS', [])
      self._gefs = [name for name in lst.split()]
      lst = getWithDefault(parser, 'proj', 'CMCE', [])
      self._cmce = [name for name in lst.split()]
      lst = getWithDefault(parser, 'proj', 'THRESH_CMCE', [])
      self._threshCmce=  [name for name in lst.split()]
      lst = getWithDefault(parser, 'proj', 'THRESH_GEFS', [])
      self._threshGefs =  [name for name in lst.split()]

      self._currentPartial = getWithDefault(parser, 'proj', 'CurrentPartial', '')
      self._inProgress = getWithDefault(parser, 'proj', 'InProgress', '')
      self._lastCompleted = getWithDefault(parser, 'proj', 'LastCompleted', '')

      self._gfsPartial = getWithDefault(parser, 'proj', 'GFSPartial', '')
      self._gfsLastDone = getWithDefault(parser, 'proj', 'GFSLastDone', '')
      self._gfsaLastGrib2toMdv = getWithDefault(parser, 'proj', 'GFSALastGrib2toMdv', '');
      self._gfsbLastGrib2toMdv = getWithDefault(parser, 'proj', 'GFSBLastGrib2toMdv', '');

      self._lirPartial = getWithDefault(parser, 'proj', 'LIRPartial', '')
      self._lirLastDone = getWithDefault(parser, 'proj', 'LIRLastDone', '')

      self._cmcePartial = getWithDefault(parser, 'proj', 'CMCEPartial', '')
      self._cmceLastDone = getWithDefault(parser, 'proj', 'CMCELastDone', '')

      self._gefsPartial = getWithDefault(parser, 'proj', 'GEFSPartial', '')
      self._gefsLastDone = getWithDefault(parser, 'proj', 'GEFSLastDone', '')
      return True
    else:
      print("----Epoch State file does not exist, initialize to empty state----")
      self._gefs = []
      self._cmce = []
      self._threshCmce = []
      self._threshGefs = []

      self._currentPartial = ""
      self._inProgress = ""
      self._lastCompleted = ""

      self._gfsPartial = ""
      self._gfsLastDone = ""
      self._gfsaLastGrib2toMdv = ""
      self._gfsbLastGrib2toMdv = ""

      self._lirPartial = ""
      self._lirLastDone = ""

      self._cmcePartial = ""
      self._cmceLastDone = ""

      self._gefsPartial = ""
      self._gefsLastDone = ""
      return True
    
  def addCMCE(self, name, debug):
    if name not in str(self._cmce):
      if debug:
        print("ADDING CMCE for ", name)
      self._cmce.append(name)

  def addGEFS(self, name, debug):
    if name not in str(self._gefs):
      if debug:
        print("ADDING GEFS for ", name)
      self._gefs.append(name)

  def addThreshGEFS(self, name):
    if name not in str(self._threshGefs):
      self._threshGefs.append(name)

  def addThreshCMCE(self, name):
    if name not in str(self._threshCmce):
      self._threshCmce.append(name)

  def hasCmce(self, name):
    return name in self._cmce
  
  def hasGefs(self, name):
    return name in self._gefs
  
  def hasThreshCMCE(self, name):
    return name in self._threshCmce

  def hasThreshGEFS(self, name):
    return name in self._threshGefs

  def removeCMCE(self, names, debug):
    for name in names:
      if debug:
        print("Remove from model state file CMCE:" + name)
      try:
        self._cmce.remove(name)
      except:
        print("Warning: Not found in model state CMCE, cannot remove:" + name)

  def removeGEFS(self, names, debug):
    for name in names:
      if debug:
        print("Remove from model state file GEFS:" + name)
      try:
        self._gefs.remove(name)
      except:
        print("Warning: Not found in model state GEFS, cannot remove:" + name)

  def start(self, ymdh):
    self._currentPartial = ymdh
    self._lastCompleted = ""
    self._inProgress = ""

  def checkForComplete(self, ymdh):
    self._currentPartial = ymdh
    if self._lastCompleted != "COMBINE":
      print("WARNING expected completion status of COMBINE but got: ", self._lastCompleted)

  def hasCompletedCmorph2(self, ymdh):
    if self._currentPartial != ymdh:
      return False
    # this is the processing order
    good = ["CMORPH2", "GFS", "LIR", "CMCE", "GEFS", "COMBINE"]
    return self._lastCompleted in good

  def setCmorph2Completed(self, ymdh):
    self._lastCompleted = "CMORPH2"
    self._inProgress = ""

  def hasCompletedGfs(self, ymdh):
    if self._currentPartial != ymdh:
      return False
    # this is the processing order
    good = ["GFS", "LIR", "CMCE", "GEFS", "COMBINE"]
    return self._lastCompleted in good

  def setGfsCompleted(self, ymdh):
    self._lastCompleted = "GFS"
    self._inProgress = ""


  def hasCompletedLir(self, ymdh):
    if self._currentPartial != ymdh:
      return False
    # this is the processing order
    good = ["LIR", "CMCE", "GEFS", "COMBINE"]
    return self._lastCompleted in good

  def setLirCompleted(self, ymdh):
    self._lastCompleted = "LIR"
    self._inProgress = ""

  def hasCompletedCmce(self, ymdh):
    if self._currentPartial != ymdh:
      return False
    # this is the processing order
    good = ["CMCE", "GEFS", "COMBINE"]
    return self._lastCompleted in good

  def setCmceCompleted(self, ymdh):
    self._lastCompleted = "CMCE"
    self._inProgress = ""

  def hasCompletedGefs(self, ymdh):
    if self._currentPartial != ymdh:
      return False
    # this is the processing order
    good = ["GEFS", "COMBINE"]
    return self._lastCompleted in good

  def setGefsCompleted(self, ymdh):
    self._lastCompleted = "GEFS"
    self._inProgress = ""

  def hasCompletedCombine(self, ymdh):
    if self._currentPartial != ymdh:
      return False
    # this is the processing order
    good = ["COMBINE"]
    return self._lastCompleted in good

  def setCombineCompleted(self, ymdh):
    self._lastCompleted = "COMBINE"
    self._inProgress = ""

  def setCmorph2InProgress(self, ymdh):
    self._inProgress = "CMORPH2"

  def setGfsInProgress(self, ymdh):
    self._inProgress = "GFS"

  def setLirInProgress(self, ymdh):
    self._inProgress = "LIR"

  def setCmceInProgress(self, ymdh):
    self._inProgress = "CMCE"
    
  def setGefsInProgress(self, ymdh):
    self._inProgress = "GEFS"
    
  def setCombineInProgress(self, ymdh):
    self._inProgress = "COMBINE"
    
  def clearPartial(self):
    self._currentPartial = ""
    self._lastCompleted = ""
    self._inProgress = ""

    self._gfsPartial = ""
    self._gfsLastDone = ""
    self._gfsaLastGrib2toMdv = ""
    self._gfsbLastGrib2toMdv = ""

    self._lirPartial = ""
    self._lirLastDone = ""

    self._cmcePartial = ""
    self._cmceLastDone = ""

    self._gefsPartial = ""
    self._gefsLastDone = ""

  def write(self, filename, debug=False):
    head, tail = os.path.split(filename)
    if not os.path.exists(head):
      print("Path for model state file nonexistant, try to create ", head)
      try:
        os.makedirs(head)
      except OSError as exception:
        if exception.errno != errno.EEXIST:
          print("ERROR creating path when writing model state file ", head, " cannot write")
          return False

    self._statefile = filename
    fp = open(filename, "w")
    fp.write('[proj]\n')
    fp.write('GEFS=')
    for name in self._gefs:
      fp.write(name + "\n     ")
    fp.write('\nCMCE=')
    for name in self._cmce:
      fp.write(name + "\n     ")
    fp.write('\nTHRESH_GEFS=')
    for name in self._threshGefs:
      fp.write(name + "\n     ")
    fp.write('\nTHRESH_CMCE=')
    for name in self._threshCmce:
      fp.write(name + "\n     ")

    fp.write('\nCurrentPartial=' + self._currentPartial)
    fp.write('\nInProgress=' + self._inProgress)
    fp.write('\nLastCompleted=' + self._lastCompleted)

    fp.write('\nGFSpartial=' +  self._gfsPartial)
    fp.write('\nGFSLastDone=' + self._gfsLastDone)
    fp.write('\nGFSALastGrib2toMdv=' + self._gfsaLastGrib2toMdv)
    fp.write('\nGFSBLastGrib2toMdv=' + self._gfsbLastGrib2toMdv)

    fp.write('\nLIRpartial=' +  self._lirPartial)
    fp.write('\nLIRLastDone=' + self._lirLastDone)

    fp.write('\nCMCEpartial=' +  self._cmcePartial)
    fp.write('\nCMCELastDone=' + self._cmceLastDone)

    fp.write('\nGEFSpartial=' + self._gefsPartial)
    fp.write('\nGEFSLastDone=' + self._gefsLastDone)
    fp.close()
    if debug:
      print("Wrote state to file ", self._statefile)
    return True

  def removeTooOld(self, tmin, tminThresh, debug):
    cmce = []
    for ymdh in self._cmce:
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if (t >= tmin):
        cmce.append(ymdh)
      else:
        if debug:
          print("Aging off CMCE from Epoch main state file ", ymdh)
    gefs = []
    for ymdh in self._gefs:
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if (t >= tmin):
        gefs.append(ymdh)
      else:
        if debug:
          print("Aging off GEFS from Epoch main state file ", ymdh)
    self._gefs = gefs

    threshCmce = []
    for ymdh in self._threshCmce:
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if (t >= tminThresh):
        threshCmce.append(ymdh)
      else:
        if debug:
          print("Aging off threshCMCE from Epoch main state file ", ymdh)
    self._threshCmce = threshCmce

    threshGefs = []
    for ymdh in self._threshGefs:
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if (t >= tminThresh):
        threshGefs.append(ymdh)
      else:
        if debug:
          print("Aging off threshGEFS from Epoch main state file ", ymdh)
    self._threshGefs = threshGefs

