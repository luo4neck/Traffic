#include "head.hpp"
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
	
	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;
	const int myid( world.rank() );
	//const int nps( world.size() );

	int ewns[4]; // to recv 4 boundary info, one array two usage..
	int ewnum, nsnum, *ewrange, *nsrange;
	int Bet, Bwt, Bnt, Bst;

	if( myid == 0 )
	{
		ewnum = 1, nsnum = 1;
		ewrange = new int[1];
		nsrange = new int[1];
		ewrange[0] = 20;
		nsrange[0] = 80;
		Bet = 100, Bwt = 0, Bnt = 100, Bst = 51;
	}
	if( myid == 1 )
	{
		ewnum = 1, nsnum = 1;
		ewrange = new int[1];
		nsrange = new int[1];
		ewrange[0] = 180;
		nsrange[0] = 80;
		Bet = 200, Bwt = 101, Bnt = 100, Bst = 51;
	}
	if( myid == 2 )
	{
		ewnum = 1, nsnum = 1;
		ewrange = new int[1];
		nsrange = new int[1];
		ewrange[0] = 20;
		nsrange[0] = 20;
		Bet = 100, Bwt = 0, Bnt = 50, Bst = 0;
	}
	if( myid == 3 )
	{
		ewnum = 1, nsnum = 1;
		ewrange = new int[1];
		nsrange = new int[1];
		ewrange[0] = 180;
		nsrange[0] = 20;
		Bet = 200, Bwt = 101, Bnt = 50, Bst = 0;
	}
	
	BOUND bound(Bet, Bwt, Bnt, Bst, ewnum, nsnum, ewrange, nsrange);

	for(int i=0; i<4; ++i)
	{
		ewns[i] = comap4[myid][i];
	}
	
	bound.Construct(spot, cross, ewns); // construct the map in this process..
	
	if(myid == 1)
	{
		cout<<"This is test of p 1"<<endl;
		string PATH;
		PATH.assign(500, 's');
		int base = 130;
		for(int i=0; i<15; ++i)
		{
			CAR newcar(base + i, 80, EAST, PATH, i);
			car.push_back(newcar);
			spot[XYtoKEY( base + i, 80 )].rt = 1;
		}
	}
	/*
	if(myid == 2)
	{
		cout<<"This is test of p 2"<<endl;
		string PATH;
		PATH.assign(500, 's');
		CAR newcar1(30, 20, WEST, PATH);
		car.push_back(newcar1);
	}
*/
	srand48(time(NULL));
	
	if(myid == 0) cout<<"simulation start!"<<endl;
	
	ofstream file("plot.dat");
	int time_i = 0, time_max = 35;
	while(time_i < time_max ) // main loop.. one loop is one time step..
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
		//map exchange part..
		
		list<class CAR> ESCAR, WSCAR, NSCAR, SSCAR; // list of cars sending to e w n s..
		for(caritr = car.begin(); caritr!=car.end(); ++caritr)  // traverse of cars..
		{
			char turn = caritr->path[0];
			int newx(0), newy(0), newdrct(0);
			bool rand = 1; // deal with the randomization..
			if ( drand48() < p_randomization ) rand = 0; // 0 is do randomization..
			
			caritr->space_detect(rand, newx, newy, newdrct, spot, cross, turn);
			
			{
				if( caritr->Y() == 80 && newx > 150 ) newx = 90;
			}
			
			caritr->Move(newx, newy, newdrct, spot);
			
			if( caritr->X() > bound.Et() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path, caritr->Id() );
				ESCAR.push_back(newcar);
				caritr->del = 1;
			}
			if( caritr->X() < bound.Wt() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path, caritr->Id() );
				WSCAR.push_back(newcar);
				caritr->del = 1;
			}
			if( caritr->Y() > bound.Nt() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path, caritr->Id() );
				NSCAR.push_back(newcar);
				caritr->del = 1;
			}
			if( caritr->Y() < bound.St() ) // if come out of range.. set del to 1, add to send list..
			{
				CAR newcar( caritr->X(), caritr->Y(), caritr->DRCT(), caritr->path, caritr->Id() );
				SSCAR.push_back(newcar);
				caritr->del = 1;
			}
		}  // end of car traverse..
		
		if (myid == 0 && time_i > 10 ) // ploting part..
		{
			world.send(1, 909, car);
		}
		else if (myid == 1 && time_i > 10 )
		{
			list<class CAR> tmpcar;
			list<class CAR> recvcar;
			tmpcar = car;
			world.recv(0, 909, recvcar);
			
			tmpcar.splice(tmpcar.begin(), recvcar);
			tmpcar.sort( Cmpare() );

			list<class CAR>::iterator itr;
			
			file<<time_i<<" ";
			for(itr = tmpcar.begin(); itr != tmpcar.end(); ++itr)
			{
				file<<itr->X()<<" ";
				cout<<itr->Id()<<" "<<itr->X()<<endl;
			}
			file<<endl;
		} // ploting part..
		
		car.remove_if( check_del() );
		if(myid == 0 ) cout<<endl;
		world.barrier();
		
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
		//if(myid == 2) cout<<"time: "<<time_i<<" "<<ERCAR.size()<<" "<<WRCAR.size()<<" "<<NRCAR.size()<<" "<<SRCAR.size()<<endl;
		
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

		time_i++;
	}
	
	file.close();
	if( myid == 1 )
	{// gnuplot part..
		FILE *gp = popen("gnuplot -persist", "w");;
		if(gp == NULL)
		{
			printf("Cannot plot the data!\n");
			exit(0);
		}
		fprintf(gp, "set key left top\n");// this line is just for proposal show..
		fprintf(gp, "set title 'Parallel Periodic Test'\n");// this line is just for proposal show..
		fprintf(gp, "set xrange[6:35]\n");
		fprintf(gp, "set xlabel 'Time / Second'\n");
		fprintf(gp, "set ylabel 'Location on the Road'\n");
		fprintf(gp, "plot 'plot.dat' u 1:2 w points title 'car 0', 'plot.dat' u 1:3 w points title 'car 1', 'plot.dat' u 1:4 w points title 'car 2', 'plot.dat' u 1:5 w points title 'car 3', 'plot.dat' u 1:6 w points title 'car 4', 'plot.dat' u 1:7 w points title 'car 5', 'plot.dat' u 1:8 w points title 'car 6', 'plot.dat' u 1:9 w points title 'car 7', 'plot.dat' u 1:10 w points title 'car 8', 'plot.dat' u 1:11 w points title 'car 9', 'plot.dat' u 1:12 w points title 'car 10', 'plot.dat' u 1:13 w points title 'car 11', 'plot.dat' u 1:14 w points title 'car 12', 'plot.dat' u 1:15 w points title 'car 13', 'plot.dat' u 1:16 w points title 'car 14'\n");
		fclose(gp);
	}

	return 0;
}
