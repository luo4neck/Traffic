#include<map>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include "head.hpp"

using namespace std;

//const int crossx = 25;
//const int crossy = 25;

int main()
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;
	
	// session 3
	int _ewnum = 4;
	int _nsnum = 3;
	int *_ewrange = new int[_ewnum]; 
	int *_nsrange = new int[_nsnum];
	_ewrange[0] = 20, _ewrange[1] = 40, _ewrange[2] = 60, _ewrange[3] = 80;
	_nsrange[0] = 25, _nsrange[1] = 50, _nsrange[2] = 75;
	BOUND bound(100, 0, 100, 0, _ewnum, _nsnum, _ewrange, _nsrange);
	
	bound.Construct(spot, cross);
	// session 3
/*
	{
		CROSS crs;
		crs.NSred = 1;
		crs.EWred = 0;
		cross.insert( pair<int, CROSS> (XYtoKEY( crossx, crossy), crs) );
	}

	for(int i=bound.Wt(); i<bound.Et(); ++i) // constructing the map on x axis..
	{
		if ( i != crossx )
		{
			LR space;
			space.lt = 0; // left side of the spot..
			space.rt = 0;// right side of the spot..
			spot.insert( pair<int, LR> ( XYtoKEY( i, crossy), space));
		}
	}
	
	for(int i=bound.St(); i<=bound.Nt(); ++i) // constructing the map on y axis..
	{
		if ( i != crossy )
		{
			LR space;
			space.lt = 0; // left side of the spot..
			space.rt = 0;// right side of the spot..
			spot.insert( pair<int, LR> ( XYtoKEY( crossx, i), space));
		}
	}
*/
	srand48(time(NULL));
	for(int i=0; i<1; ++i) // constructing the cars..
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
		else	 X = i*4+10, drct = 1;
		CAR newcar(X, X, drct);
		
		spotitr = spot.find( XYtoKEY( newcar.X(), newcar.Y() ) );
		if ( spotitr != spot.end() )
		{
			if ( newcar.DRCT()%2 == 0)  // go to north or east..
			spot[ XYtoKEY ( newcar.X(), newcar.Y() ) ].rt = 1; // make this spot to be occupied..
			else						// goto west or south.. 
			spot[ XYtoKEY ( newcar.X(), newcar.Y() ) ].lt = 1; // make this spot to be occupied..
			car.push_back(newcar);
		}
	}

	
	int time_i = 0, time_max = 0;
	
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		//cout<<"At time "<<time_i<<endl;
	
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0);
			//cout<<"Car move"<<endl;
//			cout<<caritr->X()<<" "<<caritr->Y()<<endl;
			
			caritr->space_detect(newx, newy, spot, cross, turn);
			caritr->Move(newx, newy, spot);
			//cout<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
/*	
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			//if ( spotitr->second.lt == 1 || spotitr->second.rt == 1 )  cout<<"M ";
			if ( spotitr->second.lt == 1 )  cout<<"M ";
			else 							cout<<"_ ";
		}
		cout<<" <- forward west"<<endl;
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
	//		cout<<spotitr->first<<" ";
			if ( spotitr->second.rt == 1 )  cout<<"M ";
			else 							cout<<"_ ";
		}
		cout<<" -> forward east"<<endl;
		cout<<endl;
*/		
		time_i++;
	}

	return 0;
}
