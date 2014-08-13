#include<map>
#include<string>
#include<iostream>
#include<fstream> // plot..
#include<stdlib.h>
#include<stdio.h>
#include "head.hpp"

using namespace std;

const int car_num = 15;
const double p_randomization= 0.05;
/*
void Signal_Switch(map<int, CROSS> &cross)
{
	map<int, CROSS>:: iterator itr;
	for( itr = cross.begin(); itr != cross.end(); ++itr)
	{
		itr->second.NSred = !itr->second.NSred;
		itr->second.EWred = !itr->second.EWred;
	}
}
*/
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

		CAR newcar(21 + i, 25, drct);
		
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
	
	
	int time_i = 0, time_max = 70;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		ofstream file("plot.dat");// session 3..
		
		//if ( time_i%5 == 0 ) Signal_Switch(cross);
		//cout<<"At time "<<time_i<<endl;
		//cout<<cross[ 60, 75].NSred<<" "<<cross[60, 75].EWred<<endl;

		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			caritr->space_detect(rand, newx, newy, spot, cross, turn);
			caritr->Move(newx, newy, spot);
		//	cout<<caritr->X()<<" "<<caritr->Y()<<endl;
			file<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
	
		// session 3..
		file.close();
		FILE *gp = popen("gnuplot -persist", "w");
   		if(gp == NULL)
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		fprintf(gp, "set terminal png\n");
		fprintf(gp, "set output '%2d.png'\n", time_i + 10);
//		fprintf(gp, "set key font ',10'\n");
		fprintf(gp, "set title 'Time step %d'\n", time_i);
		fprintf(gp, "set xrange[%d: %d]\n", bound.Wt(), bound.Et() );
		fprintf(gp, "set yrange[%d: %d]\n", bound.St(), bound.Nt() );
		fprintf(gp, "plot 'plot.dat' u 1:2 title 'Cars' w points, 'plot_road.dat' u 1:2 title 'Road Spot' w points\n");
		fclose(gp);
		// session 3..
		
		time_i++;
	}
	
	system("convert -delay 25 -loop 0 *.png moving.gif\n");	// session 3..
	
	return 0;
}
