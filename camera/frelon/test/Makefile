include ../../../config.inc

SRCS = testfrelon.cpp testfreloninterface.cpp testfrelonspectroscopy.cpp \
       testfreloncontrol.cpp

include ../../common/espia/include/espia.inc

LDFLAGS = -pthread -L../../../build -L$(ESPIA_DRV_LIB) \
	  -L../../../third-party/Processlib/build 
LDLIBS = -llimafrelon -llimaespia -llimacore -lespia -lprocesslib 

ifneq ($(COMPILE_CBF_SAVING), 0)
LDFLAGS += -L../../../third-party/CBFLib/lib
LDLIBS += -lcbf
endif

CXXFLAGS = -I../include -I../../common/espia/include \
	   -I../../../hardware/include -I../../../common/include \
           -I../../../control/include/ \
           -I../../../control/software_operation/include/ \
	   -I../../../third-party/Processlib/core/include \
	   -I../../../third-party/Processlib/tasks/include \
	   -I../../../third-party/Sps/Include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -g

test-progs = testfrelon testfreloninterface testfrelonspectroscopy \
	     testfreloncontrol

all: 	$(test-progs)

testfrelon:		testfrelon.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfreloninterface:	testfreloninterface.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfrelonspectroscopy:	testfrelonspectroscopy.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfreloncontrol:	testfreloncontrol.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean:
	rm -f *.o *.P testfrelon testfreloninterface testfrelonspectroscopy

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
