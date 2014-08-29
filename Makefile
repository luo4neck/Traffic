all: main.o 
	g++ main.o -Wall -O2 -o main
#	mpic++ main.o -Wall -o main

main.o: main.cc head.hpp def.hpp 
	g++ -c main.cc -Wall -O2 -o main.o
#	mpic++ -c main.cc -Wall -o main.o

test: all 
#	mpirun -n 2 ./main
	./main

clean:
	rm *.gif *.o *.png plot_cars.dat plot_road.dat main
