#!/usr/bin/env -S python3 -u
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# Copyright, University Corporation for Atmospheric Research (UCAR) 2009-2017. 
# The Government's right to use this data and/or software is restricted per 
# the terms of Cooperative Agreement between UCAR and the National  Science 
# Foundation, to government use only which includes the nonexclusive, 
# nontransferable, irrevocable, royalty-free license to exercise or have 
# exercised for or on behalf of the U.S. Government throughout the world. 
# All other rights are reserved. 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
""" epoch.py
"""

import os
from os import path
import sys
import shutil
import glob
import tarfile
import argparse
import datetime
import errno
import shlex
from os import environ
import subprocess
from subprocess import Popen, PIPE
import epochparms
import epochstate
import epochinputstate

#----------------------------------------------------------------------------
def previousHour(ymdh):
  t = datetime.datetime.strptime(ymdh + "0000", "%Y%m%d%H%M%S")
  m = t - datetime.timedelta(hours=1)
  return m.strftime("%Y%m%d%H")

#----------------------------------------------------------------------------
def previousYyyymmddSrings(ymdh, maxDaysBack):
  yyyymmdd = ymdh[0:8]
  t = datetime.datetime.strptime(yyyymmdd + "000000", "%Y%m%d%H%M%S")
  tm = []
  for d in range(1,maxDaysBack):
    m = t - datetime.timedelta(days=d)
    tm.append(m.strftime("%Y%m%d"))
  return tm

#----------------------------------------------------------------------------
def parseYmdhm(name, ymdIndex, hourIndex, minuteIndex):
  ymd = name[ymdIndex:ymdIndex+8]
  hour = name[hourIndex:hourIndex+2]
  minute = name[minuteIndex:minuteIndex+2]
  return ymd + hour + minute

#----------------------------------------------------------------------------
def parseYmdh(name, ymdIndex, hourIndex):
  ymd = name[ymdIndex:ymdIndex+8]
  hour = name[ hourIndex:hourIndex+2]
  return ymd + hour

#----------------------------------------------------------------------------
def isYmd(name):
  if (len(name) != 8):
    return False
  try:
    date = datetime.datetime.strptime(name, '%Y%m%d')
  except ValueError:
    return False
  return True
  
#----------------------------------------------------------------------------
def isSpdbData(name):
  if (len(name) != 13):
    return False
  try:
    date = datetime.datetime.strptime(name[0:8], '%Y%m%d')
  except ValueError:
    return False
  if name[8:13] == '.data':
    return True
  if name[8:13] == '.indx':
    return True
  return False
  
#----------------------------------------------------------------------------
def parseLead(name, leadIndex):
  return name[leadIndex:leadIndex+3]

#----------------------------------------------------------------------------
def gfsPathToDataFromPattern(pattern, yyyymmdd, hh):
  # find last '/' in path and everything before that is the path
  ind = pattern.rindex('/')
  path = pattern[0:ind]

  # find YYYYMMDD in pattern and replace with yyyymmdd
  path = path.replace('YYYYMMDD', yyyymmdd)

  # find HH in pattern and replace with hh
  path = path.replace('HH', hh)

  return path

#---------------------------------------------------------------------------
def setupDoubleEnv(envName, envNameActual, comoutPath, workspace, my_env, debug):
  name = my_env[envName]
  if debug:
    print("original env var for ", envName, " = ", name)
  my_env[envNameActual] = name
  if debug:
    print("Set env var for ", envNameActual, " to ", name)
  i = name.find(comoutPath)
  if i != 0:
    print("ERROR ", envName, " = ", name,  " did not have ", comoutPath)
    return False
  w = name.replace(comoutPath, workspace)
  my_env[envName] = w
  if debug:
    print("Set env var for ", envName, " to ", w)
      
#----------------------------------------------------------------------------
def cmorph2PathToDataFromPattern(pattern, yyyymmdd, hh, mm, mm2):
  path = pattern
  path = path.replace('YYYYMMDD', yyyymmdd)
  path = path.replace('HH', hh)
  path = path.replace('MM', mm)
  path = path.replace('mm', mm2)
  return path

#----------------------------------------------------------------------------
def makeDirIfNeeded(path, debug=False):
  if (debug):
    print("Making directory if needed " + path)

  try:
    os.makedirs(path)
    return True
  except OSError as exception:
    if exception.errno != errno.EEXIST:
      print("ERROR creating", path)
      return False
    else:
      return True

#----------------------------------------------------------------------------
def makeOrScrub(path, debug=False):
  if (debug):
    print("Recreating path " + path)
  if (os.path.exists(path)):
    try:
      shutil.rmtree(path)
      os.makedirs(path)
    except:
      print('WARNING: ' + path + ' not completely cleaned out.')
  else:
    os.makedirs(path)
   
#----------------------------------------------------------------------------
def makePath(path, debug=False):
  if (debug):
    print("Recreating path " + path)
  if (not os.path.exists(path)):
      os.makedirs(path)
   
#----------------------------------------------------------------------------
def doDeleteDataDir(path, debug=False):
  if (debug):
    print("Deleting the temporary data path " + path)
  if (os.path.exists(path)):
    try:
      shutil.rmtree(path)
    except:
      print('WARNING: ' + path + ' not completely cleaned out.')
    
#----------------------------------------------------------------------------
def removeFileIfThere(file, debug=False):
  if (os.path.exists(file)):
    if (debug):
      print("Removing file " + file)
    os.remove(file)

#----------------------------------------------------------------------------
def getFileNames(aDir):
  if os.path.exists(aDir):
    return [name for name in os.listdir(aDir) \
            if not os.path.isdir(os.path.join(aDir, name))]
  else:
    return []

#----------------------------------------------------------------------------
def getImmediateSubdirectories(aDir):
  if (not os.path.exists(aDir)):
    return []
   
  return [name for name in os.listdir(aDir)
           if os.path.isdir(os.path.join(aDir, name))]

#----------------------------------------------------------------------------
def pruneEpochState(ymdh, eparms, my_env):

  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)

  t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
  dt = eparms._maxModelLookbackDays
  toldest = t - datetime.timedelta(days=dt)

  # make sure latency in valid times is handled properly, with a buffer
  daysForThresh = int(eparms._maxEnsembleLeadHours/24) + 1
  if daysForThresh < dt:
    daysForThresh = dt
  threshOldest = t - datetime.timedelta(days=daysForThresh)

  estate.removeTooOld(toldest, threshOldest, eparms._debugLevel >=1)
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'])

#----------------------------------------------------------------------------
def recursiveCopy(fromPath, toPath):
  fp = tarfile.open(fromPath + '/tmp.tar', 'w')
  cwd = os.getcwd()
  os.chdir(fromPath)
  for f in glob.glob("*"):
    fp.add(f)
  fp.close()
  shutil.move(fromPath + "/tmp.tar", toPath + "/tmp.tar")
  os.chdir(toPath)
  fp = tarfile.open('./tmp.tar', 'r')
  fp.extractall()
  fp.close()
  os.remove('./tmp.tar')
  os.chdir(cwd)

#----------------------------------------------------------------------------
def recursiveCopyMdvFcst(topFromPath, topToPath, subpath, yyyymmdd, hh):
  print("Recursively copying from ", topFromPath, " to " , topToPath)
  fp = tarfile.open(topFromPath + '/tmp.tar', 'w')
  cwd = os.getcwd()
  os.chdir(topFromPath)
  subp = subpath + "/" + yyyymmdd + "/g_" + hh + "0000"
  print("Recursively copying: subpath = ", subp)
  fp.add(subp)
  fp.close()
  shutil.move(topFromPath + "/tmp.tar", topToPath + "/tmp.tar")
  os.chdir(topToPath)
  fp = tarfile.open('./tmp.tar', 'r')
  fp.extractall()
  fp.close()
  os.remove('./tmp.tar')
  os.chdir(cwd)

#----------------------------------------------------------------------------
def recursiveCopyMdvEnsFcst(model, topFromPath, topToPath, subpath, yyyymmdd, hh):
  print("Recursively copying from ", topFromPath, " to " , topToPath)
  n = 20
  if model == "CMCE":
    n = 20
  elif model == "GEFS":
    n = 30
  else:
    print("ERROR model not known ", model)
    return False
    
  ens = []
  for i in range(1,n+1):
    ens.append('gep%02d' % i)
    
  fp = tarfile.open(topFromPath + '/tmp.tar', 'w')
  cwd = os.getcwd()
  os.chdir(topFromPath)
  for e in ens:
    subp = subpath + "/" + e + "/" + yyyymmdd + "/g_" + hh + "0000"
    print("Recursively copying: subpath = ", subp)
    fp.add(subp)
  fp.close()
  shutil.move(topFromPath + "/tmp.tar", topToPath + "/tmp.tar")
  os.chdir(topToPath)
  fp = tarfile.open('./tmp.tar', 'r')
  fp.extractall()
  fp.close()
  os.remove('./tmp.tar')
  os.chdir(cwd)

#----------------------------------------------------------------------------
def copyMdvFcst2(topInput, topOutput, subpath, yyyymmdd, hh, leadhour_s, eparms, my_env):
  # copy a single file
  fullsubpath = subpath + "/" + yyyymmdd + "/g_" + hh + "0000"
  lt = int(leadhour_s)
  lt = lt * 3600
  filename = 'f_%08d.mdv' % (lt)
  fullname = topInput + "/" + fullsubpath + "/" + filename
  if not os.path.exists(fullname):
    print("ERROR expected a file to exist, not found:", fullname)
    return False

  outPath = topOutput + "/" + fullsubpath
  makePath(outPath, True)
  print("Copy from ", fullname, " to ", outPath)
  shutil.copy(fullname, outPath)
  
#----------------------------------------------------------------------------
def copyMdvFcst(topInput, topOutput, fullPath, duplicatorInstance, ymdh, yyyymmdd, hh, eparms, my_env):
  os.system('date')
  subp = fullPath[len(topInput)+1:]
  recursiveCopyMdvFcst(topInput, topOutput, subp, yyyymmdd, hh)

#----------------------------------------------------------------------------
def copySpdb(input, output, ymd, eparms, my_env):
  inputFile = input + "/" + ymd + ".data"
  print("Copying ", inputFile, " to " , output)
  if not os.path.exists(output):
    os.makedirs(output)
  if os.path.exists(inputFile):
    shutil.copy(inputFile, output)
  else:
    print("File does not exist ", inputFile)
  inputFile = input + "/" + ymd + ".indx"
  print("Copying ", inputFile, " to " , output)
  if os.path.exists(inputFile):
    shutil.copy(inputFile, output)
  else:
    print("File does not exist ", inputFile)

#----------------------------------------------------------------------------
def copyTree(fromPath, toPath, debug=False):
   """ Copy entire tree
   Inputs
   ------
   fromPath  str  source
   toPath    str  destination
   Returns
   -------
   """
   if debug:
       print("Copy tree from ", fromPath)
       print("Copy tree to   ", toPath)
   shutil.copytree(fromPath, toPath)


