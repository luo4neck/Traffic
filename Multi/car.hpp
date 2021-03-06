#include<stdlib.h>
#include<string>
#include<stdio.h>
#include "map.hpp"
#define NORTH 0
#define SOUTH 1
#define WEST  2
#define EAST  3

using namespace std;

const int base_speed = 1;
const int base_length = 3;

//int Dis_signal_NE(int x, int *sig, int num);
int Dis_signal_NE(int x, const int myid, MAP map); 
int Dis_signal_SW(int x, const int myid, MAP map);
//int Dis_signal_SW(int x, int *sig, int num);

typedef struct CAR_struct
{
	struct CAR_struct* next;
	
	int drct; // direction going..
	int x; // location of the head of the car..
	int y; // location of the head of the car..
	int length;
	string path;
} 	CAR;

CAR* Car_new(int x, int DRCT)
// malloc memory space of the car..
{
	CAR* c = new CAR; 
	c->next = NULL;
	
	if( DRCT == NORTH || DRCT == SOUTH )
		c->y = x * 5;
	else
		c->x = x * 5;// this line is only used in construction..
	
	c->drct = DRCT;
	c->length = base_length;
	c->path.assign("ssrlss");
	//printf("fine here\n");
	return c;
}

int Dis_signal_NE(int x, const int myid, MAP map) 
// x is the current location of a car, *signal is the location information of singal lights..
{
	int i=0, dis = 0;
	while( i < map.Size() )
	{
		//dis = sig[i] - x;
		dis = map.XY(myid, i) - x;
		if ( dis>0 && dis<15) return dis;
		i++;
	}
	return 60;
}

void Car_forward_NE(CAR* start, int seed, const bool Red, const int myid, MAP map)
// move function to north or east..
{
	srand48(seed);
	while( start!=NULL )
	{
		int slowdown = 0, multi = 0; 
		if (drand48() > 0.5) slowdown = 1;

		int disfr=0, dissig=0;//disfr is the distance to front car.. dissig is the distance to signal..
		if (start->drct == EAST) dissig = Dis_signal_NE(start->x, myid, map); 
		else 					 dissig = Dis_signal_NE(start->x, myid, map);

		if(start->next == NULL) disfr = 100; // no cars in front..
		else if (start->drct == EAST) disfr = start->next->x - start->x - start->next->length; // exactly distance to front car..
		else						  disfr = start->next->y - start->y - start->next->length;
		
		if( Red ) // light is red.. only thing matters is distance to light..
		{
			int dis = dissig;
			if (dis > 50)	   multi = 10 - slowdown;
			else if (dis > 20) multi = 5 - slowdown;
			else if (dis > 5)  multi = 3 - slowdown;
			else if (dis > 1)  multi = 1; // dis=2 3 4 5, move 1m forward.. 
			else multi = 0; // dis = 1, time to stop for red light.. 
		}
		else // light is green.. consider to go straight or left or right..
		{
			int dis=0;
			if (start->path[0] == 'r' || start->path[0] == 'l') // have to turn..
				dis = dissig;
			else // go straight.. 
				dis = disfr;
			
			if (dis > 50)	   multi = 10 - slowdown;
			else if (dis > 20) multi = 5 - slowdown;
			else if (dis > 5)  multi = 3 - slowdown;
			else if (dis > 1)  multi = 1; // dis=2 3 4 5, move 1m forward.. 
			else // dis = 1, time to stop for red light or trun direction.. 
			{
				if ( start->path[0] == 'l' || start->path[0] == 'r' )
				multi = 0;
				else // path[0] = 's' 
				multi = 0; 
			}
		}

		if( start->drct == NORTH) start->y = start->y + base_speed * multi;
		else 					  start->x = start->x + base_speed * multi; // east..
	
		start = start->next;// goto next car..
	}
}

