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

	CAR(int X, int Y)
	{
		x = X;
		y = Y;
		drct = 0;
		path = "ssss";
	}

	~CAR() {}

	const int X()
	{ return x; }

	const int Y()
	{ return y; }

	const int DRCT()
	{ return drct; }

	void space_detect(const int drct, int &newx, int &newy, const int x, const int y, map<int, string> spot)
	{
		map<int, string>:: iterator spotitr;
		newx = x;
		newy = y;
		for(int i=1; i<=5; ++i)
		{
			if      (drct == 0) // east.. run on right side.. space[1]..
			{
				spotitr = spot.find( XYtoKEY(x+i, y) );
				if ( spotitr->second[1] == '1' )    break;
				newx = x+i;
			}
			else if (drct == 1) // west.. run on left side.. space[0]..
			{
			    spotitr = spot.find( XYtoKEY(x-i, y) );
				if ( spotitr->second[0] == '1' )    break;
				newx = x-i;
			}
			else if (drct == 2) // north.. run on right side.. space[1]..
			{
				spotitr = spot.find( XYtoKEY(x, y+i) );
				if ( spotitr->second[1] == '1' )    break;
				newy = y+i;
			}
			else                // south.. run on left side.. space[0]..
			{
				spotitr = spot.find( XYtoKEY(x, y-i) );
				if ( spotitr->second[0] == '1' )    break;
				newy = y-i;
			}
		}
			//  cout<<x<<" "<<y<<endl;
	}

	void Move(const int newx, const int newy, map<int, string> spot)
	{
		map<int, string>::iterator spotitr;
		
		if (DRCT()%2 == 0) // east or north..
		{	
			spotitr = spot.find( XYtoKEY( X(), Y() ) );
			spotitr->second[1] = '0';	
			spotitr = spot.find( XYtoKEY(newx, newy) );
			spotitr->second[1] = '1';
		}
		else				// west or south..
		{
			spotitr = spot.find( XYtoKEY( X(), Y() ) );
			spotitr->second[0] = '0';	
			spotitr = spot.find( XYtoKEY(newx, newy) );
			spotitr->second[0] = '1';
		}
		
		x=newx;
		y=newy;
	}
};
