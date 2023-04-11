
###########################################################################
#
# Makefile for EnsLookupGen program
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros
include ../make_.cppcheck

LOC_CPPC_CFLAGS = -I. -Wall  -fpermissive 
LOC_CFLAGS = $(LOC_CPPC_CFLAGS) -D$(HOST_OST)
SYS_CFLAGS = -g -D$(HOST_OS)
LOC_INCLUDES = $(NETCDF4_INCS)

LOC_LIBS = -lEpoch -lConvWxIO -lConvWx -lConvWxParams \
	-ldsdata -lSpdb -lMdv -lRadx -lrapformats \
	-ldsserver -ldidss -leuclid -lrapmath \
	-ltoolsa -ldataport -ltdrp $(NETCDF4_LIBS) -lpthread


LOC_LDFLAGS = $(NETCDF4_LDFLAGS)


MODULE_TYPE=progcpp

TARGET_FILE=EnsLookupGen

HDRS = \
	$(PARAMS_HH) \
	ParmsEnsLookupGenIO.hh \
	ParmsEnsLookupGen. hh \
	EnsLookupGenMgr.hh \
	MultiThreshField.hh \
	ThreshField.hh

CPPC_SRCS = \
	$(PARAMS_CC) \
	DbThresh.cc \
	GriddedThresh.cc \
	Info.cc \
	MultiThreshInfo.cc \
	ParmsEnsLookupGen.cc \
	ParmsEnsLookupGenIO.cc \
	EnsLookupGenMgr.cc \
	MainEnsLookupGen.cc \
	OutputToThreshProj.cc \
	ThreshForOneObar.cc 

#	ParmsField.cc \

#
# tdrp support
#
include $(RAP_MAKE_INC_DIR)/rap_make_tdrp_macros

#
# general targets
#
include $(RAP_MAKE_INC_DIR)/rap_make_targets

#
# tdrp targets
#
include $(RAP_MAKE_INC_DIR)/rap_make_tdrp_c++_targets

#
# local targets
#

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.

