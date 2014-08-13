#include<map>
#include<string>
#include<iostream>
#include<fstream> // plot..
#include<stdlib.h>
#include<stdio.h>
#include "head.hpp"

using namespace std;

const int car_num = 1;

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
	
	bound.Construct(spot, cross); // construct the map in this process..
	// session 3
	
	srand48(time(NULL));
	for(int i=0; i < car_num; ++i) // constructing the cars in this process..
	{
		double random = drand48();
		int drct = 0;
		
		if (random < 0.25 ) 	drct = 0;
		else if (random < 0.5 ) drct = 1;
		else if (random < 0.75) drct = 2;
		else 					drct = 3;
		drct = 0;

		//int X;
		//if (i<3) X = i, drct = 0;
		//else	 X = i*4+10, drct = 1;
		
		CAR newcar(21, 25, drct);
		
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
	
	// session 3..
	ofstream file("plot.dat");
	
	// session 3..
	
	int time_i = 0, time_max = 30;
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
		//	cout<<caritr->X()<<" "<<caritr->Y()<<endl;
			file<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
		time_i++;
	}
	file.close();
   	
	
	FILE *gp = popen("gnuplot -persist", "w");
   	if(gp == NULL)
	{
		printf("Cannot plot the data!\n");
		exit(0);
	}
	fprintf(gp, "set xrange[0: 100]\n");
	fprintf(gp, "set yrange[0: 100]\n");
	fprintf(gp, "plot 'plot.dat' u 1:2 w points, 'plot_road.dat' u 1:2 w points\n");
	fclose(gp);

	return 0;
}
