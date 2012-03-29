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

ct-objs := CtSaving.o CtControl.o CtAcquisition.o CtBuffer.o \
	   CtImage.o CtSaving_Edf.o CtShutter.o CtAccumulation.o CtVideo.o \
	   CtEvent.o

INCLUDES = -I. -I../include -I../../common/include -I../../hardware/include \
		-I../software_operation/include \
		-I../../third-party/Processlib/core/include \
		-I../../third-party/Processlib/tasks/include \
		-I../../third-party/Sps/Include

ifneq ($(LIBS),0)
LIBS=
endif

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifndef COMPILE_NXS_SAVING
COMPILE_NXS_SAVING = 0
endif

ifndef COMPILE_FITS_SAVING
COMPILE_FITS_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
ct-objs += CtSaving_Cbf.o 
INCLUDES += -I../../third-party/CBFLib/include
CXXFLAGS += -DWITH_CBF_SAVING
endif

ifneq ($(COMPILE_NXS_SAVING),0)
ct-objs += CtSaving_Nxs.o 
# nexus4tango lib is not yet available but soon !
#INCLUDES += -I../../third-party/NXSLib/include
CXXFLAGS += -DWITH_NXS_SAVING
endif

ifneq ($(COMPILE_FITS_SAVING),0)
ct-objs += CtSaving_Fits.o
CXXFLAGS += -DWITH_FITS_SAVING -I../../third-party/cfitsio \
		-I../../third-party
#LIBS += -L../../third-party/CCfits/.libs \
	-L../../third-party/cfitsio/libs \
	-lcfitsio \
	-lCCfits
endif

ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
ct-objs += CtSpsImage.o  ../../third-party/Sps/Src/sps.o
CXXFLAGS += -DWITH_SPS_IMAGE
endif

SRCS = $(ct-objs:.o=.cpp)

CXXFLAGS += $(INCLUDES) -fPIC -Wall -pthread -g


all: Ct.o


Ct.o: $(ct-objs) ../software_operation/src/CtSoftwareOperation.o
	ld $(LIBS) -o $@ -r $+

clean:
	rm -f Ct.o $(ct-objs) *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
