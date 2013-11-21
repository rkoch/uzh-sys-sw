all: clean food_stand

food_stand: food_stand.o
	g++ src/food_stand.o -pthread -o food_stand

food_stand.o: src/food_stand.cpp
	g++ -c -pthread -o src/food_stand.o src/food_stand.cpp

clean:
	rm -rf **/*.o food_stand

