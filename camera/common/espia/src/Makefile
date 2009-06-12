
espia-objs = EspiaBase.o EspiaDev.o EspiaAcq.o EspiaSerialLine.o

#ESPIA_DRV_BASE = ../../../../../../bliss/source/driver/linux-2.6/espia
ESPIA_DRV_BASE = /segfs/bliss/source/driver/linux-2.6/espia
ESPIA_DRV_INC = $(ESPIA_DRV_BASE)/src

CXXFLAGS = -I../include -I../../../../hardware/include \
	   -I../../../../common/include -I$(ESPIA_DRV_INC)

all:		clean Espia.o

Espia.o:	$(espia-objs)
	$(LD) -o $@ -r $+

EspiaBase.o:	Espia.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o
