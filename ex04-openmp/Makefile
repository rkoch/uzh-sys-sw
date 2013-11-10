all: clean patt_rec

patt_rec: patt_rec.o
	g++ patt_rec.o -lgomp -o patt_rec

patt_rec.o: src/patt_rec.cpp
	g++ -c -fopenmp src/patt_rec.cpp

clean:
	rm -rf *.o patt_rec

clean-output:
	rm -f occurences.txt

