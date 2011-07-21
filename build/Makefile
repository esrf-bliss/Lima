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



src:		$(build_targets)

test:
	@true

clean:
	rm -f *.o liblima*.so*
