#include "def.hpp"

using namespace std;

void Wrong()
{
	cout<<" $ mpirun -n 4 ./ main -v 100 -f map.dat"<<endl;
	cout<<"Flag v means the number of vehicles in the map,"<<endl;
	cout<<"Flag f means the map file."<<endl;
	exit(0);
}

void Input_Check(const int nps, int argc, char *argv[], int &car_num, int &N_max, int &E_max, vector<int> &EWRANGE, vector<int> &NSRANGE)
// this function will be used to check all input variables..
{	
	if (nps != 1 && nps != 4 && nps != 16 && nps != 64)
	{
		cout<<"Number of process is wrong!"<<endl;
		Wrong();
	}

	int ch;
	opterr = 0;
	while(( ch = getopt(argc, argv, "v:f:")) != -1 )
	{
		switch(ch)
		{
			case 'v':  // v flag will be followed by the number of vehicles in the program..
					{
						int len = strlen(optarg);	
						for(int i=0; i<len; ++i)
						{
							if( optarg[i] > '9' || optarg[i] < '0' ) Wrong();
						}
						car_num = atoi(optarg);
						
						if( car_num < 1 ) Wrong();
						else if (car_num < nps)	
						{
							cout<<"Number of vehicles is too small."<<endl;
							Wrong();
						}
					}
					break;
			case 'f': // f flag will be followed by the read in map file in the program.. 
					{
						ifstream mapin( optarg );
						if ( !mapin.is_open() )  // check if the map file is successfully opened..
						{
							cout<<"Cant open: "<<optarg<<endl;
							Wrong();
						}
						
						int EWNUM, NSNUM;  // number of ports..
						mapin>>E_max, mapin>>N_max; // first line of map file..
						mapin>>EWNUM, mapin>>NSNUM;
						
						for(int i=0; i<EWNUM; ++i)
						{
							int num;
							mapin>>num;
							EWRANGE.push_back(num);
						}
						for(int i=0; i<NSNUM; ++i)
						{
							int num;
							mapin>>num;
							NSRANGE.push_back(num);
						}
						mapin.close();
				
						sort(EWRANGE.begin(), EWRANGE.end() );
						sort(NSRANGE.begin(), NSRANGE.end() );
						
						if ( EWRANGE.front() < 1 || NSRANGE.front() < 1 || EWRANGE.back() > E_max || NSRANGE.back() > N_max || E_max > EAST_HARD || N_max > NORTH_HARD )
						// check if any element in the map exceed the maximum range..
						{
							cout<<"Map is wrong!"<<endl;
							Wrong();
						}
					}
					break;
			default: 
					{
						cout<<"wrong haha"<<endl;
						Wrong(); 
					}
					break;
		}
	}
}

/*  upside is input checking, down side is spot and cross */

struct LR
// used to construct the spot of map..
{
	bool lt;
	bool rt;
};

namespace boost
// serialization of struct LR..
{
	namespace serialization
	{
		template<class Archive>
		void serialize(Archive &ar, struct LR &lr, const unsigned int version)
		{	ar & lr.lt;	ar & lr.rt;	}
	}
}

struct CROSS
// used to construct the cross of  map..
{
	bool NSred;
	bool EWred;
};

void Signal_Switch(map<int, CROSS> &cross) 
// change NSred and EWred to do signal switch..
{
	map<int, CROSS>:: iterator itr;
	for( itr = cross.begin(); itr != cross.end(); ++itr)
	{   
		itr->second.NSred = !itr->second.NSred;
		itr->second.EWred = !itr->second.EWred;
	}
}

int XYtoKEY(int x, int y)
// translate x and y into spot key..
// both x and y should not be larger than 99999, to ensure the unique of translated key value..
{
	if ( x>99999 || y>99999 ) 
	{
		printf("x = %d, y = %d, input is wrong!", x, y);
		exit(-1);
	}
	return x*10000 + y;
}

int Turn(const int DRCT, char turn)
// this function will be called when a car come to a road cross..
{
	if( turn == 's' ) return DRCT;
	else if ( turn == 'r' ) // turn right..
	{
		if ( DRCT == EAST )		return SOUTH;
		else if( DRCT == SOUTH )  return WEST;
		else if( DRCT == WEST ) 	return NORTH;
		else 						return EAST;
	}
	else // turn left.. 
	{
		if ( DRCT == EAST )		return NORTH;
		else if( DRCT == SOUTH )  return EAST;
		else if( DRCT == WEST ) 	return SOUTH;
		else 						return WEST;
	}
}

