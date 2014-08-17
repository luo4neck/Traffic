#include "head.hpp"

using namespace std;

const int car_num = 1500;
const double p_randomization= 0.15;

bool Car_Rand_Add(BOUND bound, map<int, LR> &spot, list<class CAR> &car)
{
	



	return 0;
}

int main()
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;
	
	// session 4
	int ewnum = 50;
	int nsnum = 50;
	int *ewrange = new int[ewnum]; 
	int *nsrange = new int[nsnum];
	
	for(int i=0; i<ewnum; ++i) {	ewrange[i] = 100 * i + 50;	}
	
	for(int i=0; i<nsnum; ++i) {	nsrange[i] = 100 * i + 50;	}
	
	BOUND bound(5000, 0, 5000, 0, ewnum, nsnum, ewrange, nsrange);
			  //e,    w, n,    s, 
	bound.Construct(spot, cross); // construct the map in this process..
	// session 4
	
	srand48(time(NULL));
	for(int i=0; i < car_num; ++i) // constructing the cars in this process..
	{
		bool check = 1;
		while(check) 
		{
			int x, y, drct;
			if( i < car_num/2 )  // cars on ew road..
			{
				y = nsrange[ lrand48()%nsnum ];
				x =	lrand48() % bound.Et();
				double random = drand48();
				if( random < 0.5 ) 	drct = EAST;
				else				drct = WEST;
			}
			else // cars on ns road..
			{
				x = ewrange[ lrand48()%ewnum ];
				y =	lrand48() % bound.Nt();
				double random = drand48();
				if( random < 0.5 ) 	drct = SOUTH;
				else				drct = NORTH;
			}
		
			cout<<x<<" "<<y<<endl;
		
	/*	
			if (random < 0.25 ) 	drct = 0;
			else if (random < 0.5 ) drct = 1;
			else if (random < 0.75) drct = 2;
			else 					drct = 3;
	*/	
			CAR newcar(x, y, drct);
			spotitr = spot.find( XYtoKEY( x, y) );
			if (spotitr != spot.end() )
			{
				if ( !((newcar.DRCT()%2 == 0 && spotitr->second.rt == 1) || (newcar.DRCT()%2 == 1 && spotitr->second.lt == 1)) )
				{
					if ( newcar.DRCT()%2 == 0)  // go to north or east..
					spotitr->second.rt = 1;
					else                        // goto west or south.. 
					spotitr->second.lt = 1;
					
					car.push_back(newcar);
					check = 0;
				}
			}
		}
	}

	cout<<"simulation start!"<<endl;

	int time_i = 0, time_max = 100;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		ofstream file("plot_cars.dat");// session 3..
		
		if ( time_i%10 == 0 ) Signal_Switch(cross);
		cout<<"At time "<<time_i<<" "<<endl;
		
		int here = 0;
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			cout<<"fine here"<<here<<endl;
			here++;

			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			caritr->Move(newx, newy, newdrct, spot);
			//file<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->drct<<" "<<caritr->del<<endl;
			file<<caritr->X()<<" "<<caritr->Y()<<endl;
		}
		
		//if( time_i % 3 == 0 ) // !!!! decide how often to delete a car.. 
		{
			for(caritr = car.begin(); caritr != car.end(); ++caritr)
			{
				if (caritr->del == 1)
				{
					//cout<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->del<<" "<<caritr->drct<<endl;
					if(caritr->DRCT()%2 == 0 ) spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].rt = 0;
					else					   spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].lt = 0;
				}
			}
		
			car.remove_if( check_del() );
		}
		// session 3..
		
		file.close();
		
		FILE *gp = popen("gnuplot -persist", "w");
   		if(gp == NULL)
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		//fprintf(gp, "set terminal png\n");
		//fprintf(gp, "set output '%d.png'\n", time_i + 1000);
		//fprintf(gp, "set font ',30'\n");
		fprintf(gp, "set xlabel 'WEST <-                   -> EAST\n");
		fprintf(gp, "set ylabel 'SOUTH <-                   -> NORTH\n");
		fprintf(gp, "set title 'Time step %d, Total cars num: %zu'\n", time_i, car.size() );
		fprintf(gp, "set xrange[%d: %d]\n", bound.Wt(), bound.Et() );
		fprintf(gp, "set yrange[%d: %d]\n", bound.St(), bound.Nt() );
		fprintf(gp, "plot 'plot_cars.dat' u 1:2 title 'Cars' w points, 'plot_road.dat' u 1:2 title 'Road Spot' w points\n");
		//fprintf(gp, "plot 'plot_road.dat' u 1:2 title 'Road Spot' w points\n");
		fclose(gp);
		// session 3..
	
		time_i++;
	}

	//system("convert -delay 25 -loop 0 *.png Moving.gif\n");	// session 3..
	
	return 0;
}
