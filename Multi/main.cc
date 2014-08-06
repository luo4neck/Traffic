#include<stdlib.h>
#include<pthread.h>
#include<mpi.h>
#include<stdio.h>
#include "car.hpp" // all Car_** and Dis_** functions are in this file..
#include "map.hpp" 
//#include "plotshow.hpp" // all Plot_** functions are in this file..
#define comm MPI_COMM_WORLD

using namespace std;

pthread_barrier_t barr;// barr is used to synchronization..
const int ncar = 10;
const int time_max = 20;
const int THREADS = 1; // number of roads..
bool Red=1; // Red=1 means all red lights on the road are on..
int myid, nps;

//int pos_space[THREADS][THREADS];
//int neg_space[THREADS][THREADS];

// this struct will be used to transport data from main thread into branch thread..
typedef struct info
{
	int rank;
} Info; 

MAP Map(THREADS, 2, 5);

void * OneRoad(void* arg) // this is one road..
{
	Info *me = (Info*)arg;
	int tid = me->rank;
	
	int rc = pthread_barrier_wait(&barr);
	if( rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		printf("Could not wait on barrier\n");
		exit(-1);
	}

	// initialize the double ways..
	CAR* list_east = NULL;
	CAR* list_west = NULL;
	CAR* list_north = NULL;
	CAR* list_south = NULL;
	
	// if process id (myid) is odd, the road is north-south. if process id (myid) is even, the road is east-west ..
	if( myid%2 == 0 ) //even..
	{
		list_east = Car_construct(list_east, ncar, EAST);
		list_west = Car_construct(list_west, ncar, WEST);
		printf("pid = %d, tid = %d\n",myid, tid);
		// show the location of cars on the two ways.. 
		//Car_print(list_east);
		//Car_print(list_west);
	}
	else // odd.
	{
		printf("pid = %d, tid = %d\n",myid, tid);
		list_north = Car_construct(list_north, ncar, NORTH);
		list_south = Car_construct(list_south, ncar, SOUTH);
		// show the location of cars on the two ways.. 
		//Car_print(list_north);
		//Car_print(list_south);
	}
   // int **plot = Plot_new(time_max, ncar);

	srand48(time(NULL));
	int time_i=0; // time_i is just a stop condition..
	
	while(time_i < time_max)
	{
	time_i++;
		
		printf("Sec %d th:%d in pr:%d\n", time_i, tid, myid);// lu is required to print the pthread_t..
		
		// if process id (myid) is odd, the road is north-south. if process id (myid) is even, the road is east-west ..
		// lrand48() is the seed..
		if( myid%2 == 0 )
		{
			Car_forward_NE(list_east, lrand48(), Red, myid, Map);
			Car_forward_SW(list_west, lrand48(), Red, myid, Map);
		}
		else
		{
			Car_forward_NE(list_north, lrand48(), Red, myid, Map);
			Car_forward_SW(list_south, lrand48(), Red, myid, Map);
		}
		
		pthread_barrier_wait(&barr);//??
		pthread_barrier_wait(&barr);// ??
		
		//Car_print(list_east);
		//Car_print(list_west);
    }
	
	if( myid%2 == 0 )
	{
		Car_print(list_east);
		Car_print(list_west);
	}
	else
	{
		Car_print(list_north);
		Car_print(list_south);
	}
	
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thr[THREADS];
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(comm, &myid);
	MPI_Comm_size(comm, &nps);
	
	if(myid == 0) // check all inputs..
	{
		if (nps%2 != 0) 
		{
			printf("Input is wrong!\nPlease input as $ mpirun -n 2 ./main -x 3 -y 3\n");
			exit(-1);
		}
		
	}

	const int mapsize = 3; // this is for temp use..
	int* Xsignals = new int[mapsize];
	int* Ysignals = new int[mapsize];
	if(myid == 0) // this is main process, which will construct map and boardcast to all processes..
	{
		for(int i=0; i<mapsize; ++i)// the map will be send into OneRoad..
		{
			Xsignals[i] = (i+1) * 200;
			Ysignals[i] = (i+1) * 200;
		}
	}

    // Barrier initialization
	if(pthread_barrier_init(&barr, NULL, THREADS+1))
	{
	 	printf("Could not create a barrier\n");
	    return -1;
	}
	 
	for(int i = 0; i < THREADS; ++i)
	{
		Info *arg;
		arg = (Info*)malloc(sizeof(Info));
		arg->rank = i;
	    //if(pthread_create(&thr[i], NULL, &OneRoad, (void*)i))
	    if(pthread_create(&thr[i], NULL, &OneRoad, (void*)arg))
	    {
	         printf("Could not create thread %d\n", i);
	         return -1;
	    }
	}
	 
	if (myid%2 == 0) Red = 1; // east-west road start with red light..
	else 			 Red = 0;// south-north road start with green light..
	
	pthread_barrier_wait(&barr);
	int time_i=0;
	
	while(time_i < time_max) // simulation part is in this while loop..
	{
		time_i++;
		pthread_barrier_wait(&barr);
		MPI_Barrier(comm);
	
		// MPI communication..
		//printf("%d, MAIN %d, ! in bar!\n",time_i, myid);
		
		if (myid%2 == 0)
		{
		
		}
		else
		{
		
		}

		if ( time_i%30 == 0 ) Red = !Red; //colour of the light changes every half minute..
		
		MPI_Barrier(comm);
		pthread_barrier_wait(&barr);
	}

	for(int i = 0; i < THREADS; ++i)
	{
	    if(pthread_join(thr[i], NULL))
	    {
	         printf("Could not join thread %d\n", i);
	         return -1;
	    }
	}
	
	MPI_Finalize();
	return 0;
}
