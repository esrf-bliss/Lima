
common-objs := SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o
	
CXXFLAGS += -I../include -Wall -pthread

all:	clean Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o $(common-objs)

