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
include ../../../../config.inc

focla-objs = ../src/Espia.o \
	     ../../../../hardware/src/Hw.o ../../../../common/src/Common.o
ct-objs = ../../../../control/src/Ct.o 

SRCS = testfocla.cpp testfoclahwinterface.cpp

include ../../../common/espia/include/espia.inc

LDFLAGS = -pthread -L$(ESPIA_DRV_LIB)
LDLIBS = -lespia

HWINTER_LDFLAGS = $(LDFLAGS) -L../../../../third-party/Processlib/build
HWINTER_LDLIBS = $(LDLIBS) -lprocesslib

ifneq ($(COMPILE_CBF_SAVING), 0)
HWINTER_LDFLAGS += -L../../../../third-party/CBFLib/lib
HWINTER_LDLIBS += -lcbf
endif

CXXFLAGS = -I../include \
	   -I../../../../hardware/include -I../../../../common/include \
           -I../../../../control/include/ \
           -I../../../../control/software_operation/include/ \
	   -I../../../../third-party/Processlib/core/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -g

all: testfocla testfoclahwinterface

testfocla:	testfocla.o $(focla-objs)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LDLIBS)

testfoclahwinterface:	testfoclahwinterface.o $(focla-objs) $(ct-objs)
	$(CXX) $(HWINTER_LDFLAGS) -o $@ $+ $(HWINTER_LDLIBS)

clean:
	rm -f *.o testfocla testfoclahwinterface
%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
