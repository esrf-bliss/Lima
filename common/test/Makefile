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
SRCS = testregex.cpp

CXXFLAGS = -Wall -I ../include -pthread -g
LDFLAGS = -L../../build/ -Wl,--no-as-needed,-rpath=$(shell pwd)/../../build -llimacore -lpthread

ifneq ($(COMPILE_HDF5_SAVING),0)
PROCESSOR_TYPE := $(shell uname -m)
ifeq ($(PROCESSOR_TYPE),x86_64)
LDFLAGS += -L../../third-party/hdf5-1.8.12/libs/linux.x86_64 -lhdf5_cpp -lhdf5 -lsz
else
LDFLAGS += -L../../third-party/hdf5-1.8.12/libs/linux.x86 -lhdf5_cpp -lhdf5 -lsz
endif
endif

all: testregex

testregex:	testregex.o 
	$(CXX) $(LDFLAGS) -o $@ $+
clean:
	rm -f *.o testregex

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
