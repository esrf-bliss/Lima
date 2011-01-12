common-objs := Constants.o SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o \
               MemUtils.o RegEx.o AcqState.o Debug.o VideoUtils.o

SRCS = $(common-objs:.o=.cpp)

CXXFLAGS += -I../include -I../../hardware/include -I../../third-party/Processlib/core/include -Wall -pthread -fPIC -g

all:	Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o *.P $(common-objs)


%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
