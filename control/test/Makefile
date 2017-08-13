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
LIMA_DIR := $(shell pwd)/../..

include $(LIMA_DIR)/config.inc
include $(LIMA_DIR)/control/control.inc

simutest-objs = simutest.o
testbufferalloc-objs = testbufferalloc.o
roicountertest-objs = roicountertest.o
all-objs = $(simutest-objs) $(testbufferalloc-objs) $(roicountertest-objs)

SRCS = $(all-objs:.o=.cpp) 

CPPFLAGS = $(CT_CPPFLAGS) 
CXXFLAGS = $(CT_CXXFLAGS)
DEPLIBS = $(CT_DEPLIBS) $(LIMA_DIR)/build,limacore

build_targets = roicountertest

ifndef COMPILE_SIMULATOR
COMPILE_SIMULATOR = 0
endif

ifneq ($(COMPILE_SIMULATOR),0)
build_targets += simutest testbufferalloc
CPPFLAGS += -I$(LIMA_DIR)/camera/simulator/include
DEPLIBS += $(LIMA_DIR)/build,limasimulator
endif

LDFLAGS = $(CT_LDFLAGS) $(call lib-depflags,$(DEPLIBS))
LDLIBS = $(CT_LDLIBS) $(call lib-deplibs,$(DEPLIBS))


all:	$(build_targets)

simutest: $(simutest-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testbufferalloc: $(testbufferalloc-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

roicountertest:		$(roicountertest-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean: 
	rm -f $(all-objs) simutest testbufferalloc roicountertest

%.o : %.cpp
	$(COMPILE.cpp) $(CPPFLAGS) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CPPFLAGS) -Wextra -fsyntax-only $(CXXFLAGS) $(CHK_SOURCES)