#----------------------------------------------------------------------------
def doCmdEns(cmd, app, instance, fileName, ensemNum, ensemName, eparms, my_env):
   """ Execute a command, write output and errors out, and return a status
   Parameters
   ----------
   -------
   int status of command
   """
   os.system('date')
   if eparms._debugLevel >= 1 or eparms._debugCmds:
      print('command = ', cmd)
   if eparms._debugLevel >= 2:
      print('app= ', app)
      print('instance = ', instance)
      print('file = ', fileName)
      print("ensemble name ", ensemName)
      print("ensemble num ", ensemNum)

   # set environment variables based on input environment to match those
   # found in param files
   my_env['ENSEMBLE_NUMBER'] = ensemNum
   my_env['ENSEMBLE_MEMBER'] = ensemName

   my_env['RAP_DATA_DIR'] = my_env['DATA']#eparms._temp_data_dir
   my_env['LOG_DIR'] = my_env['LOGepoch']#eparms._logDir
   my_env['MERGED_GFS'] = my_env['COMOUTmerged']#eparms._gfsMergedData
   my_env['GFS_0.25A'] = my_env['COMOUTgfs0p25a']#eparms._gfsA
   my_env['OBAR_CTH'] =  my_env['COMOUTsatellite']#eparms._obarCth
   my_env['OBAR_CMORPH'] =  my_env['COMOUTcmorph']#eparms._obarCmorph
   my_env['PBAR_CMCE'] = my_env['COMOUTpbarCmce']#eparms._pbarCMCE
   my_env['PBAR_GEFS'] = my_env['COMOUTpbarGefs']#eparms._pbarGEFS
   my_env['THRESH_APCP_CMCE'] = my_env['COMOUTthreshApcpCmce']#eparms._threshApcpCMCE
   my_env['THRESH_APCP_GEFS'] = my_env['COMOUTthreshApcpGefs']#eparms._threshApcpGEFS
   my_env['THRESH_HIST_APCP_CMCE'] = my_env['COMOUTthreshHistApcpCmce']#eparms._threshHistApcpCMCE
   my_env['THRESH_HIST_APCP_GEFS'] = my_env['COMOUTthreshHistApcpGefs']#eparms._threshHistApcpGEFS
   my_env['THRESH_ULWRF_CMCE'] = my_env['COMOUTthreshUlwrfCmce']#eparms._threshUlwrfCMCE
   my_env['THRESH_ULWRF_GEFS'] = my_env['COMOUTthreshUlwrfGefs']#eparms._threshUlwrfGEFS
   my_env['THRESH_HIST_ULWRF_CMCE'] = my_env['COMOUTthreshHistUlwrfCmce']#eparms._threshHistUlwrfCMCE   
   my_env['THRESH_HIST_ULWRF_GEFS'] = my_env['COMOUTthreshHistUlwrfGefs']#eparms._threshHistUlwrfGEFS
   my_env['EPOCH_OPT_CCT30_GRIB'] = my_env['initial_CCT30_output_dir']#eparms._grib_cct30
   my_env['EPOCH_OPT_CCT35_GRIB'] = my_env['initial_CCT35_output_dir']#eparms._grib_cct35
   my_env['EPOCH_OPT_CCT40_GRIB'] = my_env['initial_CCT40_output_dir']#eparms._grib_cct40
   my_env['EPOCH_OPT_GRIB'] = my_env['initial_Cal_output_dir']#eparms._grib_opt
   my_env['EPOCH_OPT_MDV'] = my_env['mdv_opt']
   my_env['EPOCH_OPT_CLOUDTOPS_MDV'] = my_env['mdv_cloudtop_opt']
   my_env['EPOCH_CMCE_PROB_OPT'] = my_env['COMOUTcmceProbOpt']
   my_env['EPOCH_GEFS_PROB_OPT'] = my_env['COMOUTgefsProbOpt']
   my_env['EPOCH_CMCE_PROB_CLOUDTOP_OPT'] = my_env['COMOUTcmceProbCloudTopOpt']
   my_env['EPOCH_GEFS_PROB_CLOUDTOP_OPT'] = my_env['COMOUTgefsProbCloudTopOpt']

   os.chdir(my_env['SCRIPTSepoch'])#eparms._scripts_dir)
   ppath = os.getcwd()
   os.chdir(my_env['PARMepoch'])#eparms._parms_dir)
   
   args = shlex.split(cmd)
   os.putenv("pgm", app)
   proc = Popen(args, stdout=PIPE, stderr=PIPE, env=my_env)
   #proc = Popen(args, env=my_env)
   out, err = proc.communicate()
   os.putenv("err",str(proc.returncode))
   exitcode = proc.returncode
   if not eparms._logGrib2ToMdv:
     os.chdir(ppath)
     if exitcode:
       print(str(err, 'utf-8'))
     os.system('err_chk') # wait until after errfile is printed
     return exitcode

   logDir = my_env['LOG_DIR']
   outPath = logDir + "/" + my_env['PDY']
   makeDirIfNeeded(outPath)
   outfile = outPath + "/" + app + "." + instance + "." + fileName + ".out"
   errfile = outPath + "/" + app + "." + instance + "." + fileName + ".err"
   f = open(outfile, 'w')
   f.write(str(out, 'utf-8'))
   f.close()
   f = open(errfile, 'w')
   f.write(str(err, 'utf-8'))
   f.close()
   os.chdir(ppath)
   if exitcode:
     print(str(err, 'utf-8'))
   os.system('err_chk') # wait until after errfile is printed


   return exitcode

#----------------------------------------------------------------------------
def doCmd(cmd, app, suffix, timeInfo, eparms, my_env):
   """ Execute a command, write output and errors out, and return a status
   Parameters
   ----------
   -------
   int status of command
   """
   if eparms._debugLevel >= 1 or eparms._debugCmds:
      print('command = ', cmd)
   if eparms._debugLevel >= 2:
      print('app= ', app)
      print('suffix = ', suffix)
      print('timeInfo = ', timeInfo)
   os.system('date')
   # set environment variables based on input environment to match those
   # found in param files
   my_env['RAP_DATA_DIR'] = my_env['DATA']#eparms._temp_data_dir
   my_env['LOG_DIR'] = my_env['LOGepoch']#eparms._logDir
   my_env['MERGED_GFS'] = my_env['COMOUTmerged']#eparms._gfsMergedData
   my_env['GFS_0.25A'] = my_env['COMOUTgfs0p25a']#eparms._gfsA
   my_env['OBAR_CTH'] =  my_env['COMOUTsatellite']#eparms._obarCth
   my_env['OBAR_CMORPH'] =  my_env['COMOUTcmorph']#eparms._obarCmorph
   my_env['PBAR_CMCE'] = my_env['COMOUTpbarCmce']#eparms._pbarCMCE
   my_env['PBAR_GEFS'] = my_env['COMOUTpbarGefs']#eparms._pbarGEFS
   my_env['THRESH_APCP_CMCE'] = my_env['COMOUTthreshApcpCmce']#eparms._threshApcpCMCE
   my_env['THRESH_APCP_GEFS'] = my_env['COMOUTthreshApcpGefs']#eparms._threshApcpGEFS
   my_env['THRESH_HIST_APCP_CMCE'] = my_env['COMOUTthreshHistApcpCmce']#eparms._threshHistApcpCMCE
   my_env['THRESH_HIST_APCP_GEFS'] = my_env['COMOUTthreshHistApcpGefs']#eparms._threshHistApcpGEFS
   my_env['THRESH_ULWRF_CMCE'] = my_env['COMOUTthreshUlwrfCmce']#eparms._threshUlwrfCMCE
   my_env['THRESH_ULWRF_GEFS'] = my_env['COMOUTthreshUlwrfGefs']#eparms._threshUlwrfGEFS
   my_env['THRESH_HIST_ULWRF_CMCE'] = my_env['COMOUTthreshHistUlwrfCmce']#eparms._threshHistUlwrfCMCE
   my_env['THRESH_HIST_ULWRF_GEFS'] = my_env['COMOUTthreshHistUlwrfGefs']#eparms._threshHistUlwrfGEFS
   my_env['EPOCH_OPT_CCT30_GRIB'] = my_env['initial_CCT30_output_dir']#eparms._grib_cct30
   my_env['EPOCH_OPT_CCT35_GRIB'] = my_env['initial_CCT35_output_dir']#eparms._grib_cct35
   my_env['EPOCH_OPT_CCT40_GRIB'] = my_env['initial_CCT40_output_dir']#eparms._grib_cct40
   my_env['EPOCH_OPT_GRIB'] = my_env['initial_Cal_output_dir']#eparms._grib_opt
   my_env['EPOCH_OPT_MDV'] = my_env['mdv_opt']
   my_env['EPOCH_OPT_CLOUDTOPS_MDV'] = my_env['mdv_cloudtop_opt']
   my_env['EPOCH_CMCE_PROB_OPT'] = my_env['COMOUTcmceProbOpt']
   my_env['EPOCH_GEFS_PROB_OPT'] = my_env['COMOUTgefsProbOpt']
   my_env['EPOCH_CMCE_PROB_CLOUDTOP_OPT'] = my_env['COMOUTcmceProbCloudTopOpt']
   my_env['EPOCH_GEFS_PROB_CLOUDTOP_OPT'] = my_env['COMOUTgefsProbCloudTopOpt']

   os.chdir(my_env['SCRIPTSepoch'])#eparms._scripts_dir)
   ppath = os.getcwd();
   os.chdir(my_env['PARMepoch'])#eparms._parms_dir)

   args = shlex.split(cmd)
   os.putenv("pgm", app)
   proc = Popen(args, stdout=PIPE, stderr=PIPE, env=my_env)
   #proc = Popen(args, env=my_env)
   out, err = proc.communicate()
   os.putenv("err",str(proc.returncode))
   #os.system('err_chk')
   exitcode = proc.returncode
   
   logDir = my_env['LOG_DIR']
   outPath = logDir + "/" + my_env['PDY']
   makeDirIfNeeded(outPath)
   outfile = outPath + "/" + app + "." + suffix + "." + timeInfo + ".out"
   errfile = outPath + "/" + app + "." + suffix + "." + timeInfo + ".err"
   f = open(outfile, 'w')
   f.write(str(out, 'utf-8'))
   f.close()
   f = open(errfile, 'w')
   f.write(str(err, 'utf-8'))
   f.close()
   os.chdir(ppath)

   if exitcode:
     print(str(err, 'utf-8'))
   os.system('err_chk') # wait until after errfile is printed

   return exitcode

#----------------------------------------------------------------------------
def doCommandWithInterval(app, instance, ymdh, eparms, my_env):
  cmd = my_env['EXECepoch'] + "/" + app + ' -params ' +  app + '.' + instance + ' -interval ' + ymdh + '0000 ' + ymdh + '0000'
  doCmd(cmd, app, instance, ymdh, eparms, my_env)

#----------------------------------------------------------------------------
def doCommandWithParmFileAndFileEnsemble(app, instance, path, fileName, ensemNum, ensemName, eparms, my_env):
   #print(app)
   cmd = my_env['EXECepoch'] + "/" + app + ' -params ' + app + '.' + instance + " -f " + path + "/" + fileName
   doCmdEns(cmd, app, instance, fileName, ensemNum, ensemName, eparms, my_env)

#----------------------------------------------------------------------------
def doCommandWithParmFileAndFile(app, parmfile, suffix, fileName, ymdh, eparms, my_env):
  cmd = my_env['EXECepoch'] + "/" + app + ' -params ' + parmfile + ' -f ' + fileName
  doCmd(cmd, app,  suffix,  ymdh, eparms, my_env)

#----------------------------------------------------------------------------
def doCommandWithStartEnd(app, instance, ymdh, eparms, my_env):
  yyyy = ymdh[0:4]
  mm = ymdh[4:6]
  dd = ymdh[6:8]
  hh = ymdh[8:10]
  ymdhms = yyyy + " " + mm + " " + dd + " " + hh + " 00 00"
  cmd = my_env['EXECepoch'] + "/" + app + ' -params ' + app + '.' + instance + " -start '" + ymdhms + "' -end '" + ymdhms + "'"
  doCmd(cmd, app, instance, ymdh, eparms, my_env)

