#include "head_s.hpp"
using namespace std;

const double p_randomization= 0.0;

int main(int argc, char *argv[])
{
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;

	int ewnum = 4, nsnum = 4;
	int *ewrange = new int[4];
	int *nsrange = new int[4];
	ewrange[0] = 20;
	ewrange[1] = 40;
	ewrange[2] = 60;
	ewrange[3] = 80;
		
	nsrange[0] = 20;
	nsrange[1] = 40;
	nsrange[2] = 60;
	nsrange[3] = 80;
	int Bet = 100, Bwt = 0, Bnt = 100, Bst = 0;
	
	BOUND bound(Bet, Bwt, Bnt, Bst, ewnum, nsnum, ewrange, nsrange);

	int ewns_s[4] = {-1, -1, -1, -1};	
	bound.Construct(spot, cross, ewns_s); // construct the map in this process..
	
	//if(myid == 0)
	{
		cout<<"This is test of p 0"<<endl;
		string PATH = "srrssllssss";
		CAR newcar(48, 60, EAST, PATH);
		car.push_back(newcar);
		
		string PATH1 = "ssrrssllssss";
		CAR newcar1(38, 60, EAST, PATH1);
		//car.push_back(newcar1);
		
		spotitr = spot.find( XYtoKEY( 48, 60 ));
		if ( spotitr != spot.end() )	spotitr->second.rt = 1;
		else 							cout<<"wrong!!"<<endl<<endl;
	}

	srand48(time(NULL));
	
	cout<<"simulation start!"<<endl;

	int time_i = 0, time_max = 60;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		//if (myid == 0)	cout<<"At time "<<time_i<<" "<<endl;
	if ( time_i%10 == 0 ) Signal_Switch(cross);
	for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
	{
		char turn = caritr->path[0];
		int newx(0), newy(0), newdrct(0);
		bool rand = 1; // deal with the randomization..
		if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..

		
		caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
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
	for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
	{
		cout<<"time "<<time_i<<": "<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->DRCT()<<" "<<caritr->path<<endl;
	}
	cout<<endl;
	time_i++;
	}
	return 0;
}
