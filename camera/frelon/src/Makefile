
include ../../common/espia/include/espia.inc

frelon-objs = FrelonBase.o FrelonSerialLine.o FrelonCamera.o FrelonInterface.o

CXXFLAGS = -I../include -I../../common/espia/include \
	   -I../../../hardware/include -I../../../common/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -fPIC

all:		clean Frelon.o

Frelon.o:	$(frelon-objs)
	$(LD) -o $@ -r $+

FrelonBase.o:	Frelon.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o
