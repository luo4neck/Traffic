#include<stdlib.h>
#define NORTH 0
#define SOUTH 1
#define WEST  2
#define EAST  3

class CAR
{
	private:
	int drct; // direction going..
	int x; // location of the Head of the car..
	//int y; // location of the Head of the car..
	const int length;
	int speed; // distance to move in 1 timestep..
	public:

	CAR(int myid): length(3)
	{
		x = myid * 10+5;
		drct = EAST;
		speed = 1;
	}

	~CAR()
	{}
	
	void Forward(int distance)
	{
		// cars will have different speed when the distance to previous car is different..
		// base speed is 1 meter/sec, multi will make it faster when distance is long..
		int multi=0;
		if (distance > 50) multi = 10; 
		else if (distance > 20) multi = 5;
		else if (distance > 5) multi = 3;
		else multi = 1;

		if(drct == NORTH) x = x + Speed() * multi;
		else if(drct == SOUTH) x = x - Speed() * multi; 
		else if(drct == WEST ) x = x - Speed() * multi;
		else x = x + Speed() * multi; // east 
	}

	const int Head()
	// position of the head of the car..
	{
		return x;
	}

	const int Back() 
	// position of the end of the car..
	{
		if(drct == NORTH) return (x+length);
		else if(drct == SOUTH) return (x+length);
		else if(drct == WEST ) return (x+length);
		else return (x - length);
	}
	
	const int Speed() 
	// base speed of the car..
	{
		return speed;
	}
	
	const int Smallest(int nps, int* car_back) 
	// smallest distance to previous car..
	{
		int smallest=100;
		for(int i=0; i<nps; ++i)
		{
			int dis = car_back[i] - Head();
			if ( car_back[i] > Head() && dis < smallest )
			{	smallest = dis ;}
		}
		return smallest;
	}
};
