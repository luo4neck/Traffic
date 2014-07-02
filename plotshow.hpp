#include<stdlib.h>
#include<fstream>

int** Plot_new(const int r, const int c)// r is time_max, c is ncar..
// construct a 2D pointer to plot the graph..
{
	int** plot = new int*[r];
    for(int i=0; i<r; ++i)
    {
        plot[i] = new int[c+1];
        plot[i][0] = i;
	}
	return plot;
}

void Plot_write(const int time_i, const int ncar, int** plot, const CAR* start)
// write something into the Pointer..
{
	for(int i=1; i<=ncar; ++i)
	{
		plot[time_i-1][i] = start->x;
		if(start->next != NULL) start = start->next;
	}
}

void Plot_show(int** plot, const int time_max, const int ncar)
// gnuplot part..
{
	ofstream file("plot.dat");
    for(int i=0; i<time_max; ++i)
	{
		for(int j=0; j<=ncar; ++j)
		{
			file<<plot[i][j]<<" ";
		}
		file<<endl;
	}
	file.close();

	FILE *gp = popen("gnuplot -persist", "w");;
	if(gp == NULL) 
	{
		printf("Cannot plot the data!\n");
		exit(0);
	}
	fprintf(gp, "set title 'Red light is on from time 0 to time 55 at location 150'\n");// this line is just for proposal show..
	//fprintf(gp, "set key font ',20'\n"); // seems not supported by gnuplot 4.2..
	fprintf(gp, "set key left top\n");// this line is just for proposal show..
	fprintf(gp, "set ylabel 'Location on the Road\n");
	fprintf(gp, "set xlabel 'Time / Second\n"); 
	fprintf(gp, "plot 'plot.dat' u 1:2 w points title 'car 1', 'plot.dat' u 1:3 w points title 'car 2', 'plot.dat' u 1:4 w points title 'car 3', 'plot.dat' u 1:5 w points title 'car 4', 'plot.dat' u 1:6 w points title 'car 5', 'plot.dat' u 1:7 w points title 'car 6', 'plot.dat' u 1:8 w points title 'car 7', 'plot.dat' u 1:9 w points title 'car 8', 'plot.dat' u 1:10 w points title 'car 9', 'plot.dat' u 1:11 w points title 'car 10'\n");// this line is just for proposal show..
	//fprintf(gp, "plot 'plot.dat' u 1:2 w points, 'plot.dat' u 1:3 w points, 'plot.dat' u 1:4 w points, 'plot.dat' u 1:5 w points, 'plot.dat' u 1:6 w points, 'plot.dat' u 1:7 w points, 'plot.dat' u 1:8 w points, 'plot.dat' u 1:9 w points, 'plot.dat' u 1:10  w points, 'plot.dat' u 1:11  w points, 'plot.dat' u 1:12  w points, 'plot.dat' u 1:13  w points, 'plot.dat' u 1:14  w points, 'plot.dat' u 1:15  w points, 'plot.dat' u 1:16  w points, 'plot.dat' u 1:17  w points, 'plot.dat' u 1:18  w points, 'plot.dat' u 1:19  w points, 'plot.dat' u 1:20  w points, 'plot.dat' u 1:21 w points\n");
	fclose(gp);
}

void Plot_delete(const int time_max, int **plot)
// delete the pointer safely..
{
	for(int i=0; i<time_max; ++i) delete[] plot[i];
	delete[] plot;
}
