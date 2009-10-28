SRCS = testregex.cpp

CXXFLAGS = -Wall -I ../include -pthread
LDFLAGS = -pthread

all: testregex

testregex:	testregex.o ../src/Common.o
	$(CXX) $(LDFLAGS) -o $@ $+
clean:
	rm -f *.o testregex

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)