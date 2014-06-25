all: main.o
	mpic++ main.o -Wall -o main

main.o: main.cc car.h
	mpic++ -c main.cc -Wall -o main.o

test: all 
	mpirun -n 20 ./main

clean:
	rm *.o main
