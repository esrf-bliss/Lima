
CXXFLAGS = -Wall -I ../include -pthread
LDFLAGS = -pthread

all: clean testregex

testregex:	testregex.o ../src/Common.o
	$(CXX) $(LDFLAGS) -o $@ $+
clean:
	rm -f *.o testregex
