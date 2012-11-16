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


############################
# HELPER MACROS
############################

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

# 1=name, 2=ver-file 3=flags 4=deps, 5=target, 6=objs
compile-lib	= \
	$(CXX) -shared -o $(5) $(3) $(call lib-flags,$(1),$(2)) $(6) $(4)

create-links	= \
	rm -f $(call lib-minor,$(1),$(2)) && \
	rm -f $(call lib-base,$(1),$(2)) && \
	ln -s $(call lib-full,$(1),$(2))  $(call lib-minor,$(1),$(2)) && \
	ln -s $(call lib-minor,$(1),$(2)) $(call lib-base,$(1),$(2))


############################
# LIMA - CORE
############################

CORE_LDFLAGS	:= -L../third-party/Processlib/build
CORE_LDLIBS	:= -lprocesslib -lrt

ifneq ($(COMPILE_CBF_SAVING),0)
CORE_LDFLAGS += -L../third-party/CBFLib/lib
CORE_LDLIBS += -lcbf
endif

ifneq ($(COMPILE_FITS_SAVING),0)
CORE_LDFLAGS += -L../third-party/cfitsio/lib -L../third-party/CCfits/.libs
CORE_LDLIBS += -lCCfits -lcfitsio
endif

ifneq ($(COMPILE_EDFGZ_SAVING),0)
CORE_LDLIBS += -lz
endif

ifneq ($(COMPILE_CORE),0)

core-name	:= core
core-objs	:= ../common/src/Common.o \
		   ../hardware/src/Hw.o \
		   ../control/src/Ct.o
core-ver	:= ../common/VERSION
core-flags	:= $(CORE_LDFLAGS)
core-deps	:= $(CORE_LDLIBS)

$(call lib-full,$(core-name),$(core-ver)): $(core-objs)
	$(call compile-lib,$(core-name),$(core-ver),\
			   $(core-flags),$(core-deps),$@,$+)
	$(call create-links,$(core-name),$(core-ver))

build_targets	:= $(call lib-full,$(core-name),$(core-ver))

endif

LDFLAGS		:= $(CORE_LDFLAGS) -L.
LDLIBS		:= $(CORE_LDLIBS) -llimacore


############################
# LIMA - SIMULATOR
############################

ifneq ($(COMPILE_SIMULATOR),0)

simu-name	:= simulator
simu-objs	:= ../camera/simulator/src/Simu.o
simu-ver	:= ../camera/simulator/VERSION
simu-flags	:= $(LDFLAGS)
simu-deps	:= $(LDLIBS)

$(call lib-full,$(simu-name),$(simu-ver)): $(simu-objs)
	$(call compile-lib,$(simu-name),$(simu-ver),\
			   $(simu-flags),$(simu-deps),$@,$+)
	$(call create-links,$(simu-name),$(simu-ver))

build_targets	+= $(call lib-full,$(simu-name),$(simu-ver))

endif


############################
# LIMA - ESPIA
############################


ifneq ($(COMPILE_ESPIA),0)

include ../camera/common/espia/include/espia.inc
ESPIA_LDFLAGS	:= $(LDFLAGS) -L$(ESPIA_DRV_LIB)
ESPIA_LDLIBS	:= $(LDLIBS) -lespia

espia-name	:= espia
espia-objs	:= ../camera/common/espia/src/Espia.o
espia-ver	:= ../camera/common/espia/VERSION
espia-flags	:= $(ESPIA_LDFLAGS)
espia-deps	:= $(ESPIA_LDLIBS)

$(call lib-full,$(espia-name),$(espia-ver)): $(espia-objs)
	$(call compile-lib,$(espia-name),$(espia-ver),\
			   $(espia-flags),$(espia-deps),$@,$+)
	$(call create-links,$(espia-name),$(espia-ver))

build_targets	+= $(call lib-full,$(espia-name),$(espia-ver))

endif


############################
# LIMA - FRELON
############################

ifneq ($(COMPILE_FRELON),0)

FRELON_LDFLAGS	:= $(ESPIA_LDFLAGS) -L.
FRELON_LDLIBS	:= $(ESPIA_LDLIBS) -llimaespia

frelon-name	:= frelon
frelon-objs	:= ../camera/frelon/src/Frelon.o
frelon-ver	:= ../camera/frelon/VERSION
frelon-flags	:= $(FRELON_LDFLAGS)
frelon-deps	:= $(FRELON_LDLIBS)

$(call lib-full,$(frelon-name),$(frelon-ver)): $(frelon-objs)
	$(call compile-lib,$(frelon-name),$(frelon-ver),\
			   $(frelon-flags),$(frelon-deps),$@,$+)
	$(call create-links,$(frelon-name),$(frelon-ver))

