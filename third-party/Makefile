include ../config.inc

PROJECTS = processlib.src

PROJECTS_CLEAN = processlib.clean

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
PROJECTS += cbflib.src
PROJECTS_CLEAN += cbflib.clean
endif

ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
PROJECTS += sps.src
PROJECTS_CLEAN += sps.clean
endif

all:	src test

src:	$(PROJECTS)

processlib.src:	Processlib/sip/Makefile Processlib/sip/processlib.so

processlib.clean:
	cd Processlib/sip/ && python clean.py
	$(MAKE) -C Processlib/build clean

Processlib/sip/Makefile:
	cd Processlib/sip/ && python configure.py

Processlib_build_libprocesslib.so:
	make -C Processlib/build -j2

Processlib/sip/processlib.so: Processlib_build_libprocesslib.so
	make -C Processlib/sip -j2

sps.src:	Sps/Src/sps.o

sps.clean:
	rm -f Sps/Src/sps.o

Sps/Src/sps.o:	Sps/Src/sps.c Sps/Include/sps.h
	make -C Sps/Src CPPFLAGS=-I../Include CFLAGS=-fPIC sps.o

cbflib.src:
	$(MAKE) CFLAGS=-fPIC -C CBFLib/ -f Makefile_LINUX all

cbflib.clean:
	$(MAKE) -C CBFLib/ -f Makefile_LINUX clean

test:
	@echo

clean: $(PROJECTS_CLEAN)
