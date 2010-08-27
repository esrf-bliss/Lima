include ../../config.inc

simu-objs = ../../camera/simulator/src/Simu.o

simutest-objs = simutest.o
simutest-ext-objs = $(simu-objs)

SRCS = $(simutest-objs:.o=.cpp) 

INC = -I../include -I../../common/include -I../control/include \
	-I../../hardware/include -I../../camera/simulator/include \
	-I../software_operation/include \
	-I../../third-party/Processlib/core/include \
	-I../../third-party/Processlib/tasks/include

CPPFLAGS += $(INC) 
CXXFLAGS += -g -Wall -pthread
LDFLAGS = -pthread
LDLIBS = -L../../build -llimacore \
         -L../../third-party/Processlib/build -lprocesslib

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
LDLIBS += -L../../third-party/CBFLib/lib -lcbf
endif

all: 	simutest roicountertest

simutest: $(simutest-objs) $(simutest-ext-objs) 
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

roicountertest:		roicountertest.o
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean: 
	rm -f $(simutest-objs) simutest \
	      roicountertest roicountertest.o

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
