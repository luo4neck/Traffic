all: main.o 
	mpic++ main.o -Wall -o main -lboost_mpi

main.o: main.cc head.hpp def.hpp 
	mpic++ -c main.cc -Wall -o main.o -lboost_mpi

test1: all 
	mpirun -n 4 ./main -v 100 -f map4.dat

test2: all 
	mpirun -n 16 ./main -v 10000 -f maplarge.dat

clean:
	rm *.o main
