#include "iostream"
#include "string"
#include "cpl_conv.h"
#include "stdlib.h"
#include "vicmet.hpp"

using namespace std;
typedef std::string String;


// define constructor function to instantiate object
// of this Geotiff class.
Geotiff::Geotiff( const char* tiffname ) : filename(tiffname) {
    filename = tiffname ;
    GDALAllRegister();
 
    // set pointer to Geotiff dataset as class member.
    geotiffDataset = (GDALDataset*) GDALOpen(filename, GA_ReadOnly);
 
    // set the dimensions of the Geotiff
    NROWS   = GDALGetRasterYSize( geotiffDataset );
    NCOLS   = GDALGetRasterXSize( geotiffDataset );
    NLEVELS = GDALGetRasterCount( geotiffDataset );
    NODATA = GetNoDataValue();
    
    
    //double* transform = GetGeoTransform();
//    double transform[6];
    //GDALGetGeoTransform(geotiffDataset, transform);
    double* geotransform = GetGeoTransform();
    // x direction
    XRES = geotransform[1];
    WEST = geotransform[0];
    EAST = WEST + (XRES * NCOLS);
    // y direction
    YRES = geotransform[5];
    NORTH = geotransform[3];
    SOUTH = NORTH + (YRES * NROWS);
    
    // read the band data from the geotiff
    float** data = GetRasterBand(1);
    double lat_cell, lon_cell;
    for (int i=0;i<NROWS;i++)
    {
        for (int j=0;j<NCOLS;j++)
        {
            float val = data[i][j];
            if (val != NODATA)
            {
                lat_cell = NORTH + this->YRES * (double)i + this->YRES/2.0;
                lon_cell = WEST + this->XRES * (double)j + this->XRES/2.0;
                
                this->cell.push_back(Pixel(lat_cell,lon_cell,val));
                this->elev.push_back(val);
                this->lat.push_back(lat_cell);
                this->lon.push_back(lon_cell);
            }
        }
    }
    
    
}

    // define destructor function to close dataset,
    // for when object goes out of scope or is removed
    // from memory.
 Geotiff::~Geotiff() {
      // close the Geotiff dataset, free memory for array.
      GDALClose(geotiffDataset);
      GDALDestroyDriverManager();
    }

const char* Geotiff::GetFileName() {
//      /*
//       * function GetFileName()
//       * This function returns the filename of the Geotiff.
//       */
    return filename;
}

const char* Geotiff::GetProjection() {
      /* function const char* GetProjection():
       *  This function returns a character array (string)
       *  for the projection of the geotiff file. Note that
       *  the "->" notation is used. This is because the
       *  "geotiffDataset" class variable is a pointer
       *  to an object or structure, and not the object
       *  itself, so the "." dot notation is not used.
       */
      return geotiffDataset->GetProjectionRef();
}


double* Geotiff::GetGeoTransform() {
      /*
       * function double *GetGeoTransform()
       *  This function returns a pointer to a double that
       *  is the first element of a 6 element array that holds
       *  the geotransform of the geotiff.
       *     adfGeoTransform[0] top left x
       *     adfGeoTransform[1] w-e pixel resolution
       *     adfGeoTransform[2] rotation, 0 if image is "north up"
       *     adfGeoTransform[3] top left y
       *     adfGeoTransform[4] rotation, 0 if image is "north up"
       *     adfGeoTransform[5] n-s pixel resolution
       */
      geotiffDataset->GetGeoTransform(geotransform);
      return geotransform;
}

double Geotiff::GetNoDataValue() {
      /*
       * function GetNoDataValue():
       *  This function returns the NoDataValue for the Geotiff dataset.
       *  Returns the NoData as a double.
       */
      return (double)geotiffDataset->GetRasterBand(1)->GetNoDataValue();
}

