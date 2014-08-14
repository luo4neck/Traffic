#include "head.hpp"

using namespace std;

const int car_num = 15;
const double p_randomization= 0.05;

int main()
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;
	
	// session 3
	int _ewnum = 1;
	int _nsnum = 1;
	int *_ewrange = new int[_ewnum]; 
	int *_nsrange = new int[_nsnum];
	_ewrange[0] = 50;//, _ewrange[1] = 40, _ewrange[2] = 60, _ewrange[3] = 80;
	_nsrange[0] = 50;//, _nsrange[1] = 50, _nsrange[2] = 75;
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

		CAR newcar(21 + i, 50, drct);
		
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
	
	
	int time_i = 0, time_max = 50;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		ofstream file("plot.dat");// session 3..
		
		if ( time_i%10 == 0 ) Signal_Switch(cross);
		//cout<<"At time "<<time_i<<" "<<endl;

		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			caritr->Move(newx, newy, newdrct, spot);
			file<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->drct<<" "<<caritr->del<<endl;
		}
		
		for(caritr = car.begin(); caritr != car.end(); ++caritr)
		{
			if (caritr->del == 1)
			{
				if(caritr->DRCT()%2 == 0 ) spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].rt = 0;
				else					   spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].lt = 0;

			}
		}
		
		car.remove_if( check_del() );
	
		// session 3..
		file.close();
		FILE *gp = popen("gnuplot -persist", "w");
   		if(gp == NULL)
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		fprintf(gp, "set terminal png\n");
		fprintf(gp, "set output '%d.png'\n", time_i + 10);
		fprintf(gp, "set font ',30'\n");
		fprintf(gp, "set title 'Time step %d'\n", time_i);
		fprintf(gp, "set xrange[%d: %d]\n", bound.Wt(), bound.Et() );
		fprintf(gp, "set yrange[%d: %d]\n", bound.St(), bound.Nt() );
		fprintf(gp, "plot 'plot.dat' u 1:2 title 'Cars' w points, 'plot_road.dat' u 1:2 title 'Road Spot' w points\n");
		fclose(gp);
		// session 3..
		
		time_i++;
	}

	system("convert -delay 25 -loop 0 *.png Moving.gif\n");	// session 3..
	
	return 0;
}
