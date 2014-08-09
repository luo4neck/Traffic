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
// used to construct the spot map..
{
	bool lt;
	bool rt;
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
{
	if (x>99999 || y>99999) 
	{
		printf("input is wrong!");
		exit(-1);
	}
	return x*10000 + y;
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
		path = "ssss";
	}

	~CAR() {}

	const int X()
	{ return x; }

	const int Y()
	{ return y; }

	const int DRCT()
	{ return drct; }

	void space_detect(const int drct, int &newx, int &newy, const int x, const int y, map<int, LR> spot)
	// find a new to be occupied..
	{
		newx = x;
		newy = y;
		for(int i=1; i<=5; ++i)
		{
			if      (drct == 0) // east.. run on right side.. space[1]..
			{
				if ( spot[ XYtoKEY(x+i, y) ].rt == 1) break;
				newx = x+i;
			}
			else if (drct == 1) // west.. run on left side.. space[0]..
			{
				if ( spot[ XYtoKEY(x-i, y) ].lt == 1) break;
				newx = x-i;
			}
			else if (drct == 2) // north.. run on right side.. space[1]..
			{
				if ( spot[ XYtoKEY(x, y+i) ].rt == 1) break;
				newy = y+i;
			}
			else                // south.. run on left side.. space[0]..
			{
				if ( spot[ XYtoKEY(x, y-i) ].lt == 1) break;
				newy = y-i;
			}
		}
			//  cout<<x<<" "<<y<<endl;
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