class CAR
{
	private:
	int x;
	int y;
	public:
	bool del; // 0 is not to be deleted, 1 is to be deleted..
	int drct;
	string path;
	
	CAR() {} // default constructor..
	CAR(int X, int Y, int DRCT, string PATH)
	{
		del = 0;
		x = X;
		y = Y;
		drct = DRCT;
		path = PATH;
	}

	~CAR() {}

	template<typename Archive>
	void serialize(Archive& ar, const unsigned version)
	{	// simply serialize the data members of CAR..
		ar &x;
		ar &y;
		ar &del;
		ar &drct;
		ar &path;
	}

	const int X()
	{ return x; }

	const int Y()
	{ return y; }

	const int DRCT()
	{ return drct; }

	void space_detect(bool rand, int &newx, int &newy, int &newdrct, map<int, LR> spot, map<int, CROSS> cross, const char turn)
	{
		map<int, CROSS>:: iterator crsitr;
		map<int, LR>:: iterator spotitr;
		newx = X(), newy = Y();
		newdrct = DRCT();
	
		if(	path.size() == 0 ) del = 1;
		else if(rand) // rand == 1 means no randomization.. else finish the function..
		{
			for( int i=0; i<5; ++i) // detect 5 spots maxism..
			{
				int tmpx = newx, tmpy = newy;
				if 		( newdrct == EAST )  tmpx = newx + 1;
				else if ( newdrct == WEST )  tmpx = newx - 1;
				else if ( newdrct == NORTH)  tmpy = newy + 1;
				else 						 tmpy = newy - 1; // south..

				spotitr = spot.find	( XYtoKEY(tmpx, tmpy) );
				crsitr  = cross.find( XYtoKEY(tmpx, tmpy) );
				
				if ( spotitr != spot.end() ) // there is a spot..
				{
					if( ( DRCT()%2 == 0 && spotitr->second.rt == 1) || ( DRCT()%2 == 1 && spotitr->second.lt == 1 ) )
					// go north or east && right side of road is ocpd || go west or south && left side of road is ocpd..
					break;
					else	{	newy = tmpy, newx = tmpx;	} // not occupied..	
				}
				else if ( crsitr != cross.end() ) // this is a cross 
				{
					if ( (DRCT() < 2 && crsitr->second.EWred == 1) || (DRCT() >1 && crsitr->second.NSred == 1) ) break;
					// west or east && ew red light is on 		 ||  north or south && ns red light is on
					else
					{
						int tmpdrct = Turn(DRCT(), turn);
						if( tmpdrct == EAST ) 	  tmpx = tmpx + 1;
						else if (tmpdrct == WEST) tmpx = tmpx - 1;
						else if (tmpdrct == NORTH)tmpy = tmpy + 1;
						else					  tmpy = tmpy - 1;

						spotitr = spot.find	( XYtoKEY(tmpx, tmpy) );
						if ( spotitr != spot.end() ) // there is a spot..
						{
							if( ( tmpdrct%2 == 0 && spotitr->second.rt == 1) || ( tmpdrct%2 == 1 && spotitr->second.lt == 1 ) )	break;
							else// not occupied, update newx, newy..	
							{
								newy = tmpy; 
								newx = tmpx;	
								newdrct = tmpdrct;
								path.erase(0, 1);
							} 
						}
					}
				}
				else // not cross, not spot, out of range.. 
				{
					//cout<<endl<<endl<<"hi"<<endl<<endl;
					del = 1;
					break;
				}
			}
		}
	}

	void Move(const int newx, const int newy, const int newdrct, map<int, LR> &spot)
	// move from one spot to another spot..
	{
		if( newx != X() || newy != Y() )
		{
			if (DRCT()%2 == 0) 	{	spot[ XYtoKEY( X(), Y() ) ].rt = 0;	}// east or north.. 
			else				{	spot[ XYtoKEY( X(), Y() ) ].lt = 0;	}// west or south.. 	
			
			if (newdrct%2 == 0 ){	spot[ XYtoKEY(newx, newy) ].rt = 1;	}
			else 				{	spot[ XYtoKEY(newx, newy) ].lt = 1;	}

			x = newx;
			y = newy;
			drct = newdrct;
		}
	}
};