#---------------------------------------------------------------------------
def runCombine(ymdh, eparms, my_env):

  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)

  # is it done within a partial restart?
  if estate.hasCompletedCombine(ymdh):
    print("COMBINE for ", ymdh, "Already processed within this restart")
    return True

  print("BEGIN processing COMBINE ", ymdh)

  estate.setCombineInProgress(ymdh)
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'])

  doCommandWithInterval("EnsFcstComb", "epochOpt", ymdh, eparms, my_env)
  doCommandWithInterval("EnsFcstComb", "epochCtopOpt", ymdh, eparms, my_env)
  doCommandWithStartEnd("MdvtoGrib2", "epochOpt", ymdh, eparms, my_env)
  if my_env['SENDCOM'] == 'YES':
    for x in os.listdir(my_env['initial_Cal_output_dir']+'/'+my_env['PDY']):
        fhr=x[-10:-8]
        cmd='cpreq %s/%s/%s %s/epoch.t%sz.cal.f0%s.grib2' % (my_env['initial_Cal_output_dir'],my_env['PDY'],x, my_env['output_Cal_dir'],my_env['cyc'],fhr)
        os.system(cmd)
    cmd='cpreq %s/* %s' % (my_env['output_Cal_dir'], my_env['COMOUTgrib2'])
    os.system(cmd)

  cal_list = os.listdir(my_env['COMOUTgrib2'])
  cal_list = [f for f in cal_list if 'cal' in f]

  if my_env['SENDDBN'] == 'YES':
    for x in cal_list:
        cmd='%s/bin/dbn_alert MODEL EPOCH %s %s/%s' %(my_env['DBNROOT'],my_env['job'],my_env['COMOUTgrib2'],x)
        os.system(cmd)


  doCommandWithStartEnd("MdvtoGrib2", "epochCCT30", ymdh, eparms, my_env)
  if my_env['SENDCOM'] == 'YES':     
    for x in os.listdir(my_env['initial_CCT30_output_dir']+'/'+my_env['PDY']):
        fhr=x[-10:-8]
        cmd='cpreq %s/%s/%s %s/epoch.t%sz.cct30.f0%s.grib2' % (my_env['initial_CCT30_output_dir'], my_env['PDY'],x, my_env['output_CCT30_dir'],my_env['cyc'],fhr)
        os.system(cmd)
    
    cmd='cpreq %s/* %s' % (my_env['output_CCT30_dir'], my_env['COMOUTgrib2'])
    os.system(cmd)

  cct30_list = os.listdir(my_env['COMOUTgrib2'])
  cct30_list = [f for f in cct30_list if 'cct30' in f]

  if my_env['SENDDBN'] == 'YES':
    for x in cct30_list:
        cmd='%s/bin/dbn_alert MODEL EPOCH %s %s/%s' %(my_env['DBNROOT'],my_env['job'],my_env['COMOUTgrib2'],x)
        os.system(cmd)


  doCommandWithStartEnd("MdvtoGrib2", "epochCCT35", ymdh, eparms, my_env)
  if my_env['SENDCOM'] == 'YES':
    for x in os.listdir(my_env['initial_CCT35_output_dir']+'/'+my_env['PDY']):
        fhr=x[-10:-8] 
        cmd='cpreq %s/%s/%s %s/epoch.t%sz.cct35.f0%s.grib2' % (my_env['initial_CCT35_output_dir'],my_env['PDY'],x, my_env['output_CCT35_dir'],my_env['cyc'],fhr)
        os.system(cmd)
  
    cmd='cpreq %s/* %s' % (my_env['output_CCT35_dir'], my_env['COMOUTgrib2'])
    os.system(cmd)
      
  cct35_list = os.listdir(my_env['COMOUTgrib2'])
  cct35_list = [f for f in cct35_list if 'cct35' in f]

  if my_env['SENDDBN'] == 'YES':                  
    for x in cct35_list:
        cmd='%s/bin/dbn_alert MODEL EPOCH %s %s/%s' %(my_env['DBNROOT'],my_env['job'],my_env['COMOUTgrib2'],x)
        os.system(cmd)


  doCommandWithStartEnd("MdvtoGrib2", "epochCCT40", ymdh, eparms, my_env)
  if my_env['SENDCOM'] == 'YES':
    for x in os.listdir(my_env['initial_CCT40_output_dir']+'/'+my_env['PDY']):
        fhr=x[-10:-8]
        cmd='cpreq %s/%s/%s %s/epoch.t%sz.cct40.f0%s.grib2' % (my_env['initial_CCT40_output_dir'],my_env['PDY'],x, my_env['output_CCT40_dir'],my_env['cyc'],fhr)
        os.system(cmd)

    cmd='cpreq %s/* %s' % (my_env['output_CCT40_dir'], my_env['COMOUTgrib2'])
    os.system(cmd)
          
  cct40_list = os.listdir(my_env['COMOUTgrib2'])
  cct40_list = [f for f in cct40_list if 'cct40' in f]

  if my_env['SENDDBN'] == 'YES':
      for x in cct40_list:
          cmd='%s/bin/dbn_alert MODEL EPOCH %s %s/%s' %(my_env['DBNROOT'],my_env['job'],my_env['COMOUTgrib2'],x)
          os.system(cmd)

  estate.setCombineCompleted(ymdh)
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'])
  print("END processing COMBINE ", ymdh)

  return 0

#----------------------------------------------------------------------------
def runThreshFromObarPbar(modelName, ymdh, eparms, my_env):
  print("Running Thresholds Algorithm for " + modelName + " at " + ymdh)
  doCommandWithInterval("ThreshFromObarPbar", modelName, ymdh, eparms, my_env)
  doCommandWithInterval("ThreshHist", modelName, ymdh, eparms, my_env)
  doCommandWithInterval("ThreshHist", modelName+"-cloudtop", ymdh, eparms, my_env)
  return 0
  
#----------------------------------------------------------------------------
def runNesdisNonObar(ymdh, eparms, my_env):
  parmFileLIR = 'GmgsiNcf2Mdv.globcompLir'
  yyyy = ymdh[0:4]
  mm = ymdh[4:6]
  dd = ymdh[6:8]
  hh = ymdh[8:10]
  yyyymmdd = yyyy + mm + dd

  # Process this: GLOBCOMPLIR_nc.yyyymmddhh
  fileLIR = eparms._fileNamingLIR + ymdh
  minutes = '00'
  ss = '00'
  ymdhms = yyyy + " " + mm + " " + dd + " " + hh + " " + minutes + " " + ss
  ymdhms2 = yyyy + "/" + mm + "/" + dd + "_" + hh + ":" + minutes + ":" + ss
  yyyymmddhhmmss = yyyy + mm + dd + hh + minutes + ss
  if eparms._globcompAccessMode == "WCOSS":
    doCommandWithParmFileAndFile("GmgsiNcf2Mdv", parmFileLIR, 'LIR', my_env['globcompDataPathLIR'] + '/' + ymdh[0:8] + '/globcomp_nc/' + fileLIR, ymdh, eparms, my_env)
  else:
    doCommandWithParmFileAndFile("GmgsiNcf2Mdv", parmFileLIR, 'LIR', my_env['globcompDataPathLIR'] + '/' + fileLIR, ymdh, eparms, my_env)
  doCommandWithInterval("MdvThresh", "globcomp_ir", ymdh, eparms, my_env)
  doCommandWithStartEnd("CloudHt", "NESDIS2020.3D_hgtTemp", ymdh, eparms, my_env)
  doCommandWithStartEnd("CloudHt", "NESDIS2020.gfsTrop", ymdh, eparms, my_env)
  doCommandWithStartEnd("MdvMerge2", "NESDIS2020_cloudHt_pc", ymdh, eparms, my_env)

#----------------------------------------------------------------------------
def runNesdis(ymdh_model, ymdh_minus2, ymdh_minus1, ymdh, eparms, my_env):

  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)

  print("Running NESDIS LIR Conversion Algorithm for "+ ymdh)

  parmFileLIR = 'GmgsiNcf2Mdv.globcompLir'
  yyyy = ymdh[0:4]
  mm = ymdh[4:6]
  dd = ymdh[6:8]
  hh = ymdh[8:10]
  yyyymmdd = yyyy + mm + dd

  # do some stuff for all 3 hours
  if ymdh_minus2 != "missing":
    runNesdisNonObar(ymdh_minus2, eparms, my_env)
  if ymdh_minus1 != "missing":
    runNesdisNonObar(ymdh_minus1, eparms, my_env)
  runNesdisNonObar(ymdh, eparms, my_env)   # assume actual hour is present

  # the next steps only needed for the 3 hour resolution data, i.e. ymdh
  doCommandWithStartEnd("MdvTComp", "globcomp_CTH_3hr", ymdh, eparms, my_env)
  doCommandWithStartEnd("MdvResample", "globcomp_CTH_3hr_0.5deg", ymdh, eparms, my_env)
  doCommandWithInterval("ObarCompute", "cth", ymdh, eparms, my_env)
  # here we save this SPDB as it's needed for days into the future
  copySpdb(my_env['COMOUTsatellite'], my_env['COMOUTsatelliteActual'], ymdh[0:8], eparms, my_env)
  print("All done processing LIR for ", ymdh)
  return 0
  
#----------------------------------------------------------------------------
def thresholdsUpdate(ymdh, eparms, my_env):
  maxValid = datetime.datetime.strptime(ymdh + "0000", "%Y%m%d%H%M%S")
  print("Checking for ability to compute thresholds,  MaxValid = ", str(maxValid))
  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
  maxSeconds = eparms._maxEnsembleLeadHours*3600

  # consider all the CMCE runs that have been processed but not thresholded
  didProcessCMCE = []
  for ymdhi in estate._cmce:
    #convert that to an actual ymdhms
    t = datetime.datetime.strptime(ymdhi + '0000', '%Y%m%d%H%M%S')
    # figure out maximum valid time
    tv = t + datetime.timedelta(0, maxSeconds) # days, seconds, then other fields.
    if (tv <= maxValid):
      if not estate.hasThreshCMCE(ymdhi):
        print("Thresholds for CMCE data can be computed now for model run ", ymdhi)
        runThreshFromObarPbar("CMCE", ymdhi, eparms, my_env)
        copySpdb(my_env['COMOUTthreshApcpCmce'], my_env['COMOUTthreshApcpCmceActual'], ymdhi[0:8], eparms, my_env)
        copySpdb(my_env['COMOUTthreshUlwrfCmce'], my_env['COMOUTthreshUlwrfCmceActual'], ymdhi[0:8], eparms, my_env)
        copySpdb(my_env['COMOUTthreshHistApcpCmce'], my_env['COMOUTthreshHistApcpCmceActual'], ymdhi[0:8], eparms, my_env)
        copySpdb(my_env['COMOUTthreshHistUlwrfCmce'], my_env['COMOUTthreshHistUlwrfCmceActual'], ymdhi[0:8], eparms, my_env)
        estate.addThreshCMCE(ymdhi)
        didProcessCMCE.append(ymdhi)
    else:
      if eparms._debugLevel >= 1:
        print("Not ready to compute Thresholds for CMCE data now, not all validation present", ymdhi)  

  # consider all the GEFS runs that have been processed but not thresholded
  didProcessGEFS = []
  for ymdhi in estate._gefs:
    # convert that to an actual ymdhms
    t = datetime.datetime.strptime(ymdhi + '0000', '%Y%m%d%H%M%S')
    #figure out maximum valid time
    tv = t + datetime.timedelta(0, maxSeconds)
    if (tv <= maxValid):
      if not estate.hasThreshGEFS(ymdhi):
        print("Thresholds for GEFS data can be computed now for model run ", ymdhi)
        runThreshFromObarPbar("GEFS", ymdhi, eparms, my_env)
        copySpdb(my_env['COMOUTthreshApcpGefs'], my_env['COMOUTthreshApcpGefsActual'], ymdhi[0:8], eparms, my_env)
        copySpdb(my_env['COMOUTthreshUlwrfGefs'], my_env['COMOUTthreshUlwrfGefsActual'], ymdhi[0:8], eparms, my_env)
        copySpdb(my_env['COMOUTthreshHistApcpGefs'], my_env['COMOUTthreshHistApcpGefsActual'], ymdhi[0:8], eparms, my_env)
        copySpdb(my_env['COMOUTthreshHistUlwrfGefs'], my_env['COMOUTthreshHistUlwrfGefsActual'], ymdhi[0:8], eparms, my_env)
        estate.addThreshGEFS(ymdhi)
        didProcessGEFS.append(ymdhi)
    else:
      if eparms._debugLevel >= 1:
        print("Not ready to compute Thresholds for GEFS data now, not all validation present", ymdhi)  

  # remove the ones that got thresholded from the state as they are now completely processed
  # write out the changed state
  debug = eparms._debugLevel >= 1
  estate.removeCMCE(didProcessCMCE, debug)
  estate.removeGEFS(didProcessGEFS, debug)
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)

