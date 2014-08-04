#include<stdlib.h>
#include<stdio.h>
#define NORTH 0
#define SOUTH 1
#define WEST  2
#define EAST  3

using namespace std;

const int base_speed = 1;
const int base_length = 3;

int Dis_signal_NE(int x, int *sig, int num);
int Dis_signal_SW(int x, int *sig, int num);

typedef struct CAR_struct
{
	struct CAR_struct* next;
	
	int drct; // direction going..
	int x; // location of the head of the car..
	int y; // location of the head of the car..
	int length;
} 	CAR;

CAR* Car_new(int x, int DRCT)
// malloc memory space of the car..
{
	CAR* c = (CAR*) malloc (sizeof(CAR));
	c->next = NULL;
	
	if( DRCT == NORTH || DRCT == SOUTH )
		c->y = x * 5;
	else
		c->x = x * 5;// this line is only used in construction..
	
	c->drct = DRCT;
	c->length = base_length;
	return c;
}

int Dis_signal_NE(int x, const int *sig, const int num)
// x is the current location of a car, *signal is the location information of singal lights..
{
	int i=0, dis = 0;
	while(i<num)
	{
		dis = sig[i] - x;
		if ( dis>0 && dis<15) return dis;
		i++;
	}
	return 60;
}

void Car_forward_NE(CAR* start, int seed, const bool Red, const int *sig, const int num)
// move function to north or east..
{
	srand48(seed);
	while( start!=NULL )
	{
		int slowdown = 0, multi = 0; 
		if (drand48() > 0.5) slowdown = 1;

		int dis=0;
		if(start->next == NULL) dis = 100; // no cars in front..
		else if (start->drct == EAST) dis = start->next->x - start->x - start->next->length; // exactly distance to front car..
		else						  dis = start->next->y - start->y - start->next->length;
			
		if(Red) // if the red light is on..
		{
			// Dis_signal_NE() return distance to front signal..
			if (start->drct == EAST) dis = min(Dis_signal_NE(start->x, sig, num), dis); 
			else 					 dis = min(Dis_signal_NE(start->y, sig, num), dis);
		}

		if (dis > 50)	   multi = 10 - slowdown;
		else if (dis > 20) multi = 5 - slowdown;
		else if (dis > 5)  multi = 3 - slowdown;
		else if (dis > 1)  multi = 1; // dis=2 or dis=3, move 1m forward.. 
		else multi = 0; // dis = 1, stop..
		
		if( start->drct == NORTH) start->y = start->y + base_speed * multi;
		else 					  start->x = start->x + base_speed * multi; // east..
	
		start = start->next;// goto next car..
	}
}

int Dis_signal_SW(int x, const int *sig, const int num)
// x is the current location of a car, *signal is the location information of singal lights..
{
	int dis=0, i=num-1;
	while(i>=0)
	{
		dis = x - sig[i];
		if ( dis>0 && dis<15) return dis; 
		i--;
	}
	return 60;
}

void Car_forward_SW(CAR* start, int seed, const bool Red, const int *sig, const int num)
// move function to west or south..
{
	srand48(seed);
	int dis = 100;
	while( start!=NULL )
	{
		int slowdown = 0, multi = 0; 
		if (drand48() > 0.5) slowdown = 1;
		
		if(Red) // if the red light is on..
		{	
			// Dis_signal_SW() return distance to front signal..
			if(start->drct == WEST) dis = min(Dis_signal_SW(start->x, sig, num), dis);
			else					dis = min(Dis_signal_SW(start->y, sig, num), dis);
		}

		if (dis > 50) multi = 10 - slowdown;
		else if (dis > 20) multi = 5 - slowdown;
		else if (dis > 5) multi = 3 - slowdown;
		else if (dis > 1) multi = 1; // dis=2 or dis=3, move 1m forward.. 
		else multi = 0; // dis = 1, stop..

		if( start->drct == SOUTH) start->y = start->y - base_speed * multi; // move to south..
		else 					  start->x = start->x - base_speed * multi; // move to west..
	
		if(start->next != NULL)
		{	
			if (start->drct == WEST) dis = start->next->x - start->x - start->length;
			else 					 dis = start->next->y - start->y - start->length;
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
