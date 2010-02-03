lima-objs := ../common/src/Common.o \
	     ../hardware/src/Hw.o \
	     ../control/src/Ct.o \
	     ../camera/simulator/src/Simu.o

include ../config.inc

ifneq ($(COMPILE_ESPIA),0)
lima-objs += ../camera/common/espia/src/Espia.o
include ../camera/common/espia/include/espia.inc
LDLIBS += -lespia
LDFLAGS += -L$(ESPIA_DRV_LIB)
endif

ifneq ($(COMPILE_FRELON),0)
lima-objs += ../camera/frelon/src/Frelon.o
endif

ifneq ($(COMPILE_MAXIPIX),0)
lima-objs += ../camera/maxipix/src/Maxipix.o
endif

LDFLAGS += -L../third-party/Processlib/build
LDLIBS += -lprocesslib

src:		liblima.so

test:
	@true

liblima.so:	$(lima-objs)
	$(CXX) -shared -o $@ -fPIC $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o liblima.so
