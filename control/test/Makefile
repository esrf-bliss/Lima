include ../../config.inc

lima-objs = ../../common/src/Common.o ../../hardware/src/Hw.o ../src/Ct.o
simu-objs = ../../camera/simulator/src/Simu.o

simutest-objs = simutest.o
simutest-ext-objs = $(lima-objs) $(simu-objs)

SRCS = $(simutest-objs:.o=.cpp) 

INC = -I../include -I../../common/include -I../control/include \
	-I../../hardware/include -I../../camera/simulator/include \
	-I../software_operation/include \
	-I../../third-party/Processlib/core/include

CXXFLAGS += $(INC) -g -Wall -pthread
LDFLAGS = -pthread
LDLIBS = -L../../third-party/Processlib/build -lprocesslib

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
LDLIBS += -L../../third-party/CBFLib/lib -lcbf
endif

all: 	simutest

simutest: $(simutest-objs) $(simutest-ext-objs) 
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)
	cp ../../third-party/Processlib/build/libprocesslib.so .

clean: 
	rm -f $(simutest-objs) simutest

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
