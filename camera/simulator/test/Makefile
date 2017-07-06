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
LIMA_DIR := $(shell pwd)/../../..

include $(LIMA_DIR)/config.inc
include $(LIMA_DIR)/control/control.inc

test-objs = test.o

testsimulator-objs = testsimulator.o

testsimuhwinterface-objs = testsimuhwinterface.o

SRCS = $(test-objs:.o=.cpp) $(testsimulator-objs:.o=.cpp) \
       $(testsimuhwinterface-objs:.o=.cpp)

CPPFLAGS = -I../include $(CT_CPPFLAGS)
CXXFLAGS = $(CT_CXXFLAGS)
DEPLIBS = $(CT_DEPLIBS) \
	  $(LIMA_DIR)/build,limacore \
	  $(LIMA_DIR)/build,limasimulator
LDFLAGS = $(CT_LDFLAGS) $(call lib-depflags,$(DEPLIBS))
LDLIBS = $(CT_LDLIBS) $(call lib-deplibs,$(DEPLIBS))

all: test testsimulator testsimuhwinterface

test:	$(test-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testsimulator: $(testsimulator-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testsimuhwinterface: $(testsimuhwinterface-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean: 
	rm -f $(test-objs) test *.edf
	rm -f $(testsimulator-objs) testsimulator
	rm -f $(testsimuhwinterface-objs) testsimuhwinterface
	rm -f *.P

%.o : %.cpp
	$(COMPILE.cpp) $(CPPFLAGS) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
