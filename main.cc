#include<map>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include "car.hpp"

using namespace std;

int main()
{
	map<int, string> spot;
	map<int, string>:: iterator spotitr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;

	for(int i=0; i<100; ++i) // constructing the map..
	{
		string space;
		space[0] = space[1] = '0';
		spot.insert( pair<int, string> ( XYtoKEY( i, 5), space));
	}

	for(int i=0; i<3; ++i) // constructing the cars..
	{
		CAR newcar(i*3, 5);
		if ( newcar.DRCT()%2 == 0)  // go to north or east..
		spot[ XYtoKEY (i*3, 5) ] = "01"; // make this spot to be occupied..
		else						// goto west or south.. 
		spot[ XYtoKEY (i*3, 5) ] = "10"; // make this spot to be occupied..
		
		car.push_back(newcar);
	}
	
	int time_i = 0;
	while(time_i < 2 ) // main loop.. one loop is one time step..
	{
	
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			int newx(0), newy(0), drct(caritr->DRCT()) ;
			caritr->space_detect(drct, newx, newy, caritr->X(), caritr->Y(), spot);
			caritr->Move(newx, newy, spot);
			cout<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
	
		time_i++;
	}

	return 0;
}
