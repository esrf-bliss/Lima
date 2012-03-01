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

std-objs = ../../../common/src/Common.o ../../../hardware/src/Hw.o
simu-objs = ../src/Simu.o

test-objs = test.o
test-ext-objs = $(simu-objs)

testsimulator-objs = testsimulator.o
testsimulator-ext-objs = $(simu-objs) $(std-objs)

testsimuhwinterface-objs = testsimuhwinterface.o
testsimuhwinterface-ext-objs = $(simu-objs) $(std-objs)

SRCS = $(test-objs:.o=.cpp) $(testsimulator-objs:.o=.cpp) \
       $(testsimuhwinterface-objs:.o=.cpp)

INC = -I../include -I../../../common/include -I../../../hardware/include -I../../../third-party/Processlib/core/include
CXXFLAGS += $(INC) -Wall -pthread -g
LDFLAGS = -L../../../third-party/Processlib/build -lprocesslib -pthread

all: test testsimulator testsimuhwinterface

test:	$(test-objs) $(test-ext-objs) $(std-objs)
	$(CXX) -o $@ $+ $(LDFLAGS)

testsimulator: $(testsimulator-objs) $(testsimulator-ext-objs) 
	$(CXX) -o $@ $+ $(LDFLAGS)

testsimuhwinterface: $(testsimuhwinterface-objs) \
		     $(testsimuhwinterface-ext-objs) 
	$(CXX) -o $@ $+ $(LDFLAGS)

clean: 
	rm -f $(test-objs) test *.edf
	rm -f $(testsimulator-objs) testsimulator
	rm -f $(testsimuhwinterface-objs) testsimuhwinterface
	rm -f *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