int Dis_signal_SW(int x, const int myid, MAP map) 
// x is the current location of a car, *signal is the location information of singal lights..
{
	int dis=0, i= map.Size() - 1;
	while(i>=0)
	{
		dis = x - map.XY(myid, i);
		if ( dis>0 && dis<15) return dis; 
		i--;
	}
	return 60;
}

void Car_forward_SW(CAR* start, int seed, const bool Red, const int myid, MAP map)
// move function to west or south..
{
	srand48(seed);
	int disfr = 100;
	while( start!=NULL )
	{
		int slowdown = 0, multi = 0; 
		if (drand48() > 0.5) slowdown = 1;
		
		int dissig=0;
		
		if(start->drct == WEST) dissig = Dis_signal_SW(start->x, myid, map);
		else					dissig = Dis_signal_SW(start->x, myid, map);

		if( Red )
		{
			int dis = dissig;
			if (dis > 50) multi = 10 - slowdown;
			else if (dis > 20) multi = 5 - slowdown;
			else if (dis > 5) multi = 3 - slowdown;
			else if (dis > 1) multi = 1; // dis=2 3 4 5, move 1m forward.. 
			
			else multi = 0; // dis = 1, stop..
		}
		else // green light.. 
		{
			int dis=0; 
			if (start->path[0] == 'r' || start->path[0] == 'l' )
				dis = dissig;
			else
				dis = disfr;
			
			if (dis > 50) multi = 10 - slowdown;
			else if (dis > 20) multi = 5 - slowdown;
			else if (dis > 5) multi = 3 - slowdown;
			else if (dis > 1) multi = 1; // dis=2 3 4 5, move 1m forward.. 
			else // dis = 1, stop..
			{
				if (start->path[0] == 'r' || start->path[0] == 'l' )
				multi = 0;
				else // path[0] = 's'
				multi = 0;
			}
		}

		if( start->drct == SOUTH) start->y = start->y - base_speed * multi; // move to south..
		else 					  start->x = start->x - base_speed * multi; // move to west..
	
		if(start->next != NULL)
		{
			if (start->drct == WEST) disfr = start->next->x - start->x - start->length;
			else 					 disfr = start->next->y - start->y - start->length;
		}
		start = start->next;// goto next car..
	}
}

CAR* Car_construct(CAR* start, int n, int DRCT)
{
// n is the number of cars to construct, DRCT is the direction of cars..
	CAR* c = start;
	for(int i=0; i<n; ++i)
	{
		if (c!=NULL)
		{
			while( c->next!=NULL) c = c->next;
			c->next = Car_new(i, DRCT);
			c = c->next;
		}
		else // list == NULL
		{
			c = Car_new(i, DRCT);
			start = c;
		}
	}
	return start;
}

void Car_print(CAR* start)
// print all the location of cars' head..
{
	if (start != NULL)
	{
		if ( start->drct == NORTH ) printf("North: ");
		else if ( start->drct == SOUTH ) printf("South: ");
		else if ( start->drct == WEST ) printf("West: ");
		else printf("East: ");
	}
	
	while (start!=NULL)
	{
		if ( start->drct == NORTH || start->drct == SOUTH )
			printf("%d  ", start->y);
		else 
			printf("%d  ", start->x);
		
		start = start->next;
	}
	printf("\n");
}

void Car_insert(CAR* start, int X, int DRCT)
// delete a car into the list which is at location X..
{
	bool check = 1;
	while(start!=NULL && check)
	{
		if( X < start->next->x && X > start->x)
		{
			CAR* c = Car_new(X, DRCT);
			c->next = start->next;
			start->next = c;
			c->x = X;
			check = 0;
		}
		else start = start->next;
	}
}

void Car_delete(CAR* start, int X)
// delete a car from the list which is at location X..
{
	bool check = 1;
	while(start!=NULL && check)
	{
		if (start->next->x == X)
		{
			start->next = start->next->next;
			check = 0;
		}
		else start = start->next;
	}
}
