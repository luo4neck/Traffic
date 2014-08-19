#include "head.hpp"

using namespace std;

const int car_num = 16;
const double p_randomization= 0.05;

int main(int argc, char *argv[])
{
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
	}

	BOUND bound(Bet, Bwt, Bnt, Bst, ewnum, nsnum, ewrange, nsrange);

	for(int i=0; i<4; ++i)
	{
		ewns[i] = Four[myid][i];
		// ewns[i] = Sixteen[myid][i];
	}
	
	bound.Construct(spot, cross, ewns); // construct the map in this process..
	
	if(myid == 0)
	{
		cout<<"This is test of p 0"<<endl;
		string PATH = "srssss";
		CAR newcar(48, 60, EAST, PATH);
		car.push_back(newcar);
		spotitr = spot.find( XYtoKEY( 48, 60 ));
		if ( spotitr != spot.end() )	spotitr->second.rt = 1;
		else 							cout<<"wrong!!"<<endl<<endl;
	}

	srand48(time(NULL));
/*
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
	if(myid == 0) cout<<"simulation start!"<<endl;

	int time_i = 0, time_max = 24;
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		world.barrier();
		//sleep(1);
		if (myid == 0)	cout<<"At time "<<time_i<<" "<<endl;
		if ( time_i%10 == 0 ) Signal_Switch(cross);
		
		//map exchange part.. 
		boost::mpi::request req[8];
		map<int, LR> RE, RW, RN, RS; // recv from east, west, north, south..
		if( ewns[EAST] >= 0 )
		{
			map<int, LR> ES; // east direction send..
			bound.Epackout(ES, spot);
			req[0] = world.isend( ewns[EAST], myid+100, ES); // process id+10 of sender is the tag..
			req[1] = world.irecv( ewns[EAST], ewns[EAST]+100, RE);
		} 
		if( ewns[WEST] >= 0 )
		{
			map<int, LR> WS; // west direction send..
			bound.Wpackout(WS, spot);
			req[2] = world.isend( ewns[WEST], myid+100, WS); // process id+10 of sender is the tag..
			req[3] = world.irecv( ewns[WEST], ewns[WEST]+100, RW);
		}
		if( ewns[NORTH] >= 0 )
		{
			map<int, LR> NS; // north direction send..
			bound.Npackout(NS, spot);
			req[4] = world.isend( ewns[NORTH], myid+100, NS); // process id+10 of sender is the tag..
			req[5] = world.irecv( ewns[NORTH], ewns[NORTH]+100, RN);
		}
		if( ewns[SOUTH] >= 0 )
		{
			map<int, LR> SS; // south direction send..
			bound.Spackout(SS, spot);
			req[6] = world.isend( ewns[SOUTH], myid+100, SS); // process id+10 of sender is the tag..
			req[7] = world.irecv( ewns[SOUTH], ewns[SOUTH]+100, RS);
		}
		
		boost::mpi::wait_all(req, req+8);	//waitall
		
		if( ewns[EAST]  >= 0 ) // update spot by recieved data..
		{
			map<int, LR>:: iterator itr;
			for(itr = RE.begin(); itr != RE.end(); ++itr)
			{ spot[itr->first].lt = itr->second.lt; spot[itr->first].rt = itr->second.rt; }
		}
		if( ewns[WEST]  >= 0 ) // update spot by recieved data..
		{
			map<int, LR>:: iterator itr;
			for(itr = RW.begin(); itr != RW.end(); ++itr)
			{ spot[itr->first].lt = itr->second.lt; spot[itr->first].rt = itr->second.rt; }
		}
		if( ewns[NORTH] >= 0 ) // update spot by recieved data..
		{
			map<int, LR>:: iterator itr;
			for(itr = RN.begin(); itr != RN.end(); ++itr)
			{ spot[itr->first].lt = itr->second.lt; spot[itr->first].rt = itr->second.rt; }
		}
		if( ewns[SOUTH] >= 0 ) // update spot by recieved data..
		{
			map<int, LR>:: iterator itr;
			for(itr = RS.begin(); itr != RS.end(); ++itr)
			{ spot[itr->first].lt = itr->second.lt; spot[itr->first].rt = itr->second.rt; }
		}
		//map exchange part..
		
		list<class CAR> ESCAR, WSCAR, NSCAR, SSCAR; // list of cars sending to e w n s..
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			//if ( drand48() < p_randomization ) rand = 1; // 0 is do randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			cout<<"rand "<<rand<<endl;
			cout<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->DRCT()<<" "<<caritr->del<<endl;
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			caritr->Move(newx, newy, newdrct, spot);
			
			if( caritr->X() > bound.Et() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path );
				ESCAR.push_back(newcar);
				caritr->del = 1;
			}
			if( caritr->X() < bound.Wt() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path );
				WSCAR.push_back(newcar);
				caritr->del = 1;
			}
			if( caritr->Y() > bound.Nt() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path );
				NSCAR.push_back(newcar);
				caritr->del = 1;
			}
			if( caritr->Y() < bound.St() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path );
				SSCAR.push_back(newcar);
				caritr->del = 1;
			}
			cout<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->DRCT()<<" "<<caritr->del<<endl;
		}

		for(caritr = car.begin(); caritr != car.end(); ++caritr)
		{
			cout<<"time: "<<time_i<<" in proc "<<myid<<": "<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->del<<endl<<endl;
			if (caritr->del == 1)
			{
				if(caritr->DRCT()%2 == 0 ) spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].rt = 0;
				else					   spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].lt = 0;
			}
		}
		
		car.remove_if( check_del() );
		world.barrier();
		//sleep(1);
		
		//car exchange part..
		list<class CAR> ERCAR, WRCAR, NRCAR, SRCAR; // list of cars recved from e w n s..
		if( ewns[EAST]  >= 0 )
		{
			req[0] = world.isend( ewns[EAST], myid+100, ESCAR); // process id+10 of sender is the tag..
			req[1] = world.irecv( ewns[EAST], ewns[EAST]+100, ERCAR);
		}
		if( ewns[WEST]  >= 0 )
		{
			req[2] = world.isend( ewns[WEST], myid+100, WSCAR); // process id+10 of sender is the tag..
			req[3] = world.irecv( ewns[WEST], ewns[WEST]+100, WRCAR);
		}
		if( ewns[NORTH] >= 0 )
		{
			req[4] = world.isend( ewns[NORTH], myid+100, NSCAR); // process id+10 of sender is the tag..
			req[5] = world.irecv( ewns[NORTH], ewns[NORTH]+100, NRCAR);
		}
		if( ewns[SOUTH] >= 0 )
		{
			req[6] = world.isend( ewns[SOUTH], myid+100, SSCAR); // process id+10 of sender is the tag..
			req[7] = world.irecv( ewns[SOUTH], ewns[SOUTH]+100, SRCAR);
		}
		
		boost::mpi::wait_all(req, req+8);	//waitall
		//if(myid == 1) cout<<ERCAR.size()<<" "<<WRCAR.size()<<" "<<NRCAR.size()<<" "<<SRCAR.size()<<endl;
		
		if( ewns[SOUTH]  >= 0 ) // move recieved cars into car..
		{	
			list<class CAR>::iterator itr;
			for(itr = ERCAR.begin(); itr != ERCAR.end(); ++itr)
			{
				if (itr->DRCT()%2 == 0)	spot[ XYtoKEY( itr->X(), itr->Y() ) ].rt = 1;
				else					spot[ XYtoKEY( itr->X(), itr->Y() ) ].lt = 1;
			}
			car.splice(car.begin(), ERCAR);
		}
		if( ewns[WEST]  >= 0 )  // move recieved cars into car..
		{
			list<class CAR>::iterator itr;
			for(itr = WRCAR.begin(); itr != WRCAR.end(); ++itr)
			{
				if (itr->DRCT()%2 == 0)	spot[ XYtoKEY( itr->X(), itr->Y() ) ].rt = 1;
				else					spot[ XYtoKEY( itr->X(), itr->Y() ) ].lt = 1;
			}
			car.splice(car.begin(), WRCAR);
		}
		if( ewns[NORTH] >= 0 ) // move recieved cars into car..
		{
			list<class CAR>::iterator itr;
			for(itr = NRCAR.begin(); itr != NRCAR.end(); ++itr)
			{
				if (itr->DRCT()%2 == 0)	spot[ XYtoKEY( itr->X(), itr->Y() ) ].rt = 1;
				else					spot[ XYtoKEY( itr->X(), itr->Y() ) ].lt = 1;
			}
			car.splice(car.begin(), NRCAR);
		}
		if( ewns[SOUTH] >= 0 ) // move recieved cars into car..
		{
			list<class CAR>::iterator itr;
			for(itr = SRCAR.begin(); itr != SRCAR.end(); ++itr)
			{
				if (itr->DRCT()%2 == 0)	spot[ XYtoKEY( itr->X(), itr->Y() ) ].rt = 1;
				else					spot[ XYtoKEY( itr->X(), itr->Y() ) ].lt = 1;
			}
			car.splice(car.begin(), SRCAR);
		}
		//car exchange part..

		time_i++;
	}

	//system("convert -delay 25 -loop 0 *.png Moving.gif\n");	// session 4..
	return 0;
}
