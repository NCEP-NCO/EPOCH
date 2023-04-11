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

#from configparser import ConfigParser
#from configparser import configparser
#from configparser import ConfigParser
import configparser
import os
#import errno
#import time
#from time import gmtime, strftime

#----------------------------------------------------------------------------
def chunker(seq, size):
  return (seq[pos:pos+size] for pos in xrange(0, len(seq), size))

#----------------------------------------------------------------------------
def prependPath(subpath, names):
  sp = subpath + '/'
  return [sp + n for n in names]

#----------------------------------------------------------------------------
class EpochParms:
  def __init__(self):
    self._parmfile = "not set"
    self._ok = False

    # main params
    self._maxLookbackDays = 1
    self._maxModelLookbackDays = 3
    self._maxLookaheadDays = 1
    self._maxEnsembleLeadHours = 0
    self._debugLevel = 0
    self._debugCmds = False
    self._logGrib2ToMdv = False
    
    # globcomp params
    self._globcompAccessMode = "WCOSS"
    self._fileNamingLIR = "not set"
    self._globcompDataPathLIR = "not set"
    self._globcompLirYmdIndex = 0
    self._globcompLirHourIndex = 0

    # gfs params
    self._gfsPattern = 'not set'
    self._gfsPatternb = 'not set'
    self._gfsYmdIndex = 0
    self._gfsbYmdIndex = 0
    self._gfsHourIndex = 0
    self._gfsbHourIndex = 0
    self._gfsLeadIndex = 0
    self._gfsbLeadIndex = 0
    self._gfsModelLeadTimeHours =  []

    # cmorph2 params
    self._cmorph2AccessMode = "WCOSS"
    self._cmorph2Path = "not set"
    self._cmorph2Pattern = "not set"
    self._cmorph2YmdIndex = 0
    self._cmorph2HourIndex = 0
    self._cmorph2MinuteIndex = 0
    self._cmorph2FrequencyMinutes = 0
    self._cmorph2DelayHours = 0
    
    # cmce params
    self._cmceDataPath3 = "not set"

    # gefs params
    self._gefsDataPath3 = "not set"

  def read(self, parmfile):
    self._parmfile = parmfile
    self._ok = True
    parser = configparser.ConfigParser(os.environ,strict=False)
    parser.read(self._parmfile)

    # main params
    self._maxLookbackDays = int(parser.get('main', 'maxLookbackDays'))
    self._maxModelLookbackDays = int(parser.get('main', 'maxModelLookbackDays'))
    self._maxLookaheadDays = int(parser.get('main', 'maxLookaheadDays'))
    self._maxEnsembleLeadHours = int(parser.get('main', 'maxEnsembleLeadHours'))
    self._debugLevel = int(parser.get('main', 'debugLevel'))
    if (self._debugLevel < 0):
      self._debugLevel = 0
    if (self._debugLevel > 2):
      self._debugLevel = 2;
    i = int(parser.get('main', 'debugCmd'))
    if i == 1:
      self._debugCmds = True
    else:
      self._debugCmds = False

    i = int(parser.get('main', 'logGrib2ToMdv'))
    if i == 1:
      self._logGrib2ToMdv = True
    else:
      self._logGrib2ToMdv = False
      
    i = int(parser.get('main', 'do_delete_temp_data_dir'))
    if i == 1:
      self._do_delete_temp_data_dir = True
    else:
      self._do_delete_temp_data_dir = False

    i = int(parser.get('main', 'do_delete_workspace'))
    if i == 1:
      self._do_delete_workspace = True
    else:
      self._do_delete_workspace = False

    i = int(parser.get('main', 'do_copy_into_workspace'))
    if i == 1:
      self._do_copy_into_workspace = True
    else:
      self._do_copy_into_workspace = False

    # globcomp params
    ival = int(parser.get('globcomp', 'globcompAccessModeWcoss'))
    if ival == 1:
      self._globcompAccessMode = "WCOSS"
    else:
      self._globcompAccessMode = "NCAR"
    self._fileNamingLIR = parser.get('globcomp', 'fileNamingLIR')
    self._globcompDataPathLIR = parser.get('globcomp', 'globcompDataPathLIR')
    self._globcompLirYmdIndex = int(parser.get('globcomp', 'globcompLirYmdIndex'))
    self._globcompLirHourIndex = int(parser.get('globcomp', 'globcompLirHourIndex'))
    
    # gfs params
    self._gfsPattern = parser.get('gfs', 'gfsPattern')
    self._gfsPatternb = parser.get('gfs', 'gfsPatternb')
    self._gfsYmdIndex = int(parser.get('gfs', 'gfsYmdIndex'))
    self._gfsbYmdIndex = int(parser.get('gfs', 'gfsbYmdIndex'))
    self._gfsHourIndex = int(parser.get('gfs', 'gfsHourIndex'))
    self._gfsbHourIndex = int(parser.get('gfs', 'gfsbHourIndex'))
    self._gfsLeadIndex = int(parser.get('gfs', 'gfsLeadIndex'))
    self._gfsbLeadIndex = int(parser.get('gfs', 'gfsbLeadIndex'))
    self._gfsModelLeadTimeHours =  [name for name in parser.get('gfs', 'gfsModelLeaDtimeHours').split()]

    #cmorph2 params
    ival = int(parser.get('cmorph2', 'cmorph2AccessModeWcoss'))
    if ival == 1:
      self._cmorph2AccessMode = "WCOSS"
    else:
      self._cmorph2AccessMode = "NCAR"
      
    self._cmorph2Path = parser.get('cmorph2', 'cmorph2Path')
    self._cmorph2Pattern = parser.get('cmorph2', 'cmorph2Pattern')
    self._cmorph2YmdIndex = int(parser.get('cmorph2', 'cmorph2YmdIndex'))
    self._cmorph2HourIndex = int(parser.get('cmorph2', 'cmorph2HourIndex'))
    self._cmorph2MinuteIndex = int(parser.get('cmorph2', 'cmorph2MinuteIndex'))
    self._cmorph2FrequencyMinutes = int(parser.get('cmorph2', 'cmorph2FrequencyMinutes'))
    self._cmorph2MaxLatencyHours = int(parser.get('cmorph2', 'cmorph2MaxLatencyHours'))
    self._cmorph2DelayHours = int(parser.get('cmorph2', 'cmorph2DelayHours'))
    if self._cmorph2MaxLatencyHours > self._maxLookbackDays*24:
      print("Upping max lookback days to agree with cmorph max latency of ", self._cmorph2MaxLatencyHours)
      self._maxLookbackDays = self._cmorph2MaxLatencyHours/24 + 1
      print("New max lookback = ", self._maxLookbackDays)
    if self._cmorph2DelayHours > self._maxLookbackDays*24:
      print("Upping max lookback days to agree with cmorph delay of ", self._cmorph2DelayHours)
      self._maxLookbackDays = self._cmorph2DelayHours/24 + 1
      print("New max lookback = ", self._maxLookbackDays)

    # cmce params
    self._cmceDataPath3 = parser.get('cmce', 'cmceDataPath3')
    
    # gefs params
    self._gefsDataPath3 = parser.get('gefs', 'gefsDataPath3')
    
  def printParms(self):
    print('self._maxLookbackDays',  self._maxLookbackDays)
    print('self._maxModelLookbackDays',  self._maxModelLookbackDays)
    print('self._maxLookaheadDays', self._maxLookaheadDays)
    print('self._maxEnsembleLeadHours', self._maxEnsembleLeadHours)
    print('self._debugLevel', self._debugLevel)
    print('self._debugCmds', self._debugCmds)
    print('self._logGrib2ToMdv', self._logGrib2ToMdv)

    # globcomp params
    print('self._globcompAccessMode', self._globcompAccessMode)
    print('self._fileNamingLIR', self._fileNamingLIR)
    print('self._globcompDataPathLIR', self._globcompDataPathLIR)
    print('self._globcompLirYmdIndex', self._globcompLirYmdIndex)
    print('self._globcompLirHourIndex', self._globcompLirHourIndex)

    # gfs params
    print('self._gfsPattern', self._gfsPattern)
    print('self._gfsPatternb', self._gfsPatternb)
    print('self._gfsYmdIndex', self._gfsYmdIndex)
    print('self._gfsbYmdIndex', self._gfsbYmdIndex)
    print('self._gfsHourIndex', self._gfsHourIndex)
    print('self._gfsbHourIndex', self._gfsbHourIndex)
    print('self._gfsLeadIndex', self._gfsLeadIndex)
    print('self._gfsbLeadIndex', self._gfsbLeadIndex)
    print('self._gfsModelLeadTimeHours', self._gfsModelLeadTimeHours)

    # cmorph2 params
    print('self._cmorph2AccessMode', self._cmorph2AccessMode)
    print('self._cmorph2Path', self._cmorph2Path)
    print('self._cmorph2Pattern', self._cmorph2Pattern)
    print('self._cmorph2YmdIndex', self._cmorph2YmdIndex)
    print('self._cmorph2HourIndex', self._cmorph2HourIndex)
    print('self._cmorph2MinuteIndex', self._cmorph2MinuteIndex)
    print('self._cmorph2FrequencyMinutes', self._cmorph2FrequencyMinutes)
    print('self._cmorph2DelayHours', self._cmorph2DelayHours)
    
    # cmce params
    print('self._cmceDataPath3', self._cmceDataPath3)

    # gefs params
    print('self._gefsDataPath3', self._gefsDataPath3)
