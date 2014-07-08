all: main.o 
	mpic++ main.o -Wall -o main

main.o: main.cc car.hpp plotshow.hpp map.hpp
	mpic++ -c main.cc -Wall -o main.o

test: all 
#	mpirun -n 20 ./main
	./main

clean:
	rm *.o plot.dat main
