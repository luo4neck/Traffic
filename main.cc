#include<map>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>

using namespace std;

int XYtoKEY(int x, int y)
{
	if (x>99999 || y>99999) 
	{
		printf("input is wrong!");
		exit(-1);
	}
	
	return x*10000 + y;
}

int main()
{
	map<int, string> spot;
	
	for(int i=0; i<100; ++i)
	{
		string space;
		space[0] = space[1] = '0';
		spot.insert( pair<int, string> ( XYtoKEY( i, 5), space));
	}

	map<int, string>:: iterator spotitr;
	
	spotitr = spot.find( XYtoKEY(50, 5) ); 
	if ( spotitr != spot.end())		cout<<spotitr->second[0]<<" "<<spotitr->second[1]<<endl;
	else							cout<<"cant find this spot!"<<endl;
	
	spot[ XYtoKEY( 50, 5) ] = "11"; // change the value of a key in the map..

	spotitr = spot.find( XYtoKEY(50, 5) ); 
	if ( spotitr != spot.end())		cout<<spotitr->second[0]<<" "<<spotitr->second[1]<<endl;
	else							cout<<"cant find this spot!"<<endl;


	return 0;
}
