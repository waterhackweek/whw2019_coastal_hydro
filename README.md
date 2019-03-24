# meteo_forcing_wrfhydro
## Project Title 
Meteorological forcing data for NWM/WRF-Hydro 

## Collaborators on this project 
Project lead: Yin-Phan Tsang and Yu-Fen Huang 


## The problem 
Meteorological forcings are important to any hydrological model, yet inherit data formats are required for different models. Our team propose to prepare meteorological forcings for WRF-Hydro, the core process of National Water Model (NWM). WRF-Hydro team has provided regridding scripts for data from NLDAS, HRRR, MRMS, GFS, and RAP. However, these data sources are not always available for the desired study regions (e.g., regions outside of Contiguous United State) or in certain cases ground observation could be supplemented and enhance the quality of meteorological forcings. We would like to build a script or package to use WRF-Hydro with available meteorological datasets that might be different from the available default data sources.

## Application Example
Meteorological data for Hawaii

## Example data

**Data** | **Data Descriptions** | **Resolution** | **Format** |
|---|---|---|---|
HADS or NWS_HFO | ground observations for mostly rainfall, A few stations have other meteorological variables | Spatial: NA; Temporal: 15-min to hourly | .txt or .csv |
GPM | The Global Precipitation Measurement | Spatial: 0.1 degree; Temporal: hourly | NetCDF4 |
GLDAS | Global Land Data Assimilation System | Spatial: 0.25 degree; Temporal: 3-hr | NetCDF4 |
MRMS | multi-radar multi sensor data, already operated in CONUS | Spatial: 1-km; Temporal: hourly | GRIB2 |
geo_em.dxx | the domain for WRF-Hydro, the file to specific the forcing area | Spatial: depends; Temporal: NA | NetCDF |
nwm.tXXz.analysis_assim.channel_rt.tmXX.xxx.nc | the exist meteorological forcing for WRF-Hydro for reference | Spatial: depends; Temporal: hourly | NetCDF |

The WRF-Hydro forcing has to include:
* Incoming longwave radiation (W/m2)
* Incoming shortwave radiation (W/m2)
* Specific humidity (Kg/Kg)
* Air temperature (K)
* Surface pressure (Pa)
* Near surface wind in the u-component (m/s)
* Near surface wind in the v-component (m/s)
* Liquid water precipitation rate (mm/s)

## Specific Questions
Can we build WRF-Hydro meteorological forcing file with multiple data sources, including ground-based observations, remote sensing QPE, global re-analysis data?

## Existing methods
There is no an existing method. Previously, we had used WRF output from a research group in Department of Atmospheric Science at University of Hawaii at Manoa. The WRF output is readily compatible with WRF-Hydro. 

## Proposed methods/tools
Potential package: xarray

