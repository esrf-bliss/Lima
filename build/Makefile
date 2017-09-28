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

############################
# HELPER MACROS
############################

this-dir	:= $(shell pwd)
lima-dir	:= $(this-dir)/..
third-party-dir	:= $(lima-dir)/third-party

LIMA_DIR = $(lima-dir)
include $(LIMA_DIR)/config.inc
include $(LIMA_DIR)/control/control.inc

full-version	= $(shell cat $(1))
minor-version	= $(shell echo $(call full-version,$(1)) | cut -f1-2 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	= $(call full-version,$(1))
else
link-version	= $(call minor-version,$(1))
endif

lib-base        = liblima$(1).so
lib-full        = $(call lib-base,$(1)).$(call full-version,$(2))
lib-minor       = $(call lib-base,$(1)).$(call minor-version,$(2))
lib-link	= $(call lib-base,$(1)).$(call link-version,$(2))
lib-flags	= -fPIC -Wl,-soname,$(call lib-link,$(1),$(2))

# 1=name, 2=ver-file 3=deps 4=flags 5=libs, 6=target, 7=objs
compile-lib	= \
	$(CXX) -shared -o $(6) $(4) $(call lib-depflags,$(3)) \
		$(call lib-flags,$(1),$(2)) $(7) $(5) $(call lib-deplibs,$(3))

create-links	= \
	rm -f $(call lib-minor,$(1),$(2)) && \
	rm -f $(call lib-base,$(1),$(2)) && \
	ln -s $(call lib-full,$(1),$(2))  $(call lib-minor,$(1),$(2)) && \
	ln -s $(call lib-minor,$(1),$(2)) $(call lib-base,$(1),$(2))


############################
# LIMA - CORE
############################

CORE_LDFLAGS	:= $(CT_LDFLAGS)
CORE_LDLIBS	:= $(CT_LDLIBS)
CORE_DEPLIBS	:= $(CT_DEPLIBS)

ifneq ($(COMPILE_HDF5_SAVING),0)
PROCESSOR_TYPE := $(shell uname -m)
endif

ifneq ($(COMPILE_CORE),0)

core-name	:= core
core-objs	:= ../common/src/Common.o \
		   ../hardware/src/Hw.o \
		   ../control/src/Ct.o
core-ver	:= ../common/VERSION
core-flags	:= $(CORE_LDFLAGS)
core-libs	:= $(CORE_LDLIBS)
core-deps	:= $(CORE_DEPLIBS)
core-libdeps	:= $(call lib-deps,$(core-deps))
core-libfull	:= $(call lib-full,$(core-name),$(core-ver))

$(core-libfull):	$(core-objs) $(core-libdeps)
	$(call compile-lib,$(core-name),$(core-ver),$(core-deps),\
			   $(core-flags),$(core-libs),$@,$(core-objs))
	$(call create-links,$(core-name),$(core-ver))

build_targets	:= $(core-libfull)

endif

CORE_DEPDEF	:= $(this-dir),limacore

$(call lib-deps,$(CORE_DEPDEF)):	$(core-libfull)

LDFLAGS		:= $(CORE_LDFLAGS)
LDLIBS		:= $(CORE_LDLIBS)
DEPLIBS		:= $(CORE_DEPLIBS) $(CORE_DEPDEF)


############################
# LIMA - SIMULATOR
############################

ifneq ($(COMPILE_SIMULATOR),0)

simu-name	:= simulator
simu-objs	:= ../camera/simulator/src/Simu.o
simu-ver	:= ../camera/simulator/VERSION
simu-flags	:= $(LDFLAGS)
simu-libs	:= $(LDLIBS)
simu-deps	:= $(DEPLIBS)
simu-libdeps	:= $(call lib-deps,$(simu-deps))
simu-libfull	:= $(call lib-full,$(simu-name),$(simu-ver))

$(simu-libfull): $(simu-objs) $(simu-libdeps)
	$(call compile-lib,$(simu-name),$(simu-ver),$(simu-deps),\
			   $(simu-flags),$(simu-libs),$@,$(simu-objs))
	$(call create-links,$(simu-name),$(simu-ver))

build_targets	+= $(simu-libfull)

endif


############################
# LIMA - ESPIA
############################


ifneq ($(COMPILE_ESPIA),0)

include ../camera/common/espia/include/espia.inc
ESPIA_DEPLIBS	:= $(ESPIA_DRV_LIB),espia

espia-name	:= espia
espia-objs	:= ../camera/common/espia/src/Espia.o
espia-ver	:= ../camera/common/espia/VERSION
espia-flags	:= $(LDFLAGS)
espia-libs	:= $(LDLIBS)
espia-deps	:= $(DEPLIBS) $(ESPIA_DEPLIBS)
espia-libdeps	:= $(call lib-deps,$(espia-deps))
espia-libfull	:= $(call lib-full,$(espia-name),$(espia-ver))

$(espia-libfull): $(espia-objs) $(espia-libdeps)
	$(call compile-lib,$(espia-name),$(espia-ver),$(espia-deps),\
			   $(espia-flags),$(espia-libs),$@,$(espia-objs))
	$(call create-links,$(espia-name),$(espia-ver))

build_targets	+= $(espia-libfull)

endif

ESPIA_DEPDEF	:= $(this-dir),limaespia

$(call lib-deps,$(ESPIA_DEPDEF)):	$(espia-libfull)


############################
# LIMA - FRELON
############################

ifneq ($(COMPILE_FRELON),0)

FRELON_DEPLIBS	:= $(ESPIA_DEPDEF)

frelon-name	:= frelon
frelon-objs	:= ../camera/frelon/src/Frelon.o
frelon-ver	:= ../camera/frelon/VERSION
frelon-flags	:= $(LDFLAGS)
frelon-libs	:= $(LDLIBS)
frelon-deps	:= $(DEPLIBS) $(ESPIA_DEPLIBS) $(FRELON_DEPLIBS)
frelon-libdeps	:= $(call lib-deps,$(frelon-deps))
frelon-libfull	:= $(call lib-full,$(frelon-name),$(frelon-ver))

$(frelon-libfull): $(frelon-objs) $(frelon-libdeps)
	$(call compile-lib,$(frelon-name),$(frelon-ver),$(frelon-deps),\
			   $(frelon-flags),$(frelon-libs),$@,$(frelon-objs))
	$(call create-links,$(frelon-name),$(frelon-ver))

build_targets	+= $(frelon-libfull)

endif


############################
# LIMA - MAXIPIX
############################

ifneq ($(COMPILE_MAXIPIX),0)

MAXIPIX_LDFLAGS	:= -g
MAXIPIX_DEPLIBS	:= $(ESPIA_DEPDEF)

maxipix-name	:= maxipix
maxipix-objs	:= ../camera/maxipix/src/Maxipix.o
maxipix-ver	:= ../camera/maxipix/VERSION
maxipix-flags	:= $(LDFLAGS) $(MAXIPIX_LDFLAGS)
maxipix-libs	:= $(LDLIBS)
maxipix-deps	:= $(DEPLIBS) $(ESPIA_DEPLIBS) $(MAXIPIX_DEPLIBS)
maxipix-libdeps	:= $(call lib-deps,$(maxipix-deps))
maxipix-libfull	:= $(call lib-full,$(maxipix-name),$(maxipix-ver))

$(maxipix-libfull): $(maxipix-objs) $(maxipix-libdeps)
	$(call compile-lib,$(maxipix-name),$(maxipix-ver),$(maxipix-deps),\
			   $(maxipix-flags),$(maxipix-libs),$@,$(maxipix-objs))
	$(call create-links,$(maxipix-name),$(maxipix-ver))

build_targets	+= $(maxipix-libfull)

endif

############################
# LIMA - BASLER
############################

ifneq ($(COMPILE_BASLER),0)

ifndef PYLON_ROOT
PYLON_ROOT	:= /opt/pylon
endif

PYLON_CONFIG    := $(PYLON_ROOT)/bin/pylon-config

ifeq ("$(wildcard $(PYLON_CONFIG))", "")
BASLER_LDFLAGS	:= -L. -L$(PYLON_ROOT)/lib -L$(PYLON_ROOT)/lib64
BASLER_LDLIBS	:= -lpylonbase -lpylongigesupp
else
BASLER_LDFLAGS  := $(shell $(PYLON_CONFIG) --libs-only-L)
BASLER_LDLIBS   := $(shell $(PYLON_CONFIG) --libs-only-l)
endif

basler-name	:= basler
basler-objs	:= ../camera/basler/src/Basler.o
basler-ver	:= ../camera/basler/VERSION
basler-flags	:= $(LDFLAGS) $(BASLER_LDFLAGS)
basler-libs	:= $(LDLIBS) $(BASLER_LDLIBS)
basler-deps	:= $(DEPLIBS)
basler-libdeps	:= $(call lib-deps,$(basler-deps))
basler-libfull	:= $(call lib-full,$(basler-name),$(basler-ver))

$(basler-libfull): $(basler-objs) $(basler-libdeps)
	$(call compile-lib,$(basler-name),$(basler-ver),$(basler-deps),\
			   $(basler-flags),$(basler-libs),$@,$(basler-objs))
	$(call create-links,$(basler-name),$(basler-ver))

build_targets	+= $(basler-libfull)

endif

############################
# LIMA - UEYE
############################

ifneq ($(COMPILE_UEYE),0)

UEYE_LDLIBS	:= -lueye_api

ueye-name	:= ueye
ueye-objs	:= ../camera/ueye/src/Ueye.o
ueye-ver	:= ../camera/ueye/VERSION
ueye-flags	:= $(LDFLAGS)
ueye-libs	:= $(LDLIBS) $(UEYE_LDLIBS)
ueye-deps	:= $(DEPLIBS)
ueye-libdeps	:= $(call lib-deps,$(ueye-deps))
ueye-libfull	:= $(call lib-full,$(ueye-name),$(ueye-ver))

$(ueye-libfull): $(ueye-objs) $(ueye-libdeps)
	$(call compile-lib,$(ueye-name),$(ueye-ver),$(ueye-deps),\
			   $(ueye-flags),$(ueye-libs),$@,$(ueye-objs))
	$(call create-links,$(ueye-name),$(ueye-ver))

build_targets	+= $(ueye-libfull)

endif

############################
# LIMA - PROSILICA
############################

ifneq ($(COMPILE_PROSILICA),0)

PROCESSOR_TYPE := $(shell uname -i)
ifeq ($(PROCESSOR_TYPE),i386)
PROSILICA_LDFLAGS	:= -L. -L../camera/prosilica/sdk/bin/x86
else
PROSILICA_LDFLAGS	:= -L. -L../camera/prosilica/sdk/bin/x64 \
			   -L../camera/prosilica/sdk/bin/x86
endif
PROSILICA_LDLIBS	:= -lPvAPI

prosilica-name	:= prosilica
prosilica-objs	:= ../camera/prosilica/src/Prosilica.o
prosilica-ver	:= ../camera/prosilica/VERSION
prosilica-flags	:= $(LDFLAGS) $(PROSILICA_LDFLAGS)
prosilica-libs	:= $(LDLIBS) $(PROSILICA_LDLIBS)
prosilica-deps	:= $(DEPLIBS)
prosilica-libdeps := $(call lib-deps,$(prosilica-deps))
prosilica-libfull := $(call lib-full,$(prosilica-name),$(prosilica-ver))

$(prosilica-libfull): $(prosilica-objs) $(prosilica-libdeps)
	$(call compile-lib,$(prosilica-name),$(prosilica-ver),\
			   $(prosilica-deps),\
			   $(prosilica-flags),$(prosilica-libs),$@,\
			   $(prosilica-objs))
	$(call create-links,$(prosilica-name),$(prosilica-ver))

build_targets	+= $(prosilica-libfull)

endif


############################
# LIMA - ROPERSCIENTIFIC
############################

ifneq ($(COMPILE_ROPERSCIENTIFIC),0)

ROPERSCIENTIFIC_LDLIBS	:= -lpvcam -lraw1394

ropersci-name	:= roperscientific
ropersci-objs	:= ../camera/roperscientific/src/RoperScientific.o
ropersci-ver	:= ../camera/roperscientific/VERSION
ropersci-flags	:= $(LDFLAGS)
ropersci-libs	:= $(LDLIBS) $(ROPERSCIENTIFIC_LDLIBS)
ropersci-deps	:= $(DEPLIBS)
ropersci-libdeps := $(call lib-deps,$(ropersci-deps))
ropersci-libfull := $(call lib-full,$(ropersci-name),$(ropersci-ver))

$(ropersci-libfull): $(ropersci-objs) $(ropersci-libdeps)
	$(call compile-lib,$(ropersci-name),$(ropersci-ver),$(ropersci-deps),\
			   $(ropersci-flags),$(ropersci-libs),$@,\
			   $(ropersci-objs))
	$(call create-links,$(ropersci-name),$(ropersci-ver))

build_targets	+= $(ropersci-libfull)

endif


############################
# LIMA - ADSC
############################

ifneq ($(COMPILE_ADSC),0)

ADSC_LDFLAGS	:= -L. -L../camera/adsc/sdk/adsc_sources/lib/linux
ADSC_LDLIBS	:= -lauxlib -ldetcon_th

adsc-name	:= adsc
adsc-objs	:= ../camera/adsc/src/Adsc.o
adsc-ver	:= ../camera/adsc/VERSION
adsc-flags	:= $(LDFLAGS) $(ADSC_LDFLAGS)
adsc-libs	:= $(LDLIBS) $(ADSC_LDLIBS)
adsc-deps	:= $(DEPLIBS)
adsc-libdeps	:= $(call lib-deps,$(adsc-deps))
adsc-libfull	:= $(call lib-full,$(adsc-name),$(adsc-ver))

$(adsc-libfull): $(adsc-objs) $(adsc-libdeps)
	$(call compile-lib,$(adsc-name),$(adsc-ver),$(adsc-deps),\
			   $(adsc-flags),$(adsc-libs),$@,$(adsc-objs))
	$(call create-links,$(adsc-name),$(adsc-ver))

build_targets	+= $(adsc-libfull)

endif

############################
# LIMA - MERLIN
############################
ifneq ($(COMPILE_MERLIN),0)

merlin-name	:= merlin
merlin-objs	:= ../camera/merlin/src/Merlin.o
merlin-ver	:= ../camera/merlin/VERSION
merlin-flags	:= $(LDFLAGS)
merlin-libs	:= $(LDLIBS)
merlin-deps	:= $(DEPLIBS)
merlin-libdeps	:= $(call lib-deps,$(merlin-deps))
merlin-libfull	:= $(call lib-full,$(merlin-name),$(merlin-ver))

$(merlin-libfull): $(merlin-objs) $(merlin-libdeps)
	$(call compile-lib,$(merlin-name),$(merlin-ver),$(merlin-deps),\
			   $(merlin-flags),$(merlin-libs),$@,$(merlin-objs))
	$(call create-links,$(merlin-name),$(merlin-ver))

build_targets	+= $(merlin-libfull)

endif

############################
# LIMA - MYTHEN
############################

ifneq ($(COMPILE_MYTHEN),0)

MYTHEN_LDLIBS	:= -lSlsDetector

mythen-name	:= mythen
mythen-objs	:= ../camera/mythen/src/Mythen.o
mythen-ver	:= ../camera/mythen/VERSION
mythen-flags	:= $(LDFLAGS)
mythen-libs	:= $(LDLIBS) $(MYTHEN_LDLIBS)
mythen-deps	:= $(DEPLIBS)
mythen-libdeps	:= $(call lib-deps,$(mythen-deps))
mythen-libfull	:= $(call lib-full,$(mythen-name),$(mythen-ver))

$(mythen-libfull): $(mythen-objs) $(mythen-libdeps)
	$(call compile-lib,$(mythen-name),$(mythen-ver),$(mythen-deps),\
			   $(mythen-flags),$(mythen-libs),$@,$(mythen-objs))
	$(call create-links,$(mythen-name),$(mythen-ver))

build_targets	+= $(mythen-libfull)

endif

############################
# LIMA - MYTHEN3
############################

ifneq ($(COMPILE_MYTHEN3),0)

mythen3-name	:= mythen3
mythen3-objs	:= ../camera/mythen3/src/Mythen3.o
mythen3-ver	:= ../camera/mythen3/VERSION
mythen3-flags	:= $(LDFLAGS)
mythen3-libs	:= $(LDLIBS)
mythen3-deps	:= $(DEPLIBS)
mythen3-libdeps	:= $(call lib-deps,$(mythen3-deps))
mythen3-libfull	:= $(call lib-full,$(mythen3-name),$(mythen3-ver))

$(mythen3-libfull): $(mythen3-objs) $(mythen3-libdeps)
	$(call compile-lib,$(mythen3-name),$(mythen3-ver),$(mythen3-deps),\
			   $(mythen3-flags),$(mythen3-libs),$@,$(mythen3-objs))
	$(call create-links,$(mythen3-name),$(mythen3-ver))

build_targets	+= $(mythen3-libfull)

endif

############################
# LIMA - ANDOR
############################

ifneq ($(COMPILE_ANDOR),0)

ANDOR_LDFLAGS	:= -L. -L/usr/local/lib
ANDOR_LDLIBS	:= -landor

andor-name	:= andor
andor-objs	:= ../camera/andor/src/Andor.o
andor-ver	:= ../camera/andor/VERSION
andor-flags	:= $(LDFLAGS) $(ANDOR_LDFLAGS)
andor-libs	:= $(LDLIBS) $(ANDOR_LDLIBS)
andor-deps	:= $(DEPLIBS)
andor-libdeps	:= $(call lib-deps,$(andor-deps))
andor-libfull	:= $(call lib-full,$(andor-name),$(andor-ver))

$(andor-libfull): $(andor-objs) $(andor-libdeps)
	$(call compile-lib,$(andor-name),$(andor-ver),$(andor-deps),\
			   $(andor-flags),$(andor-libs),$@,$(andor-objs))
	$(call create-links,$(andor-name),$(andor-ver))

build_targets	+= $(andor-libfull)

endif

############################
# LIMA - ANDOR3
############################

ifneq ($(COMPILE_ANDOR3),0)

ANDOR3_LDFLAGS	:= -L. -L/usr/local/lib
ANDOR3_LDLIBS	:= -latcore -latutility

andor3-name	:= andor3
andor3-objs	:= ../camera/andor3/src/Andor3.o
andor3-ver	:= ../camera/andor3/VERSION
andor3-flags	:= $(LDFLAGS) $(ANDOR3_LDFLAGS)
andor3-libs	:= $(LDLIBS) $(ANDOR3_LDLIBS)
andor3-deps	:= $(DEPLIBS)
andor3-libdeps	:= $(call lib-deps,$(andor3-deps))
andor3-libfull	:= $(call lib-full,$(andor3-name),$(andor3-ver))

$(andor3-libfull): $(andor3-objs) $(andor3-libdeps)
	$(call compile-lib,$(andor3-name),$(andor3-ver),$(andor3-deps),\
			   $(andor3-flags),$(andor3-libs),$@,$(andor3-objs))
	$(call create-links,$(andor3-name),$(andor3-ver))

build_targets	+= $(andor3-libfull)

endif

############################
# LIMA - MARCCD
############################

ifneq ($(COMPILE_MARCCD),0)

MARCCD_LDLIBS	:= -lrt
MARCCD_DEPLIBS	:= $(third-party-dir)/yat/src/.libs,yat

marccd-name	:= marccd
marccd-objs	:= ../camera/marccd/src/Marccd.o
marccd-ver	:= ../camera/marccd/VERSION
marccd-flags	:= $(LDFLAGS)
marccd-libs	:= $(LDLIBS) $(MARCCD_LDLIBS)
marccd-deps	:= $(DEPLIBS) $(MARCCD_DEPLIBS)
marccd-libdeps	:= $(call lib-deps,$(marccd-deps))
marccd-libfull	:= $(call lib-full,$(marccd-name),$(marccd-ver))

$(marccd-libfull): $(marccd-objs) $(marccd-libdeps)
	$(call compile-lib,$(marccd-name),$(marccd-ver),$(marccd-deps),\
			   $(marccd-flags),$(marccd-libs),$@,$(marccd-objs))
	$(call create-links,$(marccd-name),$(marccd-ver))

build_targets	+= $(marccd-libfull)

endif


############################
# LIMA - XH
############################

ifneq ($(COMPILE_XH),0)

xh-name		:= xh
xh-objs		:= ../camera/xh/src/Xh.o
xh-ver		:= ../camera/xh/VERSION
xh-flags	:= $(LDFLAGS)
xh-libs		:= $(LDLIBS)
xh-deps		:= $(DEPLIBS)
xh-libdeps	:= $(call lib-deps,$(xh-deps))
xh-libfull	:= $(call lib-full,$(xh-name),$(xh-ver))

$(xh-libfull): $(xh-objs) $(xh-libdeps)
	$(call compile-lib,$(xh-name),$(xh-ver),$(xh-deps),\
			   $(xh-flags),$(xh-libs),$@,$(xh-objs))
	$(call create-links,$(xh-name),$(xh-ver))

build_targets	+= $(xh-libfull)

endif

############################
# LIMA - XSPRESS3
############################

ifneq ($(COMPILE_XSPRESS3),0)

PROCESSOR_TYPE := $(shell uname -m)
ifeq ($(PROCESSOR_TYPE),x86_64)
XSPRESS3_LDFLAGS	:= -L../camera/xspress3/sdk/libs/linux.x86_64
else
XSPRESS3_LDFLAGS	:= -L../camera/xspress3/sdk/libs/linux.x86
endif
XSPRESS3_LDLIBS	:= -lxspress3 -limg_mod

xspress3-name	:= xspress3
xspress3-objs	:= ../camera/xspress3/src/Xspress3.o
xspress3-ver	:= ../camera/xspress3/VERSION
xspress3-flags	:= $(LDFLAGS) $(XSPRESS3_LDFLAGS)
xspress3-libs	:= $(LDLIBS) $(XSPRESS3_LDLIBS)
xspress3-deps	:= $(DEPLIBS)
xspress3-libdeps := $(call lib-deps,$(xspress3-deps))
xspress3-libfull := $(call lib-full,$(xspress3-name),$(xspress3-ver))

$(xspress3-libfull): $(xspress3-objs) $(xspress3-libdeps)
	$(call compile-lib,$(xspress3-name),$(xspress3-ver),$(xspress3-deps),\
			   $(xspress3-flags),$(xspress3-libs),$@,\
			   $(xspress3-objs))
	$(call create-links,$(xspress3-name),$(xspress3-ver))

build_targets	+= $(xspress3-libfull)

endif

############################
# LIMA - ULTRA
############################

ifneq ($(COMPILE_ULTRA),0)

ultra-name	:= ultra
ultra-objs	:= ../camera/ultra/src/Ultra.o
ultra-ver	:= ../camera/ultra/VERSION
ultra-flags	:= $(LDFLAGS)
ultra-libs	:= $(LDLIBS)
ultra-deps	:= $(DEPLIBS)
ultra-libdeps	:= $(call lib-deps,$(ultra-deps))
ultra-libfull	:= $(call lib-full,$(ultra-name),$(ultra-ver))

$(ultra-libfull): $(ultra-objs) $(ultra-libdeps)
	$(call compile-lib,$(ultra-name),$(ultra-ver),$(ultra-deps),\
			   $(ultra-flags),$(ultra-libs),$@,$(ultra-objs))
	$(call create-links,$(ultra-name),$(ultra-ver))

build_targets	+= $(ultra-libfull)

endif

############################
# LIMA - XPAD
############################

ifneq ($(COMPILE_XPAD),0)

XPAD_LDFLAGS	:= -L/home/xpix_user/PCI_VALIDATED/trunk/sw/xpci_lib 
XPAD_LDLIBS	:= -lxpci_lib
XPAD_DEPLIBS	:= $(third-party-dir)/yat/lib/i386/static,yat

xpad-name	:= xpad
xpad-objs	:= ../camera/xpad/src/Xpad.o
xpad-ver	:= ../camera/xpad/VERSION
xpad-flags	:= $(LDFLAGS) $(XPAD_LDFLAGS)
xpad-libs	:= $(LDLIBS) $(XPAD_LDLIBS)
xpad-deps	:= $(DEPLIBS) $(XPAD_DEPLIBS)
xpad-libdeps	:= $(call lib-deps,$(xpad-deps))
xpad-libfull	:= $(call lib-full,$(xpad-name),$(xpad-ver))

$(xpad-libfull): $(xpad-objs) $(xpad-libdeps)
	$(call compile-lib,$(xpad-name),$(xpad-ver),$(xpad-deps),\
			   $(xpad-flags),$(xpad-libs),$@,$(xpad-objs))
	$(call create-links,$(xpad-name),$(xpad-ver))

build_targets	+= $(xpad-libfull)

endif

############################
# LIMA - PILATUS
############################

ifneq ($(COMPILE_PILATUS),0)

pilatus-name	:= pilatus
pilatus-objs	:= ../camera/pilatus/src/Pilatus.o
pilatus-ver	:= ../camera/pilatus/VERSION
pilatus-flags	:= $(LDFLAGS)
pilatus-libs	:= $(LDLIBS)
pilatus-deps	:= $(DEPLIBS)
pilatus-libdeps	:= $(call lib-deps,$(pilatus-deps))
pilatus-libfull := $(call lib-full,$(pilatus-name),$(pilatus-ver))

$(pilatus-libfull): $(pilatus-objs) $(pilatus-libdeps)
	$(call compile-lib,$(pilatus-name),$(pilatus-ver),$(pilatus-deps),\
			   $(pilatus-flags),$(pilatus-libs),$@,$(pilatus-objs))
	$(call create-links,$(pilatus-name),$(pilatus-ver))

build_targets	+= $(pilatus-libfull)

endif

############################
# LIMA - POINTGREY
############################
ifneq ($(COMPILE_POINTGREY),0)

POINTGREY_LDFLAGS := -L../camera/pointgrey/sdk/lib
POINTGREY_LDLIBS  := -lflycapture

pointgrey-name	:= pointgrey
pointgrey-objs	:= ../camera/pointgrey/src/PointGrey.o
pointgrey-ver	:= ../camera/pointgrey/VERSION
pointgrey-flags	:= $(LDFLAGS) $(POINTGREY_LDFLAGS)
pointgrey-libs	:= $(LDLIBS) $(POINTGREY_LDLIBS)
pointgrey-deps	:= $(DEPLIBS)
pointgrey-libdeps := $(call lib-deps,$(pointgrey-deps))
pointgrey-libfull := $(call lib-full,$(pointgrey-name),$(pointgrey-ver))

$(pointgrey-libfull): $(pointgrey-objs) $(pointgrey-libdeps)
	$(call compile-lib,$(pointgrey-name),$(pointgrey-ver),\
			   $(pointgrey-deps),\
			   $(pointgrey-flags),$(pointgrey-libs),$@,\
			   $(pointgrey-objs))
	$(call create-links,$(pointgrey-name),$(pointgrey-ver))

build_targets	+= $(pointgrey-libfull)

endif

############################
# LIMA - IMXPAD
############################
ifneq ($(COMPILE_IMXPAD),0)

imxpad-name	:= imxpad
imxpad-objs	:= ../camera/imxpad/src/imXpad.o
imxpad-ver	:= ../camera/imxpad/VERSION
imxpad-flags	:= $(LDFLAGS)
imxpad-libs	:= $(LDLIBS)
imxpad-deps	:= $(DEPLIBS)
imxpad-libdeps	:= $(call lib-deps,$(imxpad-deps))
imxpad-libfull	:= $(call lib-full,$(imxpad-name),$(imxpad-ver))

$(imxpad-libfull): $(imxpad-objs) $(imxpad-libdeps)
	$(call compile-lib,$(imxpad-name),$(imxpad-ver),$(imxpad-deps),\
			   $(imxpad-flags),$(imxpad-libs),$@,$(imxpad-objs))
	$(call create-links,$(imxpad-name),$(imxpad-ver))

build_targets	+= $(imxpad-libfull)

endif

############################
# LIMA - RAYONIXHS
############################
ifneq ($(COMPILE_RAYONIXHS),0)

RAYONIXHS_LDFLAGS	:= -L/opt/rayonix/lib64
RAYONIXHS_LDLIBS	:= -lcraydl

rayonixhs-name	:= rayonixhs
rayonixhs-objs	:= ../camera/rayonixhs/src/RayonixHs.o
rayonixhs-ver	:= ../camera/rayonixhs/VERSION
rayonixhs-flags	:= $(LDFLAGS) $(RAYONIXHS_LDFLAGS)
rayonixhs-libs	:= $(LDLIBS) $(RAYONIXHS_LDLIBS)
rayonixhs-deps	:= $(DEPLIBS)
rayonixhs-libdeps := $(call lib-deps,$(rayonixhs-deps))
rayonixhs-libfull := $(call lib-full,$(rayonixhs-name),$(rayonixhs-ver))

$(rayonixhs-libfull): $(rayonixhs-objs) $(rayonixhs-libdeps)
	$(call compile-lib,$(rayonixhs-name),$(rayonixhs-ver),\
			   $(rayonixhs-deps),\
			   $(rayonixhs-flags),$(rayonixhs-libs),$@,\
			   $(rayonixhs-objs))
	$(call create-links,$(rayonixhs-name),$(rayonixhs-ver))

build_targets	+= $(rayonixhs-libfull)

endif

############################
# LIMA - AVIEX
############################
ifneq ($(COMPILE_AVIEX),0)

AVIEX_LDFLAGS	:= -L../camera/aviex/sdk/linux/lib
AVIEX_LDLIBS	:= -lMx

aviex-name	:= aviex
aviex-objs	:= ../camera/aviex/src/Aviex.o
aviex-ver	:= ../camera/aviex/VERSION
aviex-flags	:= $(LDFLAGS) $(AVIEX_LDFLAGS)
aviex-libs	:= $(LDLIBS) $(AVIEX_LDLIBS)
aviex-deps	:= $(DEPLIBS)
aviex-libdeps	:= $(call lib-deps,$(aviex-deps))
aviex-libfull	:= $(call lib-full,$(aviex-name),$(aviex-ver))

$(aviex-libfull): $(aviex-objs) $(aviex-libdeps)
	$(call compile-lib,$(aviex-name),$(aviex-ver),$(aviex-deps),\
			   $(aviex-flags),$(aviex-libs),$@,$(aviex-objs))
	$(call create-links,$(aviex-name),$(aviex-ver))

build_targets	+= $(aviex-libfull)

endif

############################
# LIMA - META
############################
ifneq ($(COMPILE_META),0)

meta-name	:= meta
meta-objs	:= ../camera/common/meta/src/Meta.o
meta-ver	:= ../camera/common/meta/VERSION
meta-flags	:= $(LDFLAGS)
meta-libs	:= $(LDLIBS)
meta-deps	:= $(DEPLIBS)
meta-libdeps	:= $(call lib-deps,$(meta-deps))
meta-libfull	:= $(call lib-full,$(meta-name),$(meta-ver))

$(meta-libfull): $(meta-objs) $(meta-libdeps)
	$(call compile-lib,$(meta-name),$(meta-ver),$(meta-deps),\
			   $(meta-flags),$(meta-libs),$@,$(meta-objs))
	$(call create-links,$(meta-name),$(meta-ver))

build_targets	+= $(meta-libfull)

endif

############################
# LIMA - V4l2
############################
ifneq ($(COMPILE_V4l2),0)

V4l2_LDLIBS	:= -lv4l2

v4l2-name	:= v4l2
v4l2-objs	:= ../camera/v4l2/src/V4L2.o
v4l2-ver	:= ../camera/v4l2/VERSION
v4l2-flags	:= $(LDFLAGS)
v4l2-libs	:= $(LDLIBS) $(V4l2_LDLIBS)
v4l2-deps	:= $(DEPLIBS)
v4l2-libdeps	:= $(call lib-deps,$(v4l2-deps))
v4l2-libfull	:= $(call lib-full,$(v4l2-name),$(v4l2-ver))

$(v4l2-libfull): $(v4l2-objs) $(v4l2-libdeps)
	$(call compile-lib,$(v4l2-name),$(v4l2-ver),$(v4l2-deps),\
			   $(v4l2-flags),$(v4l2-libs),$@,$(v4l2-objs))
	$(call create-links,$(v4l2-name),$(v4l2-ver))

build_targets	+= $(v4l2-libfull)

endif

############################
# LIMA - EIGER
############################
ifneq ($(COMPILE_EIGER),0)

EIGER_LDLIBS	:= -ljsoncpp -lcurl -llz4 -lzmq

eiger-name	:= eiger
eiger-objs	:= ../camera/eiger/src/Eiger.o
eiger-ver	:= ../camera/eiger/VERSION
eiger-flags	:= $(LDFLAGS)
eiger-libs	:= $(LDLIBS) $(EIGER_LDLIBS)
eiger-deps	:= $(DEPLIBS)
eiger-libdeps	:= $(call lib-deps,$(eiger-deps))
eiger-libfull	:= $(call lib-full,$(eiger-name),$(eiger-ver))

$(eiger-libfull): $(eiger-objs) $(eiger-libdeps)
	$(call compile-lib,$(eiger-name),$(eiger-ver),$(eiger-deps),\
			   $(eiger-flags),$(eiger-libs),$@,$(eiger-objs))
	$(call create-links,$(eiger-name),$(eiger-ver))

build_targets	+= $(eiger-libfull)

endif

############################
# LIMA - DEXELA
############################
ifneq ($(COMPILE_DEXELA),0)

DEXELA_LDLIBS	:= -ldexela

dexela-name	:= dexela
dexela-objs	:= ../camera/dexela/src/Dexela.o
dexela-ver	:= ../camera/dexela/VERSION
dexela-flags	:= $(LDFLAGS)
dexela-libs	:= $(LDLIBS) $(DEXELA_LDLIBS)
dexela-deps	:= $(DEPLIBS)
dexela-libdeps	:= $(call lib-deps,$(dexela-deps))
dexela-libfull	:= $(call lib-full,$(dexela-name),$(dexela-ver))

$(dexela-libfull): $(dexela-objs) $(dexela-libdeps)
	$(call compile-lib,$(dexela-name),$(dexela-ver),$(dexela-deps),\
			   $(dexela-flags),$(dexela-libs),$@,$(dexela-objs))
	$(call create-links,$(dexela-name),$(dexela-ver))

build_targets	+= $(dexela-libfull)

endif



###########################
# LIMA - PIXIRAD
############################
ifneq ($(COMPILE_PIXIRAD),0)

pixirad-name     := pixirad
pixirad-objs     := ../camera/pixirad/src/Pixirad.o
pixirad-ver      := ../camera/pixirad/VERSION
pixirad-flags    := $(LDFLAGS)
pixirad-libs     := $(LDLIBS)
pixirad-deps     := $(DEPLIBS)
pixirad-libdeps  := $(call lib-deps,$(pixirad-deps))
pixirad-libfull  := $(call lib-full,$(pixirad-name),$(pixirad-ver))

$(pixirad-libfull): $(pixirad-objs) $(pixirad-libdeps)
	$(call compile-lib,$(pixirad-name),$(pixirad-ver),$(pixirad-deps),\
			   $(pixirad-flags),$(pixirad-libs),$@,$(pixirad-objs))
	$(call create-links,$(pixirad-name),$(pixirad-ver))

build_targets   += $(pixirad-libfull)

endif

############################
# LIMA - HEXITEC
############################

ifneq ($(COMPILE_HEXITEC),0)

HEXITEC_LDFLAGS	:= -L/opt/pleora/ebus_sdk/CentOS-RHEL-7-x86_64/lib
HEXITEC_LDLIBS	:= -lPvDevice -lPvStream -lPvBuffer -lPvGenICam -lPvBase -lPvSerial -lEbUtilsLib

hexitec-name	:= hexitec
hexitec-objs	:= ../camera/hexitec/src/Hexitec.o
hexitec-ver		:= ../camera/hexitec/VERSION
hexitec-flags	:= $(LDFLAGS) $(HEXITEC_LDFLAGS)
hexitec-libs	:= $(LDLIBS) $(HEXITEC_LDLIBS)
hexitec-deps	:= $(DEPLIBS)
hexitec-libdeps := $(call lib-deps,$(hexitec-deps))
hexitec-libfull := $(call lib-full,$(hexitec-name),$(hexitec-ver))

$(hexitec-libfull): $(hexitec-objs) $(hexitec-libdeps)
	$(call compile-lib,$(hexitec-name),$(hexitec-ver),\
			   $(hexitec-deps),\
			   $(hexitec-flags),$(hexitec-libs),$@,\
			   $(hexitec-objs))
	$(call create-links,$(hexitec-name),$(hexitec-ver))

build_targets	+= $(hexitec-libfull)

endif

###########################
# LIMA - SLSDETECTOR
############################
ifneq ($(COMPILE_SLSDETECTOR),0)

SLSDETECTOR_LDFLAGS  := -L$(SLS_DETECTORS_DIR)/bin
SLSDETECTOR_LDLIBS   := -lSlsDetector -lSlsReceiver

slsdetector-name     := slsdetector
slsdetector-objs     := ../camera/slsdetector/src/SlsDetector.o
slsdetector-ver      := ../camera/slsdetector/VERSION
slsdetector-flags    := $(LDFLAGS) $(SLSDETECTOR_LDFLAGS)
slsdetector-libs     := $(LDLIBS) $(SLSDETECTOR_LDLIBS)
slsdetector-deps     := $(DEPLIBS)
slsdetector-libdeps  := $(call lib-deps,$(slsdetector-deps))
slsdetector-libfull  := $(call lib-full,$(slsdetector-name),$(slsdetector-ver))

$(slsdetector-libfull): $(slsdetector-objs) $(slsdetector-libdeps)
	$(call compile-lib,$(slsdetector-name),$(slsdetector-ver),\
			   $(slsdetector-deps),\
			   $(slsdetector-flags),$(slsdetector-libs),$@,\
			   $(slsdetector-objs))
	$(call create-links,$(slsdetector-name),$(slsdetector-ver))

build_targets   += $(slsdetector-libfull)

endif


src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so*
