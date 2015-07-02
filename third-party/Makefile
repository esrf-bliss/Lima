############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2014
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
PROJECTS_TEST =
PROJECTS_PRELIMA_CONFIG = processlib.config
PROJECTS_PRELIMA_SIP = processlib.sip
PROJECTS_POSTLIMA_CONFIG =
PROJECTS_POSTLIMA_SIP =
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

ifeq ($(wildcard cfitsio/Makefile),)
PROJECTS += cfitsio.confsrc
endif
PROJECTS += cfitsio.src

ifeq ($(wildcard CCfits/Makefile),)
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

ifndef COMPILE_GLDISPLAY
COMPILE_GLDISPLAY = 0
endif

ifneq ($(COMPILE_GLDISPLAY),0)
ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif
ifeq ($(COMPILE_SPS_IMAGE),0)
$(error "GLDisplay requires SPS. Please set COMPILE_SPS_IMAGE=1 in config.inc)
endif


PROJECTS += gldisplay.src
PROJECTS_TEST += gldisplay.test
PROJECTS_POSTLIMA_CONFIG += gldisplay.config
PROJECTS_POSTLIMA_SIP += gldisplay.sip
PROJECTS_CLEAN += gldisplay.clean
endif


ifndef COMPILE_HDF5_SAVING
COMPILE_HDF5_SAVING = 0
endif

ifneq ($(COMPILE_HDF5_SAVING),0)
ifneq ($(wildcard hdf5/Makefile),)
PROJECTS_CLEAN += hdf5.clean
else
PROJECTS += hdf5.confsrc
endif

PROJECTS += hdf5.src
endif

# meta targets
all:	src test

src:	$(PROJECTS)

test:	$(PROJECTS_TEST)
	@echo

prelima.config:		$(PROJECTS_PRELIMA_CONFIG)

prelima.sip: 		$(PROJECTS_PRELIMA_SIP)

postlima.config:	$(PROJECTS_POSTLIMA_CONFIG)
	@echo

postlima.sip:		$(PROJECTS_POSTLIMA_SIP)
	@echo

clean: $(PROJECTS_CLEAN)

# specific targets
processlib.src:			Processlib_build_libprocesslib.so

processlib.config:		processlib.clean.sip Processlib/sip/Makefile

processlib.clean.sip:
	cd Processlib/sip && python clean.py

processlib.sip:			Processlib/sip/processlib.so

processlib.clean:		processlib.clean.sip
	$(MAKE) -C Processlib/build clean

Processlib/sip/Makefile:
	cd Processlib/sip && python configure.py

Processlib_build_libprocesslib.so:
	$(MAKE) -C Processlib/build -j2

Processlib/sip/processlib.so:	Processlib_build_libprocesslib.so \
				Processlib/sip/Makefile
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
	$(MAKE) -C cfitsio install prefix=$(INSTALL_PLAT_DIR)

cfitsio.clean:
	$(MAKE) -C cfitsio clean

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
	$(MAKE) -C libconfig install prefix=$(INSTALL_PLAT_DIR)

gldisplay.src:			gldisplay_build_libgldisplay.so

gldisplay.test:
	$(MAKE) -C gldisplay test

gldisplay.config:		gldisplay.clean.sip gldisplay/sip/Makefile

gldisplay.clean.sip:
	cd gldisplay/sip && python clean.py

gldisplay.clean:		gldisplay.clean.sip
	$(MAKE) -C gldisplay clean

gldisplay.sip:			gldisplay/sip/gldisplay.so

gldisplay_build_libgldisplay.so:
	$(MAKE) -C gldisplay src

gldisplay/sip/Makefile:
	cd gldisplay/sip && python configure.py

gldisplay/sip/gldisplay.so:	gldisplay_build_libgldisplay.so \
				gldisplay/sip/Makefile
	$(MAKE) -C gldisplay sip

hdf5.src:
	$(MAKE) -C hdf5 all

hdf5.clean:
	$(MAKE) -C hdf5 clean

hdf5.confsrc:
	cd hdf5 && ./configure 	--enable-cxx \
				--enable-unsupported \
				--enable-threadsafe \
				--with-pthread=/usr/lib,/usr/include \
				--prefix=$(INSTALL_PLAT_DIR)

hdf5.install:
	$(MAKE) -C hdf5 install prefix=$(INSTALL_PLAT_DIR)
