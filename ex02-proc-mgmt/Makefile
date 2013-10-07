all: clean fileops

fileops: fileops.o
	g++ src/fileops.o -o fileops

fileops.o: src/fileops.cpp
	g++ -c -o src/fileops.o src/fileops.cpp

clean:
	rm -rf **/*.o fileops
