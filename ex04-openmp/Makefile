all: clean patt_rec

patt_rec: main.o
	g++ main.o -lgomp -o patt_rec

main.o: main.cpp
	g++ -c -fopenmp main.cpp

clean:
	rm -rf *.o patt_rec
