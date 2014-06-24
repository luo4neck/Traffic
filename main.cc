#include<stdlib.h>
#include<mpi.h>
#include<stdio.h>
#include "car.h"
#define comm MPI_COMM_WORLD

using namespace std;

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Status stat;
	
	int nps, myid;
	MPI_Comm_size(comm, &nps);
	MPI_Comm_rank(comm, &myid);

	//MPI_Barrier(comm);
	printf("No.%d in %d\n", myid, nps);
	CAR car(myid);	

	//while(1)
	{
		// broadcast the end..
		int *car_back = new int[nps];
		int my_back = car.back();
		printf("%d, %d\n", myid, my_back);

		car_back[myid] = car.back(); 
		
		//MPI_Bcast(car_back, 1, MPI_INT, myid, comm);
		MPI_Allgather(&my_back, sizeof(int), MPI_INT, car_back, sizeof(int)*nps, MPI_INT, comm);
		
		printf("i am %d, i have %d, %d, %d \n", myid, car_back[0], car_back[1], car_back[2]);
		// recive the end..
		
		
		// find the smallest distance from previous car..
		int smallest[2] = {0, 100};
		for(int i=0; i<nps; ++i)
		{
			int dis = car_back[i] - car.head();
			if ( car_back[i] > car.head() && dis < smallest[1] ) 
			{
				smallest[0] = i;
				smallest[1] = dis; 
			}
		}
		delete car_back;
		
		// go or stop
	
	}
	//MPI_Barrier(comm);

	MPI_Finalize();
	return 0;
}
