############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
include ../config.inc

PROJECTS = processlib.src

PROJECTS_CLEAN = processlib.clean

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
PROJECTS += cbflib.src
PROJECTS_CLEAN += cbflib.clean
GCCABOVE42 := $(shell expr 4.2 \<= `gcc -dumpversion | cut -f1-2 -d.`)
  ifeq "$(GCCABOVE42)" "1"
    CBFMAKEFILE=Makefile_LINUX_gcc42
  else
    CBFMAKEFILE=Makefile_LINUX
  endif
endif

ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
PROJECTS += sps.src
PROJECTS_CLEAN += sps.clean
endif

ifneq ($(COMPILE_FITS_SAVING),0)

ifeq ($(wildcard cfitsio/zutil.o),)
PROJECTS += cfitsio.confsrc
endif
PROJECTS += cfitsio.src

ifeq ($(wildcard CCfits/.libs/libCCfits.lai),)
PROJECTS += CCfits.confsrc
endif
PROJECTS += CCfits.src

ifneq ($(wildcard cfitsio/Makefile),)
PROJECTS_CLEAN += cfitsio.clean
endif

ifneq ($(wildcard CCfits/Makefile),)
PROJECTS_CLEAN += CCfits.clean
endif

endif # COMPILE_FITS_SAVING

ifndef COMPILE_CONFIG
COMPILE_CONFIG = 0
endif

ifneq ($(COMPILE_CONFIG),0)

ifneq ($(wildcard libconfig/Makefile),)
PROJECTS_CLEAN += libconfig.clean
else
PROJECTS += libconfig.confsrc
endif

PROJECTS += libconfig.src

endif

all:	src test

src:	$(PROJECTS)

sip: processlib.sip

processlib.src:	Processlib_build_libprocesslib.so

processlib.sip: Processlib/sip/Makefile Processlib/sip/processlib.so

processlib.clean:
	cd Processlib/sip/ && python clean.py
	$(MAKE) -C Processlib/build clean

Processlib/sip/Makefile:
	cd Processlib/sip/ && python configure.py

Processlib_build_libprocesslib.so:
	$(MAKE) -C Processlib/build -j2

Processlib/sip/processlib.so: Processlib_build_libprocesslib.so
	$(MAKE) -C Processlib/sip -j2

sps.src:	Sps/Src/sps.o

sps.clean:
	rm -f Sps/Src/sps.o

Sps/Src/sps.o:	Sps/Src/sps.c Sps/Include/sps.h
	$(MAKE) -C Sps/Src CPPFLAGS=-I../Include CFLAGS=-fPIC sps.o

cbflib.src:
	$(MAKE) CFLAGS=-fPIC -C CBFLib/ -f $(CBFMAKEFILE) all

cbflib.clean:
	$(MAKE) -C CBFLib/ -f $(CBFMAKEFILE) clean

cfitsio.confsrc:
	cd cfitsio && ./configure

cfitsio.src:
	cd cfitsio && $(MAKE) && $(MAKE) install

cfitsio.install:
	cd cfitsio && ./configure --prefix=$(INSTALL_PLAT_DIR) && $(MAKE) && $(MAKE) install

cfitsio.clean:
	cd cfitsio && $(MAKE) clean

CCfits.confsrc:
	cd CCfits && ./configure --with-cfitsio=`pwd`/../cfitsio --prefix=$(INSTALL_PLAT_DIR)

CCfits.src:
	cd CCfits && $(MAKE)

CCfits.install:
	$(MAKE) -C CCfits all install

CCfits.clean:
	$(MAKE) -C CCfits clean

libconfig.src:
	$(MAKE) -C libconfig all

libconfig.clean:
	$(MAKE) -C libconfig clean

libconfig.confsrc:
	cd libconfig && ./configure --prefix=$(INSTALL_PLAT_DIR)

libconfig.install:
	$(MAKE) -C libconfig install

test:
	@echo

clean: $(PROJECTS_CLEAN)
