all: main.o 
#	g++ main.o -Wall -O2 -o main
	mpic++ main.o -Wall -o main -lboost_mpi

main.o: main.cc head.hpp def.hpp 
#	g++ -c main.cc -Wall -O2 -o main.o
	mpic++ -c main.cc -Wall -o main.o -lboost_mpi

test4: all 
	mpirun -n 4 ./main -v 100 -f map4.dat

test16: all 
	mpirun -n 16 ./main -v 100 -f map16.dat

test64: all 
	mpirun -n 64 ./main -v 100 -f map64.dat

clean:
	rm *.o main
#	rm *.gif *.o *.png plot_cars.dat plot_road.dat main
