#
# Recursive make
#

include $(RAP_MAKE_INC_DIR)/rap_make_macros

TARGETS = $(GENERAL_TARGETS) $(LIB_TARGETS) $(DOC_TARGETS) $(INSTALL_TARGETS)

SUB_DIRS = \
	$(sort $(dir $(wildcard */)))

include $(RAP_MAKE_INC_DIR)/rap_make_recursive_dir_targets