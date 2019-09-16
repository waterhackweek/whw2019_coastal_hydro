#include "vicmet.hpp"
#include "geotiff.hpp"



const char* usage()
{
    const char* usage =
    "\n\n---------------------------"
    "\nLivneh Interpolation Script"
    "\n---------------------------"
    "\nModified by Tony Castronova <acastronova@cuahsi.org>\n"
    "\nUsage:"
    "\nlivneh-iterp -d <data directory> -s <states> -b <basin ascii raster> -o <output dir>"
    "\n  -d: data directory where CO-OP data is located (assumes trailing slash)"
    "\n  -s: csv list of states to load data for, e.g. wa,or,..."
    "\n  -b: path to an ascii raster that defines the area of interest"
    "\n  -o: output directory (assumes trailing slash)"
    "\n";
    
    return usage;
}

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
    
    vector<string> states;
    string datadir;
    string basin_path;
    string output_dir;
    
    int opt;
    while((opt = getopt(argc, argv, ":s:f:d:b:o:")) != -1)
    {
        switch(opt)
        {
                // input states to load data for, space separated
            case 's':
            {
                stringstream ss(optarg);
                char temp;
                string t = optarg;
                for(int i=0; i < t.length(); i += 3)
                {
                    string st(1, t[i]);
                    st += t[i+1];
                    states.push_back(st);
                }
                break;
            }
            case 'd':
            {
                datadir = optarg;
                break;
            }
            case 'b':
            {
                basin_path = optarg;
                break;
            }
            case 'o':
            {
                output_dir = optarg;
                break;
            }
            case ':':
                printf("option needs a value\n");
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
        }
    }
    // optind is for the extra arguments
    // which are not parsed
    for(; optind < argc; optind++){
        printf("extra arguments: %s\n", argv[optind]);
    }
    
    if(datadir.empty())
    {
        printf("Error: missing required parameter -> (-d) datadir");
        printf(usage());
        return 1;
    }
    if(states.empty())
    {
        printf("Error: missing required parameter -> (-s) states");
        printf(usage());
        return 1;
    }
    if(basin_path.empty())
    {
        printf("Error: missing required parameter -> (-b) basin ascii raster");
        printf(usage());
        return 1;
    }
    if(output_dir.empty())
    {
        printf("Error: missing required parameter -> (-o) output directory");
        printf(usage());
        return 1;
    }
    
    
    //  string states[]={"co","wy","ut"}; // INPUT LIST OF STATE NAMES
    //  string states[]={"wy","nm","co","sd","ne","tx","ok","ks","ia","ar","mo","la","ms","wi","il","MXAGS","MXCAMP","MXCHIH","MXCOAH","MXCOL","MXDGO","MXGRO","MXGTO","MXHGO","MXJAL","MXMEX","MXMICH","MXMOR","MXNAY","MXNL","MXPUE","MXQRO","MXQROO","MXSIN","MXSLP","MXTAB","MXTAMS","MXTLAX","MXVER","MXYUC","MXZAC"}; // INPUT LIST OF STATE NAMES
    
    //string states[] = {"wa"};
    string filename;
    
    // for each of the states, read in list of stations and then read in raw COOP data (assuming that each variable is stored in
    // separate file (VARC loop)
    // the paths where the data are stored are assumed to be in the format state/variable/state_variable_stn.txt and
    // state/variable/state_variable_dat.txt in the current directory. Need to change lines below if these are different (filename=...)
    for (int s=0;s<sizeof(states)/sizeof(string);s++) {
        
        // read list of stations
        printf("- Reading stations for %s\n", states[s].c_str());
        //filename=string("/Users/castro/Documents/work/meteo_forcing_wrfhydro/livneh/data/")+states[s]+string("/PRCP/")+states[s]+string("_prcp_stn.txt");
        filename=string(datadir)+states[s]+string("/PRCP/")+states[s]+string("_prcp_stn.txt");
        ReadStations(filename.c_str(),stations);
        
        printf("- Reading CO-OP data for %s\n", states[s].c_str());
        
        // load raw COOP data for each variable
        for (varc=0;varc<sizeof(vars)/sizeof(char *);varc++) {
            var=vars[varc];
            varl=string(var);
            printf("  - processing variable: %s\n", varl.c_str());
            
            transform(varl.begin(),varl.end(),varl.begin(),::tolower);
            
            filename=string(datadir)+states[s]+string("/")+varl+string("/")+states[s]+string("_")+varl+string("_dat.txt");
            
            // read COOP raw data
            LoadCoopData(filename.c_str(),startdate,enddate,data);
        }
    }
    

    
    // set study domain as a Basin object
    printf("- Reading basin DEM\n");
    //BasinDem b("/tmp/livneh/dem/SaukDEM_150m_wgs84.txt");
    BasinDem b(basin_path.c_str());

    
    int t, t0, t1;
    map<char *,vector<double> > VICdata;
    vector<int> Month(numdays,0);
    double monthly_prcp[12];
    Pixel *P;
    
    // keep only stations with at least 20 years of data
    printf("- Finding good stations\n");
    for (varc=0;varc<sizeof(vars)/sizeof(char *);varc++) {
        var=vars[varc];
        good_stations[var]=FindGoodStations(stations,var,data);
    }
    
    
    for (int cell=0;cell<b.size();cell++)
    {
        printf("  - processing cell %d of %lu\n", cell, b.cell.size());
        
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
            //cout << "startdate > 1961 & enddate < 1991\n ";
            numyears = (enddate-startdate).days()/365.25;
            t0=(startdate-startdate).days();
            t1=(enddate-startdate).days();
            //cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
        }
        else if (startdate>date(1961,1,1))
        {
            //cout << "startdate > 1961 \n ";
            t0=(startdate-startdate).days();
            t1=(date(1991,1,1)-startdate).days();
            numyears = (date(1991,1,1)-startdate).days()/365.25;
            //cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
        }
        else if (enddate<date(1991,1,1))
        {
            //cout << "enddate < 1991 \n ";
            t0=(date(1961,1,1)-startdate).days(); // only use period 1961-1990
            t1=(enddate-startdate).days();
            numyears = (enddate-date(1961,1,1)).days()/365.25;
            //cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
        }
        else if (startdate>date(1991,1,1) || enddate<date(1961,1,1))
        {
            //cout << "startdate > 1991 or enddate < 1961\n ";
            t0=0;
            t1=0;
            numyears = 1.0;
            //cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
        }
        else
        {
            t0=(date(1961,1,1)-startdate).days(); // only use period 1961-1990
            t1=(date(1991,1,1)-startdate).days();
            numyears = 30.0;
            //cout << "t0 " << t0 <<  " t1 " << t1 << " numyears " << numyears << endl;
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
            
            // write precipitation and temperature data to file
            //    P->writePT("/tmp/blivneh/nldas.100.95.25.37//forcing.ascii/a_ptt_binary",VICdata["PRCP"],VICdata["TMAX"],VICdata["TMIN"],"binary",5); // INPUT OUTPUT FOLDER AND NUMBER OF DECIMAL PLACES IN OUTPUT DATA
            P->writePT(output_dir.c_str(),
                       VICdata[(char*)"PRCP"],
                       VICdata[(char*)"TMAX"],
                       VICdata[(char*)"TMIN"],
                       "binary",
                       5); // INPUT OUTPUT FOLDER AND NUMBER OF DECIMAL PLACES IN OUTPUT DATA//
        //P->writePT("/tmp/livneh/output",VICdata[(char*)"PRCP"],VICdata[(char*)"TMAX"],VICdata[(char*)"TMIN"],"binary",5); // INPUT OUTPUT FOLDER AND NUMBER OF DECIMAL PLACES
        //cout << "done with pixel " << cell << endl;
        
    }
    
    return 0;
}
