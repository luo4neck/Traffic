all: main.o main_s.o
	g++ main_s.o -Wall -O2 -o main_s
	mpic++ main.o -Wall -o main -lboost_mpi

main.o: main.cc head.hpp def.hpp main_s.cc head_s.hpp def_s.hpp 
	g++ -c main_s.cc -Wall -O2 -o main_s.o
	mpic++ -c main.cc -Wall -o main.o -lboost_mpi

test: all 
	./main_s
	mpirun -n 4 ./main

clean:
	rm *.o plot.dat plot_s.dat main main_s
