// Station class declaration

class Station {
  double T;
  double H;
  int cid;
  double lat;
  double lon;
  double elev;
public:
  bool valid;
  double W;
  double dist;
  double val;
  Station(int,double,double,double);
  double getLat() { return lat; };
  double getLon() { return lon; };
  int getCid() { return cid; };
  double getElev() { return elev; };
  double getWeight() { return W; };
  double distance(double,double);
  double distance(Station*);
  double calcTheta(double,double,Station*);
  void calcT(double,double,list<Station*>);
  void calcH(double,double,list<Station*>);
  void calcW(double,double,list<Station*>);
};
