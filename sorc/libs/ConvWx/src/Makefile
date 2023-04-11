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
#
# Recursive make - makes the subdirectory code
#

include $(RAP_MAKE_INC_DIR)/rap_make_macros

TARGETS = $(GENERAL_TARGETS) 

MODULE_NAME = ConvWx

LIBNAME = lib$(MODULE_NAME).a

SUB_DIRS = ConvWx

include $(RAP_MAKE_INC_DIR)/rap_make_recursive_dir_targets

include $(RAP_MAKE_INC_DIR)/rap_make_inc_targets
include $(RAP_MAKE_INC_DIR)/rap_make_lib_targets
