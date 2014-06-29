#include<stdlib.h>
#include<stdio.h>
#define NORTH 0
#define SOUTH 1
#define WEST  2
#define EAST  3

using namespace std;

const int base_speed = 1;
const int base_length = 1;

typedef struct CAR_struct
{
	struct CAR_struct* next;
	
	int drct; // direction going..
	int x; // location of the head of the car..
	int y; // location of the head of the car..
	int length;
} CAR;

CAR* Car_new(int x, int DRCT)
{
	CAR* c = (CAR*) malloc (sizeof(CAR));
	c->next = NULL;
	c->x = x * 5;// this line is only used in construction..
	c->drct = DRCT;
	c->length = base_length;
	return c;
}

void Car_forward(CAR* start)
{
	srand(time(NULL));
	int multi = 0; 
	while( start!=NULL )
	{
		int slowdown = 0;
		double random = drand48();
		if (random > 0.5) slowdown = 1;

		int dis=0;
		if(start->next == NULL) dis = 100; // no cars in front..
		else dis = start->next->x - start->x;
		
		if (dis > 50) multi = 10 - slowdown;
		else if (dis > 20) multi = 5 - slowdown;
		else if (dis > 5) multi = 3 - slowdown;
		else multi = 1;

		if( start->drct == NORTH) start->y = start->y + base_speed * multi;
		else if( start->drct == SOUTH) start->y = start->y - base_speed * multi;
		else if( start->drct == WEST) start->x = start->x - base_speed * multi;
		else start->x = start->x + base_speed * multi; // east..
		
		//printf("%d \n", start->x);
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
{
	while (start!=NULL)
	{
		printf("%d  ", start->x);
		start = start->next;
	}
	printf("\n");
}

void Car_insert(CAR* start, int X, int DRCT)
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
{
	bool check = 1;
	while(start!=NULL && check)
	{
		if (start->next->x == X)
		{
			start->next = start->next->next;
			check = 0;
		}
		else 
		{
			start = start->next;
	printf("%d, fine\n", start->x);
		}
	printf("haha fine\n");
	}
}
