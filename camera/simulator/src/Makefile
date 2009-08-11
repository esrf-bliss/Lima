
simu-objs = FrameBuilder.o Simulator.o SimuHwInterface.o
	
CXX = g++
INC = -I../include -I../../../common/include -I../../../hardware/include 
CXXFLAGS += $(INC) -Wall -pthread -fPIC

all: 	clean Simu.o

Simu.o:	$(simu-objs)
	$(LD) -o $@ -r $+

clean:
	rm -f Simu.o $(simu-objs)