#----------------------------------------------------------------------------
def wcossExistingCmorph2File(ymdhm, ymdhm2, my_env):
  for n in ['PDYm8','PDYm7','PDYm6','PDYm5','PDYm4','PDYm3','PDYm2','PDYm1','PDY']:
    filename_n = my_env['COMINcmorph2']+'/'+my_env[n]+'/cmorph2/CMORPH2_0.25deg-30min_S' + ymdhm + '_E' + ymdhm2 + '.RT.nc'
    if os.path.exists(filename_n):
      return filename_n
  print('ERROR cmorph2 file CMORPH2_0.25deg-30min_S' + ymdhm + '_E' + ymdhm2 + '.RT.nc not found for any of the previous 8 COMINcmorph2 days')
  return ""

#----------------------------------------------------------------------------
def runCmorph2Conversion(ymdhm, eparms, my_env):
  cmorph2ParmFile = 'NetCDF2Mdv.cmorph2_25'

  # for cmorph2 it's organized by subdirectories for yyyymmdd, whereas input is yyyymmddhhmm
  ymd = ymdhm[:-4]

  year = ymdhm[0:4]
  month = ymdhm[4:6]
  day = ymdhm[6:8]
  hour = ymdhm[8:10]
  min = ymdhm[10:12]
  ymdhms = year + " " + month + " " + day + " " + hour + " " + min + " 00" 
  
  # convert ymdhm to an int and set expire time to the total frequency minus 1
  iymdhm = int(ymdhm)
  iymdhm = iymdhm + eparms._cmorph2FrequencyMinutes - 1
  ymdhm2 = str(iymdhm)
  min2 = ymdhm2[10:12]
  if eparms._cmorph2AccessMode == "WCOSS":
    fileName = wcossExistingCmorph2File(ymdhm, ymdhm2, my_env)
    if not fileName:
      return False
  elif eparms._cmorph2AccessMode == "NCAR":
    # expected file name:
    fileName = cmorph2PathToDataFromPattern(my_env['cmorph2Pattern'], ymd, hour, min, min2)
  else:
    print("ERROR unknown cmorph2 access mode ", eparms._cmorph2AccessMode)
    return False

  # make sure this file exists
  if os.path.exists(fileName):
    #unique command line interface
    cmd = my_env['EXECepoch'] + "/" + "NetCDF2Mdv -params " + cmorph2ParmFile + " -f " + fileName
    doCmd(cmd, "NetCDF2Mdv", "cmorph2", ymdhm, eparms, my_env)
    return True
  else:
    print("cmorph2 file not there ", fileName)
    return False

#----------------------------------------------------------------------------
def runCmorph2Ave(ymdhm0, ymdhm1, ymdhOut, eparms, my_env):

  cmd =  my_env['EXECepoch'] + "/" + "CmorphAverager -params CmorphAverager.default -interval " + ymdhm0 + "00 " + ymdhm1 + "00"
  doCmd(cmd, "CmorphAverager", "cmorph2", ymdhm1, eparms, my_env)
  cmd =  my_env['EXECepoch'] + "/" + "ObarCompute -params ObarCompute.cmorph23Hr -interval " + ymdhOut + "0000 " + ymdhOut + "0000"
  doCmd(cmd, "ObarCompute", "cmorph2", ymdhOut, eparms, my_env)
  copySpdb(my_env['COMOUTcmorph'], my_env['COMOUTcmorphActual'], ymdhOut[0:8], eparms, my_env)
  return 0

#----------------------------------------------------------------------------
def runGfs(ymdh, eparms, my_env):
  print("Running GFS Conversion Algorithm for "+ ymdh)
  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
  # is this one partially done?
  if estate._gfsPartial == ymdh:
    print("Partial GFS for ", ymdh)
    print("GFS Last done = ", estate._gfsLastDone)
    print("Last Grib2 conversion A = ", estate._gfsaLastGrib2toMdv)
    print("Last Grib2 conversion B = ", estate._gfsbLastGrib2toMdv)
  else:
    estate._gfsPartial = ymdh
    estate._gfsLastDone = ""
    estate._gfsaLastGrib2toMdv = ""
    estate._gfsbLastGrib2toMdv = ""
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)

  hh = ymdh[8:10]
  yyyymmdd = ymdh[0:8]
  ymdhms = ymdh + "0000"
  ymdhms0 = yyyymmdd[0:4] + " " + yyyymmdd[4:6] + " " + yyyymmdd[6:8] + " " + hh + " 00 00"
  
  if estate._gfsLastDone == "":
    pathToDataA = gfsPathToDataFromPattern(my_env['gfsPattern'], yyyymmdd, hh)

    # at this point assume both A and B are present for the given yyyymmddhh, based on this method getting called
    # so do both conversions and merge.  A better design would be to check and compare files and filesb, maybe later.

    ### A FILES ###

    files = glob.glob(pathToDataA + '/gfs.t*z.pgrb2.0p25.f*[!.idx]')
    files = [a for a in files if a[-3:] in ['000', '003', '006', '009', '012']]
    files.sort()
    lenAll = len(files)
    fnames = []
    for f in files:
      ind = f.rindex('gfs.')
      f = f[ind:]
      fnames.append(f)
    fnames = pruneAlreadyDone(fnames, estate._gfsaLastGrib2toMdv)
    lenKeep = len(fnames)
    print("Converting ", lenKeep, " of ", lenAll, " GFS A files from Grib2 to Mdv")
    for f in fnames:
      doCommandWithParmFileAndFileEnsemble("Grib2toMdv", "gfs_0.25a", pathToDataA, f, '01', '01', eparms, my_env)
      # f has format gfs.tHHz.pgrb2.0p25.fHHH
      # it should have yyyymmdd and hh should equal HH in the file name
      # parse out the lead hour 3 digits and can form MDV file name
      leadhour_s = f[-3:]
      copyMdvFcst2(my_env['WORKSPACE'], my_env['COMOUT'], "mdv/model/gfs_0p25a",  yyyymmdd, hh, leadhour_s, eparms, my_env)
      estate._gfsaLastGrib2toMdv = f
      estate.write(my_env['workspace_state'])#eparms._epochStateFile)
      estate.write(my_env['COMOUTrestartS'])

    estate._gfsLastDone = "Grib2toMdv.a"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._gfsLastDone == "Grib2toMdv.a":

    ### Do the B FILES ###
    pathToDataB = gfsPathToDataFromPattern(my_env['gfsPatternb'], yyyymmdd, hh)
    filesb = glob.glob(pathToDataB + '/gfs.t*z.pgrb2b.0p25.f*[!.idx]')
    filesb = [a for a in filesb if a[-3:] in ['000', '003', '006', '009', '012']]
    filesb.sort()
    lenAll = len(filesb)
    fnames = []
    for f in filesb:
      ind = f.rindex('gfs.')
      f = f[ind:]
      fnames.append(f)
    fnames = pruneAlreadyDone(fnames, estate._gfsbLastGrib2toMdv)
    lenKeep = len(fnames)
    print("Converting ", lenKeep, " of ", lenAll,  " GFS B files from Grib2 to Mdv")
    for f in fnames:
      leadhour_s = f[-3:]
      doCommandWithParmFileAndFileEnsemble("Grib2toMdv", "gfs_0.25b", pathToDataB, f, '01', '01', eparms, my_env)
      copyMdvFcst2(my_env['DATA'] + '/EpochOps', my_env['COMOUTrestart'], "mdv/model/gfs_0.25b",  yyyymmdd, hh, leadhour_s, eparms, my_env)
      estate._gfsbLastGrib2toMdv = f
      estate.write(my_env['workspace_state'])#eparms._epochStateFile)
      estate.write(my_env['COMOUTrestartS'])

    estate._gfsLastDone = "Grib2toMdv.b"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._gfsLastDone == "Grib2toMdv.b":
    # merge now
    doCommandWithStartEnd("MdvMerge2", "gfs_0.25", ymdh, eparms, my_env)
    copyMdvFcst(my_env['WORKSPACE'], my_env['COMOUT'], my_env['COMOUTmerged'], 'merged_gfs', ymdh, yyyymmdd, hh, eparms, my_env)
    estate._gfsLastDone = "MdvMerge.gfs"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._gfsLastDone == "MdvMerge.gfs":
    # if here, all done
    estate._gfsLastDone = ""
    estate._gfsPartial = ""
    estate._gfsaLastGrib2toMdv = ""
    estate._gfsbLastGrib2toMdv = ""
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])
    print("All done processing GFS data for ", ymdh)

  return 0

#----------------------------------------------------------------------------
def gatherGfs(mem, toldest, tnewest, gfsDataPath, gfsPattern, gfsYmdIndex, gfsHourIndex, gfsLeadIndex, eparms, my_env):
  
  if eparms._debugLevel >= 2:
    print('gathering GFS files ', mem)
    print('gfspath = ', gfsDataPath)

  gfsFiles = []

  # take the data path and pull out the yyyymmdd and hh
  yyyymmdd = gfsDataPath[gfsYmdIndex:gfsYmdIndex+8]
  hh = gfsDataPath[gfsHourIndex:gfsHourIndex+2]
  if eparms._debugLevel >= 2:
    print('pulled ymd and h from input data path ', yyyymmdd, ' ', hh)
    print('loop through looking for older files')
  
  # convert that to a datetime
  t = datetime.datetime.strptime(yyyymmdd+hh+"00", "%Y%m%d%H%M%S")
  for h in [0, 6, 12, 18, 24, 30, 36]:
    ti = t - datetime.timedelta(hours=h)
    ymdi = ti.strftime("%Y%m%d")
    hi = ti.strftime("%H")
    if eparms._debugLevel >= 2:
      print("for lookback=", h, ' the ymd and h are ', ymdi, ' ', hi)
    
    pathi = gfsPathToDataFromPattern(gfsPattern, ymdi, hi)
    if mem == 'a':
      gfsFilesi = glob.glob(pathi+'/gfs.t*z.pgrb2.0p25*[!.idx]')
    else:
      gfsFilesi = glob.glob(pathi+'/gfs.t*z.pgrb2b*0p25*[!.idx]')
    if eparms._debugLevel >= 2:
      print("for lookback=", h, " found this many files:", len(gfsFilesi))
    gfsFiles.extend(gfsFilesi)

  if eparms._debugLevel >= 2:
    print("Total number of gfs files = ", len(gfsFiles))

  gfsFiles = [a for a in gfsFiles if a[-3:] in ['000', '003', '006', '009', '012']]
  if eparms._debugLevel >= 2:
    print("Total number of gfs files for hours 0,3,6,9,12= ", len(gfsFiles))
  gfsFiles.sort()
  
  gfsYmdh = []
  goodGfsYmdh = []
  for name in gfsFiles:
    ymdh = parseYmdh(name, gfsYmdIndex, gfsHourIndex)
    if ymdh not in str(gfsYmdh):
      t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
      if t > toldest:
        if t <= tnewest:
          gfsYmdh.append(ymdh)
  nl = len(eparms._gfsModelLeadTimeHours)
  for ymdhi in gfsYmdh:
    leads = []
    for f in gfsFiles:
      ymdh = parseYmdh(f, gfsYmdIndex, gfsHourIndex)
      if ymdh == ymdhi:
        lt = parseLead(f, gfsLeadIndex)
        leads.append(lt)
    
    if (nl == len(leads)):
      goodGfsYmdh.append(ymdhi)
    else:
      print("Bad gfs: ", ymdhi)
      print("Wanted nleads = ", nl)
      print("Got nleads = ", len(leads))

  if eparms._debugLevel >= 1:
    print("Final number of gfs ", mem, " files = ", len(goodGfsYmdh))
  return goodGfsYmdh
  
