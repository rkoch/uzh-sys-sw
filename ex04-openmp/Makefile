all: clean patt_rec

patt_rec: main.o
	g++ main.o -lgomp -o patt_rec

main.o: src/main.cpp
	g++ -c -fopenmp src/main.cpp

clean:
	rm -rf *.o patt_rec

clean-output:
	rm -f occurences.txt
