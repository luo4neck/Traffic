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
	int x; // location of the head of the car..
	//int y; // location of the head of the car..
	const int length;

	public:

	CAR(int myid): length(3)
	{
		x = myid * 10+5;
		drct = EAST;
	}

	~CAR()
	{
	
	}
	
	void MoveForward()
	{
	
	}

	const int head()
	{
		return x;
	}

	const int back() // position of the end of the car..
	{
		if(drct == NORTH) return (x+length);
		else if(drct == SOUTH) return (x+length);
		else if(drct == WEST ) return (x+length);
		else return (x - length);
	}
};
