#!/bin/bash

rm CloudHt.cd/CloudHt
rm CmorphAverager.cd/CmorphAverager
rm EnsFcstComb.cd/EnsFcstComb
rm EnsLookupGen.cd/EnsLookupGen
rm GmgsiNcf2Mdv.cd/GmgsiNcf2Mdv
rm Grib2toMdv.cd/Grib2toMdv
rm MdvMerge2.cd/MdvMerge2
rm MdvResample.cd/MdvResample
rm MdvTComp.cd/MdvTComp
rm MdvThresh.cd/MdvThresh
rm MdvtoGrib2.cd/MdvtoGrib2
rm NetCDF2Mdv.cd/NetCDF2Mdv
rm ObarCompute.cd/ObarCompute
rm PbarCompute.cd/PbarCompute
rm PrecipAccumCalc.cd/PrecipAccumCalc

find . -name "*.o" -delete
find . -name "*.a" -delete
