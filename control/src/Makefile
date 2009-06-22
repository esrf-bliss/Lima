ct-objs := CtSaving.o

SRC=CtSaving.cpp

INCLUDES=-I../include -I../../common/include -I../../third-party/Processlib/core/include
CXXFLAGS += $(INCLUDES) -Wall -pthread

all: Ct.o

depend :
	makedepend $(INCLUDES) $(SRC)

Ct.o: $(ct-objs) ../software_operation/src/CtSoftwareOperation.o
	ld -o $@ -r $+

clean:
	rm -f Ct.o $(ct-objs)

