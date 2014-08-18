// This file contains all hard definations
#include<string>
#include<boost/serialization/list.hpp>
#include<boost/serialization/map.hpp>
#include<boost/mpi.hpp>
#include<map>
#include<list>
#include<fstream>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>

#define EAST  0
#define WEST  1
#define NORTH 2
#define SOUTH 3

#define EAST_HARD  5000
#define WEST_HARD  0
#define NORTH_HARD 5000
#define SOUTH_HARD 0

const int Four[4][4] ={ 
{ 1, -1, -1, 2}, //0
{-1, 0, -1, 3 },  //1
{ 2, -1, 0, -1},  //2
{-1, 2, 1, -1 }   //3
};

const int Sixteen[16][4] = {
{1, -1, -1, 4},  //0
{2, 0, -1, 5}, //1
{3, 1, -1, 6}, //2 
{-1, 2, -1, 7}, //3
{5, -1, 0, 8}, //4
{6, 4, 1, 9}, //5
{7, 5, 2, 10}, //6
{-1, 6, 3, 11}, //7
{9, -1, 4, 12}, //8
{10, 8, 5, 13}, //9
{11, 9, 6, 14}, //10
{-1, 10, 7, 15}, //11
{13, -1, 8, -1}, //12
{14, 12, 9, -1}, //13
{15, 13, 10,-1}, //14
{-1, 14, 11,-1}  //15
};
