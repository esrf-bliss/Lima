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

ct-objs := CtSaving.o CtControl.o CtAcquisition.o CtBuffer.o \
	   CtImage.o CtSaving_Edf.o CtShutter.o CtAccumulation.o CtVideo.o \
	   CtEvent.o

CT_INCLS = -I. -I$(LIMA_DIR)/common/include -I$(LIMA_DIR)/hardware/include \
		-I$(LIMA_DIR)/control/include \
		-I$(LIMA_DIR)/control/software_operation/include \
		-I$(LIMA_DIR)/third-party/Processlib/core/include \
		-I$(LIMA_DIR)/third-party/Processlib/tasks/include \
		-I$(LIMA_DIR)/third-party/Sps/Include # \
#		-I/usr/include/nexuscpp/include

ifneq ($(CT_LIBS),0)
CT_LIBS=
endif

ifndef COMPILE_CBF_SAVING
COMPILE_CBF_SAVING = 0
endif

ifneq ($(COMPILE_CBF_SAVING),0)
ct-objs += CtSaving_Cbf.o 
CT_INCLS += -I$(LIMA_DIR)/third-party/CBFLib/include
CT_CXXFLAGS += -DWITH_CBF_SAVING
endif

ifndef COMPILE_HDF5_SAVING
COMPILE_HDF5_SAVING = 0
endif

ifneq ($(COMPILE_HDF5_SAVING),0)
ct-objs += CtSaving_Hdf5.o 
CT_INCLS += -I$(LIMA_DIR)/third-party/hdf5/src
CT_INCLS += -I$(LIMA_DIR)/third-party/hdf5/c++/src
CT_CXXFLAGS += -DWITH_HDF5_SAVING
endif

ifndef COMPILE_NXS_SAVING
COMPILE_NXS_SAVING = 0
endif

ifneq ($(COMPILE_NXS_SAVING),0)
ct-objs += CtSaving_Nxs.o 
# nexuscpp lib is not yet available but soon !
#CT_INCLS += -I$(LIMA_DIR)/third-party/NXSLib/include
CT_INCLS += `pkg-config nexuscpp --cflags-only-I`
CT_CXXFLAGS += -DWITH_NXS_SAVING
endif

ifndef COMPILE_FITS_SAVING
COMPILE_FITS_SAVING = 0
endif

ifneq ($(COMPILE_FITS_SAVING),0)
ct-objs += CtSaving_Fits.o
CT_CXXFLAGS += -DWITH_FITS_SAVING -I$(LIMA_DIR)/third-party/cfitsio \
		-I$(LIMA_DIR)/third-party
#CT_LIBS += -L$(LIMA_DIR)/third-party/CCfits/.libs \
	-L$(LIMA_DIR)/third-party/cfitsio/libs \
	-lcfitsio \
	-lCCfits
endif

ifndef COMPILE_EDFGZ_SAVING
COMPILE_EDFGZ_SAVING = 0
endif

ifneq ($(COMPILE_EDFGZ_SAVING),0)
CT_CXXFLAGS += -DWITH_EDFGZ_SAVING
endif

ifndef COMPILE_EDFLZ4_SAVING
COMPILE_EDFLZ4_SAVING = 0
endif

ifneq ($(COMPILE_EDFLZ4_SAVING),0)
CT_CXXFLAGS += -DWITH_EDFLZ4_SAVING
endif

ifndef COMPILE_TIFF_SAVING
COMPILE_TIFF_SAVING = 0
endif

ifneq ($(COMPILE_TIFF_SAVING),0)
ct-objs += CtSaving_Tiff.o 
CT_CXXFLAGS += -DWITH_TIFF_SAVING
endif

ifndef COMPILE_SPS_IMAGE
COMPILE_SPS_IMAGE = 0
endif

ifneq ($(COMPILE_SPS_IMAGE),0)
ct-objs += CtSpsImage.o  $(LIMA_DIR)/third-party/Sps/Src/sps.o
CT_CXXFLAGS += -DWITH_SPS_IMAGE
endif

ifndef COMPILE_CONFIG
COMPILE_CONFIG = 0
endif

ifneq ($(COMPILE_CONFIG),0)
ct-objs += CtConfig.o
CT_CXXFLAGS += -DWITH_CONFIG -I$(LIMA_DIR)/third-party/libconfig/lib
endif

CT_CXXFLAGS += $(CT_INCLS) -fPIC -Wall -pthread -g

