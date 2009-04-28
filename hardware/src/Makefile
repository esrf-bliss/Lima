
hw-objs := HwInterface.o HwCap.o HwSyncCtrlObj.o
	
CXXFLAGS += -I../include -I../../common/include -Wall

all:	clean Hw.o

Hw.o:	$(hw-objs)
	ld -o $@ -r $+

clean:
	rm -f Hw.o $(hw-objs)
