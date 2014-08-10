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

	for(int i=0; i<40; ++i) // constructing the map..
	{
		LR space;
		space.lt = 0; // left side of the spot..
		space.rt = 0;// right side of the spot..
		spot.insert( pair<int, LR> ( XYtoKEY( i, 5), space));
	}

	srand48(time(NULL));
	for(int i=0; i<6; ++i) // constructing the cars..
	{
		double random = drand48();
		int drct = 0;
		/*
		if (random < 0.25 ) 	drct = 0;
		else if (random < 0.5 ) drct = 1;
		else if (random < 0.75) drct = 2;
		else 					drct = 3;
*/	
		int X;
		if (i<3) X = i, drct = 0;
		else	 X = i*4+25, drct = 1;
		CAR newcar(X, 5, drct);
		
		if ( newcar.DRCT()%2 == 0)  // go to north or east..
		spot[ XYtoKEY ( newcar.X(), newcar.Y() ) ].rt = 1; // make this spot to be occupied..
		else						// goto west or south.. 
		spot[ XYtoKEY ( newcar.X(), newcar.Y() ) ].lt = 1; // make this spot to be occupied..
		
		car.push_back(newcar);
	}
	
	int time_i = 0;
	while(time_i < 7 ) // main loop.. one loop is one time step..
	{
		cout<<"At time "<<time_i<<endl;

		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
		//	cout<<spotitr->second.lt<<" ";
		}
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
		//	cout<<spotitr->second.rt<<" ";
		}
	
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			int newx(0), newy(0), drct(caritr->DRCT()) ;
	//		cout<<caritr->X()<<" "<<caritr->Y()<<endl;
			caritr->space_detect(drct, newx, newy, caritr->X(), caritr->Y(), spot);
			caritr->Move(newx, newy, spot);
	//		cout<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
	
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
		//	cout<<spotitr->second.lt<<" ";
			if ( spotitr->second.lt == 1 )  cout<<"M ";
			else 							cout<<"_ ";
		}
		cout<<" <- forward west"<<endl;
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			//cout<<spotitr->second.rt<<" ";
			if ( spotitr->second.rt == 1 )  cout<<"M ";
			else 							cout<<"_ ";
		}
		cout<<" -> forward east"<<endl;
		cout<<endl;
		
		time_i++;
	}

	return 0;
}
