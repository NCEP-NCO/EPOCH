# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 17:4:51 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#
# Recursive make - makes the subdirectory code
#

include $(RAP_MAKE_INC_DIR)/rap_make_macros

TARGETS = $(GENERAL_TARGETS) $(LIB_TARGETS) $(DOC_TARGETS) $(INSTALL_TARGETS)


SUB_DIRS = \
	ConvWx \
	ConvWxIO \
	ConvWxParams \
	Epoch \
	Fmq \
	Mdv \
	Ncxx \
	Radx \
	Spdb \
	dataport \
	didss \
	dsdata \
	dsserver \
	euclid \
	grib2 \
	perl5 \
	physics \
	python \
	radar \
	rapformats \
	rapmath \
	tdrp \
	toolsa 

$(TARGETS): FORCE
	$(MAKE) $(SUB_DIRS) TARGET=$@

$(SUB_DIRS): FORCE
	cd $@; $(MAKE) $(TARGET)

FORCE:

