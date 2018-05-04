all: part1 part2

part1: WindowsPart1.o
	g++ WindowsPart1.o -o part1 -lpthread

part2: WindowsPart2.o
	g++ WindowsPart2.o -o part2 -lpthread

WindowsPart1.o: WindowsPart1.cpp
	g++ -c WindowsPart1.cpp

WindowsPart2.o: WindowsPart2.cpp
	g++ -c WindowsPart2.cpp

clean:
	rm *.o part1 part2