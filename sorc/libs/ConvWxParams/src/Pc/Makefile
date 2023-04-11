# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:42 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
###########################################################################
#
# Makefile for params for Projection 
#
# Dave Albo
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

# destination for files:
DEST_INC_DIR = ../include/ConvWxParams

TARGET_FILE=../libConvWxParams.a
TDRP_FILE = ParamsPc

PARAMS_HH = $(TDRP_FILE).hh
PARAMS_CC = $(TDRP_FILE).cc
CPPC_SRCS = $(PARAMS_CC) 

all: $(PARAMS_CC) 

# (PARAMS_HH): (PARAMS_CC)
# (PARAMS_CC): paramdef.(TDRP_FILE)
# 	tdrp_gen -f paramdef.(TDRP_FILE) -c++ -class (TDRP_FILE)
# 	(CP) (PARAMS_HH) (DEST_INC_DIR)

# clean_tdrp:
# 	(CP) (PARAMS_CC) (PARAMS_HH)

#
# general targets
#
include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets


#
# local targets
#

# DO NOT DELETE THIS LINE -- make depend depends on it.
