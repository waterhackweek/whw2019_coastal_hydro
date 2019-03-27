#!/bin/bash
FILES=./GLDAS/*.nc4
for f in $FILES
do
#  echo "Processing $f file..."
  cdo -v -f grb -copy $f $f.grb
done

