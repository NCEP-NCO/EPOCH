
###########################################################################
#
# Makefile for FSSClimo program
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros
include ../make.cppcheck

LOC_CPPC_CFLAGS = -I. -Wall -fpermissive 
LOC_CFLAGS = $(LOC_CPPC_CFLAGS) -D$(HOST_OST)
SYS_CFLAGS = -g -D$(HOST_OS)
LOC_INCLUDES = $(NETCDF4_INCS) $(HDF5_INCS)

LOC_LIBS = -lConvWxIO -lConvWx -lConvWxParams -ldsdata \
	-lSpdb -lMdv -ldsserver -ldidss \
	-lrapmath -ldataport -lrapformats -ltoolsa \
	-ldataport -leuclid -ltdrp -lc -lRadx \
	-lbz2 -lz -lm -lpthread $(NETCDF4_LIBS) 

LOC_LDFLAGS = $(NETCDF4_LDFLAGS)  


MODULE_TYPE=progcpp

TARGET_FILE=PrecipAccumCalc

HDRS = \
	$(PARAMS_HH) \
	InputDataState.hh \
	ParmsPrecipAccumCalcIO.hh \
	ParmsPrecipAccumCalc.hh \
	PrecipAccumCalcMgr.hh \
	PrecipAccumCalc.hh \
	CalcInstanceInfo.hh

CPPC_SRCS = \
	$(PARAMS_CC) \
	InputDataState.cc \
	ParmsPrecipAccumCalcIO.cc \
	ParmsPrecipAccumCalc.cc \
	PrecipAccumCalcMgr.cc \
	PrecipAccumCalc.cc \
	MainPrecipAccumCalc.cc
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

