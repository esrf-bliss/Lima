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
include ../../config.inc

common-objs := Constants.o SizeUtils.o Timestamp.o ThreadUtils.o Exceptions.o \
               MemUtils.o RegExUtils.o AcqState.o Debug.o VideoUtils.o Event.o \
	       Timer.o DirectoryEventUtils.o

has_not_inotify = $(shell grep INOTIFY_USER /boot/config-$(shell uname -r) >/dev/null 2>&1; echo $$?)

SRCS = $(common-objs:.o=.cpp)

CXXFLAGS += -I../include -I../../hardware/include -I../../third-party/Processlib/core/include -Wall -pthread -fPIC -g

ifeq ($(has_not_inotify),0)
CXXFLAGS += -DHAS_INOTIFY
endif

ifndef COMPILE_CONFIG
COMPILE_CONFIG = 0
endif

ifneq ($(COMPILE_CONFIG),0)
common-objs += ConfigUtils.o
CXXFLAGS += -I../../third-party/libconfig/lib
endif

all:	Common.o

Common.o:	$(common-objs)
	ld -o $@ -r $+

clean:
	rm -f Common.o *.P $(common-objs)


%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)

.PHONY: check-syntax
check-syntax:
	$(CXX) -Wall -Wextra -fsyntax-only $(CXXFLAGS) $(CHK_SOURCES)