#----------------------------------------------------------------------------
def LirFilesNcar(eparms, debug):
  if debug:
    print("Doing LIR the NCAR way")
  LirFiles = []
  for root,dirs,files in os.walk(eparms._globcompDataPathLIR):
    for name in files:
      if "latest_data" not in name and "Janitor" not in name:
        LirFiles.append(os.path.join(root, name))
  return LirFiles

#----------------------------------------------------------------------------
def LirFilesWcoss(my_env, eparms, debug):
  if debug:
    print("Doing LIR the WCOSS way")
  LirFiles = []
  for n in ['PDYm8','PDYm7','PDYm6','PDYm5','PDYm4','PDYm3','PDYm2','PDYm1','PDY']:
    print(my_env[n])
    path = my_env['COMINglobcomp']+'/'+my_env[n]+'/globcomp_nc'
    print('globcomp_path:',path)
    if debug:
      print('path:',path)
    lirFiles = glob.glob(path+'/'+eparms._fileNamingLIR+'*')
    for f in lirFiles:
      LirFiles.append(f)
  return LirFiles

#----------------------------------------------------------------------------
def processGoodLir(estate, inputState, ymdh, ymdh_model, LirYmdh, eparms, my_env):
  if (inputState.hasLir(ymdh)):
    return False
  
  print("Found a new unprocessed ymdh: ", ymdh, " run the nesdis alg now")
  ymdh_minus1 = previousHour(ymdh)
  ymdh_minus2 = previousHour(ymdh_minus1)
  if ymdh_minus1 in LirYmdh and ymdh_minus2 in LirYmdh:
    runNesdis(ymdh_model, ymdh_minus2, ymdh_minus1, ymdh, eparms, my_env)
  elif ymdh_minus1 in LirYmdh and ymdh_minus2 not in LirYmdh:
    runNesdis(ymdh_model, "missing", ymdh_minus1, ymdh, eparms, my_env)
  elif ymdh_minus1 not in LirYmdh and ymdh_minus2 in LirYmdh:
    runNesdis(ymdh_model, ymdh_minus2, "missing", ymdh, eparms, my_env)
  else:
    runNesdis("missing", "missing", ymdh, eparms, my_env)

  # not doing partial stuff for thresholds, if we get past
  # LIR and cmorph processing the thresholds will have updated for sure
  # if they can be updated
  if inputState.hasCmorph(ymdh):
    thresholdsUpdate(ymdh, eparms, my_env)

  estate._lirLastDone = ymdh
  estate.write(my_env['workspace_state'], True)#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'], True)
  inputState.updateLIR(ymdh)
  inputState.write(my_env['workspace_inputstate'], True)#eparms._inputStateFile)
  inputState.write(my_env['COMOUTrestartIS'], True)
  return True

#----------------------------------------------------------------------------
def checkLir(ymdh_model, toldest, tnewest, eparms, my_env):
  debug= eparms._debugLevel >= 2
  if eparms._globcompAccessMode == "NCAR":
    LirFiles = LirFilesNcar(eparms, debug)
  elif eparms._globcompAccessMode == "WCOSS":
    LirFiles = LirFilesWcoss(my_env, eparms, debug)
  else:
    print("WARNING: unknown LIR processing mode ", eparms._globcompAccessMode)
    return

  LirYmdh = []
  for name in LirFiles:
     ymdh = parseYmdh(name, eparms._globcompLirYmdIndex, eparms._globcompLirHourIndex)
     if ymdh not in str(LirYmdh):
       t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
       if t > toldest:
         if t <= tnewest:
           LirYmdh.append(ymdh)

  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)

  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
  estate._lirPartial = ymdh_model
  estate._lirLastDone = ""
  estate.write(my_env['workspace_state'], True)#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'], True)

  goodhours = ['00', '03', '06', '09', '12', '15', '18', '21']
    
  LirYmdh.sort()
  for ymdh in LirYmdh:
    hh = ymdh[8:10]
    if hh in goodhours:
      if processGoodLir(estate, inputState, ymdh, ymdh_model, LirYmdh, eparms, my_env):
        # read states back in as they may have changed
        estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
        inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)        
  # update state to indicate all done
  estate._lirPartial = ""
  estate._lirLastDone = ""
  estate.write(my_env['workspace_state'], True)#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'], True)
  print("DONE PROCESSING LIR FOR ", ymdh_model)

#----------------------------------------------------------------------------
def updateRawCmorphInputs(ymdhm, toldest, tnewest, eparms, my_env):
  t = datetime.datetime.strptime(ymdhm + '00', '%Y%m%d%H%M%S')
  if t < toldest:
    return 0
  if t > tnewest:
    return 0
  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)
  if inputState.hasRawCmorph(ymdhm):
    if eparms._debugLevel >= 2:
      print("Already have raw cmorph in state: ", ymdhm)
    return 0
  inputState.updateRawCMORPH(ymdhm)
  inputState.write(my_env['workspace_inputstate'])#eparms._inputStateFile)
  return 0

#----------------------------------------------------------------------------
def checkOneCmorph2(ymdh, ymdhm, cmorphYmdhm, toldest, tnewest, goodhours, eparms, my_env):
  # is this a good hour?
  hour = ymdhm[8:10]
  minute = ymdhm[10:12]
  if hour not in str(goodhours):
    return 0
  if minute != '00':
    return 0
  t = datetime.datetime.strptime(ymdhm + '00', '%Y%m%d%H%M%S')
  if t < toldest:
    return 0
  if t > tnewest:
    return 0
  outHour = hour
  ymdhOut = ymdhm[0:8] + outHour

  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)

  if inputState.hasCmorph(ymdhOut):
    if eparms._debugLevel >= 2:
      print("Already processed cmorph: ", ymdhOut)
    return 0
    
  # get latency between current ymdh and the cmorph ymdh
  t = datetime.datetime.strptime(ymdhOut + '0000', '%Y%m%d%H%M%S')
  tnow = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
  # Get interval as timedelta object
  diff = tnow - t
  # Get interval between two timstamps in hours
  diff_in_hours = diff.total_seconds() / 3600
  if diff_in_hours < eparms._cmorph2DelayHours:
    print("cmorph2: continue to wait to process ", ymdhOut, " need to wait ", eparms._cmorph2DelayHours, " hours, current delay=", diff_in_hours)
    return 0

  # does input state have raw cmorph data for the 5 previous times?
  minBack = [30, 60, 90, 120, 150]
  canProcess = True
  numMissing = 0
  for m in minBack:
    ti = t - datetime.timedelta(minutes=m)
    ymdhmi = ti.strftime("%Y%m%d%H%M")
    if not inputState.hasRawCmorph(ymdhmi):
      numMissing = numMissing + 1
      print("Missing raw cmorph for ", ymdhmi, " prior to ", ymdhOut)
      canProcess = False
      
  if not canProcess:
    if diff_in_hours >= eparms._cmorph2MaxLatencyHours:
      print("Give up waiting for full previous 3 hours for cmorph averaging at ", ymdhOut, " process now with ", numMissing, " missing files")
    else:
      print("Waiting for previous 3 hours cmorphs to process ", ymdhOut, ". ", numMissing, " previous files are not yet available")
      return 0

  # the oldest time will be t - 2.5 hours
  # get the 5 previous times to use 30 minute back, 60, etc.
  # we convert all these files to mdv, including the one at t

  # the current time is there for sure, already know that
  numConverted = 1
  runCmorph2Conversion(ymdhm, eparms, my_env)

  # get all the older ones converted if possible
  for m in minBack:
    t1 = t - datetime.timedelta(minutes=m)
    ymdhm1 = t1.strftime("%Y%m%d%H%M")
    if ymdhm1 in str(cmorphYmdhm):
      numConverted = numConverted + 1
      runCmorph2Conversion(ymdhm1, eparms, my_env)

  t1 = t - datetime.timedelta(minutes=150)
  ymdhm0 = t1.strftime("%Y%m%d%H%M")

  if numConverted < 6:
    print("WARNING: Logic problem Did not convert all the cmorphs to MDV at ", ymdhm, " wanted 6 got ", numConverted)

  #now do the averaging, and update state
  runCmorph2Ave(ymdhm0, ymdhm, ymdhOut, eparms, my_env)

  inputState.updateCMORPH(ymdhOut)
  inputState.write(my_env['workspace_inputstate'])#eparms._inputStateFile)
      
  # check if LIR is also there, so you can then
  # do the thresholds from obar pbar tests
  if inputState.hasLir(ymdhOut):
    thresholdsUpdate(ymdhOut, eparms, my_env)
  return 0

#----------------------------------------------------------------------------
def cmorph2FilesNcar(eparms):
  cmorph2Files = []
  if eparms._debugLevel >= 1:
    print("Doing Cmorph the NCAR way, looking here:", eparms._cmorph2Path)
  for root,dirs,files in os.walk(eparms._cmorph2Path):
    for name in files:
      if "latest_data" not in name and "Janitor" not in name:
        cmorph2Files.append(os.path.join(root, name))
  return cmorph2Files

#----------------------------------------------------------------------------
def cmorph2FilesWcoss(my_env,eparms):
  cmorph2Files = []
  if eparms._debugLevel >= 1:
    print("Doing Cmorph the WCOSS way, looking here:")
  for n in ['PDYm8','PDYm7','PDYm6','PDYm5','PDYm4','PDYm3','PDYm2','PDYm1','PDY']:
    path = my_env['COMINcmorph2']+'/'+my_env[n]+'/cmorph2'
    if eparms._debugLevel >= 1:
      print(path) 
    cm2Files = glob.glob(path+'/*.nc')
    if len(cm2Files)==0:
      print('WARNING: Running without CMORPH2 files for %s' % n)
    for f in cm2Files:
      cmorph2Files.append(f)
  return cmorph2Files

#----------------------------------------------------------------------------
def checkCmorph2(ymdh, toldest, tnewest, eparms, my_env):
  print("Checking for new CMORPH2 data")

  # get all files on disk
  cmorph2Files = []
  if eparms._cmorph2AccessMode == "NCAR":
    cmorph2Files = cmorph2FilesNcar(eparms)
  elif eparms._cmorph2AccessMode == "WCOSS":
    cmorph2Files = cmorph2FilesWcoss(my_env,eparms)
  else:
    print("WARNING: Unknown mode for cmorph2 access ", eparms._cmorph2AccessMode)

  # store ymdhm for each file into an array and sort
  cmorphYmdhm = []
  for name in cmorph2Files:
    ymdhm = parseYmdhm(name, eparms._cmorph2YmdIndex, eparms._cmorph2HourIndex, eparms._cmorph2MinuteIndex)
    if ymdhm not in str(cmorphYmdhm):
      cmorphYmdhm.append(ymdhm)
  cmorphYmdhm.sort()
  
  for ymdhm in cmorphYmdhm:
    updateRawCmorphInputs(ymdhm, toldest, tnewest, eparms, my_env)

  # cmorph is unique in when we fire it off at hour 0,3,6,9,..
  goodhours = ['00', '03', '06', '09', '12', '15', '18', '21']

  for ymdhm in cmorphYmdhm:
    checkOneCmorph2(ymdh, ymdhm, cmorphYmdhm, toldest, tnewest, goodhours, eparms, my_env)

  # in case anythiing got processed, write out now to COMOUTrestartS
  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)
  inputState.write(my_env['COMOUTrestartIS'])

