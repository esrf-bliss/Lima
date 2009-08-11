lima-objs := ../common/src/Common.o \
	     ../hardware/src/Hw.o \
	     ../control/src/Ct.o \
	     ../camera/simulator/src/Simu.o \
	     ../camera/common/espia/src/Espia.o \
	     ../camera/frelon/src/Frelon.o

OS := $(shell /csadmin/common/scripts/get_os.share)
ifeq ($(OS),redhate5)
ARCH := $(shell /csadmin/common/scripts/get_esrflinux --architecture)
else
ARCH := 
endif

LDFLAGS += -L../third-party/Processlib/build \
	   -L /users/blissadm/driver/$(OS)/lib/$(ARCH)

LDLIBS += -lprocesslib -lespia

liblima.so:	$(lima-objs)
	$(CXX) -shared -o $@ -fPIC $+ $(LDFLAGS) $(LDLIBS)
