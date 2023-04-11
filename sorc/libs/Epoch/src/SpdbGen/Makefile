# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:50 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
######################################################################
#
# Makefile for Threshold Mapping
#
#
######################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INCLUDES = -I../include
LOC_CFLAGS =
LOC_INC_DIR = ../include 

TARGET_FILE = ../libEpoch.a

#
# file lists
#

HDRS = 

CPPC_SRCS = \
	HistGenTime.cc \
	MultiObarThreshTileThresholds.cc \
	MultiTileThresholdsGenBased.cc \
	PbarAtLead.cc \
	PbarAtLead2.cc \
	PbarAtLeadThresh.cc \
	PbarAtLeadThresh2.cc \
	SingleTileThresholdsGenBased.cc \
	SpdbGenBasedMetadata.cc \
	SpdbGenBasedThreshHandler.cc \
	SpdbObsHandler.cc \
	SpdbObsMetadata.cc \
	SpdbPbarHandler.cc \
	SpdbPbarHandler2.cc \
	SpdbPbarMetadata.cc \
	SpdbPbarMetadata2.cc \
	ThreshObarInfo.cc \
	ThresholdDatabaseParms.cc \
	ThresholdDatabaseParmsIO.cc \
	TileObarInfo.cc \
	TileThreshInfoGenBased.cc



#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets

#
# local targets
#

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
