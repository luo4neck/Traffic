#include<stdlib.h>

// using namespace std;
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
	
	void Forward()
	{
		if(drct == NORTH) x = x;
		else if(drct == SOUTH) x = x; 
		else if(drct == WEST ) x = x - Speed();
		else x = x + Speed(); // east 
	}

	const int Head()
	{
		return x;
	}

	const int Back() // position of the end of the car..
	{
		if(drct == NORTH) return (x+length);
		else if(drct == SOUTH) return (x+length);
		else if(drct == WEST ) return (x+length);
		else return (x - length);
	}
	
	const int Speed()
	{
		return speed;
	}
	
	const int Smallest(int nps, int* car_back)
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
