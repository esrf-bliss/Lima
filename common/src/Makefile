SRCS = Constants.cpp SizeUtils.cpp Timestamp.cpp ThreadUtils.cpp Exceptions.cpp \
               BufferSave.cpp MemUtils.cpp RegEx.cpp AcqFinished.cpp

common-objs := Constants.o SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o \
               BufferSave.o MemUtils.o RegEx.o AcqFinished.o

CXXFLAGS += -I../include -I../../hardware/include -Wall -pthread -fPIC -g

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