#---------------------------------------------------------------------------
def checkGfs(ymdh, toldest, tnewest, eparms, my_env):
  print("Checking for new GFS data")
  gfsDataPath = my_env['GFS_MODEL_DIR']
  gfsbDataPath = my_env['GFSB_MODEL_DIR']
  if eparms._debugLevel >= 2:
    print('gfsPath:', gfsDataPath)
    print('gfsbPath:', gfsbDataPath)
    print('gfsPattern:',my_env['gfsPattern'])
  goodGfsaYmdh = gatherGfs('a', toldest, tnewest, gfsDataPath, my_env['gfsPattern'], eparms._gfsYmdIndex, eparms._gfsHourIndex, eparms._gfsLeadIndex, eparms, my_env)
  goodGfsbYmdh = gatherGfs('b', toldest, tnewest, gfsbDataPath, my_env['gfsPatternb'], eparms._gfsbYmdIndex, eparms._gfsbHourIndex, eparms._gfsbLeadIndex, eparms, my_env)
  if eparms._debugLevel >= 1:
    print("Good GFSA", goodGfsaYmdh)
    print("Good GFSB", goodGfsbYmdh)

  # consider only times with both a and b
  goodGfs = []
  for ymdh in goodGfsaYmdh:
    if ymdh in str(goodGfsbYmdh):
      goodGfs.append(ymdh)
    else:
      print("WARNING ", ymdh, " not in GFSb only in GFSa")
  for ymdh in goodGfsbYmdh:
    if not ymdh in str(goodGfsaYmdh):
      print("WARNING ", ymdh, " not in GFSa only in GFSb")
  goodGfs.sort()
  
  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)

  # for each good gfs, see if it's already processed or not
  # if not process now and update state
  for ymdhi in goodGfs:
    if (not inputState.hasGfs(ymdhi)):
      runGfs(ymdhi, eparms, my_env)
      # NOTE: A tiny chance a crash could happen after the above
      # but before the next, i.e. right here.
      # The two state files would be out of synch
      # Not sure what to do
      inputState.updateGFS(ymdhi)
      inputState.write(my_env['workspace_inputstate'])
      inputState.write(my_env['COMOUTrestartIS'])
    else:
      if eparms._debugLevel >= 1:
        print("Already processed Gfs: " + ymdhi)

#---------------------------------------------------------------------------
def runThresholdsBackfill(ymdh, eparms, my_env):
  # NOTE this is not being called at this point, too risky at this late stage
  # but I'm leaving it in for later.
  
  # read in state files
  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)
  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)

  # go back 4 days in 6 hour increments
  t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
  toldest = t - datetime.timedelta(days=4)
  tnewest = t
  ti = toldest
  while ti <= tnewest:
    ymdhi = ti.strftime("%Y%m%d%H")
    if inputState.hasCmorph(ymdhi) and inputState.hasLir(ymdhi):
      print("Checking ", ymdhi, " for backfill of thresholds")
      hh = ymdhi[8:10]
      if hh == "00" or hh == "12":
        if ymdhi not in estate._threshGefs or ymdhi not in estate._threshCmce:
          thresholdsUpdate(ymdhi, eparms, my_env)
      if hh == "06" or hh == "18":
        if ymdhi not in estate._threshGefs:
          thresholdsUpdate(ymdhi, eparms, my_env)
    ti = ti + datetime.timedelta(hours=6)   

  # write this state file which can change
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)
  estate.write(my_env['COMOUTrestartS'])
  
#---------------------------------------------------------------------------
def checkInputs(ymdh, eparms, my_env):
  t = datetime.datetime.strptime(ymdh + '0000', '%Y%m%d%H%M%S')
  dt = eparms._maxLookbackDays
  toldest = t - datetime.timedelta(days=dt)
  dt = eparms._maxLookaheadDays
  tnewest = t + datetime.timedelta(days=dt)
  
  # read in input state, prune, write
  inputState = epochinputstate.EpochInputState()
  inputState.readOrCreate(my_env['workspace_inputstate'])#eparms._inputStateFile)
  inputState.removeTooOld(toldest, eparms._debugLevel >= 1)
  inputState.write(my_env['workspace_inputstate'])#(eparms._inputStateFile)
  inputState.write(my_env['COMOUTrestartIS'])

  # read in state 
  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
  # has this run already processed cmorph2, if so skip, otherwise process
  if not estate.hasCompletedCmorph2(ymdh):
    print("BEGIN processing Cmorph2 ", ymdh)
    # process
    estate.setCmorph2InProgress(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

    checkCmorph2(ymdh, toldest, tnewest, eparms, my_env)

    estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
    estate.setCmorph2Completed(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])
    print("END processing Cmorph2 ", ymdh)
  else:
    print("SKIP processing Cmorph2 ", ymdh)

  if not estate.hasCompletedGfs(ymdh):
    print("BEGIN processing Gfs ", ymdh)
    estate.setGfsInProgress(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

    checkGfs(ymdh, toldest, tnewest, eparms, my_env)

    estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
    estate.setGfsCompleted(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])
    print("END processing Gfs ", ymdh)
  else:
    print("SKIP processing Gfs ", ymdh)

  if not estate.hasCompletedLir(ymdh):
    print("BEGIN processing Lir ", ymdh)
    estate.setLirInProgress(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

    checkLir(ymdh, toldest, tnewest, eparms, my_env)

    estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
    estate.setLirCompleted(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])
    print("END processing Lir ", ymdh)
  else:
    print("SKIP processing Lir ", ymdh)

  return 0
  
#----------------------------------------------------------------------------
def pruneAlreadyDone(filesAll, lastDone):
  ret = []
  if lastDone:
    for f in filesAll:
      if f > lastDone:
        ret.append(f)
  else:
    ret = filesAll
  return ret

#----------------------------------------------------------------------------
# This function is specifically for pruneBadCmce and pruneBadGefs below
# At least once there was a file ending in "tmp" that made pruneBadCmce blow up.
# Return an integer of the last 3 digits of the filename, which SHOULD be the fcst hour, but
# if not, then we don't want to include the file (want to prune it), but still process the good ones.
def fcstFromFn(fn):
    try:
        fcst = int(fn[-3:])
    except:
        print('WARNING: Problematic CMCE filename encountered: ', fn)
        return -1 # Intentionally fails inclusion test below in pruneBadCmce and pruneBadGefs (is less than 000.)
    return fcst

#----------------------------------------------------------------------------
def pruneBadCmce(filesAll):
  ret = []
  for f in filesAll:
    if "gespr" not in f and "cmc_geavg" not in f and "cmc_gec" not in f and f[-2:] != "00":
      ret.append(f)
  ret = [c for c in ret if c[-4:] != '.idx']
  ret = [d for d in ret if 000.< fcstFromFn(d) <058.]
  return ret
      
#----------------------------------------------------------------------------
def pruneBadGefs(filesAll):
  ret = []
  for f in filesAll:
    if "gec" not in f and f[-2:] != "00":
      ret.append(f)
  ret = [f for f in ret if f[-4:]!='.idx']
  ret = [c for c in ret if 000.< fcstFromFn(c) <058.]
  return ret
      
#----------------------------------------------------------------------------
def convertCmce(ymdh, eparms, estate, my_env):
  hh = ymdh[8:10]
  yyyymmdd = ymdh[0:8]
  my_env = os.environ.copy()
  # get all the files in the path
  pathToData = my_env['CMCE_MODEL_DIR']#eparms._cmceDataPath1 + yyyymmdd + "/" + hh + eparms._cmceDataPath2
  if eparms._debugLevel >= 2:
    print("Path to CMCE: ", pathToData)
  filesAll = getFileNames(pathToData)
  filesAll.sort()

  files = pruneBadCmce(filesAll)
  lenAll = len(files)
  #files = pruneAlreadyDone(files, estate._cmceLastGrib2toMdv)
  lenKeep = len(files)
  
  print("Converting ", lenKeep, " of ", lenAll, " CMCE files from Grib2 to Mdv")
  if len(files)==0:
      print('WARNING: Missing valid CMCE data for %s' %my_env['PDY'])
  for f in files:
    if f[0:7] == eparms._cmceDataPath3:
      ensemName = f[4:9]
      ensemNum = "%d" % (int(f[7:9]))
      doCommandWithParmFileAndFileEnsemble("Grib2toMdv", "cmce", pathToData, f, ensemNum, ensemName, eparms, my_env)

  estate._cmceLastDone = "Grib2toMdv"
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)

#----------------------------------------------------------------------------
def convertGefs(ymdh, eparms, estate, my_env):
  hh = ymdh[8:10]
  yyyymmdd = ymdh[0:8]
  pathToData = my_env['GEFSA_MODEL_DIR']
  if eparms._debugLevel >= 2:
    print('gefsPathToData:',pathToData)

  filesAll = getFileNames(pathToData)
  filesAll.sort()
  
  files = pruneBadGefs(filesAll)
  lenAll = len(files)
  #files = pruneAlreadyDone(files, estate._gefsLastGrib2toMdv)
  lenKeep = len(files)

  print("Converting ", lenKeep, " of ", lenAll, " GEFS files from Grib2 to Mdv")
  for f in files:
    # names are like:  gepEE.thhz.pgrb2a.0p50.fhhh
    ensemName = f[0:5]
    if ensemName[0:3] == eparms._gefsDataPath3:
      ensemNum = "%d" % (int(f[3:5]))
      doCommandWithParmFileAndFileEnsemble("Grib2toMdv", "gefs", pathToData, f, ensemNum, ensemName, eparms, my_env)
    else:
      print(ensemName[0:3],eparms._gefsDataPath3)
  estate._gefsLastDone = "Grib2toMdv"
  estate.write(my_env['workspace_state'])#eparms._epochStateFile)

