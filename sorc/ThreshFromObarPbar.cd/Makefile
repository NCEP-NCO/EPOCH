
###########################################################################
#
# Makefile for ThreshFromObarPbar program
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

TARGET_FILE=ThreshFromObarPbar

HDRS = \
	$(PARAMS_HH)

CPPC_SRCS = \
	$(PARAMS_CC) \
	Info.cc \
	LeadtimeThreadData.cc \
	LeadtimeThreadDataForField.cc \
	MainThreshFromObarPbar.cc \
	ParmsThreshFromObarPbar.cc \
	ParmsThreshFromObarPbarIO.cc \
	PbarVector.cc \
	State.cc \
	ForecastState.cc \
	ThreshFromObarPbarMgr.cc \
	ThresholdForEachTile.cc 

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

