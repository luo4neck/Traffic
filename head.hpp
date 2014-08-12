#include<list>
#include<map>
#include<iostream>
#include<string>
#include<stdlib.h>

#define EAST 0
#define WEST 1
#define NORTH 2
#define SOUTH 3

using namespace std;

struct LR
// used to construct the spot of map..
{
	bool lt;
	bool rt;
};

struct CROSS
// used to construct the cross of  map..
{
	bool NSred;
	bool EWred;
};

void print(map<int, LR> spot)
{
	map<int, LR>:: iterator itr;
	for(itr = spot.begin(); itr != spot.end(); ++itr)
	{
			cout<<itr->second.lt<<" ";
	}
	cout<<endl;
	
	for(itr = spot.begin(); itr != spot.end(); ++itr)
	{
			cout<<itr->second.rt<<" ";
	}
	cout<<endl;
}

int XYtoKEY(int x, int y)
// translate x and y into spot key..
// both x and y should not be larger than 99999, to ensure the unique of translated key value..
{
	if (x>99999 || y>99999) 
	{
		printf("input is wrong!");
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
	int drct;
	string path;

	CAR(int X, int Y, int DRCT)
	{
		x = X;
		y = Y;
		drct = DRCT;
		path = "l";
	}

	~CAR() {}

	const int X()
	{ return x; }

	const int Y()
	{ return y; }

	const int DRCT()
	{ return drct; }

	void space_detect(int &newx, int &newy, map<int, LR> spot, map<int, CROSS> cross, const char turn)
	{
		map<int, CROSS>:: iterator crsitr;
		map<int, LR>:: iterator spotitr;
		newx = X(), newy = Y();

		for( int i=0; i<5; ++i) // detect 5 spots maxism..
		{
			int tmpx = newx, tmpy = newy;
			if ( DRCT() == EAST ) 		tmpx = newx + 1;
			else if ( DRCT() == WEST )  tmpx = newx - 1;
			else if ( DRCT() == NORTH ) tmpy = newy + 1;
			else 						tmpy = newy - 1; // south..
			
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
						if( ( DRCT()%2 == 0 && spotitr->second.rt == 1) || ( DRCT()%2 == 1 && spotitr->second.lt == 1 ) )	break;
						else// not occupied, update newx, newy..	
						{
							newy = tmpy, newx = tmpx;	
							drct = tmpdrct;
							path.erase(0, 1);
						} 
					}
				}
			}
			else break;
		}
	}

	void Move(const int newx, const int newy, map<int, LR> &spot)
	// move from one spot to another spot..
	{
		if( newx != X() || newy != Y() )
		{
			map<int, LR>::iterator spotitr;
		
			if (DRCT()%2 == 0) // east or north.. second[1] will be changed..
			{
				spot[ XYtoKEY( X(), Y() ) ].rt = 0;
				spot[ XYtoKEY(newx, newy) ].rt = 1;
			}
			else				// west or south.. second[0] will be changed..
			{
				spot[ XYtoKEY( X(), Y() ) ].lt = 0;
				spot[ XYtoKEY(newx, newy) ].lt = 1;
			}

		x=newx;
		y=newy;
		}
	}
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
		int *EWpnt = new int[EWnum];
		int *NSpnt = new int[NSnum];
		for( int i=0; i<EWnum; ++i)	{	EWpnt[i] = EWRANGE[i]; cout<<i<<" "<<EWpnt[i]<<endl; }
		for( int i=0; i<NSnum; ++i)	{	NSpnt[i] = NSRANGE[i]; cout<<i<<" "<<NSpnt[i]<<endl; }
	}

	~BOUND() 
	{
		delete[] EWpnt;
		delete[] NSpnt;
	}

	const int Et() { return et; }
	const int Wt() { return wt; }
	const int Nt() { return nt; }
	const int St() { return st; }

	void Construct(map<int, LR> &spot, map<int, CROSS> &cross)
	{
		// cross part..
		for(int i=0; i<EWnum; ++i)
		{
			for(int j=0; j<NSnum; ++j)
			{
				cout<<i<<" "<<j<<endl;
				cout<<EWpnt[i]<<" "<<NSpnt[i]<<endl; // STRANGE !!!!!!!!
				
				int x = EWpnt[i];
				int y = NSpnt[j];
				cout<<x<<" "<<y<<endl;
				
				CROSS crs;
				crs.NSred = 0;
				crs.EWred = 1;
			    cross.insert( pair<int, CROSS> (XYtoKEY(x, y), crs) );
		
			}
		}

			cout<<"fine here"<<endl;

		map<int, CROSS>::iterator citr;
		int count = 0;
		for(citr = cross.begin(); citr != cross.end(); ++citr)
		{
			count++;
			cout<<citr->second.NSred<<" "<<citr->second.EWred<<" "<<count<<endl;
		}

		// ns part..
		
		
		// ew part..
	
	
	}




};
