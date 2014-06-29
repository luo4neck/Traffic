#include<stdlib.h>
#include<mpi.h>
#include<stdio.h>
#include "_car.h"
#define comm MPI_COMM_WORLD

using namespace std;

const int ncar = 20;
const int time_max = 200;

int main(int argc, char *argv[])
{
	// initialize the double ways..
	CAR* list_east = NULL;
	list_east = Car_construct(list_east, ncar, EAST);
	CAR* list_west = NULL;
	list_west = Car_construct(list_west, ncar, WEST);
	
	// show the location of cars on the two ways.. 
	Car_print(list_east);
	Car_print(list_west);
	
	
	int time_i=0; // time_i is just a stop condition..
	while(time_i < time_max)
	{
	time_i++;
		
		Car_forward_NE(list_east);
		Car_forward_SW(list_west);
		
		printf("East: ");
		Car_print(list_east);
		
		printf("West: ");
		Car_print(list_west);

		printf("\n");
	}

	//MPI_Finalize();
	return 0;
}
