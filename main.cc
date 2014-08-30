#include "head.hpp"

using namespace std;

const double p_randomization= 0.05;

int main(int argc, char *argv[])
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;
	
	// session 4
	cout<<"Map constructing..."<<endl;
	int ewnum = 4;
	int nsnum = 4;
	int *ewrange = new int[ewnum]; 
	int *nsrange = new int[nsnum];
	for(int i=0; i<ewnum; ++i) {	ewrange[i] = 100 * i + 50;	}
	for(int i=0; i<nsnum; ++i) {	nsrange[i] = 100 * i + 50;	}
	
	BOUND bound(400, 0, 400, 0, ewnum, nsnum, ewrange, nsrange);
			  //e,    w, n,    s, 
	int ewns[4] = {-1, -1, -1, -1};
	bound.Construct(spot, cross, ewns); // construct the map in this process..
	// session 4	
	cout<<spot.size()<<" spots ("<<spot.size()*sizeof(spot)/1024<<"KB) and ";
	cout<<cross.size()<<" road crosses ("<<cross.size()*sizeof(cross)/1024<<"KB) constructed for the map "<<endl<<endl;

	cout<<"Cars constructing..."<<endl;
	srand48(time(NULL));

	int car_num = atoi(argv[1]);
	cout<<car_num<<endl;
	Add_Car(bound, car, ewnum, nsnum, car_num, spot);
	
	cout<<car.size()<<" cars ("<<car.size()*sizeof(car)/1024<<"KB) constructed on the map"<<endl<<endl;

	int time_i = 0, time_max = 300;
	long int accu = 0;
	time_t start = time(NULL);
	cout<<"Simulation Start!"<<endl;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		ofstream file("plot_cars.dat");// plot
		
		if ( time_i%20 == 0 ) Signal_Switch(cross);
		cout<<"At time "<<time_i<<" "<<endl;
		
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			
			accu = accu + abs( caritr->X() - newx ) + abs( caritr->Y() - newy );
			
			caritr->Move(newx, newy, newdrct, spot);
			file<<caritr->X()<<" "<<caritr->Y()<<endl;   // plot
		}
		
		{ // this is scope is for car delete from the map..
			for(caritr = car.begin(); caritr != car.end(); ++caritr)
			{
				if (caritr->del == 1)
				{
					if(caritr->DRCT()%2 == 0 ) spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].rt = 0;
					else					   spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].lt = 0;
				}
			}
			car.remove_if( check_del() );
		}
		file.close();  // plot
		
		FILE *gp = popen("gnuplot -persist", "w");
   		if(gp == NULL)
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		fprintf(gp, "set terminal png\n");
		fprintf(gp, "set output '%d.png'\n", time_i + 1000);
		fprintf(gp, "set xlabel 'WEST <-                   -> EAST\n");
		fprintf(gp, "set ylabel 'SOUTH <-                   -> NORTH\n");
		fprintf(gp, "set title 'Time step %d, Total cars num: %zu'\n", time_i, car.size() );
		fprintf(gp, "set xrange[%d: %d]\n", bound.Wt(), bound.Et()+60 );
		fprintf(gp, "set yrange[%d: %d]\n", bound.St(), bound.Nt() );
		fprintf(gp, "plot 'plot_road.dat' u 1:2 title 'Road Spot' w points ps 1 pt 3 lt 2, 'plot_cars.dat' u 1:2 title 'Cars' w points ps 1 lt 1\n");
		fclose(gp);
		// session 4..
	
		time_t end = time(NULL);
		cout<<end-start<<" seconds spent for this step"<<endl<<endl;
		
		time_i++;

		if( (int)car.size() < car_num ) Add_Car(bound, car, ewnum, nsnum, car_num - car.size(), spot);
		cout<<car.size()<<endl;
	}
	
	system("convert -delay 20 -loop 0 *.png Moving.gif\n");	// session 4..
	
	double re = (double)accu/ (double)time_max/ (double)car_num ;
	cout<<"Average movement in this test is: "<<re<<endl;
	
	return 0;
}
