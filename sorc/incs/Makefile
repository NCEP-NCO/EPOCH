# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:28 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#
# installs the includes
#

include $(RAP_MAKE_INC_DIR)/rap_make_macros

install: mk_inc_dir FORCE
	$(CP) *.h $(RAP_INC_DIR)
	if $(TEST) -d raptemplates; then \
		$(CP) raptemplates/*.h $(RAP_INC_DIR)/raptemplates ;\
	fi

mk_inc_dir:
	if $(TEST) -d $(RAP_INC_DIR); then :; else \
		$(MKDIR) $(RAP_INC_DIR); fi
	if $(TEST) -d $(RAP_INC_DIR)/raptemplates; then :; else \
		$(MKDIR) $(RAP_INC_DIR)/raptemplates; fi
install_shared:
	$(MAKE) RAP_INC_DIR=$(RAP_SHARED_INC_DIR) install

FORCE:

#
# Just here for compatibility - Niles.
#
install_include:
clean:
staticopt:
dbgopt:


