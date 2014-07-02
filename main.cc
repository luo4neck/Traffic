#include<stdlib.h>
#include<mpi.h>
#include<stdio.h>
#include "_car.hpp" // all Car_** and Dis_** functions are in this file..
#include "map.hpp" 
#include "plotshow.hpp" // all Plot_** functions are in this file..
#define comm MPI_COMM_WORLD

using namespace std;

const int ncar = 10;
const int time_max = 100;

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

	int Signal_Number = 3;
	int *Signal_x = new int[Signal_Number];
	Signal_x[0] = -150;
	Signal_x[1] = 150;
	Signal_x[2] = 300;

    int **plot = Plot_new(time_max, ncar);

	srand48(time(NULL));
	int time_i=0; // time_i is just a stop condition..
	bool Red=1; // Red=1 means all red lights on the road are on..
	while(time_i < time_max)
	{
	time_i++;
		
		if (time_i >= 55) Red = 0;
		printf("Sec %d\n", time_i);
		
		Car_forward_NE(list_east, lrand48(), Red, Signal_x, Signal_Number);
		Car_forward_SW(list_west, lrand48(), Red, Signal_x, Signal_Number);
		// lrand48() is the seed..
		Car_print(list_east);
		Car_print(list_west);
		
		Plot_write(time_i, ncar, plot, list_east);
    }
    
	Plot_show(plot, time_max, ncar);
	Plot_delete(time_max, plot);
	
	return 0;
}
