
include ../config.inc

############################
# LIMA - CORE
############################

CORE_LDFLAGS = -fPIC -L../third-party/Processlib/build
CORE_LDLIBS = -lprocesslib

ifneq ($(COMPILE_CORE),0)

core-objs := ../common/src/Common.o \
	     ../hardware/src/Hw.o \
	     ../control/src/Ct.o 

liblimacore.so:		$(core-objs)
	$(CXX) -shared -o $@ $(CORE_LDFLAGS) $+ $(CORE_LDLIBS)

build_targets += liblimacore.so

endif

LDFLAGS = $(CORE_LDFLAGS) -L.
LDLIBS = $(CORE_LDLIBS) -llimacore


############################
# LIMA - SIMULATOR
############################

ifneq ($(COMPILE_SIMULATOR),0)

simu-objs = ../camera/simulator/src/Simu.o

liblimasimulator.so:	$(simu-objs)
	$(CXX) -shared -o $@ $(LDFLAGS) $+ $(LDLIBS)

build_targets += liblimasimulator.so

endif
	    

############################
# LIMA - ESPIA
############################

include ../camera/common/espia/include/espia.inc
ESPIA_LDFLAGS = $(LDFLAGS) -L$(ESPIA_DRV_LIB)
ESPIA_LDLIBS = $(LDLIBS) -lespia

ifneq ($(COMPILE_ESPIA),0)

espia-objs = ../camera/common/espia/src/Espia.o

liblimaespia.so:	$(espia-objs)
	$(CXX) -shared -o $@ $(ESPIA_LDFLAGS) $+ $(ESPIA_LDLIBS)

build_targets += liblimaespia.so

endif


############################
# LIMA - FRELON
############################

ifneq ($(COMPILE_FRELON),0)

frelon-objs = ../camera/frelon/src/Frelon.o

FRELON_LDFLAGS = $(ESPIA_LDFLAGS) -L.
FRELON_LDLIBS = $(ESPIA_LDLIBS) -llimaespia

liblimafrelon.so:	$(frelon-objs)
	$(CXX) -shared -o $@ $(FRELON_LDFLAGS) $+ $(FRELON_LDLIBS)

build_targets += liblimafrelon.so

endif


############################
# LIMA - MAXIPIX
############################

ifneq ($(COMPILE_MAXIPIX),0)

maxipix-objs += ../camera/maxipix/src/Maxipix.o

MAXIPIX_LDFLAGS = $(ESPIA_LDFLAGS) -L.
MAXIPIX_LDLIBS = $(ESPIA_LDLIBS) -llimaespia

liblimamaxipix.so:	$(maxipix-objs)
	$(CXX) -shared -o $@ $(MAXIPIX_LDFLAGS) $+ $(MAXIPIX_LDLIBS)

build_targets += liblimamaxipix.so

endif

src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so
