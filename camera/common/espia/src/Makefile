
espia-objs = EspiaBase.o EspiaDev.o EspiaAcq.o EspiaSerialLine.o \
	     EspiaBufferMgr.o EspiaFocla.o FoclaHwInterface.o

SRCS = $(espia-objs:.o=.cpp)

include ../include/espia.inc

CXXFLAGS = -I../include -I../../../../third-party/Processlib/core/include \
	   -I../../../../hardware/include -I../../../../common/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -fPIC -g

all: Espia.o

Espia.o:	$(espia-objs)
	$(LD) -o $@ -r $+

EspiaBase.o:	Espia.cpp
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
