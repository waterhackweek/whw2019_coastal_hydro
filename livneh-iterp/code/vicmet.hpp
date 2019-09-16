#include <cmath>
#include <cstdio>
#include <algorithm>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace std;
using namespace boost::gregorian;

typedef map<int,map<string,vector<double> > > datamap;

const double pi=atan(1)*4.0;
const double radius=6378.0;
const double circum=2.0*pi*radius;
const double dtor=pi/180.0;
const double nullval=-99.0;

//NOTE: MAXNEIGHBORS needs to be really big to ensure at least 4//
//stations will be founds for each grid-cell ESPECIALLY when //
//excluding station sets, i.e. DSI-3206
//const int MAXNEIGHBORS=50;//Default value
const int MAXNEIGHBORS=4000; // Correct value, used in v.1.1
const int MINNEIGHBORS=4;
//const int MINNEIGHBORS=15;

#include "station.hpp"
#include "pixel.hpp"
#include "basin.hpp"
#include "geotiff.hpp"

double unitConvert(double, string);
bool validDate(int, int, int);
bool validValue(double, string);
void LoadCoopData(const char *, date, date, map<int,map<string,vector<double> > > &);
void ReadStations(const char *, list<Station> &);
bool compareDistance(Station, Station);
list<Station> FindGoodStations(list<Station> &, string, datamap &);
