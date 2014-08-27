#include "head_s.hpp"
using namespace std;

const double p_randomization= 0.01;

int main(int argc, char *argv[])
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;

	int ewnum = 2, nsnum = 2;
	int *ewrange = new int[2];
	int *nsrange = new int[2];
	ewrange[0] = 20;
	ewrange[1] = 180;
		
	nsrange[0] = 20;
	nsrange[1] = 80;
	int Bet = 200, Bwt = 0, Bnt = 100, Bst = 0;
	
	BOUND bound(Bet, Bwt, Bnt, Bst, ewnum, nsnum, ewrange, nsrange);

	int ewns_s[4] = {-1, -1, -1, -1};	
	bound.Construct(spot, cross, ewns_s); // construct the map in this process..
	
	//if(myid == 0)
	{
		cout<<"This is test of p 0"<<endl;
		string PATH;
		PATH.assign(500, 's');
		int base = 130;
		for(int i=0; i<15; ++i)
		{
			CAR newcar(base + i, 80, EAST, PATH);
			car.push_back(newcar);
			spot[ XYtoKEY( base + i, 80 )].rt = 1;
		}
	}

	srand48(time(NULL));
	cout<<"simulation start!"<<endl;

	ofstream file("plot_s.dat");
	int time_i = 0, time_max = 35;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		//if (myid == 0)	cout<<"At time "<<time_i<<" "<<endl;
		//if ( time_i%10 == 0 ) Signal_Switch(cross);
	for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
	{
		char turn = caritr->path[0];
		//char turn = 'r'; 
		int newx(0), newy(0), newdrct(0);
		bool rand = 1; // deal with the randomization..
		if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
		
		caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
		{
			if (newy == 80 && newx > 150 ) newx = 90;
		}
		caritr->Move(newx, newy, newdrct, spot);
		
		if( caritr->X() > bound.Et() ) // if come out of range.. set del to 1, add to send list..
		{
			caritr->del = 1;
		}
		if( caritr->X() < bound.Wt() ) // if come out of range.. set del to 1, add to send list..
		{
			caritr->del = 1;
		}
		if( caritr->Y() > bound.Nt() ) // if come out of range.. set del to 1, add to send list..
		{
			caritr->del = 1;
		}
		if( caritr->Y() < bound.St() ) // if come out of range.. set del to 1, add to send list..
		{
			caritr->del = 1;
		}
		car.remove_if( check_del() );
	}

		if( time_i > 10 ) 
		{
			file<<time_i<<" ";
			for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
			{
				//cout<<"time "<<time_i<<": "<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->DRCT()<<" "<<caritr->path<<endl;
				cout<<"time "<<time_i<<": "<<caritr->X()<<" "<<caritr->DRCT()<<" "<<endl;
				file<<caritr->X()<<" ";
			}
			file<<endl;
			cout<<endl;
		}
		time_i++;
	}
	
	{// gnuplot part..
		FILE *gp = popen("gnuplot -persist", "w");;
		if(gp == NULL)
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		fprintf(gp, "set key left top\n");// this line is just for proposal show..
		fprintf(gp, "set title 'Serial Periodic Test'\n");// this line is just for proposal show..
		fprintf(gp, "set xrange[6:35]\n");
		fprintf(gp, "set xlabel 'Time / Second'\n");
		fprintf(gp, "set ylabel 'Location on the Road'\n");
		fprintf(gp, "plot 'plot_s.dat' u 1:2 w points title 'car 0', 'plot_s.dat' u 1:3 w points title 'car 1', 'plot_s.dat' u 1:4 w points title 'car 2', 'plot_s.dat' u 1:5 w points title 'car 3', 'plot_s.dat' u 1:6 w points title 'car 4', 'plot_s.dat' u 1:7 w points title 'car 5', 'plot_s.dat' u 1:8 w points title 'car 6', 'plot_s.dat' u 1:9 w points title 'car 7', 'plot_s.dat' u 1:10 w points title 'car 8', 'plot_s.dat' u 1:11 w points title 'car 9', 'plot_s.dat' u 1:12 w points title 'car 10', 'plot_s.dat' u 1:13 w points title 'car 11', 'plot_s.dat' u 1:14 w points title 'car 12', 'plot_s.dat' u 1:15 w points title 'car 13', 'plot_s.dat' u 1:16 w points title 'car 14'\n"); 
					////fprintf(gp, "pause -1\n");
		fclose(gp);
	}


	file.close();
	return 0;
}
