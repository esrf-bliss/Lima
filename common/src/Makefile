
common-objs := SizeUtils.o Timestamp.o
	
CXXFLAGS += -I../include -Wall

all:	clean Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o $(common-objs)

