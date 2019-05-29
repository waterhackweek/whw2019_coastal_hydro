# meteo_forcing_wrfhydro
## Project Title 
Meteorological forcing data for NWM/WRF-Hydro 

## Google Doc
[Link to Open Water draft paper](https://docs.google.com/document/d/1U9nkF0-n_Ht-qLZVu9zKtrYcnrf5f1aD6lqx3uS2jtI/edit?usp=sharing)

[Link to Figure Slides](https://docs.google.com/presentation/d/1QzqulsWKCXmITVXmtKKTu46j82e3p9PoCdk5AwHC0wk/edit#slide=id.p)

## Collaborators on this project 
Project lead: Yin-Phan Tsang  
Data Science lead: Yu-Fen Huang  
Awesome members: Tony Castronova, AnyiChen  



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
Hydro Estimator | Satellite Quantitative Precipitation Estimation | Spatial: 4 km; Temporal: hourly | ASCII |
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
Can we build WRF-Hydro meteorological forcing file with multiple data sources, including ground-based observations, remote sensing QPE, global re-analysis data? Or add additional observation in the exist meteorological input?

## Existing methods
There is no an existing method. Previously, we had used Weather Research and Forecasting (WRF) output from a research group in Department of Atmospheric Science at University of Hawaii at Manoa. The WRF output is readily compatible with WRF-Hydro. 

## Proposed methods/tools
#### Workflow/methods:
1. Understand the output format (i.e. the format of meteorological input for WRF-Hydro): investigate the structure of expected output.
2. Subset the new data with our interested domain or check if the new data is in the interested region.
3. Blending data for each variable:
    * point data and gridded data: re-mesh potentially by simply re-grid and add spatial autocorelation, etc.
    * gridded data and gridded data: re-mesh
4. Re-structure each variables and melt variables into one file.

#### What help we need/what we don't know:
* We know what the final product looks like, and we have blur idea and workflow for reaching it. Yet we don't really know if the re-mesh or re-grid is reasonable and what tools out there that we can use.

## Other information about WRF-Hydro/NWM
* [CUAHSI Domain subsetter for NWM v1.2.2 (CONUS)](http://subset.cuahsi.org/) - *Tony Castronova*
