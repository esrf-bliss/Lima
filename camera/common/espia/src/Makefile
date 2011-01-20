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

espia-objs = EspiaBase.o EspiaDev.o EspiaAcq.o EspiaSerialLine.o \
	     EspiaBufferMgr.o EspiaFocla.o FoclaHwInterface.o

SRCS = $(espia-objs:.o=.cpp)

include ../include/espia.inc

CXXFLAGS = -I../include \
	   -I../../../../hardware/include -I../../../../common/include \
	   -I$(ESPIA_DRV_INCL) -Wall -pthread -fPIC -g

all: Espia.o

Espia.o:	$(espia-objs)
	$(LD) -o $@ -r $+

EspiaBase.o:	Espia.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
