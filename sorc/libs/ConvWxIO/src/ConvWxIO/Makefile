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
# Makefile for ConvWXIO
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INC_DIR = ../include 
MY_CPPCHECKFLAGS = -E InterfaceIO.cc -E InterfaceParm.cc

LOC_CPPC_CFLAGS = -I$(LOC_INC_DIR) -Wall -DALL 
LOC_CFLAGS = $(LOC_CPPC_CFLAGS)

MODULE_TYPE=library
TARGET_FILE= ../libConvWxIO.a
HDRS = 

CPPC_SRCS = \
  BlendingHtml.cc \
  BlendingHtmlLine.cc \
  BlendingInputData.cc \
  CiFlagHandler.cc \
  ConvWxMultiThreadMgr.cc \
  ConvWxThreadMgr.cc \
  DynamicWeightsIO.cc \
  EarthRadius.cc \
  PhaseCorrectHtml.cc \
  FcstGridReader.cc \
  FcstState.cc \
  FcstWait.cc \
  FcstWithLatencyState.cc \
  InterfaceIO.cc \
  InterfaceParm.cc \
  LpcStateIO.cc \
  ParmFcstIO.cc \
  ParmPcFcstIO.cc \
  ParmSetUvIO.cc \
  PathRead.cc \
  Trigger.cc


#
# general targets
#
include $(RAP_MAKE_INC_DIR)/rap_make_targets

# DO NOT DELETE THIS LINE -- make depend depends on it.

