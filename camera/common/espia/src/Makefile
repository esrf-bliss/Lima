
espia-objs = EspiaBase.o EspiaDev.o EspiaAcq.o EspiaSerialLine.o \
	     EspiaBufferMgr.o EspiaFocla.o FoclaHwInterface.o

include ../include/espia.inc

CXXFLAGS = -I../include \
	   -I../../../../hardware/include -I../../../../common/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -fPIC

all:		clean Espia.o

Espia.o:	$(espia-objs)
	$(LD) -o $@ -r $+

EspiaBase.o:	Espia.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o
