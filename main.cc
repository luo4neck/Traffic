#include<stdlib.h>
#include<mpi.h>
#include<stdio.h>
#include "car.h"
#define comm MPI_COMM_WORLD

using namespace std;

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	// MPI_Status stat;
	
	int nps, myid;
	MPI_Comm_size(comm, &nps);
	MPI_Comm_rank(comm, &myid);
	srand48(time(NULL) + myid);
	
	printf("No.%d in %d\n", myid, nps);
	CAR car(myid);	
	
	int k=0; // k is just a stop condition..
	while(k<250)
	{
	k++;
		// broadcast and recv the ends..
		int *car_back = new int[nps];
		int my_back = car.Back();
		//printf("%d, %d\n", myid, my_back);

		//Here should using allgather because each car have to send its location to all other cars, then recv location of all other cars..
		MPI_Allgather(&my_back, 1, MPI_INT, car_back, 1, MPI_INT, comm);
	
		delete car_back;
		
		// go or stop
		double random = drand48();
		//if ( car.Smallest(nps, car_back) >= 2 ) // at least 2 meters from the previous car..
		if ( car.Head() < 1000 && car.Smallest(nps, car_back) >= 2 ) // test if all cars stop at some condition.. 
		car.Forward( car.Smallest(nps, car_back), random);
		
		if( myid == 0 ) printf("i am %d, time %d, at %d \n", myid, k, car.Head() );
		
	}

	MPI_Finalize();
	return 0;
}
