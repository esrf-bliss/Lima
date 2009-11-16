
include ../../common/espia/include/espia.inc

frelon-objs = FrelonBase.o FrelonSerialLine.o FrelonCamera.o FrelonInterface.o

SRCS = $(frelon-objs:.o=.cpp) 

CXXFLAGS = -I../include -I../../common/espia/include \
	   -I../../../hardware/include -I../../../common/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -fPIC -g

all:	Frelon.o

Frelon.o:	$(frelon-objs)
	$(LD) -o $@ -r $+

FrelonBase.o:	Frelon.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
