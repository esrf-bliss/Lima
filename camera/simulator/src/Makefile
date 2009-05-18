
objs = FrameBuilder.o Simulator.o
	
CXX = g++
INC = -I../include -I../../../common/include -I../../../hardware/include 
CXXFLAGS += $(INC) -Wall -pthread

all: 	clean $(objs)

clean:
	rm -f $(objs)
