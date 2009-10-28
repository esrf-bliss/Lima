SRCS = FrameBuilder.cpp Simulator.cpp SimuHwInterface.cpp
simu-objs = FrameBuilder.o Simulator.o SimuHwInterface.o

CXX = g++
INC = -I../include -I../../../common/include -I../../../hardware/include 
CXXFLAGS += $(INC) -Wall -pthread -fPIC

all: 	Simu.o

Simu.o:	$(simu-objs)
	$(LD) -o $@ -r $+

clean:
	rm -f Simu.o $(simu-objs)

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)