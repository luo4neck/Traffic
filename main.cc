#include "head.hpp"
using namespace std;

const double p_randomization= 0.05; // probability of randomization..

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
	const int nps( world.size() );
	int car_num;
	
	// start checking input.. 
	// only process 0 doing input checking part here..
	int North_Max, East_Max;
	vector<int> EWRANGE, NSRANGE;
	if( myid == 0 ) 
	{
		Input_Check(nps, argc, argv, car_num, North_Max, East_Max, EWRANGE, NSRANGE);
	}  
	// input checking part finish here..

	// job part start here allocating..
	int ewnum, nsnum, *ewrange, *nsrange;
	int Bet, Bwt, Bnt, Bst;
	int *forp0 = new int[4];

	if(nps != 1) 
	{
		if( myid == 0 )// process 0 do job sending.. 
		{
			int nsblock = North_Max/sqrt(nps);
			int ewblock =  East_Max/sqrt(nps);
		
			int pid = 0;
			for(int i=sqrt(nps)-1; i>=0; --i) // loop from south to north direction..
			{
				Bnt = i * nsblock + nsblock;
				Bst = i * nsblock + 1;
				if ( i == (sqrt(nps) -1) )  Bnt = North_Max;
				else if ( i == 0 )			Bst = 0;
			
				vector<int>::iterator nsitr;
				vector<int> ns;
				for(nsitr = NSRANGE.begin(); nsitr != NSRANGE.end(); ++nsitr)
				{
					if ( *nsitr > Bst && *nsitr < Bnt ) ns.push_back( *nsitr );
				}

				for(int j=0; j<sqrt(nps); ++j) // loop from west to east direction..
				{
					Bet = j * ewblock + ewblock;
					Bwt = j * ewblock + 1;
					if ( j == (sqrt(nps) -1) )  Bet = East_Max;
					if ( j == 0 )				Bwt = 0;
				
					vector<int>::iterator ewitr;
					vector<int> ew;
					for(ewitr = EWRANGE.begin(); ewitr != EWRANGE.end(); ++ewitr)
					{
						if ( *ewitr > Bwt && *ewitr < Bet ) ew.push_back( *ewitr );
					}
					
					if (pid == 0)
					{
						forp0[0] = Bet, forp0[1] = Bwt, forp0[2] = Bnt, forp0[3] = Bst;
						ewnum = ew.size();
						ewrange = new int[ewnum];
						for(int jj=0; jj<ewnum; ++jj)	ewrange[jj] = ew.at(jj);
					
						nsnum = ns.size();
						nsrange = new int[nsnum];
						for(int ii=0; ii<nsnum; ++ii)   nsrange[ii] = ns.at(ii);
					}
					else
					{
						int limit5[5] = { Bet, Bwt, Bnt, Bst, car_num};
						world.send(pid, 444, limit5);
						world.send(pid, 333, ns);
						world.send(pid, 222, ew);
					}
					pid++;
				}
			}
		}// end if of myid == 0
		else			// other processes recv job..
		{
			int limit5[5];
			world.recv(0, 444, limit5);
			Bet = limit5[0], Bwt = limit5[1], Bnt = limit5[2], Bst = limit5[3], car_num = limit5[4];
		
			vector<int> ew, ns;
			world.recv(0, 333, ns);
			world.recv(0, 222, ew);
		
			ewnum = ew.size();
			ewrange = new int[ewnum];
			for(int jj=0; jj<ewnum; ++jj) ewrange[jj] = ew.at(jj);
		
			nsnum = ns.size();
			nsrange = new int[nsnum];
			for(int ii=0; ii<nsnum; ++ii) nsrange[ii] = ns.at(ii);
		}	
	
		if(myid == 0) { Bet = forp0[0], Bwt = forp0[1], Bnt = forp0[2], Bst = forp0[3];	}
		delete[] forp0;
		// job part finish here allocating..
	} // end if of nps != 1
	else
	{
		Bet = East_Max, Bwt = 0, Bnt = North_Max, Bst = 0;
		ewnum = EWRANGE.size();
		nsnum = NSRANGE.size();
		ewrange = new int[ewnum];
		nsrange = new int[nsnum];
		
		vector<int>::iterator itr;
		int i=0;
		for(itr = EWRANGE.begin(); itr != EWRANGE.end(); ++itr)
		{ ewrange[i] = *itr; i++; }
		i=0;	
		for(itr = NSRANGE.begin(); itr != NSRANGE.end(); ++itr)
		{ nsrange[i] = *itr; i++; }
	}
	
	// map constructing part start here..
	cout<<Bet<<" "<<Bwt<<" "<<Bnt<<" "<<Bst<<endl;
	BOUND bound(Bet, Bwt, Bnt, Bst, ewnum, nsnum, ewrange, nsrange);
	int ewns[4]; // to recv 4 boundary info, one array two usage..
	for(int i=0; i<4; ++i)
	{
		if( nps == 4 )			ewns[i] = comap4[myid][i];
		else if ( nps == 16)	ewns[i] = comap16[myid][i];
		else if ( nps == 64)	ewns[i] = comap64[myid][i];
		else					ewns[i] = -1;
	}
	bound.Construct(spot, cross, ewns); // construct the map in this process..
	// map constructing part finish here..
	

	// constructing the cars in this process..
	srand48(time(NULL) + pow(myid, 5) ); // make each process have a unique seed..
	car_num = car_num/nps;				// each process do have same number of vehicles..
	
	//Add_Car(bound, car, ewnum, nsnum, car_num, spot); 

	if(nps == 1)
	{
			string PATH;
			PATH.assign(500, 'r');
			int base = 11;
			for( int i=0; i<car_num/4; ++i )
			{
				CAR newcar( base + i, 90, EAST, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( base + i, 90) ].rt = 1;
			}
			for( int i=0; i<car_num/4; ++i )
			{
				CAR newcar( base + i, 10, WEST, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( base + i, 10) ].lt = 1;
			}
			for( int i=0; i<car_num/4; ++i)
			{
				CAR newcar( 10, base + i, NORTH, PATH );
				car.push_back(newcar);
				spot[ XYtoKEY( 10, base + i) ].rt = 1;
			}
			for( int i=0; i<car_num/4; ++i)
			{
				CAR newcar( 90, base + i, SOUTH, PATH );
				car.push_back(newcar);
				spot[ XYtoKEY( 90, base + i) ].lt = 1;
			}
	}
	else  // nps = 4;
	{
		string PATH;
		PATH.assign(500, 'r');
		int base = 11;
		if(myid == 0)
		{
			for(int i=0; i<car_num/2; ++i)  // x 
			{
				CAR newcar( base + i, 90, EAST, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( base + i, 90 ) ].rt = 1;
			}
			for(int i=0; i<car_num/2; ++i)  // y 
			{
				CAR newcar( 10, 51+i, NORTH, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( 10, 51+i ) ].rt = 1;
			}
		}
		else if(myid == 1)
		{
			for(int i=0; i<car_num/2; ++i)  // x 
			{
				CAR newcar( 51 + i, 90, EAST, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( 51+i, 90 ) ].rt = 1;
			}
			for(int i=0; i<car_num/2; ++i)  // y 
			{
				CAR newcar( 90, 51+i, SOUTH, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( 90, 51+i ) ].lt = 1;
			}
		}
		else if(myid == 2)
		{
			for(int i=0; i<car_num/2; ++i)  // y 
			{
				CAR newcar( 10, base+i, NORTH, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( 10, base+i ) ].rt = 1;
			}
			for(int i=0; i<car_num/2; ++i)  // x 
			{
				CAR newcar( base + i, 10, WEST, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( base + i, 10 ) ].lt = 1;
			}
		}
		else //if(myid == 3)
		{
			for(int i=0; i<car_num/2; ++i)  // x 
			{
				CAR newcar( 51 + i, 10, WEST, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( 51 + i, 10 ) ].lt = 1;
			}
			for(int i=0; i<car_num/2; ++i)  // y 
			{
				CAR newcar( 90, base+i, SOUTH, PATH);
				car.push_back(newcar);
				spot[ XYtoKEY( 90, base+i ) ].lt = 1;
			}
		}
	}
	
	world.barrier();
	// car insert part..

	if(myid == 0)
	{	
		cout<<endl<<"Each process simulate:"<<endl;
		cout<<car.size()<<" vehicles "<<sizeof(car)*car.size()/1024<<"KB"<<endl;
		cout<<spot.size()<<" spots "<<sizeof(spot)*spot.size()/1024<<"KB"<<endl;
		cout<<cross.size()<<" crosses "<<sizeof(cross)*cross.size()/1024<<"KB"<<endl<<endl;
		cout<<"simulation start!"<<endl;
	}
	int time_i = 0, time_max = 1000, den = 0, flow = 0;
	time_t start = time(NULL);
	while(time_i < time_max ) // main loop.. one loop is one time step..
	{
		//if ( time_i%20 == 0 ) Signal_Switch(cross);
		//cross initialize function in head.hpp is also changed..
		//if (myid == 0)	cout<<"At time "<<time_i<<" "<<endl;
		
		if( nps != 1 )
		{
			world.barrier();
		
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
		}
		//map exchange part..
		
		// traverse of cars..
		bool flow_check = 0;
		list<class CAR> ESCAR, WSCAR, NSCAR, SSCAR; // list of cars sending to e w n s..
		for(caritr = car.begin(); caritr!=car.end(); ++caritr) 
		{
			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			//cout<<"rand "<<rand<<endl;
			//if(myid == 3)cout<<"P: "<<myid<<" "<<caritr->X()<<" "<<caritr->Y()<<" "<<caritr->DRCT()<<" "<<caritr->del;
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			
			{// flow test..
				if ( caritr->Y() == 90 )
				{
					if ( caritr->X() < 35 && newx > 36 )	flow_check = 1;
				}
			}
			
			caritr->Move(newx, newy, newdrct, spot);
		
			if(nps != 1)
			{
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
			}
		}

		for(caritr = car.begin(); caritr != car.end(); ++caritr)
		{
			if (caritr->del == 1)
			{
				if(caritr->DRCT()%2 == 0 ) spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].rt = 0;
				else					   spot[ XYtoKEY( caritr->X(), caritr->Y() ) ].lt = 0;
			}
		}
		// traverse of cars..
		
		car.remove_if( check_del() );
		
		if(nps != 1) 
		{	
			world.barrier();
			boost::mpi::request req[8];
			
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
		
			if( ewns[EAST]  >= 0 ) // move recieved cars into car..
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
		}	
	
		//if( (int)car.size() < car_num )	Add_Car(bound, car, ewnum, nsnum, car_num-car.size(), spot); 
		// the number of deleted vehicle are added into the map..
		
		{// density test..
			spotitr = spot.find( XYtoKEY( 35, 90 ) );
			if (spotitr != spot.end() && spotitr->second.rt == 1 ){	den++;	}
		}
		
		if(flow_check) flow++;
			
		if(myid == 0) 
		{
			time_t end = time(NULL);
		//	cout<<end-start<<" seconds spent for this step"<<endl<<endl;
		}
		time_i++;
	}

	if( myid == 0 )
	{// this scope is for flow test..
		double flow_p = (double)flow / (double)time_max;
		double den_p = (double)den / (double)time_max;
		cout<<"flow: "<<flow_p<<", density: "<<den_p<<" "<<car.size()<<endl;
	}
	if( nps != 1 )
	{
		cout<<myid<<" "<<car.size()<<endl;
	}

	return 0;
}
