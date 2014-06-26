all: main.o periodic.o
	mpic++ main.o -Wall -o main
	mpic++ periodic.o -Wall -o periodic

main.o: main.cc car.h
	mpic++ -c main.cc -Wall -o main.o

periodic.o: periodic.cc car.h
	mpic++ -c periodic.cc -Wall -o periodic.o

test: all 
	mpirun -n 20 ./periodic

clean:
	rm *.o main periodic plot.dat
