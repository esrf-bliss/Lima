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
# LIMA - CORE
############################

CORE_LDFLAGS	:= -L../third-party/Processlib/build
CORE_LDLIBS	:= -lprocesslib

ifneq ($(COMPILE_CBF_SAVING),0)
CORE_LDFLAGS += -L../third-party/CBFLib/lib
CORE_LDLIBS += -lcbf
endif

ifneq ($(COMPILE_CORE),0)

name		:= core
core-objs	:= ../common/src/Common.o \
		   ../hardware/src/Hw.o \
		   ../control/src/Ct.o
version-file	:= ../common/VERSION
core-flags	:= $(CORE_LDFLAGS)
core-deps	:= $(CORE_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

core-base	:= liblima$(name).so
core-full	:= $(core-base).$(full-version)
core-link	:= $(core-base).$(link-version)
core-flags	+= -fPIC -Wl,-soname,$(core-link)

$(core-full):		$(core-objs)
	$(CXX) -shared -o $@ $(core-flags) $+ $(core-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(core-link); ln -s $(core-full) $(core-link)
endif
	rm -f $(core-base); ln -s $(core-link) $(core-base)

build_targets := $(core-full)

endif

LDFLAGS		:= $(CORE_LDFLAGS) -L.
LDLIBS		:= $(CORE_LDLIBS) -llimacore


############################
# LIMA - SIMULATOR
############################

ifneq ($(COMPILE_SIMULATOR),0)

name		:= simulator
simu-objs	:= ../camera/simulator/src/Simu.o
version-file	:= ../camera/simulator/VERSION
simu-flags	:= $(LDFLAGS)
simu-deps	:= $(LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

simu-base	:= liblima$(name).so
simu-full	:= $(simu-base).$(full-version)
simu-link	:= $(simu-base).$(link-version)
simu-flags	+= -fPIC -Wl,-soname,$(simu-link)

$(simu-full):		$(simu-objs)
	$(CXX) -shared -o $@ $(simu-flags) $+ $(simu-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(simu-link); ln -s $(simu-full) $(simu-link)
endif
	rm -f $(simu-base); ln -s $(simu-link) $(simu-base)

build_targets += $(simu-full)

endif


############################
# LIMA - ESPIA
############################


ifneq ($(COMPILE_ESPIA),0)

include ../camera/common/espia/include/espia.inc
ESPIA_LDFLAGS	:= $(LDFLAGS) -L$(ESPIA_DRV_LIB)
ESPIA_LDLIBS	:= $(LDLIBS) -lespia

name		:= espia
espia-objs	:= ../camera/common/espia/src/Espia.o
version-file	:= ../camera/common/espia/VERSION
espia-flags	:= $(ESPIA_LDFLAGS)
espia-deps	:= $(ESPIA_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

espia-base	:= liblima$(name).so
espia-full	:= $(espia-base).$(full-version)
espia-link	:= $(espia-base).$(link-version)
espia-flags	+= -fPIC -Wl,-soname,$(espia-link)

$(espia-full):		$(espia-objs)
	$(CXX) -shared -o $@ $(espia-flags) $+ $(espia-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(espia-link); ln -s $(espia-full) $(espia-link)
endif
	rm -f $(espia-base); ln -s $(espia-link) $(espia-base)

build_targets += $(espia-full)

endif


############################
# LIMA - FRELON
############################

ifneq ($(COMPILE_FRELON),0)

FRELON_LDFLAGS	:= $(ESPIA_LDFLAGS) -L.
FRELON_LDLIBS	:= $(ESPIA_LDLIBS) -llimaespia

name		:= frelon
frelon-objs	:= ../camera/frelon/src/Frelon.o
version-file	:= ../camera/frelon/VERSION
frelon-flags	:= $(FRELON_LDFLAGS)
frelon-deps	:= $(FRELON_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

frelon-base	:= liblima$(name).so
frelon-full	:= $(frelon-base).$(full-version)
frelon-link	:= $(frelon-base).$(link-version)
frelon-flags	+= -fPIC -Wl,-soname,$(frelon-link)

$(frelon-full):		$(frelon-objs)
	$(CXX) -shared -o $@ $(frelon-flags) $+ $(frelon-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(frelon-link); ln -s $(frelon-full) $(frelon-link)
endif
	rm -f $(frelon-base); ln -s $(frelon-link) $(frelon-base)

build_targets += $(frelon-full)

endif


############################
# LIMA - MAXIPIX
############################

ifneq ($(COMPILE_MAXIPIX),0)

MAXIPIX_LDFLAGS	:= $(ESPIA_LDFLAGS) -L.
MAXIPIX_LDLIBS	:= $(ESPIA_LDLIBS) -llimaespia

name		:= maxipix
maxipix-objs	:= ../camera/maxipix/src/Maxipix.o
version-file	:= ../camera/maxipix/VERSION
maxipix-flags	:= $(MAXIPIX_LDFLAGS)
maxipix-deps	:= $(MAXIPIX_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

maxipix-base	:= liblima$(name).so
maxipix-full	:= $(maxipix-base).$(full-version)
maxipix-link	:= $(maxipix-base).$(link-version)
maxipix-flags	+= -fPIC -Wl,-soname,$(maxipix-link)

$(maxipix-full):		$(maxipix-objs)
	$(CXX) -shared -o $@ $(maxipix-flags) $+ $(maxipix-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(maxipix-link); ln -s $(maxipix-full) $(maxipix-link)
endif
	rm -f $(maxipix-base); ln -s $(maxipix-link) $(maxipix-base)

build_targets += $(maxipix-full)

endif

############################
# LIMA - BASLER
############################

ifneq ($(COMPILE_BASLER),0)

BASLER_LDFLAGS	:= -L. -L/opt/pylon/lib -L/opt/pylon/lib64
BASLER_LDLIBS	:= -lpylongigesupp

name		:= basler
basler-objs	:= ../camera/basler/src/Basler.o
version-file	:= ../camera/basler/VERSION
basler-flags	:= $(BASLER_LDFLAGS)
basler-deps	:= $(BASLER_LDLIBS)
basler-base	:= liblima$(name).so
full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif
basler-full	:= $(basler-base).$(full-version)
basler-link	:= $(basler-base).$(link-version)
basler-flags	+= -fPIC -Wl,-soname,$(basler-link)

$(basler-full):		$(basler-objs)
	$(CXX) -shared -o $@ $(basler-flags) $+ $(basler-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(basler-link); ln -s $(basler-full) $(basler-link)
endif
	rm -f $(basler-base); ln -s $(basler-link) $(basler-base)

build_targets += $(basler-full)
endif

############################
# LIMA - UEYE
############################

ifneq ($(COMPILE_UEYE),0)

UEYE_LDFLAGS	:=
UEYE_LDLIBS	:= -lueye_api

name		:= ueye
ueye-objs	:= ../camera/ueye/src/Ueye.o
version-file	:= ../camera/ueye/VERSION
ueye-flags	:= $(UEYE_LDFLAGS)
ueye-deps	:= $(UEYE_LDLIBS)
ueye-base	:= liblima$(name).so
full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif
ueye-full	:= $(ueye-base).$(full-version)
ueye-link	:= $(ueye-base).$(link-version)
ueye-flags	+= -fPIC -Wl,-soname,$(ueye-link)

$(ueye-full):		$(ueye-objs)
	$(CXX) -shared -o $@ $(ueye-flags) $+ $(ueye-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(ueye-link); ln -s $(ueye-full) $(ueye-link)
endif
	rm -f $(ueye-base); ln -s $(ueye-link) $(ueye-base)

build_targets += $(ueye-full)
endif

############################
# LIMA - PROSILICA
############################

ifneq ($(COMPILE_PROSILICA),0)

PROCESSOR_TYPE := $(shell uname -i)
ifeq ($(PROCESSOR_TYPE),i386)
PROSILICA_LDFLAGS	:= -L. -L../camera/prosilica/sdk/bin/x86
else
PROSILICA_LDFLAGS	:= -L. -L../camera/prosilica/sdk/bin/x64 -L../camera/prosilica/sdk/bin/x86
endif
PROSILICA_LDLIBS	:= -lPvAPI

name		:= prosilica
prosilica-objs	:= ../camera/prosilica/src/Prosilica.o
version-file	:= ../camera/prosilica/VERSION
prosilica-flags	:= $(PROSILICA_LDFLAGS)
prosilica-deps	:= $(PROSILICA_LDLIBS)
full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif
prosilica-base	:= liblima$(name).so
prosilica-full	:= $(prosilica-base).$(full-version)
prosilica-link	:= $(prosilica-base).$(link-version)
prosilica-flags	+= -fPIC -Wl,-soname,$(prosilica-link)

$(prosilica-full):		$(prosilica-objs)
	$(CXX) -shared -o $@ $(prosilica-flags) $+ $(prosilica-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(prosilica-link); ln -s $(prosilica-full) $(prosilica-link)
endif
	rm -f $(prosilica-base); ln -s $(prosilica-link) $(prosilica-base)

build_targets += $(prosilica-full)
endif


############################
# LIMA - ROPERSCIENTIFIC
############################

ifneq ($(COMPILE_ROPERSCIENTIFIC),0)

ROPERSCIENTIFIC_LDLIBS	:= -lpvcam -lraw1394
name		:= roperscientific
roperscientific-objs	:= ../camera/roperscientific/src/RoperScientific.o
version-file	:= ../camera/roperscientific/VERSION
roperscientific-flags	:= $(LDFLAGS)
roperscientific-deps	:= $(ROPERSCIENTIFIC_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

roperscientific-base	:= liblima$(name).so
roperscientific-full	:= $(roperscientific-base).$(full-version)
roperscientific-link	:= $(roperscientific-base).$(link-version)
roperscientific-flags	+= -fPIC -Wl,-soname,$(roperscientific-link)

$(roperscientific-full):		$(roperscientific-objs)
	$(CXX) -shared -o $@ $(roperscientific-flags) $+ $(roperscientific-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(roperscientific-link); ln -s $(roperscientific-full) $(roperscientific-link)
endif
	rm -f $(roperscientific-base); ln -s $(roperscientific-link) $(roperscientific-base)

build_targets += $(roperscientific-full)

endif

############################
# LIMA - ADSC
############################

ifneq ($(COMPILE_ADSC),0)

ADSC_LDFLAGS	:= -L. -L../camera/adsc/sdk/adsc_sources/lib/linux
ADSC_LDLIBS	:= -lauxlib -ldetcon_th
name		:= adsc
adsc-objs	:= ../camera/adsc/src/Adsc.o
version-file	:= ../camera/adsc/VERSION
adsc-flags	:= $(ADSC_LDFLAGS)
adsc-deps	:= $(ADSC_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

adsc-base	:= liblima$(name).so
adsc-full	:= $(adsc-base).$(full-version)
adsc-link	:= $(adsc-base).$(link-version)
adsc-flags	+= -fPIC -Wl,-soname,$(adsc-link)

$(adsc-full):		$(adsc-objs)
	$(CXX) -shared -o $@ $(adsc-flags) $+ $(adsc-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(adsc-link); ln -s $(adsc-full) $(adsc-link)
endif
	rm -f $(adsc-base); ln -s $(adsc-link) $(adsc-base)

build_targets += $(adsc-full)

endif


############################
# LIMA - MYTHEN
############################

ifneq ($(COMPILE_MYTHEN),0)

MYTHEN_LDLIBS	:= -lSlsDetector
name		:= mythen
mythen-objs	:= ../camera/mythen/src/Mythen.o
version-file	:= ../camera/mythen/VERSION
mythen-flags	:= $(LDFLAGS)
mythen-deps	:= $(MYTHEN_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

mythen-base	:= liblima$(name).so
mythen-full	:= $(mythen-base).$(full-version)
mythen-link	:= $(mythen-base).$(link-version)
mythen-flags	+= -fPIC -Wl,-soname,$(mythen-link)

$(mythen-full):		$(mythen-objs)
	$(CXX) -shared -o $@ $(mythen-flags) $+ $(mythen-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(mythen-link); ln -s $(mythen-full) $(mythen-link)
endif
	rm -f $(mythen-base); ln -s $(mythen-link) $(mythen-base)

build_targets += $(mythen-full)

endif

############################
# LIMA - ANDOR
############################

ifneq ($(COMPILE_ANDOR),0)

ANDOR_LDFLAGS	:= -L. -L/usr/local/lib
ANDOR_LDLIBS	:= -landor

name		:= andor
andor-objs	:= ../camera/andor/src/Andor.o
version-file	:= ../camera/andor/VERSION
andor-flags	:= $(ANDOR_LDFLAGS)
andor-deps	:= $(ANDOR_LDLIBS)
andor-base	:= liblima$(name).so
full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif
andor-full	:= $(andor-base).$(full-version)
andor-link	:= $(andor-base).$(link-version)
andor-flags	+= -fPIC -Wl,-soname,$(andor-link)

$(andor-full):		$(andor-objs)
	$(CXX) -shared -o $@ $(andor-flags) $+ $(andor-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(andor-link); ln -s $(andor-full) $(andor-link)
endif
	rm -f $(andor-base); ln -s $(andor-link) $(andor-base)

build_targets += $(andor-full)
endif

############################
# LIMA - XPAD
############################

ifneq ($(COMPILE_XPAD),0)

XPAD_LDLIBS	:= -L/home/xpix_user/PCI_VALIDATED/trunk/sw/xpci_lib -lxpci_lib -L../third-party/yat/lib/i386/static -lyat
name		:= xpad
xpad-objs	:= ../camera/xpad/src/Xpad.o
version-file	:= ../camera/xpad/VERSION
xpad-flags	:= $(LDFLAGS)
xpad-deps	:= $(XPAD_LDLIBS)

full-version	:= $(shell cat $(version-file))
maj-version	:= $(shell echo $(full-version) | cut -f1 -d.)
ifneq ($(LINK_STRICT_VERSION),0)
link-version	:= $(full-version)
else
link-version	:= $(maj-version)
endif

xpad-base	:= liblima$(name).so
xpad-full	:= $(xpad-base).$(full-version)
xpad-link	:= $(xpad-base).$(link-version)
xpad-flags	+= -fPIC -Wl,-soname,$(xpad-link)

$(xpad-full):		$(xpad-objs)
	$(CXX) -shared -o $@ $(xpad-flags) $+ $(xpad-deps)
ifeq ($(LINK_STRICT_VERSION),0)
	rm -f $(xpad-link); ln -s $(xpad-full) $(xpad-link)
endif
	rm -f $(xpad-base); ln -s $(xpad-link) $(xpad-base)

build_targets += $(xpad-full)

endif

src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so*
