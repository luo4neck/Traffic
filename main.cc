#include "head.hpp"

using namespace std;

const int car_num = 16;
const double p_randomization= 0.05;

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
	
	int ewns[4]; // to recv 4 boundary info, one array two usage..
	int ewnum, nsnum, *ewrange, *nsrange;
	//BOUND bound;
	int Bet, Bwt, Bnt, Bst;
	if( myid == 0 )
	{
		ewnum = 2, nsnum = 2;
		ewrange = new int[2];
		nsrange = new int[2];
		ewrange[0] = 20;
		ewrange[1] = 40;
		nsrange[0] = 60;
		nsrange[1] = 80;
		Bet = 50, Bwt = 0, Bnt = 100, Bst = 51;
	//	bound(50, 0, 100, 51, ewnum, nsnum, ewrange, nsrange);
				 	  //e,    w, n,    s, 
	}
	if( myid == 1 )
	{
		ewnum = 2, nsnum = 2;
		ewrange = new int[2];
		nsrange = new int[2];
		ewrange[0] = 60;
		ewrange[1] = 80;
		nsrange[0] = 60;
		nsrange[1] = 80;
		Bet = 100, Bwt = 51, Bnt = 100, Bst = 51;
	//	BOUND bound(100, 51, 100, 51, ewnum, nsnum, ewrange, nsrange);
	}
	if( myid == 2 )
	{
		ewnum = 2, nsnum = 2;
		ewrange = new int[2];
		nsrange = new int[2];
		ewrange[0] = 20;
		ewrange[1] = 40;
		nsrange[0] = 20;
		nsrange[1] = 40;
		Bet = 50, Bwt = 0, Bnt = 50, Bst = 0;
	//	BOUND bound(50, 0, 50, 0, ewnum, nsnum, ewrange, nsrange);
	}
	if( myid == 3 )
	{
		ewnum = 2, nsnum = 2;
		ewrange = new int[2];
		nsrange = new int[2];
		ewrange[0] = 60;
		ewrange[1] = 80;
		nsrange[0] = 20;
		nsrange[1] = 40;
		Bet = 100, Bwt = 51, Bnt = 50, Bst = 0;
	//	BOUND bound(100, 51, 50, 0, ewnum, nsnum, ewrange, nsrange);
	}

	BOUND bound(Bet, Bwt, Bnt, Bst, ewnum, nsnum, ewrange, nsrange);

	for(int i=0; i<4; ++i)
	{
		ewns[i] = Four[myid][i];
		// ewns[i] = Sixteen[myid][i];
	}
	printf("for proc %d: e=%d w=%d n=%d s=%d\n", myid, ewns[EAST], ewns[WEST], ewns[NORTH], ewns[SOUTH]);
	
	bound.Construct(spot, cross, ewns); // construct the map in this process..
	// session 4
	
/*	
	if(myid == 0)
	{
		cout<<"This is test of p 0"<<endl;
		for(spotitr = spot.begin(); spotitr != spot.end(); ++spotitr)
		{
			cout<<spotitr->first<<endl;
		}
	}
*/
/*
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
*/
	cout<<"simulation start!"<<endl;

	int time_i = 0, time_max = 1;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		if ( time_i%10 == 0 ) Signal_Switch(cross);
		cout<<"At time "<<time_i<<" "<<endl;
		
		//map exchange part.. 
		boost::mpi::request req[8];// tmp..
		map<int, LR> RE, RW, RN, RS; // recv from east, west, north, south..
		if( ewns[EAST] >= 0 )
		{
			if(myid == 0)
			{spot[460080].rt = 1;
			spot[460080].lt = 1;}
			map<int, LR> ES; // east direction send..
			bound.Epackout(ES, spot);
			req[0] = world.isend( ewns[EAST], myid+100, ES); // process id+10 of sender is the tag..
			req[1] = world.irecv( ewns[EAST], ewns[EAST]+100, RE);
		} 
		if( ewns[WEST] >= 0 )
		{
			map<int, LR> WS; // east direction send..
			bound.Wpackout(WS, spot);
			req[2] = world.isend( ewns[WEST], myid+100, WS); // process id+10 of sender is the tag..
			req[3] = world.irecv( ewns[WEST], ewns[WEST]+100, RW);
		}
		if( ewns[NORTH] >= 0 )
		{
			map<int, LR> NS; // east direction send..
			bound.Npackout(NS, spot);
			req[4] = world.isend( ewns[NORTH], myid+100, NS); // process id+10 of sender is the tag..
			req[5] = world.irecv( ewns[NORTH], ewns[NORTH]+100, RN);
		}
		if( ewns[SOUTH] >= 0 )
		{
			map<int, LR> SS; // east direction send..
			bound.Spackout(SS, spot);
			req[6] = world.isend( ewns[SOUTH], myid+100, SS); // process id+10 of sender is the tag..
			req[7] = world.irecv( ewns[SOUTH], ewns[SOUTH]+100, RS);
		}
		
		//waitall
		boost::mpi::wait_all(req, req+8);
		if(myid == 1)
		{
			cout<<"hey!!"<<endl;
			map<int, LR>:: iterator itr2;
			for(itr2 = RW.begin(); itr2 != RW.end(); ++itr2)
			{
				cout<<"ha"<<endl;
				cout<<itr2->first<<" "<<itr2->second.rt<<" "<<itr2->second.lt<<endl;
			}
		}
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
		cout<<myid<<" succ at time "<<time_i<<endl;
	}

	//system("convert -delay 25 -loop 0 *.png Moving.gif\n");	// session 3..
	return 0;
}
