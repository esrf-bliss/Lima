
std-objs = ../../../common/src/Common.o ../../../hardware/src/Hw.o

test-objs = test.o BufferSave.o
test-ext-objs = ../src/FrameBuilder.o
	
testsimulator-objs = testsimulator.o 
testsimulator-ext-objs = ../src/Simulator.o ../src/FrameBuilder.o $(std-objs)

INC = -I../include -I../../../common/include -I../../../hardware/include 
CXXFLAGS += $(INC) -Wall -pthread
LDFLAGS = -pthread

all: 	clean test testsimulator

test:	$(test-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(test-ext-objs) 

testsimulator: $(testsimulator-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(testsimulator-ext-objs) 

clean: test-clean testsimulator-clean

test-clean:
	rm -f $(test-objs) test *.edf

testsimulator-clean:
	rm -f $(testsimulator-objs) testsimulator

