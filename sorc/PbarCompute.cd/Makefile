
###########################################################################
#
# Makefile for PbarCompute program
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros
include ../make_.cppcheck

LOC_CPPC_CFLAGS = -I. -Wall  -fpermissive -std=c++11
LOC_CFLAGS = $(LOC_CPPC_CFLAGS) -D$(HOST_OST)
SYS_CFLAGS = -g -D$(HOST_OS)
LOC_INCLUDES = $(NETCDF4_INCS)

LOC_LIBS = -lEpoch -lConvWxIO -lConvWx -lConvWxParams -lEpoch \
	-ldsdata -lSpdb -lMdv -lRadx -lrapformats \
	-ldsserver -ldidss -leuclid -lrapmath \
	-ltoolsa -ldataport -ltdrp $(NETCDF4_LIBS) -lpthread

LOC_LDFLAGS = $(NETCDF4_LDFLAGS)

MODULE_TYPE=progcpp

TARGET_FILE=PbarCompute

HDRS = \
	$(PARAMS_HH)

CPPC_SRCS = \
	$(PARAMS_CC) \
	MainPbarCompute.cc \
	AdditionalInput.cc \
	AdditionalInputs.cc \
	ForecastState.cc \
	Info.cc \
	LeadtimeThreadData.cc \
	PbarVector.cc \
	ParmsPbarCompute.cc \
	ParmsPbarComputeIO.cc \
	PbarComputeMgr.cc


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

