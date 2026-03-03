CC = gcc
CXX = g++
CFLAGS = -O3 -std=c11
CXXFLAGS = -O3

all: test main

main: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

test: test.c
	$(CC) $(CFLAGS) -o test test.c
	./test

clean:
	rm -f test main

.PHONY: all clean test