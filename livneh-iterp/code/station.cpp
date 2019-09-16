// Station class definition

#include "vicmet.hpp"

// Station class constructor
Station::Station(int cid_,double lat_,double lon_,double elev_) {
  cid=cid_;
  lat=lat_;
  lon=lon_;
  elev=elev_;
}

// Calculate distance between station and (OLAT,OLON) pair
double Station::distance(double olat,double olon) {
  double theta1, theta2;
  double phi1, phi2;
  double term1, term2, term3;
  double dist;
  theta1=dtor*lon;
  phi1=dtor*lat;
  theta2=dtor*olon;
  phi2=dtor*olat;
  term1=cos(phi1)*cos(theta1)*cos(phi2)*cos(theta2);
  term2=cos(phi1)*sin(theta1)*cos(phi2)*sin(theta2);
  term3=sin(phi1)*sin(phi2);
  //  dist=radius*acos(min(1.0,term1+term2+term3));
  // prevent zero distance from being in the denominator
  // make 1 km the minimum distance, rather than zero
  //  dist=max(0.1,radius*acos(min(1.0,term1+term2+term3)));
  dist=max(0.1,radius*acos(min(1.0,term1+term2+term3)));
  //cerr << "distance variable " << dist << endl;
  return dist;
}

// Calculate distance between current and OSTATION stations
double Station::distance(Station *ostation) {
  double theta1, theta2;
  double phi1, phi2;
  double term1, term2, term3;
  double olat, olon;
  olat=ostation->getLat();
  olon=ostation->getLon();
  return this->distance(olat,olon);
}

// Calculate theta weights
double Station::calcTheta(double lat, double lon, Station *ostation) {
  double a, b, c, iw;
  a=this->distance(ostation);
  b=ostation->distance(lat,lon);
  c=this->distance(lat,lon);
  a=a/circum*360.0*dtor;
  b=b/circum*360.0*dtor;
  c=c/circum*360.0*dtor;
  iw=(cos(a)-cos(b)*cos(c))/max(1.0e-4,sin(b)*sin(c));
  if (abs(iw)>1.0)
    iw=double(round(iw));
  return max(1.0-iw,1.0e-6);
}

// Calculate T component of interpolation weight
void Station::calcT(double lat, double lon, list<Station*> nstations) {
  Station *s;
  this->T=0.0;
  list<Station*>::iterator S = nstations.begin(); // iterate over stations used to calculate weight
  while (S != nstations.end()) {
    s=*S; // restore pointer to Station
    if (s->getCid() != this->getCid())
      this->T += (1.0/s->distance(lat,lon))*this->calcTheta(lat,lon,s);
    S++;
  }
}

// Calculate H component of interpolation weight
void Station::calcH(double lat, double lon, list<Station*> nstations) {
  Station *s;
  this->H=0.0;
  list<Station*>::iterator S = nstations.begin(); // iterate over stations used to calculate weight
  while (S != nstations.end()) {
    s=*S; // restore pointer to Station
    this->H += 1.0/s->distance(lat,lon);
    S++;
  }
}

// Calculate interpolation weight
void Station::calcW(double lat, double lon, list<Station*> nstations) {
  this->calcT(lat,lon,nstations);
  this->calcH(lat,lon,nstations);
  this->W = (1.0/pow(this->distance(lat,lon),2.0))*(this->H + this->T);
}
