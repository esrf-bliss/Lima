
common-objs := Constants.o SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o \
               BufferSave.o MemUtils.o RegEx.o AcqFinished.o
	
CXXFLAGS += -I../include -I../../hardware/include -Wall -pthread -fPIC

all:	clean Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o $(common-objs)

