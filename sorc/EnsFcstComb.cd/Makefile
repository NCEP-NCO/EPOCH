
###########################################################################
#
# Makefile for EnsFcstGen program
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_CPPC_CFLAGS = -I. -Wall  -fpermissive  -std=c++11
LOC_CFLAGS = $(LOC_CPPC_CFLAGS) -D$(HOST_OST) 
SYS_CFLAGS = -g -D$(HOST_OS)
LOC_INCLUDES = $(NETCDF4_INCS)

LOC_LIBS = -lConvWxIO -lConvWx -lConvWxParams -ldsdata \
	-lSpdb -lMdv -ldsserver -ldidss \
	-lrapmath -ldataport -lrapformats -ltoolsa \
	-leuclid -ltdrp -lc -lrapmath \
	-lMdv -lRadx $(NETCDF4_LIBS) -lbz2 \
	-lz -lpthread -lm

LOC_LDFLAGS = $(NETCDF4_LDFLAGS)


MODULE_TYPE=progcpp

TARGET_FILE=EnsFcstComb

HDRS = \
	$(PARAMS_HH) \
	ParmsEnsFcstCombIO.hh \
	ParmsEnsFcstComb. hh \
	EnsFcstCombMgr.hh \

CPPC_SRCS = \
	$(PARAMS_CC) \
	ParmsEnsFcstCombIO.cc \
	EnsFcstCombMgr.cc \
	MainEnsFcstComb.cc

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

