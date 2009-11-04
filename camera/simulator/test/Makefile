SRCS = test.cpp testsimulator.cpp testsimuhwinterface.cpp

std-objs = ../../../common/src/Common.o ../../../hardware/src/Hw.o
simu-objs = ../src/Simu.o

test-objs = test.o
test-ext-objs = $(simu-objs)

testsimulator-objs = testsimulator.o
testsimulator-ext-objs = $(simu-objs) $(std-objs)

testsimuhwinterface-objs = testsimuhwinterface.o
testsimuhwinterface-ext-objs = $(simu-objs) $(std-objs)

INC = -I../include -I../../../common/include -I../../../hardware/include 
CXXFLAGS += $(INC) -Wall -pthread -g
LDFLAGS = -pthread

all: test testsimulator testsimuhwinterface

test:	$(test-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(test-ext-objs) $(std-objs)

testsimulator: $(testsimulator-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(testsimulator-ext-objs) 

testsimuhwinterface: $(testsimuhwinterface-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(testsimuhwinterface-ext-objs) 

clean: 
	rm -f $(test-objs) test *.edf
	rm -f $(testsimulator-objs) testsimulator
	rm -f $(testsimuhwinterface-objs) testsimuhwinterface
	rm -f *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
