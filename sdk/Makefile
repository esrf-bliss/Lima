
PROCESSOR_TYPE := $(shell uname -i)
ifeq ($(PROCESSOR_TYPE),i386)
LDFLAGS = -Lbin/x86
else
LDFLAGS = -Lbin/x64 -Lbin/x86
endif

all: CLIpConfig

clean:
	rm -f CLIpConfig

CLIpConfig: src/CLIpConfig.cpp include/PvApi.h
	gcc -pthread -Bdynamic -Iinclude -o CLIpConfig $(LDFLAGS) -lPvAPI src/CLIpConfig.cpp