SRCS = testfocla.cpp testfoclahwinterface.cpp

focla-objs = ../src/Espia.o \
	     ../../../../hardware/src/Hw.o ../../../../common/src/Common.o

include ../../../common/espia/include/espia.inc

LDFLAGS = -pthread -L$(ESPIA_DRV_LIB)
LDLIBS = -lespia

CXXFLAGS = -I../include \
	   -I../../../../hardware/include -I../../../../common/include \
           -I../../../../control/include/ \
           -I../../../../control/software_operation/include/ \
	   -I../../../../third-party/Processlib/core/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -g

all: testfocla testfoclahwinterface

testfocla:	testfocla.o $(focla-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfoclahwinterface:	testfoclahwinterface.o $(focla-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS) ../../../../control/src/Ct.o \
	-L../../../../third-party/Processlib/build -lprocesslib

clean:
	rm -f *.o testfocla testfoclahwinterface
%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
