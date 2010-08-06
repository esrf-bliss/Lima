include ../config.inc

ifneq ($(COMPILE_CORE), 0)
sip-modules += core
endif

ifneq ($(COMPILE_SIMULATOR), 0)
sip-modules += simulator
endif

ifneq ($(COMPILE_ESPIA), 0)
sip-modules += espia
endif

ifneq ($(COMPILE_FRELON), 0)
sip-modules += frelon
endif

ifneq ($(COMPILE_MAXIPIX), 0)
sip-modules += maxipix
endif

all:	src

src:
	for d in $(sip-modules); do \
		$(MAKE) -C $$d || break; \
	done

config:	clean mrproper
	python configure.py

clean:
	for d in $(sip-modules); do \
		$(MAKE) -C $$d clean || break; \
	done

mrproper:
	python mrproper.py

test:
	@true