#----------------------------------------------------------------------------
def runCmce(ymdh, eparms, my_env):
  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)

  # is this one completely done?
  if estate.hasCmce(ymdh):
    print("SKIP processing CMCE ", ymdh, "..Already completed")
    return True
  
  # is it done within a partial restart?
  if estate.hasCompletedCmce(ymdh):
    print("SKIP processing CMCE ", ymdh, "..Already processed within this restart")
    return True

  print("BEGIN processing CMCE ", ymdh)
  hh = ymdh[8:10]
  yyyymmdd = ymdh[0:8]

  # is this one partially done?
  if estate._cmcePartial == ymdh and estate._inProgress == "CMCE":
    print("Partial CMCE for ", ymdh)
    print("Last done = ", estate._cmceLastDone)
  else:
    estate.setCmceInProgress(ymdh)
    estate._cmcePartial = ymdh
    estate._cmceLastDone = ""
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])#eparms._epochStateFile)
    
  if estate._cmceLastDone == "":
    convertCmce(ymdh, eparms, estate, my_env)
    # read back in state file which was changed by convertCmce, write to comoutrestart
    estate = epochstate.EpochState()
    estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
    estate._cmceLastDone = "Grib2toMdv"
    # write the MDV ensemble model data to COMOUTrestartS
    topPath = my_env['DATA'] + '/EpochOps'
    subpath = 'mdv/model/cmce'
    recursiveCopyMdvEnsFcst("CMCE", topPath, my_env['COMOUTrestart'], subpath, yyyymmdd, hh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])#eparms._epochStateFile)

  if estate._cmceLastDone == "Grib2toMdv":
    doCommandWithInterval("PrecipAccumCalc", "cmce", ymdh, eparms, my_env)
    estate._cmceLastDone = "PrecipAccumCalc"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    # write the MDV ensemble model data to COMOUTrestartS
    topPath = my_env['DATA'] + '/EpochOps'
    subpath = 'mdv/model/cmce3hr/3hrAccum'
    recursiveCopyMdvEnsFcst("CMCE", topPath, my_env['COMOUTrestart'], subpath, yyyymmdd, hh)
    estate.write(my_env['COMOUTrestartS'])

  if estate._cmceLastDone == "PrecipAccumCalc":
    # with new cmce data, we now run EnsLookupGen, both precip and cloud top, no checking
    doCommandWithInterval("EnsLookupGen", "CMCE", ymdh, eparms, my_env)
    # copy to COMOUT, as we need to combine previous times output in the combine step
    recursiveCopyMdvFcst(my_env['WORKSPACE'], my_env['COMOUT'], "mdv/model/cmceProbOpt", yyyymmdd, hh)
    estate._cmceLastDone = "EnsLookupGen.CMCE"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._cmceLastDone == "EnsLookupGen.CMCE":
    doCommandWithInterval("EnsLookupGen", "CMCE-cloudtop", ymdh, eparms, my_env)
    # copy to COMOUT, as we need to combine previous times output in the combine step
    recursiveCopyMdvFcst(my_env['WORKSPACE'], my_env['COMOUT'], "mdv/model/cmceProbCloudTopOpt", yyyymmdd, hh)
    estate._cmceLastDone = "EnsLookupGen.CMCE-cloudtop"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._cmceLastDone == "EnsLookupGen.CMCE-cloudtop":
    # we also run PbarCompute
    doCommandWithInterval("PbarCompute", "CMCE", ymdh, eparms, my_env)
    copySpdb(my_env['COMOUTpbarCmce'], my_env['COMOUTpbarCmceActual'], ymdh[0:8], eparms, my_env)
    estate._cmceLastDone = "PbarCompute"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._cmceLastDone == "PbarCompute":
    # indicate a new model run in the python param file, we are all done
    if eparms._debugLevel >= 1:
      print("Finishing up CMCE for ", ymdh)

    estate.addCMCE(ymdh, eparms._debugLevel >= 2)
    estate._cmcePartial = ""
    estate._cmceLastDone = ""
    estate.setCmceCompleted(ymdh)
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  print("END processing CMCE ", ymdh)
  return True

#----------------------------------------------------------------------------
def runGefs(ymdh, eparms, my_env):

  estate = epochstate.EpochState()
  estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)

  # is this one completely done?
  if estate.hasGefs(ymdh):
    print("SKIP processing GEFS ", ymdh, "..Already completed")
    return True

  # is it done within a partial restart?
  if estate.hasCompletedGefs(ymdh):
    print("SKIP processing GEFS ", ymdh, "..Already processed within this restart")
    return True
  
  print("BEGIN processing GEFS ", ymdh)
  hh = ymdh[8:10]
  yyyymmdd = ymdh[0:8]

  # is this one partially done?
  if estate._gefsPartial == ymdh:
    print("Partial GEFS for ", ymdh)
    print("Last done = ", estate._gefsLastDone)
  else:
    estate.setGefsInProgress(ymdh)
    estate._gefsPartial = ymdh
    estate._gefsLastDone = ""
    estate.write(my_env['workspace_state'])
    estate.write(my_env['COMOUTrestartS']) #eparms._epochStateFile)

  if estate._gefsLastDone == "":
    convertGefs(ymdh, eparms, estate, my_env)
    # read back in state file which was changed by convertGefs
    estate = epochstate.EpochState()
    estate.readOrCreate(my_env['workspace_state'])#eparms._epochStateFile)
    estate._gefsLastDone = "Grib2toMdv"
    # write the MDV ensemble model data to COMOUTrestartS
    topPath = my_env['DATA'] + '/EpochOps'
    subpath = 'mdv/model/gefs'
    recursiveCopyMdvEnsFcst("GEFS", topPath, my_env['COMOUTrestart'], subpath, yyyymmdd, hh)
    estate.write(my_env['workspace_state'])
    estate.write(my_env['COMOUTrestartS'])#eparms._epochStateFile)

  if estate._gefsLastDone == "Grib2toMdv":
    doCommandWithInterval("PrecipAccumCalc", "gefs", ymdh, eparms, my_env)
    estate._gefsLastDone = "PrecipAccumCalc"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    # write the MDV ensemble model data to COMOUTrestartS
    topPath = my_env['DATA'] + '/EpochOps'
    subpath = 'mdv/model/gefs3hr/3hrAccum'
    recursiveCopyMdvEnsFcst("GEFS", topPath, my_env['COMOUTrestart'], subpath, yyyymmdd, hh)
    estate.write(my_env['workspace_state'])
    estate.write(my_env['COMOUTrestartS'])

  if estate._gefsLastDone == "PrecipAccumCalc":
    # with new gefs data, we now run EnsLookupGen, both precip and cloud top, no checking
    doCommandWithInterval("EnsLookupGen", "GEFS", ymdh, eparms, my_env)
    # copy to COMOUTrestart, as we need to combine previous times output in the combine step
    recursiveCopyMdvFcst(my_env['WORKSPACE'], my_env['COMOUTrestart'], "mdv/model/gefsProbOpt", yyyymmdd, hh)
    estate._gefsLastDone = "EnsLookupGen.GEFS"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._gefsLastDone == "EnsLookupGen.GEFS":
    doCommandWithInterval("EnsLookupGen", "GEFS-cloudtop", ymdh, eparms, my_env)
    recursiveCopyMdvFcst(my_env['WORKSPACE'], my_env['COMOUTrestart'], "mdv/model/gefsProbCloudTopOpt", yyyymmdd, hh)
    estate._gefsLastDone = "EnsLookupGen.GEFS-cloudtop"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._gefsLastDone == "EnsLookupGen.GEFS-cloudtop":
    # we also run PbarCompute
    doCommandWithInterval("PbarCompute", "GEFS", ymdh, eparms, my_env)
    copySpdb(my_env['COMOUTpbarGefs'], my_env['COMOUTpbarGefsActual'], ymdh[0:8], eparms, my_env)
    estate._gefsLastDone = "PbarCompute"
    estate._lastCompleted = "CMCE"
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if estate._gefsLastDone == "PbarCompute":
    # indicate a new model run in the python param file, we are all done
    if eparms._debugLevel >= 1:
      print("Finishing up GEFS for ", ymdh)
    estate.addGEFS(ymdh, eparms._debugLevel >= 2)
    estate.setGefsCompleted(ymdh)
    estate._gefsPartial = ""
    estate._gefsLastDone = ""
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  return True
  
#----------------------------------------------------------------------------
def checkForPartial(ymdh, estate, workspace, comoutPath, my_env):
  isPartial = False
  if estate._currentPartial:
    if estate._currentPartial != ymdh:
      print("WARNING, partial run from previous time was aborted do not finish previous ymdh:", estate._currentPartial)
      estate.clearPartial()
      estate.start(ymdh)
      estate.write(my_env['workspace_state'], False)#eparms._epochStateFile)
      estate.write(my_env['COMOUTrestartS'])
    else:
      isPartial = True
  else:
    estate.start(ymdh)
    estate.write(my_env['workspace_state'], False)#eparms._epochStateFile)
    estate.write(my_env['COMOUTrestartS'])

  if isPartial:
    print("Restarting a partially completed run now for ", estate._currentPartial)
    
    if not os.path.exists(workspace):
      print("WARNING restarting a partially completed run, but no workspace ", workspace, " so need to redo entire run")
      makeDirIfNeeded(workspace)
      isPartial = false
      estate.clearPartial()
      estate.start(ymdh)
      estate.write(my_env['workspace_state'], False)#eparms._epochStateFile)
      estate.write(my_env['COMOUTrestartS'])
    if not os.path.exists(comoutPath):
      print("WARNING restarting a partially completed run, but no output directory ", comout, " may have missing results")
      makeDirIfNeeded(comoutPath)

  return isPartial
  
#----------------------------------------------------------------------------
def repopulateWorkspace(ymdh, estate, isPartial, comoutPath, workspace, comoutHeadLen, my_env):

  # repopulate workspace from previous days
  #
  #
  # It'd be better to use old thresholds, than to not use any and instead use coldstart values.
  # Therefore, do not do the following, even though it would speed up restarts:
  #    if isPartial and estate.beyondThresholdComputations():
  #        # go back only a few days for SPDB.  If thresholds are not computed from 4 days ago for some reason, it should go to the cold start values.
  #       new_previousYmd = ['PDY','PDYm1','PDYm2', 'PDYm3', 'PDYm4']
  #else:

  # go back a month for SPDB in case we do thresholds averaging, and to make sure we have some thresholds to use.
  new_previousYmd = ['PDY','PDYm1','PDYm2','PDYm3','PDYm4','PDYm5','PDYm6','PDYm7','PDYm8','PDYm9','PDYm10','PDYm11','PDYm12','PDYm13','PDYm14','PDYm15','PDYm16','PDYm17','PDYm18','PDYm19','PDYm20','PDYm21','PDYm22','PDYm23','PDYm24','PDYm25','PDYm26','PDYm27','PDYm28','PDYm29','PDYm30']
    
  ymd = ymdh[0:8]
  previousYmd = []

  for n in new_previousYmd:
    previousYmd.append(my_env[n])
    
  # keep this pretty big in case GFS goes down for a few days, or LIR is behind by a few days (which should not happen),
  # so that LIR will find something to work with in either case
  previousYmdMdv = [my_env['PDYm3'],my_env['PDYm2'], my_env['PDYm1'], my_env['PDY']]

  #print(previousYmd)
  
  # sort these previous ones from oldest to newest, NOTE this MUST be done oldest to newest so that SPDB will update correctly.
  previousYmd.reverse()

  print('previousYmd:',previousYmd)
  print('previousYmdMdv:',previousYmdMdv)
  
  # now copy all the previous days into the workspace, oldest to newest
  print('comin:',my_env['COMINepoch'])
  comoutHead = my_env['COMINepoch']
  for p in previousYmd:
    cprev = comoutHead + '/epoch.' + p + '/spdb'
    if os.path.exists(cprev):
      print("Recursively copying data from ", cprev, " to ", workspace + '/spdb')
      recursiveCopy(cprev, workspace + '/spdb')
    else:
      print("No data to copy from ", cprev, " it does not exist")

  for p in previousYmdMdv:
    cprev = comoutHead + '/epoch.' + p + '/mdv'
    if os.path.exists(cprev):
      print("Recursively copying data from ", cprev, " to ", workspace + '/mdv')
      recursiveCopy(cprev, workspace + '/mdv')
    else:
      print("No data to copy from ", cprev, " it does not exist")
  
