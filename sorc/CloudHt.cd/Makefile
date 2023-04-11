###########################################################################
#
# Makefile for CloudHt.
#
# Gary Blackburn, RAP, NCAR, Boulder, CO, 80307, USA
#
# March 2002
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

MODULE_TYPE=progcpp

TARGET_FILE = CloudHt

LOC_INCLUDES = $(NETCDF4_INCS)

LOC_LIBS = -ldsdata -lMdv -lRadx -lNcxx -ldidss \
	-lSpdb -lrapformats -leuclid -lrapmath \
	-ltdrp -ldsserver -ldataport -ltoolsa \
	$(NETCDF4_LIBS) -lbz2 -lz

LOC_LDFLAGS = $(NETCDF4_LDFLAGS)

LOC_CFLAGS =
LOC_CPPC_CFLAGS = 

HDRS = \
	Params.hh \
	Args.hh \
	CloudHt.hh

CPPC_SRCS = \
	Params.cc \
	Main.cc \
	Args.cc \
	CloudHt.cc

#
# standard targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_targets

#
# local targets
#

depend: depend_generic

#
# TDRP support
#

# Params.cc: paramdef.(TARGET_FILE)
# 	tdrp_gen -f paramdef.(TARGET_FILE) -c++ \
# 		-prog (TARGET_FILE)

# clean_tdrp:
# 	(RM) Params.hh Params.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.

