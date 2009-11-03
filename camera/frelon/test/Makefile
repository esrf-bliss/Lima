SRCS = ./src/Frelon.cpp ../../common/espia/src/Espia.cpp \
	      ../../../hardware/src/Hw.cpp ../../../common/src/Common.cpp

frelon-objs = ../src/Frelon.o ../../common/espia/src/Espia.o \
	      ../../../hardware/src/Hw.o ../../../common/src/Common.o 

include ../../common/espia/include/espia.inc

LDFLAGS = -pthread -L$(ESPIA_DRV_LIB) -L../../../third-party/Processlib/build
LDLIBS = -lespia -lprocesslib

CXXFLAGS = -I../include -I../../common/espia/include \
	   -I../../../hardware/include -I../../../common/include \
           -I../../../control/include/ \
           -I../../../control/software_operation/include/ \
	   -I../../../third-party/Processlib/core/include \
	   -I../../../third-party/Processlib/tasks/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -g

all: testfrelon testfreloninterface testfrelonspectroscopy

testfrelon:	testfrelon.o $(frelon-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfreloninterface:	testfreloninterface.o $(frelon-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS) ../../../control/src/Ct.o

testfrelonspectroscopy:	testfrelonspectroscopy.o $(frelon-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS) ../../../control/src/Ct.o

clean:
	rm -f *.o testfrelon testfreloninterface

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
