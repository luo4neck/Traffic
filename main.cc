#include "head.hpp"

using namespace std;

const int car_num = 1600;
const double p_randomization= 0.15;

int main(int argc, char *argv[])
{
	//for(int i=0; i<16; ++i)		cout<<Four[i][0]<<" "<<Four[i][1]<<" "<<Four[i][2]<<" "<<Four[i][3]<<endl;
	map<int, LR> spot;
	map<int, LR>:: iterator spotitr;
	map<int, CROSS> cross;
	map<int, CROSS> crositr;
	list<class CAR> car;
	list<class CAR>:: iterator caritr;
	
	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;
	const int myid( world.rank() );

	// session 4
	int ewnum = 20;
	int nsnum = 20;
	int *ewrange = new int[ewnum]; 
	int *nsrange = new int[nsnum];
	
	for(int i=0; i<ewnum; ++i) {	ewrange[i] = 100 * i + 50;	}
	
	for(int i=0; i<nsnum; ++i) {	nsrange[i] = 100 * i + 50;	}
	
	BOUND bound(2000, 0, 2000, 0, ewnum, nsnum, ewrange, nsrange);
			  //e,    w, n,    s, 
	int ewns[4];
	for(int i=0; i<4; ++i)
	{
		ewns[i] = Four[myid][i];
		// ewns[i] = Sixteen[myid][i];
	}
	printf("for proc %d: e=%d w=%d n=%d s=%d\n", myid, ewns[EAST], ewns[WEST], ewns[NORTH], ewns[SOUTH]);
	
	bound.Construct(spot, cross, ewns); // construct the map in this process..
	// session 4

	srand48(time(NULL));
	for(int i=0; i < car_num; ++i) // constructing the cars in this process..
	{
		bool check = 1;
		while(check) 
		{
			int x, y, drct;
			if( i < car_num/2 )  // cars on ew road..
			{
				y = nsrange[ lrand48()%nsnum ];
				x =	lrand48() % bound.Et();
				double random = drand48();
				if( random < 0.5 ) 	drct = EAST;
				else				drct = WEST;
			}
			else // cars on ns road..
			{
				x = ewrange[ lrand48()%ewnum ];
				y =	lrand48() % bound.Nt();
				double random = drand48();
				if( random < 0.5 ) 	drct = SOUTH;
				else				drct = NORTH;
			}
		
			CAR newcar(x, y, drct);
			spotitr = spot.find( XYtoKEY( x, y) );
			if (spotitr != spot.end() )
			{
				if ( !((newcar.DRCT()%2 == 0 && spotitr->second.rt == 1) || (newcar.DRCT()%2 == 1 && spotitr->second.lt == 1)) )
				{
					if ( newcar.DRCT()%2 == 0)  spotitr->second.rt = 1;  // go to north or east..
					else                       	spotitr->second.lt = 1;	 // goto west or south.. 
					
					car.push_back(newcar);
					check = 0;
				}
			}
		}
	}

	cout<<"simulation start!"<<endl;

	int time_i = 0, time_max = 5;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		if ( time_i%10 == 0 ) Signal_Switch(cross);
		cout<<"At time "<<time_i<<" "<<endl;
		
		//map exchange part..
		


		//map exchange part..

		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			caritr->Move(newx, newy, newdrct, spot);
		}
		
		{
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
		
		//car exchange part..
		
		
		
		
		//car exchange part..

		time_i++;
	}

	//system("convert -delay 25 -loop 0 *.png Moving.gif\n");	// session 3..
	return 0;
}
