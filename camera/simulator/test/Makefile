
std-objs = ../../../common/src/Common.o ../../../hardware/src/Hw.o
simu-objs = ../src/Simu.o

test-objs = test.o BufferSave.o
test-ext-objs = $(simu-objs)
	
testsimulator-objs = testsimulator.o BufferSave.o
testsimulator-ext-objs = $(simu-objs) $(std-objs)

INC = -I../include -I../../../common/include -I../../../hardware/include 
CXXFLAGS += $(INC) -Wall -pthread
LDFLAGS = -pthread

all: 	clean test testsimulator

test:	$(test-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(test-ext-objs) $(std-objs)

testsimulator: $(testsimulator-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(testsimulator-ext-objs) 

clean: test-clean testsimulator-clean

test-clean:
	rm -f $(test-objs) test *.edf

testsimulator-clean:
	rm -f $(testsimulator-objs) testsimulator