build_targets	+= $(call lib-full,$(frelon-name),$(frelon-ver))

endif


############################
# LIMA - MAXIPIX
############################

ifneq ($(COMPILE_MAXIPIX),0)

MAXIPIX_LDFLAGS	:= $(ESPIA_LDFLAGS) -L.
MAXIPIX_LDLIBS	:= $(ESPIA_LDLIBS) -llimaespia

maxipix-name	:= maxipix
maxipix-objs	:= ../camera/maxipix/src/Maxipix.o
maxipix-ver	:= ../camera/maxipix/VERSION
maxipix-flags	:= $(MAXIPIX_LDFLAGS)
maxipix-deps	:= $(MAXIPIX_LDLIBS)

$(call lib-full,$(maxipix-name),$(maxipix-ver)): $(maxipix-objs)
	$(call compile-lib,$(maxipix-name),$(maxipix-ver),\
			   $(maxipix-flags),$(maxipix-deps),$@,$+)
	$(call create-links,$(maxipix-name),$(maxipix-ver))

build_targets	+= $(call lib-full,$(maxipix-name),$(maxipix-ver))

endif

############################
# LIMA - BASLER
############################

ifneq ($(COMPILE_BASLER),0)

BASLER_LDFLAGS	:= $(LDFLAGS) -L. -L/opt/pylon/lib -L/opt/pylon/lib64
BASLER_LDLIBS	:= $(LDLIBS) -lpylongigesupp

basler-name	:= basler
basler-objs	:= ../camera/basler/src/Basler.o
basler-ver	:= ../camera/basler/VERSION
basler-flags	:= $(BASLER_LDFLAGS)
basler-deps	:= $(BASLER_LDLIBS)

$(call lib-full,$(basler-name),$(basler-ver)): $(basler-objs)
	$(call compile-lib,$(basler-name),$(basler-ver),\
			   $(basler-flags),$(basler-deps),$@,$+)
	$(call create-links,$(basler-name),$(basler-ver))

build_targets	+= $(call lib-full,$(basler-name),$(basler-ver))

endif

############################
# LIMA - UEYE
############################

ifneq ($(COMPILE_UEYE),0)

UEYE_LDFLAGS	:= $(LDFLAGS)
UEYE_LDLIBS	:= $(LDLIBS) -lueye_api

ueye-name	:= ueye
ueye-objs	:= ../camera/ueye/src/Ueye.o
ueye-ver	:= ../camera/ueye/VERSION
ueye-flags	:= $(UEYE_LDFLAGS)
ueye-deps	:= $(UEYE_LDLIBS)

$(call lib-full,$(ueye-name),$(ueye-ver)): $(ueye-objs)
	$(call compile-lib,$(ueye-name),$(ueye-ver),\
			   $(ueye-flags),$(ueye-deps),$@,$+)
	$(call create-links,$(ueye-name),$(ueye-ver))

build_targets	+= $(call lib-full,$(ueye-name),$(ueye-ver))

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
prosilica-deps	:= $(LDLIBS) $(PROSILICA_LDLIBS)

$(call lib-full,$(prosilica-name),$(prosilica-ver)): $(prosilica-objs)
	$(call compile-lib,$(prosilica-name),$(prosilica-ver),\
			   $(prosilica-flags),$(prosilica-deps),$@,$+)
	$(call create-links,$(prosilica-name),$(prosilica-ver))

build_targets	+= $(call lib-full,$(prosilica-name),$(prosilica-ver))

endif


############################
# LIMA - ROPERSCIENTIFIC
############################

ifneq ($(COMPILE_ROPERSCIENTIFIC),0)

ROPERSCIENTIFIC_LDLIBS	:= $(LDLIBS) -lpvcam -lraw1394

ropersci-name	:= roperscientific
ropersci-objs	:= ../camera/roperscientific/src/RoperScientific.o
ropersci-ver	:= ../camera/roperscientific/VERSION
ropersci-flags	:= $(LDFLAGS)
ropersci-deps	:= $(ROPERSCIENTIFIC_LDLIBS)

$(call lib-full,$(ropersci-name),$(ropersci-ver)): $(ropersci-objs)
	$(call compile-lib,$(ropersci-name),$(ropersci-ver),\
			   $(ropersci-flags),$(ropersci-deps),$@,$+)
	$(call create-links,$(ropersci-name),$(ropersci-ver))

build_targets	+= $(call lib-full,$(ropersci-name),$(ropersci-ver))

endif


############################
# LIMA - ADSC
############################

ifneq ($(COMPILE_ADSC),0)

