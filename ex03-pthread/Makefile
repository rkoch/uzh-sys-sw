all: clean matcher

matcher: matcher.o
	g++ src/matcher.o -pthread -o matcher

matcher.o: src/matcher.cpp
	g++ -c -pthread -o src/matcher.o src/matcher.cpp

clean:
	rm -rf **/*.o matcher

clean-output:
	rm -f occurences.txt
