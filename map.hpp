#ifndef _MAP_
#define _MAP_

#include<stdlib.h>

using namespace std;

class MAP
{
	private:
	int size;
	int *X;
	int *Y;
	public:

	MAP(const int Size, const int nps, const int nts)
	// Size is the size of the map, nps is the number of process, nts is the number of thread..
	{
		size = Size;
		X = new int[Size];
		Y = new int[Size];
		for(int i=0; i<Size; ++i)
		{
			X[i] = Y[i] = i * 200;
		}
	}

	~MAP()
	{
		delete[] X;
		delete[] Y;
	}

	const int Size()
	{ return size; }

	const int XY(const int myid, const int block)
	// if process id (myid) is odd, the road is north-south. if process id (myid) is even, the road is east-west ..
	{
		if (myid%2 == 0) return X[block];
		else 			 return Y[block];
	}
};

#endif
