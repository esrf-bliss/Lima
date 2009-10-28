lima-objs := ../common/src/Common.o \
	     ../hardware/src/Hw.o \
	     ../control/src/Ct.o \
	     ../camera/simulator/src/Simu.o \
	     ../camera/common/espia/src/Espia.o \
	     ../camera/frelon/src/Frelon.o

include ../camera/common/espia/include/espia.inc

LDFLAGS += -L../third-party/Processlib/build -L$(ESPIA_DRV_LIB)
LDLIBS += -lprocesslib -lespia

src:		liblima.so

test:
	@true

liblima.so:	$(lima-objs)
	$(CXX) -shared -o $@ -fPIC $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o liblima.so
