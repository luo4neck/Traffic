all: main.o 
#	g++ main.o -Wall -O2 -o main
	mpic++ main.o -Wall -o main -lboost_mpi

main.o: main.cc head.hpp def.hpp 
#	g++ -c main.cc -Wall -O2 -o main.o
	mpic++ -c main.cc -Wall -o main.o -lboost_mpi

test4: all 
	mpirun -n 4 ./main -v 100 -f map2.dat

test1: all 
	mpirun -n 1 ./main -v 100 -f map2.dat

clean:
	rm *.o main
#	rm *.gif *.o *.png plot_cars.dat plot_road.dat main
