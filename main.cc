#include<map>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include "car.hpp"

using namespace std;

int main()
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;

	for(int i=0; i<30; ++i) // constructing the map..
	{
		LR space;
		space.lt = 0; // left side of the spot..
		space.rt = 0;// right side of the spot..
		spot.insert( pair<int, LR> ( XYtoKEY( i, 5), space));
	}

	for(int i=0; i<3; ++i) // constructing the cars..
	{
		CAR newcar(i*3, 5);
		if ( newcar.DRCT()%2 == 0)  // go to north or east..
		spot[ XYtoKEY (i*3, 5) ].rt = 1; // make this spot to be occupied..
		else						// goto west or south.. 
		spot[ XYtoKEY (i*3, 5) ].lt = 1; // make this spot to be occupied..
		
		car.push_back(newcar);
	}
	
	int time_i = 0;
	while(time_i < 2 ) // main loop.. one loop is one time step..
	{

		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			cout<<spotitr->second.lt<<" ";
		}
		cout<<endl;
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			cout<<spotitr->second.rt<<" ";
		}
		cout<<endl;
	
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			int newx(0), newy(0), drct(caritr->DRCT()) ;
			caritr->space_detect(drct, newx, newy, caritr->X(), caritr->Y(), spot);
			cout<<newx<<" "<<newy<<endl;
			caritr->Move(newx, newy, spot);
			cout<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
	
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			cout<<spotitr->second.lt<<" ";
		}
		cout<<endl;
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			cout<<spotitr->second.rt<<" ";
		}
		cout<<endl;
		cout<<endl;
		
		time_i++;
	}

	return 0;
}
