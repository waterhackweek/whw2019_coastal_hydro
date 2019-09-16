//
// Basin class declaration
//

class Basin {
public:
  vector<double> lat;
  vector<double> lon;
  vector<double> elev;
  vector<Pixel> cell;
  double north;
  double south;
  double east;
  double west;
  double res;
  Basin();
  double getLat(int c) { return cell[c].lat; }
  double getLon(int c) { return cell[c].lon; }
  double getVal(int c) { return cell[c].elev; }
  int size() { return cell.size(); } // returns number of pixels
  double **toGrid();
  void writeDem(char *);
  void aggregate(Basin);
  void subset(Basin);
};

class BasinSoil: public Basin {
public:
  BasinSoil() {};
  BasinSoil(char *);
};

class BasinDem: public Basin {
public:
  BasinDem() {};
  BasinDem(const char *);
};



