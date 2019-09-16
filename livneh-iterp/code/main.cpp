#include "vicmet.hpp"
#include "geotiff.hpp"


int main(int argc, char *argv[])
{
  int varc, pidx;
  int gooddata;
  char *vars[]={(char*)"PRCP",(char*)"TMAX",(char*)"TMIN"};
  char *var, varst[5];
  string varl;
  int numdays;
  double lat, lon, elev,numyears;
  string mons[12]={"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
  date startdate, enddate;
  datamap data;
  list<Station> stations, near_stations;
  map<char *,list<Station> > good_stations;

  startdate=date(1950,1,1); // INPUT START DATE
  enddate=date(2013,12,31); // INPUT END DATE
  numdays=(enddate-startdate).days()+1;

//  string states[]={"co","wy","ut"}; // INPUT LIST OF STATE NAMES
//  string states[]={"wy","nm","co","sd","ne","tx","ok","ks","ia","ar","mo","la","ms","wi","il","MXAGS","MXCAMP","MXCHIH","MXCOAH","MXCOL","MXDGO","MXGRO","MXGTO","MXHGO","MXJAL","MXMEX","MXMICH","MXMOR","MXNAY","MXNL","MXPUE","MXQRO","MXQROO","MXSIN","MXSLP","MXTAB","MXTAMS","MXTLAX","MXVER","MXYUC","MXZAC"}; // INPUT LIST OF STATE NAMES

  string states[] = {"wa"};
  string filename;

  // for each of the states, read in list of stations and then read in raw COOP data (assuming that each variable is stored in
  // separate file (VARC loop)
  // the paths where the data are stored are assumed to be in the format state/variable/state_variable_stn.txt and
  // state/variable/state_variable_dat.txt in the current directory. Need to change lines below if these are different (filename=...)
  for (int s=0;s<sizeof(states)/sizeof(string);s++) {
    // example filename = "az/prcp/az_prcp_stn.txt"
    //filename=string("/projects/beli1098/stationdata/")+states[s]+string("/prcp/")+states[s]+string("_prcp_stn.txt");
    filename=string("/Users/castro/Documents/work/meteo_forcing_wrfhydro/livneh/data/")+states[s]+string("/PRCP/")+states[s]+string("_prcp_stn.txt");
 
    // read list of stations
    ReadStations(filename.c_str(),stations); 

    // load raw COOP data for each variable
    for (varc=0;varc<sizeof(vars)/sizeof(char *);varc++) { 
      var=vars[varc];
      varl=string(var);
      transform(varl.begin(),varl.end(),varl.begin(),::tolower);
      // example filename = "wy/tmax/wy_tmax_dat.txt"
      //filename=string("/projects/beli1098/stationdata/")+states[s]+string("/")+varl+string("/")+states[s]+string("_")+varl+string("_dat.txt");
      filename=string("/Users/castro/Documents/work/meteo_forcing_wrfhydro/livneh/data/")+states[s]+string("/")+varl+string("/")+states[s]+string("_")+varl+string("_dat.txt");
      LoadCoopData(filename.c_str(),startdate,enddate,data); // read COOP raw data
      cout << "Finished reading COOP data file " << filename << endl;
    }
  }

  //#include <iostream>
  //#include <fstream>
  //using namespace std;

  //  ofstream myfile;
  //  myfile.open ("stalist.txt");

  //  for (list<Station>::iterator p=stations.begin();p!=stations.end();p++) {
  //    myfile << p->getCid() << " " << p->getLat() << " " << p->getLon() << endl;
  //  }

  //  myfile.close();

  //  for (list<Station>::iterator p=stations.begin();p!=stations.end();p++) {
  //    cout << p->getCid() << " " << p->getLat() << " " << p->getLon() << endl;
  //  }

  // set study domain as a Basin object
  //BasinDem b("/home/raid/blivneh/forcings/demdata/sandy1250.dem"); // INPUT DEM FILENAME
//  BasinDem b("/tmp/blivneh/nldas.100.95.25.37//nldas.elev.-100.-95.25.37.2.asc"); // INPUT DEM FILENAME
  BasinDem b("/tmp/livneh/dem/SaukDEM_150m_wgs84.txt");
//    Geotiff b("/Users/castro/Documents/work/meteo_forcing_wrfhydro/livneh/livneh-xcode/livneh-xcode/SaukDEM_150m_wgs84.tif");

// // COMMENTING THIS OUT B/C IT THE DOCS SUGGEST NOT TO USE PRISM DATA IN THESE SCRIPT
//  // read in prism basins for each month and aggregate to vic resolution
//  BasinDem prism[12]; // monthly prism
//  string prismfile;
//  for (int m=0;m<12;m++) 
//  {
//    prismfile=string("/projects/beli1098/prismdata/can_us_prism.");
//    prismfile+=mons[m];
//    prism[m]=BasinDem(prismfile.c_str()); // prism precipitation file
//    prism[m].aggregate(b); // aggregate prism data to vic basin resolution and extent
//  }

  int t, t0, t1;
  map<char *,vector<double> > VICdata;
  vector<int> Month(numdays,0);
  double monthly_prcp[12];
  Pixel *P;

  // keep only stations with at least 20 years of data
  for (varc=0;varc<sizeof(vars)/sizeof(char *);varc++) {
    var=vars[varc];
    good_stations[var]=FindGoodStations(stations,var,data); 
  }

  for (int cell=0;cell<b.size();cell++)
  { 
      cout << "RUNNING";
    P=&b.cell[cell]; // pointer to current grid cell
   // near_stations=P->nearestStations(stations); // find max neighbor nearest stations for current vic grid cell
    for (varc=0;varc<sizeof(vars)/sizeof(char *);varc++) 
    {
      var=vars[varc];
      near_stations=P->nearestStations(good_stations[var]); // find max neighbor nearest stations for current VIC grid cell
      VICdata[var]=vector<double>(numdays,0.0);
      for (t=0;t<numdays;t++) 
      {
	Month[t]=(startdate+date_duration(t)).month(); // store month to facilitate calculation of monthly averages
	VICdata[var][t]=P->calcRegridVal(data,near_stations,var,t); // calculate regridded value for current grid cell
      }
    }
    // calculate monthly values
    strcpy(varst,"PRCP");
    var=varst;
    for (t=0;t<12;t++)
    {
      monthly_prcp[t]=0.0;
    }
    if (startdate>date(1961,1,1) && enddate<date(1991,1,1)) 
    {
      cout << "startdate > 1961 & enddate < 1991\n ";
      numyears = (enddate-startdate).days()/365.25;
      t0=(startdate-startdate).days();
      t1=(enddate-startdate).days();
      cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
    }
    else if (startdate>date(1961,1,1)) 
    {
      cout << "startdate > 1961 \n ";
      t0=(startdate-startdate).days();
      t1=(date(1991,1,1)-startdate).days();
      numyears = (date(1991,1,1)-startdate).days()/365.25;
      cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
    }
    else if (enddate<date(1991,1,1)) 
    {
      cout << "enddate < 1991 \n ";
      t0=(date(1961,1,1)-startdate).days(); // only use period 1961-1990
      t1=(enddate-startdate).days();
      numyears = (enddate-date(1961,1,1)).days()/365.25;
      cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
    }
    else if (startdate>date(1991,1,1) || enddate<date(1961,1,1)) 
    {
      cout << "startdate > 1991 or enddate < 1961\n ";
      t0=0;
      t1=0;
      numyears = 1.0;
      cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
    }
    else 
    {
      t0=(date(1961,1,1)-startdate).days(); // only use period 1961-1990
      t1=(date(1991,1,1)-startdate).days();
      numyears = 30.0;
      cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
    }
    for (t=t0;t<t1;t++) 
    {
      monthly_prcp[Month[t]-1]+=VICdata[(char*)"PRCP"][t];
    }
    for (t=0;t<12;t++)
    {
      //      monthly_prcp[t]/=30.0; // divide by number of years in prism period
      monthly_prcp[t]/=numyears; // divide by number of years in prism period
    }

    // rescale precipitation with prism
       var=vars[0];
       for (t=0;t<numdays;t++)
         //if (prism[Month[t]-1].cell[cell].elev != nullval)
          //VICdata["PRCP"][t]*=prism[Month[t]-1].cell[cell].elev/monthly_prcp[Month[t]-1];

    // write precipitation and temperature data to file
	//    P->writePT("/tmp/blivneh/nldas.100.95.25.37//forcing.ascii/a_ptt_binary",VICdata["PRCP"],VICdata["TMAX"],VICdata["TMIN"],"binary",5); // INPUT OUTPUT FOLDER AND NUMBER OF DECIMAL PLACES IN OUTPUT DATA
    	P->writePT("/tmp/livneh/output",VICdata[(char*)"PRCP"],VICdata[(char*)"TMAX"],VICdata[(char*)"TMIN"],"binary",5); // INPUT OUTPUT FOLDER AND NUMBER OF DECIMAL PLACES IN OUTPUT DATA//
    cout << "done with pixel " << cell << endl;

  }

  return 0;
}
