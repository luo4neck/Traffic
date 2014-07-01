#include<stdlib.h>
#include<fstream>
#include<mpi.h>
#include<stdio.h>
#include "_car.h"
#include "map.h"
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

// show..
    int **plot = new int*[time_max];
    for(int i=0; i<time_max; ++i)
    {
        plot[i] = new int[ncar+1];
        plot[i][0] = i;
    }
// show..

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
	    
// show..
        CAR* start = list_east;
        for(int i=1; i<=ncar; ++i)
        {
            plot[time_i-1][i] = start->x;
            if (start->next != NULL) start = start->next;
        }
// show..
    }
    
// show..
    ofstream file("plot.dat");
    for(int i=0; i<time_max; ++i)
    {
        for(int j=0; j<=ncar; ++j)
        {
            file<<plot[i][j]<<", ";
        }
        file<<endl;
    }
    file.close();

    for(int i=0; i<time_max; ++i) delete[] plot[i];
    delete[] plot;
   
    FILE *gp = popen("gnuplot -persist", "w");;
    if(gp == NULL) 
    {
        printf("Cannot plot the data!\n");
        exit(0);
    }
    fprintf(gp, "plot 'plot.dat' u 1:2 w points, 'plot.dat' u 1:3 w points, 'plot.dat' u 1:4 w points, 'plot.dat' u 1:5 w points, 'plot.dat' u 1:6 w points, 'plot.dat' u 1:7 w points, 'plot.dat' u 1:8 w points, 'plot.dat' u 1:9 w points, 'plot.dat' u 1:10 w points, 'plot.dat' u 1:11 w points\n");
    //fprintf(gp, "plot 'plot.dat' u 1:2 w points, 'plot.dat' u 1:3 w points, 'plot.dat' u 1:4 w points, 'plot.dat' u 1:5 w points, 'plot.dat' u 1:6 w points, 'plot.dat' u 1:7 w points, 'plot.dat' u 1:8 w points, 'plot.dat' u 1:9 w points, 'plot.dat' u 1:10  w points, 'plot.dat' u 1:11  w points, 'plot.dat' u 1:12  w points, 'plot.dat' u 1:13  w points, 'plot.dat' u 1:14  w points, 'plot.dat' u 1:15  w points, 'plot.dat' u 1:16  w points, 'plot.dat' u 1:17  w points, 'plot.dat' u 1:18  w points, 'plot.dat' u 1:19  w points, 'plot.dat' u 1:20  w points, 'plot.dat' u 1:21 w points\n");
        //fprintf(gp, "pause -1\n");
    fclose(gp);
// show..
	return 0;
}
