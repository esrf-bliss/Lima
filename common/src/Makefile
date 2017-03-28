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

common-objs := Constants.o SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o \
               MemUtils.o RegExUtils.o AcqState.o Debug.o VideoUtils.o Event.o \
	       Timer.o DirectoryEventUtils.o

has_not_inotify = $(shell grep INOTIFY_USER /boot/config-$(shell uname -r) >/dev/null 2>&1; echo $$?)

SRCS = $(common-objs:.o=.cpp)

CPPFLAGS = $(CT_CPPFLAGS)
CXXFLAGS = $(CT_CXXFLAGS)

ifeq ($(has_not_inotify),0)
CPPFLAGS += -DHAS_INOTIFY
endif

ifndef COMPILE_CONFIG
COMPILE_CONFIG = 0
endif

ifneq ($(COMPILE_CONFIG),0)
common-objs += ConfigUtils.o
CPPFLAGS += -DWITH_CONFIG -I$(LIMA_DIR)/third-party/libconfig/lib
endif

all:	Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o *.P $(common-objs)


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
