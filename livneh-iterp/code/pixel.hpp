// Pixel class declaration

class Pixel {
  double lat;
  double lon;
public:
  double elev;
  Pixel() {};
  Pixel(double, double, double);
  list<Station> nearestStations(list<Station> &);
  double calcRegridVal(map<int,map<string,vector<double> > > &, list<Station> &, string, int);
  void writePT(const char *, vector<double> &, vector<double> &, vector<double> &, const char *, const int filedecimal=4);
  friend class Basin; // allow Basin class to access private members of Pixel class
};
