
std-objs = ../../../common/src/Common.o ../../../hardware/src/Hw.o
simu-objs = ../src/Simu.o

test-objs = test.o
test-ext-objs = $(simu-objs)

testsimulator-objs = testsimulator.o
testsimulator-ext-objs = $(simu-objs) $(std-objs)

testsimuhwinterface-objs = testsimuhwinterface.o
testsimuhwinterface-ext-objs = $(simu-objs) $(std-objs)

SRCS = $(test-objs:.o=.cpp) $(testsimulator-objs:.o=.cpp) \
       $(testsimuhwinterface-objs:.o=.cpp)

INC = -I../include -I../../../common/include -I../../../hardware/include -I../../../third-party/Processlib/core/include
CXXFLAGS += $(INC) -Wall -pthread -g
LDFLAGS = -L../../../third-party/Processlib/build -lprocesslib -pthread

all: test testsimulator testsimuhwinterface

test:	$(test-objs) $(test-ext-objs) $(std-objs)
	$(CXX) $(LDFLAGS) -o $@ $+

testsimulator: $(testsimulator-objs) $(testsimulator-ext-objs) 
	$(CXX) $(LDFLAGS) -o $@ $+

testsimuhwinterface: $(testsimuhwinterface-objs) \
		     $(testsimuhwinterface-ext-objs) 
	$(CXX) $(LDFLAGS) -o $@ $+

clean: 
	rm -f $(test-objs) test *.edf
	rm -f $(testsimulator-objs) testsimulator
	rm -f $(testsimuhwinterface-objs) testsimuhwinterface
	rm -f *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
