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

ct-objs = CtSaving.o CtControl.o CtAcquisition.o CtBuffer.o \
	   CtImage.o CtSaving_Edf.o CtShutter.o CtAccumulation.o CtVideo.o \
	   CtEvent.o

CT_INCLS = 	-I$(LIMA_DIR)/common/include \
		-I$(LIMA_DIR)/hardware/include \
		-I$(LIMA_DIR)/control/include \
		-I$(LIMA_DIR)/control/software_operation/include \
		-I$(LIMA_DIR)/third-party/Processlib/core/include \
		-I$(LIMA_DIR)/third-party/Processlib/tasks/include \
		-I$(LIMA_DIR)/third-party/Sps/Include # \
#		-I/usr/include/nexuscpp/include

CT_CPPFLAGS = 
CT_CXXFLAGS = 
CT_DEPLIBS = $(LIMA_DIR)/third-party/Processlib/build,processlib
CT_LDFLAGS = -pthread
CT_LDLIBS = -lrt

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
ct-objs += CtSaving_Cbf.o 
CT_INCLS += -I$(LIMA_DIR)/third-party/CBFLib/include
CT_CPPFLAGS += -DWITH_CBF_SAVING
CT_DEPLIBS += $(LIMA_DIR)/third-party/CBFLib/lib,cbf,'$${d}/lib$${l}.a'
CT_LDLIBS += -lcrypto
endif

ifndef COMPILE_HDF5_SAVING
COMPILE_HDF5_SAVING = 0
endif

ifneq ($(COMPILE_HDF5_SAVING),0)
ct-objs += CtSaving_Hdf5.o 
CT_INCLS += -I$(LIMA_DIR)/third-party/hdf5/src
CT_INCLS += -I$(LIMA_DIR)/third-party/hdf5/c++/src
CT_CPPFLAGS += -DWITH_HDF5_SAVING
CT_DEPLIBS += $(LIMA_DIR)/third-party/hdf5/src/.libs,hdf5 \
	      $(LIMA_DIR)/third-party/hdf5/c++/src/.libs,hdf5_cpp
endif

ifndef COMPILE_NXS_SAVING
COMPILE_NXS_SAVING = 0
endif

ifneq ($(COMPILE_NXS_SAVING),0)
ct-objs += CtSaving_Nxs.o 
# nexuscpp lib is not yet available but soon !
#CT_INCLS += -I$(LIMA_DIR)/third-party/NXSLib/include
CT_INCLS += `pkg-config nexuscpp --cflags-only-I`
CT_CPPFLAGS += -DWITH_NXS_SAVING
endif

ifndef COMPILE_FITS_SAVING
COMPILE_FITS_SAVING = 0
endif

ifneq ($(COMPILE_FITS_SAVING),0)
ct-objs += CtSaving_Fits.o
CT_INCLS += -I$(LIMA_DIR)/third-party/cfitsio -I$(LIMA_DIR)/third-party
CT_CPPFLAGS += -DWITH_FITS_SAVING
CT_DEPLIBS += $(LIMA_DIR)/third-party/cfitsio/lib,cfitsio,'$${d}/lib$${l}.a' \
	      $(LIMA_DIR)/third-party/CCfits/.libs,CCfits
endif

ifndef COMPILE_EDFGZ_SAVING
COMPILE_EDFGZ_SAVING = 0
endif

ifneq ($(COMPILE_EDFGZ_SAVING),0)
CT_CPPFLAGS += -DWITH_EDFGZ_SAVING
CT_LDLIBS += -lz
endif

ifndef COMPILE_EDFLZ4_SAVING
COMPILE_EDFLZ4_SAVING = 0
endif

ifneq ($(COMPILE_EDFLZ4_SAVING),0)
CT_CPPFLAGS += -DWITH_EDFLZ4_SAVING
CT_LDLIBS += -llz4
endif

ifndef COMPILE_TIFF_SAVING
COMPILE_TIFF_SAVING = 0
endif

ifneq ($(COMPILE_TIFF_SAVING),0)
ct-objs += CtSaving_Tiff.o
CT_CPPFLAGS += -DWITH_TIFF_SAVING
CT_LDLIBS += -ltiff
endif

ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
ct-objs += CtSpsImage.o  $(LIMA_DIR)/third-party/Sps/Src/sps.o
CT_CPPFLAGS += -DWITH_SPS_IMAGE
endif

ifndef COMPILE_CONFIG
COMPILE_CONFIG = 0
endif

ifneq ($(COMPILE_CONFIG),0)
ct-objs += CtConfig.o
CT_INCLS += -I$(LIMA_DIR)/third-party/libconfig/lib
CT_CPPFLAGS += -DWITH_CONFIG
CT_DEPLIBS += $(LIMA_DIR)/third-party/libconfig/lib/.libs,config++
endif

CT_CPPFLAGS += $(CT_INCLS)
CT_CXXFLAGS += -fPIC -Wall -pthread -g


############################
# HELPER MACROS
############################

lib-depshcmd    = for p in $(1); do \
                      d=$$(echo $${p} | cut -d, -f1); \
                      l=$$(echo $${p} | cut -d, -f2); \
                      template=$$(echo $${p} | cut -d, -f3); \
                      if [ ! -z $${template} ] ; then \
                         eval echo $${template}; \
                      else \
                         echo $(2); \
                      fi \
                  done
lib-depflagsstr = "-L $${d} -Wl,-rpath=$${d}"
lib-depflags    = $(shell $(call lib-depshcmd,$(1),$(lib-depflagsstr)))
lib-deplibs     = $(shell $(call lib-depshcmd,$(1),"-l$${l}"))
lib-deps        = $(shell $(call lib-depshcmd,$(1),"$${d}/lib$${l}.so"))
