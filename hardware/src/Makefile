SRCS = HwInterface.cpp HwCap.cpp HwSyncCtrlObj.cpp HwFrameInfo.cpp \
	HwFrameCallback.cpp HwBufferCtrlObj.cpp HwBufferMgr.cpp \
	HwShutterCtrlObj.cpp HwDetInfoCtrlObj.cpp HwBinCtrlObj.cpp \
	HwRoiCtrlObj.cpp HwSerialLine.cpp

hw-objs := HwInterface.o HwCap.o HwSyncCtrlObj.o HwFrameInfo.o \
	   HwFrameCallback.o HwBufferCtrlObj.o HwBufferMgr.o \
	   HwShutterCtrlObj.o HwDetInfoCtrlObj.o HwBinCtrlObj.o \
	   HwRoiCtrlObj.o HwSerialLine.o

CXXFLAGS += -I../include -I../../common/include -Wall -pthread -fPIC -g

all:	Hw.o

Hw.o:	$(hw-objs)
	ld -o $@ -r $+

clean:
	rm -f Hw.o *.P $(hw-objs)

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
