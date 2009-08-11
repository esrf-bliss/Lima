
lima-objs = ../../common/src/Common.o ../../hardware/src/Hw.o ../src/Ct.o
simu-objs = ../../camera/simulator/src/Simu.o

simutest-objs = simutest.o
simutest-ext-objs = $(lima-objs) $(simu-objs)

INC = -I../include -I../../common/include -I../control/include \
	-I../../hardware/include -I../../camera/simulator/include \
	-I../../third-party/Processlib/core/include

CXXFLAGS += $(INC) -Wall -pthread
LDFLAGS = -pthread
LDLIBS = -L../../third-party/Processlib/build -lprocesslib

all: 	clean simutest

simutest: $(simutest-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS) $(simutest-ext-objs) 
	cp ../../third-party/Processlib/build/libprocesslib.so .

clean: 
	rm -f $(simutest-objs) simutest

