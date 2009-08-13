
frelon-objs = ../src/Frelon.o ../../common/espia/src/Espia.o \
	      ../../../hardware/src/Hw.o ../../../common/src/Common.o 

include ../../common/espia/include/espia.inc

LDFLAGS = -pthread -L$(ESPIA_DRV_LIB)
LDLIBS = -lespia 

CXXFLAGS = -I../include -I../../common/espia/include \
	   -I../../../hardware/include -I../../../common/include \
           -I../../../control/include/ \
           -I../../../control/software_operation/include/ \
	   -I../../../third-party/Processlib/core/include \
	   -I../../../third-party/Processlib/tasks/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread

all:		clean testfrelon testfreloninterface

testfrelon:	testfrelon.o $(frelon-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfreloninterface:	testfreloninterface.o $(frelon-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS) ../../../control/src/Ct.o -L../../../third-party/Processlib/build -lprocesslib

clean:
	rm -f *.o testfrelon testfreloninterface
