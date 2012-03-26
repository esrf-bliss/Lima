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

ifneq ($(COMPILE_FITS_SAVING),0)
CORE_LDFLAGS += -L../usr/local/lib
CORE_LDLIBS += -lCCfits -lcfitsio
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

core-base	:= liblima$(name).so
core-full	:= $(core-base).$(full-version)
core-maj	:= $(core-base).$(maj-version)
core-flags	+= -fPIC -Wl,-soname,$(core-maj)

$(core-full):		$(core-objs)
	$(CXX) -shared -o $@ $(core-flags) $+ $(core-deps)
	rm -f $(core-maj);  ln -s $(core-full) $(core-maj)
	rm -f $(core-base); ln -s $(core-maj)  $(core-base)

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

simu-base	:= liblima$(name).so
simu-full	:= $(simu-base).$(full-version)
simu-maj	:= $(simu-base).$(maj-version)
simu-flags	+= -fPIC -Wl,-soname,$(simu-maj)

$(simu-full):		$(simu-objs)
	$(CXX) -shared -o $@ $(simu-flags) $+ $(simu-deps)
	rm -f $(simu-maj);  ln -s $(simu-full) $(simu-maj)
	rm -f $(simu-base); ln -s $(simu-maj)  $(simu-base)

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

espia-base	:= liblima$(name).so
espia-full	:= $(espia-base).$(full-version)
espia-maj	:= $(espia-base).$(maj-version)
espia-flags	+= -fPIC -Wl,-soname,$(espia-maj)

$(espia-full):		$(espia-objs)
	$(CXX) -shared -o $@ $(espia-flags) $+ $(espia-deps)
	rm -f $(espia-maj);  ln -s $(espia-full) $(espia-maj)
	rm -f $(espia-base); ln -s $(espia-maj)  $(espia-base)

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

frelon-base	:= liblima$(name).so
frelon-full	:= $(frelon-base).$(full-version)
frelon-maj	:= $(frelon-base).$(maj-version)
frelon-flags	+= -fPIC -Wl,-soname,$(frelon-maj)

$(frelon-full):		$(frelon-objs)
	$(CXX) -shared -o $@ $(frelon-flags) $+ $(frelon-deps)
	rm -f $(frelon-maj);  ln -s $(frelon-full) $(frelon-maj)
	rm -f $(frelon-base); ln -s $(frelon-maj)  $(frelon-base)

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

maxipix-base	:= liblima$(name).so
maxipix-full	:= $(maxipix-base).$(full-version)
maxipix-maj	:= $(maxipix-base).$(maj-version)
maxipix-flags	+= -fPIC -Wl,-soname,$(maxipix-maj)

$(maxipix-full):		$(maxipix-objs)
	$(CXX) -shared -o $@ $(maxipix-flags) $+ $(maxipix-deps)
	rm -f $(maxipix-maj);  ln -s $(maxipix-full) $(maxipix-maj)
	rm -f $(maxipix-base); ln -s $(maxipix-maj)  $(maxipix-base)

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
basler-full	:= $(basler-base).$(full-version)
basler-maj	:= $(basler-base).$(maj-version)
basler-flags	+= -fPIC -Wl,-soname,$(basler-maj)

$(basler-full):		$(basler-objs)
	$(CXX) -shared -o $@ $(basler-flags) $+ $(basler-deps)
	rm -f $(basler-maj);  ln -s $(basler-full) $(basler-maj)
	rm -f $(basler-base); ln -s $(basler-maj)  $(basler-base)

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
ueye-full	:= $(ueye-base).$(full-version)
ueye-maj	:= $(ueye-base).$(maj-version)
ueye-flags	+= -fPIC -Wl,-soname,$(ueye-maj)

$(ueye-full):		$(ueye-objs)
	$(CXX) -shared -o $@ $(ueye-flags) $+ $(ueye-deps)
	rm -f $(ueye-maj);  ln -s $(ueye-full) $(ueye-maj)
	rm -f $(ueye-base); ln -s $(ueye-maj)  $(ueye-base)

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
prosilica-base	:= liblima$(name).so
prosilica-full	:= $(prosilica-base).$(full-version)
prosilica-maj	:= $(prosilica-base).$(maj-version)
prosilica-flags	+= -fPIC -Wl,-soname,$(prosilica-maj)

$(prosilica-full):		$(prosilica-objs)
	$(CXX) -shared -o $@ $(prosilica-flags) $+ $(prosilica-deps)
	rm -f $(prosilica-maj);  ln -s $(prosilica-full) $(prosilica-maj)
	rm -f $(prosilica-base); ln -s $(prosilica-maj)  $(prosilica-base)

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

roperscientific-base	:= liblima$(name).so
roperscientific-full	:= $(roperscientific-base).$(full-version)
roperscientific-maj	:= $(roperscientific-base).$(maj-version)
roperscientific-flags	+= -fPIC -Wl,-soname,$(roperscientific-maj)

$(roperscientific-full):		$(roperscientific-objs)
	$(CXX) -shared -o $@ $(roperscientific-flags) $+ $(roperscientific-deps)
	rm -f $(roperscientific-maj);  ln -s $(roperscientific-full) $(roperscientific-maj)
	rm -f $(roperscientific-base); ln -s $(roperscientific-maj)  $(roperscientific-base)

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

adsc-base	:= liblima$(name).so
adsc-full	:= $(adsc-base).$(full-version)
adsc-maj	:= $(adsc-base).$(maj-version)
adsc-flags	+= -fPIC -Wl,-soname,$(adsc-maj)

$(adsc-full):		$(adsc-objs)
	$(CXX) -shared -o $@ $(adsc-flags) $+ $(adsc-deps)
	rm -f $(adsc-maj);  ln -s $(adsc-full) $(adsc-maj)
	rm -f $(adsc-base); ln -s $(adsc-maj)  $(adsc-base)

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

mythen-base	:= liblima$(name).so
mythen-full	:= $(mythen-base).$(full-version)
mythen-maj	:= $(mythen-base).$(maj-version)
mythen-flags	+= -fPIC -Wl,-soname,$(mythen-maj)

$(mythen-full):		$(mythen-objs)
	$(CXX) -shared -o $@ $(mythen-flags) $+ $(mythen-deps)
	rm -f $(mythen-maj);  ln -s $(mythen-full) $(mythen-maj)
	rm -f $(mythen-base); ln -s $(mythen-maj)  $(mythen-base)

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
andor-full	:= $(andor-base).$(full-version)
andor-maj	:= $(andor-base).$(maj-version)
andor-flags	+= -fPIC -Wl,-soname,$(andor-maj)

$(andor-full):		$(andor-objs)
	$(CXX) -shared -o $@ $(andor-flags) $+ $(andor-deps)
	rm -f $(andor-maj);  ln -s $(andor-full) $(andor-maj)
	rm -f $(andor-base); ln -s $(andor-maj)  $(andor-base)

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

xpad-base	:= liblima$(name).so
xpad-full	:= $(xpad-base).$(full-version)
xpad-maj	:= $(xpad-base).$(maj-version)
xpad-flags	+= -fPIC -Wl,-soname,$(xpad-maj)

$(xpad-full):		$(xpad-objs)
	$(CXX) -shared -o $@ $(xpad-flags) $+ $(xpad-deps)
	rm -f $(xpad-maj);  ln -s $(xpad-full) $(xpad-maj)
	rm -f $(xpad-base); ln -s $(xpad-maj)  $(xpad-base)

build_targets += $(xpad-full)

endif

src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so*
