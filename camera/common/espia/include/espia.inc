CS_SCRIPTS   = /csadmin/common/scripts
OS           = $(shell $(CS_SCRIPTS)/get_os.share)

BLISS_DRV_BASE = /segfs/bliss/source/driver

ifndef ESPIA_DRV_BASE
ifeq ($(OS), suse82)
ARCH = x86
ESPIA_DRV_BASE = $(BLISS_DRV_BASE)/linux/espia/rel
else # OS != suse82
ESRF_VER       = $(shell $(CS_SCRIPTS)/get_esrflinux)
ESRF_MAJ_VER   = $(shell echo $(ESRF_VER) | cut -c1-4)
ESRF_COMP_BASE = $(BLISS_DRV_BASE)/linux-2.6/compile/$(ESRF_MAJ_VER)
ARCH           = $(shell $(CS_SCRIPTS)/get_esrflinux --architecture)
ifneq ($(OS), redhate5)
ESPIA_DRV_BASE = $(error Invalid ESRF/Linux version: $(ESRF_VER))
else # OS == redhate5
ifneq ($(ARCH), x86_64)
ESPIA_DRV_BASE = $(error Invalid ESRF/Linux architecture: $(ARCH))
else # ARCH == x86_64
ESPIA_DRV_BASE = $(ESRF_COMP_BASE)/espia/release
endif # ARCH == x86_64
endif # ESRF_VER == redhate5
endif # OS != suse82
endif # ESPIA_DRV_BASE

ifndef ESPIA_DRV_INCL
ESPIA_DRV_INCL = $(ESPIA_DRV_BASE)/src
endif

ifndef ESPIA_DRV_LIB
ESPIA_DRV_LIB = $(ESPIA_DRV_BASE)/lib/$(ARCH)
endif
