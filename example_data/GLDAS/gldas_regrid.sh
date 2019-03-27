#!/usr/bin/env bash


SRC=*.nc4
DOMAIN='../WRF_hydro_domain/geo_em.d01.nc'

ncl 'srcFileName="'$SRC'"'\
 'dstGridName="'$DOMAIN'"' \
GLDAS2WRFHydro_regrid.ncl
