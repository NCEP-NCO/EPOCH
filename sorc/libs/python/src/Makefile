# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:28 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#---------------------------------------------------------------------
#
# Makefile for script files
#
#---------------------------------------------------------------------

#	Default INSTALL_DIR, should be overridden on command line

INSTALL_DIR=$(RAP_BIN_DIR)

#	Default LIB install dir for Python library

INSTALL_LIB_DIR=$(RAP_LIB_DIR)/python

#	Script files to be installed

SUB_DIRS = \
	icing \
	ConfigMaster

LIBS = \
	ftptools.py \
	ldata.py \
	pmu.py \
	ral_datetime.py \



#	Targets

$(TARGETS): FORCE
	$(MAKE) $(SUB_DIRS) TARGET=$@

$(SUB_DIRS): FORCE
	cd $@; $(MAKE) $(TARGET)

FORCE:

all: install

install_shared:
	make RAP_BIN_DIR=$(RAP_SHARED_BIN_DIR) RAP_LIB_DIR=$(RAP_SHARED_LIB_DIR) install

install: $(SUB_DIRS) create_subdir create_sub_libdir
	if test $(INSTALL_LIB_DIR) != . ;\
	then \
		cp $(LIBS) $(INSTALL_LIB_DIR) ;\
	fi	

create_subdir:
	if test -d $(INSTALL_DIR) ;\
	then \
		cd . ;\
	else \
		mkdir $(INSTALL_DIR) ;\
	fi 

create_sub_libdir:
	if test -d $(INSTALL_LIB_DIR) ;\
	then \
		cd . ;\
	else \
		mkdir -p $(INSTALL_LIB_DIR) ;\
	fi

install_bin: install
install_shared_bin: install_shared

# unused targets - needed to prevent make failures

clean:
debug:
dbgopt:
profile:
opt:
lint:
depend:
pure:
insure:
install_include:
clean_bin:
clean_lib:
clean_tdrp:
clean_insure:
clean_depend:
clean_all:


