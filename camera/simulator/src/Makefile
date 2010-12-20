simu-objs = FrameBuilder.o Simulator.o SimuHwInterface.o

SRCS = $(simu-objs:.o=.cpp)

CXX = g++
INC = -I../include -I../../../common/include -I../../../hardware/include -I../../../third-party/Processlib/core/include
CXXFLAGS += $(INC) -Wall -pthread -fPIC -g

all: 	Simu.o

Simu.o:	$(simu-objs)
	$(LD) -o $@ -r $+

clean:
	rm -f Simu.o *.P $(simu-objs)

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
