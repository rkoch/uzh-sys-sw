all: client server

client: src/client.o
	g++ src/client.o -o client

client.o: src/client.cpp
	g++ -c src/client.cpp

server: src/server.o
	g++ src/server.o -o server

server.o: src/server.cpp
	g++ -c src/server.cpp

clean:
	rm -rf **/*.o client server