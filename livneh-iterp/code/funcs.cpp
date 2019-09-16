#include "vicmet.hpp"

// Convert units depending on variable VAR
double unitConvert(double val, string var) {
  if (val==-99999 || val==99999)
    return nullval;
  else if (var.compare("PRCP")==0)
    return val/100.0*25.4;
  else if (var.compare("TMAX")==0 || var.compare("TMIN")==0)
    return (val-32.0)*5.0/9.0;
  else
    return val;
}

// Check if date is valid
bool validDate(int day, int month, int year) {
  int dpm[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
  return (day<=dpm[month] || (year%4==0 && month==2 && day==29));
}

// Check for valid value
bool validValue(double v, string var) {
  if (var.compare("PRCP")==0)
    return (v!=nullval && v<350.0 && v>=0.0);
  else
    return (v!=nullval && v<50.0 && v>-50.0);
}

// Load raw COOP station data
void LoadCoopData(const char *filename, date startdate, date enddate, map<int,map<string,vector<double> > > &data) {
  string line, var, yearmonth, day,flag;
  vector<string> x;
  char *y,*cstr;
  int cid, pos, i, t, dsi;
  int yr, mn, dy, numdays, hr;
  float val, part, partval,debugval,oldval;
  int dt, dtmax, dt0;
  ifstream file(filename);
  FILE * fdebugout;
  //  if (var.compare("PRCP")==0) {
  //  fdebugout = fopen("debugfile.txt","w");//
  //}
    
  numdays = (enddate-startdate).days()+1;
  dtmax  = (enddate-startdate).days();
  if (file.is_open()) {
    getline(file,line); 
    getline(file,line); // skip header 2 lines
    cstr=new char[line.size()+1];
    getline(file,line);
    while (! line.empty()) {
      strcpy(cstr,line.c_str()); //DSI-number, 3200,3206,3210
      y=strtok(cstr,","); 
      dsi=atoi(y);
      y=strtok(NULL,","); // COOPID
      cid=atoi(y);
      y=strtok(NULL,",");
      if (cid==999999) cid=atoi(y); // WBANID
      for (i=0;i<3;i++) y=strtok(NULL,",");
      var=y;
      for (i=0;i<2;i++) y=strtok(NULL,",");
      yearmonth=y;
      yr=atoi(yearmonth.substr(0,4).c_str());
      mn=atoi(yearmonth.substr(4,2).c_str());
      //fprintf(fdebugout,"%d %d %d\n",cid,yr,mn);//
      if (date(yr,mn,1)>=startdate && date(yr,mn,1)<enddate) {
	for (t=0;t<31;t++) {
	  day=strtok(NULL,",");
	  dy=atoi(day.substr(0,2).c_str());
	  hr=atoi(day.substr(2,4).c_str());
	  val=atof(strtok(NULL,","));
	  //fprintf(fdebugout,"%d %d %.f\n",dy,hr,val);//
	  //for (i=0;i<2;i++) y=strtok(NULL,",");
	  y=strtok(NULL,","); //"N" flag
	  flag=std::string(y);
	  if (flag == "A" || flag == "B" || flag == "E" || flag == "S") {
	    val=-99999;
	  }
	  y=strtok(NULL,","); //"O" flag
	  flag=std::string(y);
	  if (flag == "2" || flag == "3" || flag == "K" || flag == "L" || flag == "T" || flag == "U") {
	    val=-99999;
	  }
	  if (dsi == 3206 && var.compare("PRCP")==0) { // This appears to be the key to pre-48 PRCP/ However, the "4" value (or "J") for flag 2 could also be responsible, although it discards a lot of stations/finally the tobs precip partitioning should be reexamined
	    val=-99999;
	  }
	  //if (val==99999) { 
	  //  val=-99999;
	  //}
	  //B.L. added precipitation partitioning to account for non-midnight//
	  //observation times//
	  if (validDate(dy,mn,yr)) {
	    //	    if (hr>23) { //99 case//
	    //  hr=23;
	    //}
	    //if (hr<0) {
	    //  hr=23;
	    //}//
	    dt=(date(yr,mn,dy)-startdate).days();
	    // partval=val; // //B.L. preserve original value for non-PRCP cases//
	    part=1.0; //had to change logic, since 99999's were getting missed when pre-multiplied//
	    if (var.compare("PRCP")==0 && dt<dtmax && hr<=23) { //B.L. default PRCP on last day//
	      part=(hr+1.)/24;
	      //	      partval=val*part;//
	    }
	    if (data[cid][var].size()==0) {
	      data[cid][var]=vector<double>(numdays,nullval);
	      //	      data[cid][var][dt]=unitConvert(partval,var);//
	      debugval=unitConvert(val,var)*part;
	      data[cid][var][dt]=debugval;
	    }
	    else {
	      //	      data[cid][var][dt]=unitConvert(partval,var);//
	      debugval=unitConvert(val,var)*part;
	      data[cid][var][dt]=debugval;
	    }
	    if (var.compare("PRCP")==0 && dt>0 && hr<=23) { //B.L. add remainder of PRCP to previous day//
	      dt0=dt-1;
	      part=1.-((hr+1.)/24);
	      //	      data[cid][var][dt0]+=unitConvert(partval,var);//
	      oldval=data[cid][var][dt0];
	      debugval=unitConvert(val,var)*part;
	      data[cid][var][dt0]=(oldval+debugval);
	    }
	  }
 	}
      }
      getline(file,line);	
    }
    file.close();
  }
  else {
    cout << "File " << filename << " not found" << endl;
  }
  //fclose(fdebugout);//

}

// Read in list of stations
void ReadStations(const char *filename, list<Station> &stations) {
  string line;
  ifstream file(filename);
  int coopid;
  double lat, lon, elev;
  char *y, *cstr;
  cstr=new char[8];
  getline(file,line);
  getline(file,line);
  getline(file,line);
  while (!line.empty()) {
    istringstream iss(line);
    vector<string> sline;
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter<vector<string> >(sline));
    coopid=atoi(sline[0].c_str());
    if (coopid==999999) coopid=atoi(sline[1].c_str());
    strcpy(cstr,sline[sline.size()-3].c_str());
    y=strtok(cstr,":"); lat=atof(y);
    y=strtok(NULL,": "); lat+=atof(y)/60.0;
    strcpy(cstr,sline[sline.size()-2].c_str());
    y=strtok(cstr,":"); lon=atof(y);
    y=strtok(NULL,": "); lon+=(-1)*(atof(y)/60.0);


    if (atof(sline[sline.size()-1].c_str()) != -99999) {
      elev=atof(sline[sline.size()-1].c_str())*0.3048;
    }
    else {
      elev=-99999;
    }

    stations.push_back(Station(coopid,lat,lon,elev));
    getline(file,line);
  }
  file.close();
  delete cstr;
}

list<Station> FindGoodStations(list<Station> &stations, string var, datamap &data) {
  int gooddata, pidx;
  list<Station> gstations; // list of "good" stations
  for (list<Station>::iterator p=stations.begin();p!=stations.end();p++) {
    gooddata=0;

    // use only stations with a valid elevation (EAR)
    if (p->getElev()!=-99999){

      for (pidx=0;pidx<data[p->getCid()][var].size();pidx++) {
	if (data[p->getCid()][var][pidx]!=nullval)
	  gooddata++;
      }
      if (gooddata>=(20*365)) {//
      //if (gooddata>=(10*365)) {
	gstations.push_back(*p);
      }
 
    } // end check for valid elevation (EAR)

  }
  return gstations;
}
