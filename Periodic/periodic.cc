#include<fstream>
#include<stdlib.h>
#include<mpi.h>
#include<stdio.h>
#include "car.h"
#define comm MPI_COMM_WORLD

using namespace std;

const int bound = 400; // this is the periodic boundary condition..
const int time_max = 200; // this is the time boundary condition..

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	
	int nps, myid;
	MPI_Comm_size(comm, &nps);
	MPI_Comm_rank(comm, &myid);
	srand48(time(NULL) + myid);
	
	printf("No.%d in %d\n", myid, nps);
	CAR car(myid);	
	
	int time_k = 0; // k is just a stop condition..
	ofstream file("plot.dat");
	while( time_k < time_max ) // make sure the system will run for times..
	{
	time_k++;

		// broadcast and recv the information of ends of cars..
		//Here should using allgather because each car have to send its location to all other cars, then recv location of all other cars..
		int *car_back = new int[nps];
		int my_back = car.Back();
		MPI_Allgather(&my_back, 1, MPI_INT, car_back, 1, MPI_INT, comm);
		
		// write the head locations into plot.dat..
		file<<time_k<<" ";
		for(int i=0; i<nps; ++i)
		{
			file<<car_back[i]<<" ";
		}
		file<<endl;

		delete car_back;
		
		// go or stop
		double random = drand48();
		if ( car.Smallest(nps, car_back) >= 2 ) // at least 2 meters from the previous car..
		car.Forward( car.Smallest(nps, car_back), random);
		
		//if( myid == 0 ) printf("i am %d, time %d, at %d \n", myid, time_k, car.Head() );
		
		// if going out of bound, go back to 0..
		if ( car.Head() > bound ) car.Periodic();	
	}
	file.close();

	if ( myid == 0 ) //process 0 is in charge of print all routes to gnuplot..
	{
		FILE *gp = popen("gnuplot -persist", "w");;
		if(gp == NULL) 
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		//The code below will show 20 path as lines..
		//fprintf(gp, "plot 'plot.dat' u 1:2 w l, 'plot.dat' u 1:3 w l, 'plot.dat' u 1:4 w l, 'plot.dat' u 1:5 w l, 'plot.dat' u 1:6 w l, 'plot.dat' u 1:7 w l, 'plot.dat' u 1:8 w l, 'plot.dat' u 1:9 w l, 'plot.dat' u 1:10  w l, 'plot.dat' u 1:11  w l, 'plot.dat' u 1:12  w l, 'plot.dat' u 1:13  w l, 'plot.dat' u 1:14  w l, 'plot.dat' u 1:15  w l, 'plot.dat' u 1:16  w l, 'plot.dat' u 1:17  w l, 'plot.dat' u 1:18  w l, 'plot.dat' u 1:19  w l, 'plot.dat' u 1:20  w l, 'plot.dat' u 1:21 w l\n");
		//The code below will show 20 path as points..
		fprintf(gp, "plot 'plot.dat' u 1:2 w points, 'plot.dat' u 1:3 w points, 'plot.dat' u 1:4 w points, 'plot.dat' u 1:5 w points, 'plot.dat' u 1:6 w points, 'plot.dat' u 1:7 w points, 'plot.dat' u 1:8 w points, 'plot.dat' u 1:9 w points, 'plot.dat' u 1:10  w points, 'plot.dat' u 1:11  w points, 'plot.dat' u 1:12  w points, 'plot.dat' u 1:13  w points, 'plot.dat' u 1:14  w points, 'plot.dat' u 1:15  w points, 'plot.dat' u 1:16  w points, 'plot.dat' u 1:17  w points, 'plot.dat' u 1:18  w points, 'plot.dat' u 1:19  w points, 'plot.dat' u 1:20  w points, 'plot.dat' u 1:21 w points\n");
		//fprintf(gp, "pause -1\n");
		fclose(gp);
	}
	MPI_Finalize();
	if ( myid == 0 ) cout<<"finish"<<endl;
	return 0;
}
