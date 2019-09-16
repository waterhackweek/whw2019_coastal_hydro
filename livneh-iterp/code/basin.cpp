// Basin class definition

#include "vicmet.hpp"
//#include "Geotiff.cpp"

//
// Basin class generic constructor
//
Basin::Basin() {
  north=0.0;
  south=0.0;
  west=0.0;
  east=0.0;
  res=0.0;
}

//
// Transform a Basin object into a 2D grid
//
double **Basin::toGrid() {
  int nrows, ncols, c, ci, cj;
  double **tmpelev;
  double lat_ul, lon_ul;
  // find grid number of rows and columns
  nrows=(int)round((this->north - this->south)/this->res);
  ncols=(int)round((this->east - this->west)/this->res);
  // allocate grid (nrows x ncols)
  tmpelev=new double*[nrows];
  for (c=0;c<nrows;c++)
    tmpelev[c]=new double[ncols];
  // initialize grid values
  for (int i=0;i<nrows;i++)
    for (int j=0;j<ncols;j++)
      tmpelev[i][j]=nullval;
  lat_ul=this->north - this->res/2.0; // upper left corner latitude
  lon_ul=this->west + this->res/2.0; // upper left corner longitude
  // store values into grid
  for (c=0;c<this->lat.size();c++) {
    ci=(int)round((lat_ul - this->getLat(c))/this->res);
    cj=(int)round((this->getLon(c) - lon_ul)/this->res);
    tmpelev[ci][cj]=this->getVal(c);
//     ci=(int)round((lat_ul - this->lat[c])/this->res);
//     cj=(int)round((this->lon[c] - lon_ul)/this->res);
//     tmpelev[ci][cj]=this->elev[c];
  }
  return tmpelev;
}

//
// Aggregate Basin to OBASIN resolution and extent
//
void Basin::aggregate(Basin obasin) {
  double northmask, longmask, lat, lon;
  double **sgrid, valsum, valnum;
  vector<double> val(obasin.size(),nullval);
  int nr, ll, cc, i, j, c;
  northmask=obasin.north-obasin.res/2.0;
  longmask=-obasin.west-obasin.res/2.0;
  sgrid=this->toGrid();
  for (c=0;c<obasin.size();c++) {
    lat=obasin.getLat(c)+obasin.res/2.0-this->res/2.0;
    lon=-obasin.getLon(c)+obasin.res/2.0-this->res/2.0;
//     lat=obasin.lat[c]+obasin.res/2.0-this->res/2.0;
//     lon=-obasin.lon[c]+obasin.res/2.0-this->res/2.0;
    nr=(int)round(obasin.res/this->res);
    ll=(int)round((this->north-lat)/this->res);
    cc=(int)round((-this->west-lon)/this->res);
    valsum=0.0;
    valnum=0;
    for (i=ll;i<ll+nr;i++)
      for (j=cc;j<cc+nr;j++)
	if (sgrid[i][j]!=nullval) {
	  valsum+=sgrid[i][j];
	  valnum+=1.0;
	}
    if (valnum>0.0)
      val[c]=valsum/valnum;
    else
      val[c]=nullval;
    this->cell[c].elev=val[c];
    this->cell[c].lat=obasin.cell[c].lat;
    this->cell[c].lon=obasin.cell[c].lon;
  }
//   this->lat=obasin.lat;
//   this->lon=obasin.lon;
//   this->elev=val;

  this->res=obasin.res;
  this->north=obasin.north;
  this->south=obasin.south;
  this->east=obasin.east;
  this->west=obasin.west;
}

// tony: START HERE. It seems that I need to create an arc/info grid on a windows machine and pass this as the basinDem in main.cpp
//

//
// Basin class constructor from ARC/INFO file
//
BasinDem::BasinDem(const char *demfilename) {
  string desc;
  int ncols, nrows, posi;
  double lon_ll, lat_ll, lat_ul, nodata;
  double lat, lon, val;
  ifstream file(demfilename); // input file stream
  file >> desc >> ncols;
  file >> desc >> nrows;
  file >> desc >> lon_ll; // lower left corner longitude
  file >> desc >> lat_ll; // lower left corner latitude
  file >> desc >> this->res;
  file >> desc >> nodata;
  lat_ul=lat_ll+res*(double)nrows;
  // basin extent
  this->south=lat_ll;
  this->west=lon_ll;
  this->north=lat_ul;
  this->east=lon_ll+this->res*(double)ncols;

//   this->cell=vector<Pixel>(nrows*ncols,Pixel(0.0,0.0,0.0));
//   posi=0;

  for (int i=0;i<nrows;i++) {
    for (int j=0;j<ncols;j++) {
      file >> val;
      if (val!=nodata) {
        lat=lat_ul-this->res*(double)i-this->res/2.0;
        lon=lon_ll+this->res*(double)j+this->res/2.0;

    // 	this->cell[posi]=Pixel(lat,lon,val);
    // 	posi++;

        this->cell.push_back(Pixel(lat,lon,val));
        this->elev.push_back(val);
        this->lat.push_back(lat_ul-this->res*(double)i-this->res/2.0);
        this->lon.push_back(lon_ll+this->res*(double)j+this->res/2.0);
      }
    }
  }
//   this->cell.erase(this->cell.begin()+posi,this->cell.end());
  file.close();
}
