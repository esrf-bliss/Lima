sub-dirs = common simulator

include ../config.inc

ifndef COMPILE_FRELON
COMPILE_FRELON = 0
endif

ifneq ($(COMPILE_FRELON),0)
sub-dirs += frelon
endif

ifndef COMPILE_MAXIPIX
COMPILE_MAXIPIX = 0
endif

ifneq ($(COMPILE_MAXIPIX),0)
sub-dirs += maxipix
endif

ifndef COMPILE_PROSILICA
COMPILE_PROSILICA = 0
endif

ifneq ($(COMPILE_PROSILICA),0)
sub-dirs += prosilica
endif

include ../global.inc
