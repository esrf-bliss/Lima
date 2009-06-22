ct-objs := SoftOpInternalMgr.o

SRC=SoftOpInternalMgr.cpp

INCLUDES=-I../include  -I../../../third-party/Processlib/core/include -I../../../third-party/Processlib/tasks/include

CXXFLAGS += $(INCLUDES) -Wall -pthread

all: CtSoftwareOperation.o

depend :
	makedepend $(INCLUDES) $(SRC)

CtSoftwareOperation.o: $(ct-objs)
	ld -o $@ -r $+

clean:
	rm -f CtSoftwareOperation.o $(ct-objs)

