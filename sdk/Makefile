ifndef INSTALL_PLAT
ifeq (/csadmin/common/scripts/get_os.share,)
INSTALL_PLAT=""
else
INSTALL_PLAT=$(shell /csadmin/common/scripts/get_os.share)
endif
endif

DEP_FILES = src/CLIpConfig.cpp include/PvApi.h

PROCESSOR_TYPE := $(shell uname -i)
ifeq ($(PROCESSOR_TYPE),i386)
ifeq ($(INSTALL_PLAT),redhate4)
	LDFLAGS = -L.
	DEP_FILES += libPvAPI.so
else
	LDFLAGS = -Lbin/x86
endif
else
ifeq ($(INSTALL_PLAT),redhate5)
	LDFLAGS = -L.
	DEP_FILES += libPvAPI.so
else
	LDFLAGS = -Lbin/x64 -Lbin/x86
endif
endif

all: CLIpConfig

clean:
	rm -f CLIpConfig
	rm -f libPvAPI.so

CLIpConfig: $(DEP_FILES)
	gcc -pthread -Bdynamic -Iinclude -o CLIpConfig $(LDFLAGS) -lPvAPI src/CLIpConfig.cpp

libPvAPI.so:
ifeq ($(INSTALL_PLAT),redhate4)
	ln -s bin/x86/old_libPvApi.so libPvAPI.so
endif
ifeq ($(INSTALL_PLAT),redhate5)
	ln -s bin/x64/old_libPvApi.so libPvAPI.so
endif