int* Geotiff::GetDimensions() {
      /*
       * int *GetDimensions():
       *
       *  This function returns a pointer to an array of 3 integers
       *  holding the dimensions of the Geotiff. The array holds the
       *  dimensions in the following order:
       *   (1) number of columns (x size)
       *   (2) number of rows (y size)
       *   (3) number of bands (number of bands, z dimension)
       */
      dimensions[0] = NROWS;
      dimensions[1] = NCOLS;
      dimensions[2] = NLEVELS;
      return dimensions;
}

float** Geotiff::GetRasterBand(int z) {
 
      /*
       * function float** GetRasterBand(int z):
       * This function reads a band from a geotiff at a
       * specified vertical level (z value, 1 ...
       * n bands). To this end, the Geotiff's GDAL
       * data type is passed to a switch statement,
       * and the template function GetArray2D (see below)
       * is called with the appropriate C++ data type.
       * The GetArray2D function uses the passed-in C++
       * data type to properly read the band data from
       * the Geotiff, cast the data to float**, and return
       * it to this function. This function returns that
       * float** pointer.
       */
 
      float** bandLayer = new float*[NROWS];
      switch( GDALGetRasterDataType(geotiffDataset->GetRasterBand(z)) ) {
        case 0:
          return NULL; // GDT_Unknown, or unknown data type.
        case 1:
          // GDAL GDT_Byte (-128 to 127) - unsigned  char
          return GetArray2D<unsigned char>(z,bandLayer);
        case 2:
          // GDAL GDT_UInt16 - short
          return GetArray2D<unsigned short>(z,bandLayer);
        case 3:
          // GDT_Int16
          return GetArray2D<short>(z,bandLayer);
        case 4:
          // GDT_UInt32
          return GetArray2D<unsigned int>(z,bandLayer);
        case 5:
          // GDT_Int32
          return GetArray2D<int>(z,bandLayer);
        case 6:
          // GDT_Float32
          return GetArray2D<float>(z,bandLayer);
        case 7:
          // GDT_Float64
          return GetArray2D<double>(z,bandLayer);
        default:
          break;
      }
      return NULL;
}

    template<typename T>
    float** Geotiff::GetArray2D(int layerIndex,float** bandLayer) {
 
       /*
        * function float** GetArray2D(int layerIndex):
        * This function returns a pointer (to a pointer)
        * for a float array that holds the band (array)
        * data from the geotiff, for a specified layer
        * index layerIndex (1,2,3... for GDAL, for Geotiffs
        * with more than one band or data layer, 3D that is).
        *
        * Note this is a template function that is meant
        * to take in a valid C++ data type (i.e. char,
        * short, int, float), for the Geotiff in question
        * such that the Geotiff band data may be properly
        * read-in as numbers. Then, this function casts
        * the data to a float data type automatically.
        */
 
       // get the raster data type (ENUM integer 1-12,
       // see GDAL C/C++ documentation for more details)
       GDALDataType bandType = GDALGetRasterDataType(
         geotiffDataset->GetRasterBand(layerIndex));
        
       // get number of bytes per pixel in Geotiff
        int nbytes = GDALGetDataTypeSize(bandType);
 
       // allocate pointer to memory block for one row (scanline)
       // in 2D Geotiff array.
       T *rowBuff = (T*) CPLMalloc(nbytes*NCOLS);
 
       for(int row=0; row<NROWS; row++) {     // iterate through rows
 
         // read the scanline into the dynamically allocated row-buffer
         CPLErr e = geotiffDataset->GetRasterBand(layerIndex)->RasterIO(
           GF_Read,0,row,NCOLS,1,rowBuff,NCOLS,1,bandType,0,0);
         if(!(e == 0)) {
           cout << "Warning: Unable to read scanline in Geotiff!" << endl;
           exit(1);
         }
           
         bandLayer[row] = new float[NCOLS];
         for( int col=0; col<NCOLS; col++ ) { // iterate through columns
           bandLayer[row][col] = (float)rowBuff[col];
         }
       }
       CPLFree( rowBuff );
       return bandLayer;
    }
