#include<map>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include "head.hpp"

using namespace std;

const int crossx = 25;
const int crossy = 25;

int main()
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;
	BOUND bound(50, 0, 50, 0);

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
		CAR newcar(X, crossy, drct);
		
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

	// session 2 display use..
	cout<<"North boundary of the map is: "<<bound.Nt()<<endl;	
	cout<<"South boundary of the map is: "<<bound.St()<<endl;	
	cout<<"East  boundary of the map is: "<<bound.Et()<<endl;	
	cout<<"West  boundary of the map is: "<<bound.Wt()<<endl<<endl;	
	cout<<"Road cross is at X = "<<crossx<<", Y = "<<crossy<<endl<<endl;
	// session 2 display use..
	
	int time_i = 0, time_max = 13;
	
	// session 2 display use..
	int *ses2x = new int[time_max]; 
	int *ses2y = new int[time_max]; 
	// session 2 display use..
	
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		//cout<<"At time "<<time_i<<endl;
	
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0);
			//cout<<"Car move"<<endl;
//			cout<<caritr->X()<<" "<<caritr->Y()<<endl;
			
			// session 2 display use..
			ses2x[time_i] = caritr->X();
			ses2y[time_i] = caritr->Y();
			// session 2 display use..
			
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
	
	// session 2 display use..
	for(int i=0; i<time_max; ++i)
	{
		printf("Time%2d |", i);
	}	cout<<endl;
	for(int i=0; i<time_max; ++i)
	{
		printf("X = %2d |", ses2x[i]);
	}	cout<<endl;
	for(int i=0; i<time_max; ++i)
	{
		printf("Y = %2d |", ses2y[i]);
	}	cout<<endl;
	cout<<endl<<"Vehicle starting point is: X = "<<ses2x[0]<<", Y = "<<ses2y[0]<<endl;
	cout<<endl<<"Vehicle destination point is: X = "<<ses2x[time_max - 1]<<", Y = "<<ses2y[time_max - 1]<<endl;
	cout<<endl<<"Spent "<<time_max - 1<<" time steps"<<endl<<endl;
	delete[] ses2x;
	delete[] ses2y;
	// session 2 display use..

	return 0;
}
