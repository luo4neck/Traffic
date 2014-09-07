all: main.o 
	mpic++ main.o -Wall -o main -lboost_mpi

main.o: main.cc head.hpp def.hpp 
	mpic++ -c main.cc -Wall -o main.o -lboost_mpi

test: all 
	mpirun -n 4 ./main -v 100 -f map2.dat
	mpirun -n 1 ./main -v 100 -f map2.dat

clean:
	rm *.o main