class check_del
// this function is used to delete cars with bool del==1..
{
	public:
	bool operator() (const CAR &car)
	{	return car.del;	}
};

/*  upside is class CAR, down side is class BOUND */

class BOUND
// this class will be used to define the boundray of a subregion of a map..
{
	private:
	// const int nt, st, et, wt;
	const int nt, st, et, wt;
	int *EWpnt, *NSpnt;  // points where have a road..
	const int EWnum, NSnum;
	public:
	
	BOUND( int E, int W, int N, int S, int EWNUM, int NSNUM, int *EWRANGE, int *NSRANGE):  
			nt(N), st(S), et(E), wt(W), EWnum(EWNUM), NSnum(NSNUM)
	{
		EWpnt = new int[EWnum];
		NSpnt = new int[NSnum];
		for( int i=0; i<EWnum; ++i)	{	EWpnt[i] = EWRANGE[i];}
		for( int i=0; i<NSnum; ++i)	{	NSpnt[i] = NSRANGE[i];}
	}

	~BOUND() 
	{
		delete[] EWpnt;
		delete[] NSpnt;
	}

	int EW_pnt (int i) const 
	{
		if(i>=0 && i<EWnum) return EWpnt[i];
		else {	Wrong(); return -1;	}
	}
	int NS_pnt (int i) const 
	{
		if(i>=0 && i<NSnum)	return NSpnt[i];
		else {	Wrong(); return -1;	}
	}

	int Et()const 		  { return et; }
	const int Etin()  { return et - 4; }
	const int Etout() { return et + 5; }
	void Epackout(map<int, LR> &ES, map<int, LR> spot)
	{
		for(int i=0; i<NSnum; ++i)
		{
			const int y = NSpnt[i];
			for(int j = Etin(); j<= Et(); ++j)
			{
				int x = j;
				ES[ XYtoKEY(x,y) ].rt = spot[ XYtoKEY(x,y) ].rt; 
				ES[ XYtoKEY(x,y) ].lt = spot[ XYtoKEY(x,y) ].lt; 
			}
		}
	}

	int Wt()const 		  { return wt; }
	const int Wtin()  { return wt + 4; }
	const int Wtout() { return wt - 5; }
	void Wpackout(map<int, LR> &WS, map<int, LR> spot)
	{
		for(int i=0; i<NSnum; ++i)
		{
			const int y = NSpnt[i];
			for(int j = Wtin(); j>= Wt(); --j)
			{
				int x = j;
				WS[ XYtoKEY(x,y) ].rt = spot[ XYtoKEY(x,y) ].rt; 
				WS[ XYtoKEY(x,y) ].lt = spot[ XYtoKEY(x,y) ].lt; 
			}
		}
	}
	
	int Nt()const  		  { return nt; }
	const int Ntin()  { return nt - 4; }
	const int Ntout() { return nt + 5; }
	void Npackout(map<int, LR> &NS, map<int, LR> spot)
	{
		for(int i=0; i<EWnum; ++i)
		{
			const int x = EWpnt[i];
			for(int j = Ntin(); j<= Nt(); ++j)
			{
				int y = j;
				NS[ XYtoKEY(x,y) ].rt = spot[ XYtoKEY(x,y) ].rt; 
				NS[ XYtoKEY(x,y) ].lt = spot[ XYtoKEY(x,y) ].lt; 
			}
		}
	}
	
	int St()const  		  { return st; }
	const int Stin()  { return st + 4; }
	const int Stout() { return st - 5; }
	void Spackout(map<int, LR> &SS, map<int, LR> spot)
	{
		for(int i=0; i<NSnum; ++i)
		{
			const int x = EWpnt[i];
			for(int j = Stin(); j>= St(); --j)
			{
				int y = j;
				SS[ XYtoKEY(x,y) ].rt = spot[ XYtoKEY(x,y) ].rt; 
				SS[ XYtoKEY(x,y) ].lt = spot[ XYtoKEY(x,y) ].lt; 
			}
		}
	}

	void Construct(map<int, LR> &spot, map<int, CROSS> &cross, int ewns[4])
	{
//		ofstream file("plot_road.dat"); //session 3..
		// cross part..
		for(int i=0; i<EWnum; ++i)
		{
			for(int j=0; j<NSnum; ++j)
			{
				int x = EWpnt[i];
				int y = NSpnt[j];
				
				CROSS crs;
				crs.NSred = 1;
				crs.EWred = 0;
			    cross.insert( pair<int, CROSS> (XYtoKEY(x, y), crs) );
			}
		}

		map<int, CROSS>::iterator citr;
		map<int, LR>::iterator sitr;
		
		// ns part..
		int ET, WT;
		if (ewns[EAST] >= 0 ) ET = Etout();
		else					 ET = Et();
		if (ewns[WEST] >= 0 ) WT = Wtout();
		else					 WT = Wt();
		for( int i = 0; i<NSnum; ++i)
		{
			const int y = NSpnt[i];
			for(int j = WT; j <= ET; ++j)
			{
				int x = j;
				citr = cross.find( XYtoKEY(x, y));
				if( citr == cross.end())
				{
					LR space;
					space.lt = 0;
					space.rt = 0;
					spot.insert(pair<int, LR> ( XYtoKEY(x, y) , space));
				}
			}
		}
		
		// ew part..
		int NT, ST;
		if (ewns[NORTH] >= 0) NT = Ntout();
		else					 NT = Nt();
		if (ewns[SOUTH] >= 0) ST = Stout();
		else					 ST = St();
		for(int i=0; i<EWnum; ++i)
		{
			const int x = EWpnt[i];
			for(int j = ST; j <= NT; ++j )
			{
				int y = j;
				citr = cross.find( XYtoKEY (x, y));
				if (citr == cross.end())
				{
					LR space;
					space.lt = 0;
					space.rt = 0;
					spot.insert(pair<int, LR> ( XYtoKEY(x, y) , space));
				}
	//			cout<<x<<" "<<y<<endl;//session 3..;
			}
		}
	}

};

