// Pixel class definition

#include "vicmet.hpp"

// Pixel class constructor
Pixel::Pixel(double lat_, double lon_, double elev_) {
  this->lat=lat_;
  this->lon=lon_;
  this->elev=elev_;
}

// Calculate regridded value for VIC grid cell
double Pixel::calcRegridVal(map<int,map<string,vector<double> > > &data, list<Station> &near_stations, string var, int dt) {
  double V=0.0, Vsum=0.0;
  double v,statweight,testweight;
  int statval, testID, badID;
  list<Station*> valid_stations; // list of pointers to stations with valid values for current timestep
  list<Station>::iterator p=near_stations.begin(); // iterator to maximum neighbor stations
  Station *s;
  badID=32767;
  while (p!=near_stations.end() && valid_stations.size()<MINNEIGHBORS) { // iterate over near stations while valid ones are less than the minimum neighbors
    //BL: this part above has the potential to include fewer than//
    // 4 stations, unless the MAXNEIGHBORS number is high enough//
    // to ensure that >4 nearby stations with valid data are stored//
    //in memory for each time step; originally MAXNEIGHBORS was 50//
    //but this needs to be way bigger, e.g. 2000//
    v=data[p->getCid()][var][dt]; // COOP station data value
    testID=p->getCid(); // BL added
    if (p->getCid() == badID) {
      //cout << "weird station testID " << testID << " found" << endl; //BL added 
    }
    if (validValue(v,var) && testID != 32767) { //BL added second condition
      p->val=v; // store valid value
      s=&(*p);
      valid_stations.push_back(s); // store pointer to valid station
    }
    p++;
  }
  for (list<Station*>::iterator pp=valid_stations.begin();pp!=valid_stations.end();pp++) {
//    cout << dt << s->getCid() << endl;//////////////////////
    s=*pp; // pointer to valid station
    s->calcW(this->lat,this->lon,valid_stations); // calculate SYMAP weight
    if ((var.compare("TMAX")==0 || var.compare("TMIN")==0) && s->getElev()!=99999.0 && s->getElev()!=-99999.0) {// lapse temperature unless station elevation is missing
      V+=s->getWeight()*(s->val+0.0065*(s->getElev() - this->elev));
      testweight=s->val+0.0065*(s->getElev() - this->elev); } // BL added
    else {
      V+=s->getWeight()*(s->val);
      testweight=s->val; } // BL added
    Vsum+=s->getWeight(); // sum of weight
    statval=s->getCid();//BL added
    statweight=s->getWeight(); //BL added
    //cout << "dt " << dt <<  " statval " << statval << " testweight " << testweight << " var " <<  var << endl;//
  }
  V/=Vsum;
  return V;
}

// Function used to sort stations according to distance from grid cell
bool compareDistance(Station stat1, Station stat2) {
  if (stat1.dist<stat2.dist)
    return true;
  else
    return false;
}

// Find stations that are nearest to current grid cell, up to a number of maximum neighbors
list<Station> Pixel::nearestStations(list<Station> &stations) {
  int s=0;
  list<Station> near_stations; // list of nearest stations
  list<Station>::iterator p=stations.begin(); // iterator to entire set of stations
  while (p!=stations.end()) {
    p->dist=p->distance(this->lat,this->lon); // calculate distance between station P and grid cell
    p++;
  }
  stations.sort(compareDistance); // sort stations according to distance
  p=stations.begin();
  while (p!=stations.end() && s<MAXNEIGHBORS) { // keep only MAXNEIGHBORS stations into list
    near_stations.push_back(*p);
    p++;
    s++;
  }
  return near_stations;
}

// Write VIC precipitation and temperature data
// void Pixel::writePT(const char *coutputdir, map<char *,vector<double> > &data, const char *outputfmt, const int filedecimal) {
void Pixel::writePT(const char *coutputdir, vector<double> &prec, vector<double> &tmax, vector<double> &tmin, const char *outputfmt, const int filedecimal) {
  char *vars[]={(char*)"PRCP",(char*)"TMAX",(char*)"TMIN"};
  char *var, varst[5];
  ofstream outfile;
  ostringstream oss;
  string outfilename, outputdir(coutputdir), varl;
  unsigned short int p;
  short int txn;
  oss.setf(ios::fixed,ios::floatfield); // set fixed decimal field
  oss.precision(filedecimal); // set number of decimals (default is 4, check declaration)
  oss << "/data_" << this->lat << "_" << this->lon; // construct filename
  outfilename=outputdir+oss.str(); // append filename to output directory
  strcpy(varst,"PRCP");
  if (strncmp(outputfmt,"binary",6)==0) {
    outfile.open(outfilename.c_str(),ios::out | ios::binary); // open output file in binary mode
    for (int t=0;t<prec.size();t++) {//data[varst].size();t++) {
      p=(unsigned short int)(prec[t]*40.0);
      outfile.write(reinterpret_cast<char *>(&p),sizeof(unsigned short int));
      txn=(short int)(tmax[t]*100.0);
      outfile.write(reinterpret_cast<char *>(&txn),sizeof(short int));
      txn=(short int)(tmin[t]*100.0);
      outfile.write(reinterpret_cast<char *>(&txn),sizeof(short int));
    }
    outfile.close();
  }

  else {
    outfile.open(outfilename.c_str(),ios::out); // open output file in text (default) mode
    outfile.setf(ios::fixed,ios::floatfield);
    for (int t=0;t<prec.size();t++) { 
      outfile << setprecision(4) << prec[t] << "\t" << setprecision(2) << tmax[t] << "\t" << tmin[t] << endl;
    }
    outfile.close();
  }
}
      
