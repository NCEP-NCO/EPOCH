
###########################################################################
#
# Makefile for ObarCompute
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros
include ../make_.cppcheck

LOC_INC_DIR = .
LOC_CPPC_CFLAGS = -I. -Wall $(SYS_XVIEW_INCLUDES) 
LOC_CFLAGS = $(LOC_CPPC_CFLAGS) -D$(HOST_OST)
SYS_CFLAGS = -g -D$(HOST_OS) 
LOC_INCLUDES = $(NETCDF4_INCS)

LOC_LIBS = -lEpoch -lConvWxIO -lConvWx -lConvWxParams \
	-ldsdata -lSpdb -lMdv -lRadx -lrapformats \
	-ldsserver -ldidss -leuclid -lrapmath \
	-ltoolsa -ldataport -ltdrp $(NETCDF4_LIBS) -lpthread

LOC_LDFLAGS = $(NETCDF4_LDFLAGS)

MODULE_TYPE=progcpp
HDRS =

TARGET_FILE=ObarCompute

CPPC_SRCS = \
$(PARAMS_CC) \
MainObarCompute.cc \
ObarForEachThresh.cc \
ObarForEachTile.cc \
ParmsObarCompute.cc \
ParmsObarComputeIO.cc \
ObarComputeMgr.cc \



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