ADSC_LDFLAGS	:= $(LDFLAGS) -L. -L../camera/adsc/sdk/adsc_sources/lib/linux
ADSC_LDLIBS	:= $(LDLIBS) -lauxlib -ldetcon_th

adsc-name	:= adsc
adsc-objs	:= ../camera/adsc/src/Adsc.o
adsc-ver	:= ../camera/adsc/VERSION
adsc-flags	:= $(ADSC_LDFLAGS)
adsc-deps	:= $(ADSC_LDLIBS)

$(call lib-full,$(adsc-name),$(adsc-ver)): $(adsc-objs)
	$(call compile-lib,$(adsc-name),$(adsc-ver),\
			   $(adsc-flags),$(adsc-deps),$@,$+)
	$(call create-links,$(adsc-name),$(adsc-ver))

build_targets	+= $(call lib-full,$(adsc-name),$(adsc-ver))

endif


############################
# LIMA - MYTHEN
############################

ifneq ($(COMPILE_MYTHEN),0)

MYTHEN_LDLIBS	:= $(LDLIBS) -lSlsDetector

mythen-name	:= mythen
mythen-objs	:= ../camera/mythen/src/Mythen.o
mythen-ver	:= ../camera/mythen/VERSION
mythen-flags	:= $(LDFLAGS)
mythen-deps	:= $(MYTHEN_LDLIBS)

$(call lib-full,$(mythen-name),$(mythen-ver)): $(mythen-objs)
	$(call compile-lib,$(mythen-name),$(mythen-ver),\
			   $(mythen-flags),$(mythen-deps),$@,$+)
	$(call create-links,$(mythen-name),$(mythen-ver))

build_targets	+= $(call lib-full,$(mythen-name),$(mythen-ver))

endif


############################
# LIMA - ANDOR
############################

ifneq ($(COMPILE_ANDOR),0)

ANDOR_LDFLAGS	:= $(LDFLAGS) -L. -L/usr/local/lib
ANDOR_LDLIBS	:= $(LDLIBS) -landor

andor-name	:= andor
andor-objs	:= ../camera/andor/src/Andor.o
andor-ver	:= ../camera/andor/VERSION
andor-flags	:= $(ANDOR_LDFLAGS)
andor-deps	:= $(ANDOR_LDLIBS)

$(call lib-full,$(andor-name),$(andor-ver)): $(andor-objs)
	$(call compile-lib,$(andor-name),$(andor-ver),\
			   $(andor-flags),$(andor-deps),$@,$+)
	$(call create-links,$(andor-name),$(andor-ver))

build_targets	+= $(call lib-full,$(andor-name),$(andor-ver))

endif

############################
# LIMA - MARCCD
############################

ifneq ($(COMPILE_MARCCD),0)

MARCCD_LDFLAGS	:= $(LDFLAGS) -L. -L../third-party/yat/src/.libs
MARCCD_LDLIBS	:= $(LDLIBS) -lyat -lrt

marccd-name	:= marccd
marccd-objs	:= ../camera/marccd/src/Marccd.o
marccd-ver	:= ../camera/marccd/VERSION
marccd-flags	:= $(MARCCD_LDFLAGS)
marccd-deps	:= $(MARCCD_LDLIBS)

$(call lib-full,$(marccd-name),$(marccd-ver)): $(marccd-objs)
	$(call compile-lib,$(marccd-name),$(marccd-ver),\
			   $(marccd-flags),$(marccd-deps),$@,$+)
	$(call create-links,$(marccd-name),$(marccd-ver))

build_targets	+= $(call lib-full,$(marccd-name),$(marccd-ver))

endif

############################
# LIMA - XPAD
############################

ifneq ($(COMPILE_XPAD),0)

XPAD_LDLIBS	:= $(LDLIBS) -L/home/xpix_user/PCI_VALIDATED/trunk/sw/xpci_lib \
		   -lxpci_lib -L../third-party/yat/lib/i386/static -lyat

xpad-name	:= xpad
xpad-objs	:= ../camera/xpad/src/Xpad.o
xpad-ver	:= ../camera/xpad/VERSION
xpad-flags	:= $(LDFLAGS)
xpad-deps	:= $(XPAD_LDLIBS)

$(call lib-full,$(xpad-name),$(xpad-ver)): $(xpad-objs)
	$(call compile-lib,$(xpad-name),$(xpad-ver),\
			   $(xpad-flags),$(xpad-deps),$@,$+)
	$(call create-links,$(xpad-name),$(xpad-ver))

build_targets	+= $(call lib-full,$(xpad-name),$(xpad-ver))

endif

src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so*
