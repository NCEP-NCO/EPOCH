#!/bin/bash
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:28 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

set -x
module reset
module use .
source ../versions/build.ver
module load envvar/${envvar_ver}

module load PrgEnv-intel/${PrgEnv_intel_ver}
module load craype/${craype_ver}
module load intel/${intel_ver}
module load cray-mpich/${cray_mpich_ver}

module load gsl/${gsl_ver}
module load netcdf/${netcdf_ver}
module load hdf5/${hdf5_ver}
module load udunits/${udunits_ver}
module load nco/${nco_ver}
module load jasper/${jasper_ver}
module load prod_util/${prod_util_ver}
module load wgrib2/${wgrib2_ver}
module load cray-fftw/${cray_fftw_ver}


module list

module load epoch_env.lua
echo ${REPO_DIR}  

# Install Libs

cd $EPOCH_SRC_DIR/libs/perl5
make install_shared

cd $EPOCH_SRC_DIR/libs/python
make install_shared

cd $EPOCH_SRC_DIR/libs
make install_include

cd $EPOCH_SRC_DIR/libs
make opt install

cd $EPOCH_SRC_DIR/incs
make install


# Install apps

function build_dir () {
	cd ${EPOCH_SRC_DIR}/$1
	make opt
	make install
	make clean
	if [ $? -ne 0 ]; then
		echo "ERROR: build of $1 FAILED!"
	fi
}

cd $EPOCH_SRC_DIR
for source_dir in *.cd; do
	echo $source_dir
	build_dir $source_dir
done




# check on build results
echo Looking in RAP_BIN_DIR: $RAP_BIN_DIR
ls -l $RAP_BIN_DIR

echo Looking in RAP_LIB_DIR: $RAP_LIB_DIR
ls -l $RAP_LIB_DIR

echo Looking in RAP_LIB_DIR/python: $RAP_LIB_DIR/python
ls -l $RAP_LIB_DIR/python

