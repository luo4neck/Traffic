all: main.o 
	g++ main.o -Wall -o main
#	mpic++ main.o -Wall -o main

main.o: main.cc head.hpp def.hpp 
	g++ -c main.cc -Wall -o main.o
#	mpic++ -c main.cc -Wall -o main.o

test: all 
#	mpirun -n 2 ./main
	./main

clean:
	rm *.o main
#	rm *.o plot.dat main
