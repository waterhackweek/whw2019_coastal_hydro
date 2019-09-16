//#include "gdal_priv.h"
#ifndef Num_H
#define Num_H

#include "gdal_priv.h"
#include "gdalwarper.h"


class Geotiff {
    private:
        const char* filename;
        GDALDataset *geotiffDataset;
        double geotransform[6];
        int dimensions[3];
        int NROWS;
        int NCOLS;
        int NLEVELS;
        double NORTH, SOUTH, EAST, WEST, XRES, YRES, NODATA;
    public:
        std::vector<Pixel> cell;
        std::vector<double> lat;
        std::vector<double> lon;
        std::vector<double> elev;
        Geotiff();
        Geotiff( const char* tiffname );
        ~Geotiff();
        int size() { return cell.size(); }
        const char *GetFileName();
        const char *GetProjection();
        double *GetGeoTransform();
        double GetNoDataValue();
        int *GetDimensions();
        float** GetRasterBand(int z);
        template<typename T> float** GetArray2D(int layerIndex,float** bandLayer);
};

#endif
