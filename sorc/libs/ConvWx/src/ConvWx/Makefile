# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
# All rights reserved.  The Government's right to use this data and/or 
# software (the "Work") is restricted, per the terms of Cooperative 
# Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
# Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
# royalty-free license to exercise or have exercised for or on behalf of 
# the U.S. throughout the world all the exclusive rights provided by 
# copyrights.  Such license, however, does not include the right to sell 
# copies or phonorecords of the copyrighted works to the public."   The 
# Work is provided "AS IS" and without warranty of any kind.  UCAR 
# EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
# ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
# PURPOSE.  
#  
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
###########################################################################
#
# Makefile for ConvWX
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INC_DIR = ../include 
MY_CPPCHECKFLAGS = -AddStyleFAA -exclude-rules=4.16
#MY_CPPCHECKFLAGS = -AddStyleFAA

LOC_CPPC_CFLAGS = -I$(LOC_INC_DIR) -Wall 
LOC_CFLAGS = $(LOC_CPPC_CFLAGS)

MODULE_TYPE=library
TARGET_FILE= ../libConvWx.a
HDRS = 

CPPC_SRCS = \
  BlendingModeMean.cc \
  BlendingType.cc \
  ClimoHistRdr.cc \
  ContingencyTable.cc \
  ContingencyTableWindowed.cc \
  ConvThresh.cc \
  ConvWxFuzzy.cc \
  ConvWxFuzzy2.cc \
  ConvWxPath.cc \
  ConvWxPjgAzimEquidistMath.cc \
  ConvWxPjgLambertAzimMath.cc \
  ConvWxPjgLambertConfMath.cc \
  ConvWxPjgLatlonMath.cc \
  ConvWxPjgMath.cc \
  ConvWxProjection.cc \
  ConvWxTime.cc \
  DebugPoint.cc \
  DigitalVilConverter.cc \
  DistStats.cc \
  DistFile.cc \
  DynamicWeightsHandler.cc \
  EdgeFinder.cc \
  FcstGrid.cc \
  FcstInfo.cc \
  FcstThreshInfo.cc \
  FcstThreshMetadata.cc \
  Grid.cc \
  GridAverage.cc \
  GridData.cc \
  GridDistToNonMissing.cc \
  GridsForPc.cc \
  GridLoopA.cc \
  GridLoopAlg.cc \
  GridTraverse.cc \
  GridTraverse2d.cc \
  Histogram.cc \
  InsideBuilder.cc \
  InterfaceLL.cc \
  IsInsidePolygons.cc \
  LpcModelState.cc \
  LpcOutputState.cc \
  LpcState.cc \
  LtInterp.cc \
  LtInterpPoint.cc \
  Merge.cc \
  MetaData.cc \
  MetaDataXml.cc \
  MultiGrid.cc \
  MultiGridsForPc.cc \
  MultiFcstGrid.cc \
  Nbrhd.cc \
  NbrhdClump.cc \
  NbrhdClumps.cc \
  NbrhdValues.cc \
  ParmConv.cc \
  ParmFcst.cc \
  ParmHtml.cc \
  ParmMain.cc \
  ParmPc.cc \
  ParmPcAlg.cc \
  ParmPcFcst.cc \
  ParmSetUV.cc \
  ParmProjection.cc \
  ParmTiling.cc \
  PcInput.cc \
  PcMotion.cc \
  PcScore.cc \
  PcStats.cc \
  PcType.cc \
  PcVolume.cc \
  PhaseCorrect.cc \
  QuantileBins.cc \
  Score.cc \
  Stat.cc \
  Stats.cc \
  Subgrid.cc \
  SubgridValWgtCalculator.cc \
  TriggerSequenceState.cc \
  TriggerState.cc \
  UvHtml.cc \
  UvOutput.cc \
  UvUnits.cc \
  VectorMaxCounts.cc \
  VerifNames.cc \
  VerifThresh.cc \
  Xml.cc \
  XyRun.cc

X=  BlendingState1.cc \
  BlendingStateMean1.cc \
  BlendingStateMedian1.cc \
#
# comment this out as it does stuff not in CVS but leave it around for Dave Al.
# include html_targets
#

#
# general targets
#
include $(RAP_MAKE_INC_DIR)/rap_make_targets

# DO NOT DELETE THIS LINE -- make depend depends on it.

