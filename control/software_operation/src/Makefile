ct-objs := SoftOpInternalMgr.o SoftOpExternalMgr.o SoftOpId.o

SRCS=SoftOpInternalMgr.cpp SoftOpExternalMgr.cpp SoftOpId.cpp

INCLUDES=-I../include  -I../../../third-party/Processlib/core/include -I../../../third-party/Processlib/tasks/include -I../../../common/include

CXXFLAGS += $(INCLUDES) -fPIC -Wall -pthread

all: CtSoftwareOperation.o


CtSoftwareOperation.o: $(ct-objs)
	ld -o $@ -r $+

clean:
	rm -f CtSoftwareOperation.o $(ct-objs) *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)

