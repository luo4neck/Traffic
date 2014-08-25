all: main.o main_s.o
	g++ main_s.o -Wall -O2 -o main_s
	mpic++ main.o -Wall -o main -lboost_mpi

main.o: main.cc head.hpp def.hpp main_s.cc head_s.hpp def_s.hpp 
	g++ -c main_s.cc -Wall -O2 -o main_s.o
	mpic++ -c main.cc -Wall -o main.o -lboost_mpi

test: all 
	mpirun -n 4 ./main
	./main_s

clean:
	rm *.o main main_s
