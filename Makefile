CXX = g++
CXXFLAGS = -Wall -O2 -std=c++11

all: rc4 test_rc4

rc4: rc4.cpp
	$(CXX) $(CXXFLAGS) -o rc4 rc4.cpp

test_rc4: test_rc4.cpp
	$(CXX) $(CXXFLAGS) -o test_rc4 test_rc4.cpp

run: rc4
	./rc4

test: test_rc4
	./test_rc4

clean:
	rm -f rc4 test_rc4 file.txt

.PHONY: all run test clean
