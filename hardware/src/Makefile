
hw-objs := HwInterface.o HwCap.o HwSyncCtrlObj.o HwFrameInfo.o \
	   HwFrameCallback.o HwBufferCtrlObj.o HwBufferMgr.o \
	   HwShutterCtrlObj.o HwDetInfoCtrlObj.o HwBinCtrlObj.o \
	   HwRoiCtrlObj.o HwSerialLine.o
	
CXXFLAGS += -I../include -I../../common/include -Wall -pthread -fPIC

all:	clean Hw.o

Hw.o:	$(hw-objs)
	ld -o $@ -r $+

clean:
	rm -f Hw.o $(hw-objs)
