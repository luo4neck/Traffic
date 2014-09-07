all: main.o 
	g++ main.o -Wall -O2 -o main

main.o: main.cc head.hpp def.hpp 
	g++ -c main.cc -Wall -O2 -o main.o

test: all 
	./main 50000

clean:
	rm main.o main 
