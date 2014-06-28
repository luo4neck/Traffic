all: periodic.o
	mpic++ periodic.o -Wall -o periodic

periodic.o: periodic.cc car.h
	mpic++ -c periodic.cc -Wall -o periodic.o

test: all 
	mpirun -n 20 ./periodic

clean:
	rm *.o periodic plot.dat
