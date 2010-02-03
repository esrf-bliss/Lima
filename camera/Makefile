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

ifndef ($(COMPILE_MAXIPIX),0)
sub-dirs += maxipix
endif

include ../global.inc