#----------------------------------------------------------------------------
def runEpoch(ymdh, parmfile):

  print("Starting exepoch-7.py for ", ymdh)

  my_env = os.environ.copy()
  workspace = my_env['WORKSPACE']
  cominPath = my_env['COMINepoch']
  comoutPath = my_env['COMOUT']
  print("Env for WORKSPACE = ", workspace)
  print("Env for COMOUT = ", comoutPath)
  print("Env for PDY = ", my_env['PDY'])
  print("Env for GFS_MODEL_DIR = ", my_env['GFS_MODEL_DIR'])
  print("Env for GFSB_MODEL_DIR = ", my_env['GFSB_MODEL_DIR'])
  print("Env for GEFSA_MODEL_DIR = ", my_env['GEFSA_MODEL_DIR'])
  print("Env for CMCE_MODEL_DIR = ", my_env['CMCE_MODEL_DIR'])
  
  eparms = epochparms.EpochParms()
  eparms.read(parmfile)
  #eparms.printParms()
  if (not eparms._ok):
    print("FATAL ERROR: reading main Epoch parm file ", parmfile)
    return 1
  else:
    print("Did read main Epoch parm file ", parmfile)

  if (not os.path.exists(my_env['PARMepoch'])):  #eparms._parms_dir)):
    print("FATAL ERROR: parms path does not exist ", my_env['PARMepoch'])
    return 1

  # empty initial states
  estate = epochstate.EpochState()
  einputstate = epochinputstate.EpochInputState()

  # expect $COMOUT to be <path>/epoch.ymd
  ymd = ymdh[0:8]

  debug2 = eparms._debugLevel >= 2

  tail = 'epoch.' + ymd
  comoutHeadLen= comoutPath.find(tail)
  if comoutHeadLen == -1:
    print("FATAL ERROR: expected COMOUT=", comoutPath, " to end with ", tail)
    return 1

  # might need to create the current comoutPath
  makeDirIfNeeded(comoutPath, debug2)

  # might need to create the workspace, and some subdirs
  # and data, and some subdirs
  makeDirIfNeeded(workspace, debug2)
  makeDirIfNeeded(workspace + '/mdv', debug2)
  makeDirIfNeeded(workspace + '/spdb', debug2)
  makeDirIfNeeded(my_env['DATA'], debug2)
  makeDirIfNeeded(my_env['DATA'] + '/EpochOps', debug2)
  makeDirIfNeeded(my_env['DATA'] + '/EpochOps/mdv', debug2)

  # check for crash
  if os.path.exists(my_env['COMINrestart']):
    # yes crash, copy state files from restart area to workspace
    estate.readOrCreate(my_env['COMINrestart']+'/Epoch.state')
    print('Copying %s/Epoch.state into workspace' % (my_env['COMINrestart']))
    cmd='cpreq %s/Epoch.state %s' % (my_env['COMINrestart'], workspace)
    os.system(cmd)
    #read after copying in
    estate.readOrCreate(my_env['workspace_state'])

    print('Copying %s/EpochInputs.state into workspace' % (my_env['COMINrestart']))
    cmd='cpreq %s/EpochInputs.state %s' % (my_env['COMINrestart'], workspace)
    os.system(cmd)
    #read after copying in
    einputstate.readOrCreate(my_env['workspace_inputstate'])

    # copy mdv/gefsProbOpt and mdv/gefsProbCloudTopOpt into WORKSPACE
    hh = ymdh[8:10]
    yyyymmdd = ymdh[0:8]
    path = my_env['COMINrestart'] + '/mdv/model/gefsProbOpt'
    if os.path.exists(path):
      recursiveCopyMdvFcst(my_env['COMINrestart'], my_env['WORKSPACE'], 'mdv/model/gefsProbOpt', yyyymmdd, hh)
    path = my_env['COMINrestart'] + '/mdv/model/gefsProbCloudTopOpt'
    if os.path.exists(path):
      recursiveCopyMdvFcst(my_env['COMINrestart'], my_env['WORKSPACE'], 'mdv/model/gefsProbCloudTopOpt', yyyymmdd, hh)

    # copy all the other mdv data into $DATA
    path = my_env['COMINrestart'] + '/mdv/model/gefs'
    if os.path.exists(path):
      recursiveCopyMdvEnsFcst("GEFS", my_env['COMINrestart'], my_env['DATA'] + '/EpochOps', 'mdv/model/gefs', yyyymmdd, hh)
    else:
      print("No data to copy in from ", path)
      
    path = my_env['COMINrestart'] + '/mdv/model/gefs3hr/3hrAccum'
    if os.path.exists(path):
      recursiveCopyMdvEnsFcst("GEFS", my_env['COMINrestart'], my_env['DATA'] + '/EpochOps', 'mdv/model/gefs3hr/3hrAccum', yyyymmdd, hh)
    else:
      print("No data to copy in from ", path)

    path = my_env['COMINrestart'] + '/mdv/model/cmce'
    if os.path.exists(path):
      recursiveCopyMdvEnsFcst("CMCE", my_env['COMINrestart'], my_env['DATA'] + '/EpochOps', 'mdv/model/cmce', yyyymmdd, hh)
    else:
      print("No data to copy in from ", path)

    path = my_env['COMINrestart'] + '/mdv/model/cmce3hr/3hrAccum'
    if os.path.exists(path):
      recursiveCopyMdvEnsFcst("CMCE", my_env['COMINrestart'], my_env['DATA'] + '/EpochOps', 'mdv/model/cmce3hr/3hrAccum', yyyymmdd, hh)
    else:
      print("No data to copy in from ", path)

    path = my_env['COMINrestart'] + '/mdv/model/gfs_0.25b'
    if os.path.exists(path):
      outpath = my_env['DATA'] + '/EpochOps/mdv/model/gfs_0.25b'
      if not os.path.exists(outpath):
        os.makedirs(outpath)
      print("Copying data from ", path, " to ", outpath)
      recursiveCopy(path, outpath)
    else:
      print("No data to copy in from ", path)
  else:
    print('Not a restart.')

    # create the output restart directory now
    makeDirIfNeeded(my_env['COMOUTrestart'], True)

    if os.path.isfile(my_env['COMINstate']):
        print('Copying %s into workspace' % (my_env['COMINstate']))
        cmd='cpreq %s %s' % (my_env['COMINstate'], workspace)
        os.system(cmd)
        estate.readOrCreate(my_env['workspace_state'])
    else:
        estate.readOrCreate(my_env['workspace_state'])
    if os.path.isfile(my_env['COMINinputstate']):
        print('Copying %s into workspace' % (my_env['COMINinputstate']))
        cmd='cpreq %s %s' % (my_env['COMINinputstate'], workspace)
        os.system(cmd)
        einputstate.readOrCreate(my_env['workspace_inputstate'])
    else:
        einputstate.readOrCreate(my_env['workspace_inputstate'])

  # read state (now in workspace) to see if it is a partially done restart

  isPartial = checkForPartial(ymdh, estate, workspace, comoutPath, my_env)
  # reread, as it might have been changed in checkForPartial
  estate.readOrCreate(my_env['workspace_state'])

  if eparms._do_copy_into_workspace:
    repopulateWorkspace(ymdh, estate, isPartial, comoutPath, workspace, comoutHeadLen, my_env)

  # for each 'comout' environment variable, change comout value to a workspace value, and also
  # create a new environment variable for the real thing for use later, with 'Actual' in the name
  # note these are all hardwired to agree with J_EPOCH
  setupDoubleEnv('COMOUTmerged', 'COMOUTmergedActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTgfs0p25a', 'COMOUTgfs0p25aActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTsatellite', 'COMOUTsatelliteActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTcmorph', 'COMOUTcmorphActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTpbarCmce', 'COMOUTpbarCmceActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTpbarGefs', 'COMOUTpbarGefsActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshApcpCmce', 'COMOUTthreshApcpCmceActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshApcpGefs', 'COMOUTthreshApcpGefsActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshHistApcpCmce', 'COMOUTthreshHistApcpCmceActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshHistApcpGefs', 'COMOUTthreshHistApcpGefsActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshUlwrfCmce', 'COMOUTthreshUlwrfCmceActual' , comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshUlwrfGefs', 'COMOUTthreshUlwrfGefsActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshHistUlwrfCmce', 'COMOUTthreshHistUlwrfCmceActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTthreshHistUlwrfGefs', 'COMOUTthreshHistUlwrfGefsActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTcmceProbOpt', 'COMOUTcmceProbOptActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTgefsProbOpt', 'COMOUTgefsProbOptActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTcmceProbCloudTopOpt', 'COMOUTcmceProbCloudTopOptActual', comoutPath, workspace, my_env, debug2)
  setupDoubleEnv('COMOUTgefsProbCloudTopOpt', 'COMOUTgefsProbCloudTopOptActual', comoutPath, workspace, my_env, debug2)

  # make temporary output path if not there, this is RAP_DATA_DIR
  if not makeDirIfNeeded(my_env['DATA'], debug2):#eparms._temp_data_dir
    return 1

  # DONT DO THIS YET, put in for use later
  ##runThresholdsBackfill(ymdh, eparms, my_env)

  retStat = 1
  if len(ymdh) == 10:
    # what to do depends on hour
    hh = ymdh[8:10]
    if hh == '00' or hh == '12':
      checkInputs(ymdh, eparms, my_env)
      runCmce(ymdh, eparms, my_env)
      runGefs(ymdh, eparms, my_env)
      retStat = 0
    elif hh == '06' or hh == '18':
      checkInputs(ymdh, eparms, my_env)
      runGefs(ymdh, eparms, my_env)
      retStat = 0
    else:
      print("Invalid hour in yyyymmddhh input, want 00, 06, 12, or 18 got ", hh)
      estate.clearPartial()
      estate.write(my_env['workspace_state'])#eparms._epochStateFile)
      retStat = 1

    if retStat == 0:
      runCombine(ymdh, eparms, my_env)

    # if make it here, the ymdh is done, read state in because it did change
    estate = epochstate.EpochState()
    estate.readOrCreate(my_env['workspace_state'])
    estate.checkForComplete(ymdh)
    estate.clearPartial()
    estate.write(my_env['workspace_state'])#eparms._epochStateFile)
    pruneEpochState(ymdh, eparms, my_env)
  else:
    print("Invalid format of yyyymmddhh input, not 10 characters: ", ymdh)
    retStat = 1

  # delete this every time if you get this far (if configured to do so)
  if eparms._do_delete_temp_data_dir:
    doDeleteDataDir(my_env['DATA'])#eparms._temp_data_dir
  
  # copy these out prior to exit every time
  cmd='cpreq %s %s' \
  % (my_env['workspace_inputstate'],my_env['COMOUTgrib2'])
  os.system(cmd)
  cmd='cpreq %s %s' \
  % (my_env['workspace_state'],my_env['COMOUTgrib2'])
  os.system(cmd)

  # delete this every time if you get this far (if configured to do so)
  if eparms._do_delete_workspace:
    doDeleteDataDir(workspace)

  # Delete restart dir if completed
  print('All files and processes were completed. Deleting restart dir...')
  cmd='rm -rf %s/restart' % (my_env['COMOUTgrib2'])
  os.system(cmd)
  
  # Temporary fix to delete tmpnwprd
  cmd='rm -rf %s/tmpnwprd' % (my_env['PACKAGEROOT'])
  os.system(cmd)

  return retStat

#----------------------------------------------------------------------------
def main(argv):
  parser = argparse.ArgumentParser()
  parser.add_argument("-t", help = "yyyymmddhh")
  parser.add_argument("-p", help = "python parameter file")
  args = parser.parse_args()
  usage = "Usage: epoch.py -t YYYYMMDDHH -p PARMFILE"
  isError = False
  if args.p == None:
    print( "FATAL ERROR: need '-p PARMFILE' arguments")
    isError = True
  if args.t == None:
    print( "FATAL ERROR: need '-t YYYYMMDDHH' arguments")
    isError = True
  if isError:
    print(usage)
    return 1
  else:
    return runEpoch(args.t, args.p)

#----------------------------------------------
if __name__ == "__main__":
   main(sys.argv[1:])
