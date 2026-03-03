CXX = g++
CXXFLAGS = -O3 -std=c++11

all: main test

main: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

test: test.cpp
	$(CXX) $(CXXFLAGS) -o test test.cpp
	./test

clean:
	rm -f test main