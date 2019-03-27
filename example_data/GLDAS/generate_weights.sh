#!/usr/bin/env bash

FILE=`find . -name '*.nc4' | head -1`
#FILE=$(pwd)/GLDAS_NOAH025_3H.A20190228.1500.021.nc4
DOMAIN='../WRF_hydro_domain/geo_em.d01.nc'

echo "-------------------------------"
echo "Generating regridding weights"
echo "  GRIB File   - $FILE"
echo "  DOMAIN File - $DOMAIN"
echo "-------------------------------"
ncl 'interp_opt="bilinear"' \
'srcGridName="'$FILE'"' \
'dstGridName="'$DOMAIN'"' \
GLDAS_WRFHydro_generate_weights.ncl
