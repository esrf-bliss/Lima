
common-objs := Constants.o SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o \
               BufferSave.o
	
CXXFLAGS += -I../include -I../../hardware/include -Wall -pthread

all:	clean Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o $(common-objs)

