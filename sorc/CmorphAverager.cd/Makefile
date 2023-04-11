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

include $(RAP_MAKE_INC_DIR)/rap_make_macros
include ../make.cppcheck

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

TARGET_FILE = CmorphAverager


HDRS = \
	$(PARAMS_HH) \
	ParmsCmorphAverager.hh \
	CmorphAveragerMgr.hh 

CPPC_SRCS = \
	$(PARAMS_CC) \
	ParmsCmorphAveragerIO.cc \
	Main.cc \
	CmorphAveragerMgr.cc

#
# tdrp macros
#

include $(RAP_MAKE_INC_DIR)/rap_make_tdrp_macros

#
# C++ targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_c++_targets

#
# tdrp targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_tdrp_c++_targets

#
# local targets
#

#depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.

