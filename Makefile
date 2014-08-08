all: main.o 
	mpic++ main.o -Wall -o main

main.o: main.cc car.hpp 
	mpic++ -c main.cc -Wall -o main.o

test: all 
#	mpirun -n 2 ./main
	./main

clean:
	rm *.o main
#	rm *.o plot.dat main