/*  upside is class BOUND, down side is car adding */

void Add_Car(const BOUND &bound, list<class CAR> &car, const int ew, const int ns, const int car_num, map<int, LR> &spot)
{
	list<class CAR>::iterator citr;  // iterator of cars..
	map<int, LR>::iterator sitr;	// iterator of spots..
	for(int i=0; i<car_num/2; ++i)  // cars on e-w direction road..
	{
		int count=0;
		bool check = 1;
		while(check && count<100)
		{
			count++;
			int x, y, drct, length;
			y = bound.NS_pnt( lrand48()%ns );
			x = bound.Wt() + lrand48()% (bound.Et() - bound.Wt());
			if(drand48() > 0.5 ) drct = EAST;
			else				 drct = WEST;
			
			sitr = spot.find( XYtoKEY(x, y));
			if( sitr == spot.end() || (drct == EAST && sitr->second.rt == 1) || (drct == WEST && sitr->second.lt == 1) )
			{	check = 1;	}
			else
			{
				check = 0;
				length = 3 + lrand48()%12;
				string path;
				for(int j=0; j<length; ++j) 
				{
					double rand = drand48();
					if(rand < 0.15) 		path.insert(0, "r");
					else if(rand > 0.85)	path.insert(0, "l");
					else 					path.insert(0, "s");
				}
				CAR newcar(x, y, drct, path);
				car.push_back(newcar);
				if(drct == EAST) sitr->second.rt = 1;
				else			 sitr->second.lt = 1;
			}
		}
	}

	for(int i = car_num/2; i<car_num; ++i)  // cars on n-s direction road..
	{
		int count=0;
		bool check = 1;
		while(check && count<100)
		{
			count++;
			int x, y, drct, length;
			x = bound.EW_pnt( lrand48()%ew );
			y = bound.St() + lrand48()% (bound.Nt() - bound.St());
			if(drand48() > 0.5 ) drct = SOUTH;
			else				 drct = NORTH;
			
			sitr = spot.find( XYtoKEY(x, y));
			if( sitr == spot.end() || (drct ==NORTH && sitr->second.rt == 1) || (drct ==SOUTH && sitr->second.lt == 1) )
			{	check = 1;	}
			else
			{
				check = 0;
				length = 3 + lrand48()%12;
				string path; 
				for(int j=0; j<length; ++j) 
				{
					double rand = drand48();
					if(rand < 0.15) 		path.insert(0, "r");
					else if(rand > 0.85)	path.insert(0, "l");
					else 					path.insert(0, "s");
				}
				CAR newcar(x, y, drct, path);
				car.push_back(newcar);
				if(drct == NORTH) sitr->second.rt = 1;
				else			  sitr->second.lt = 1;
			}
		}
	}
}
