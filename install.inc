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

ifndef INSTALL_DIR
INSTALL_DIR = install
endif

ifndef INSTALL_LIMA_DIR
INSTALL_LIMA_DIR = $(INSTALL_DIR)/Lima
endif

ifndef INSTALL_PLAT
ifeq (/csadmin/common/scripts/get_os.share,)
INSTALL_PLAT=""
else
INSTALL_PLAT=$(shell /csadmin/common/scripts/get_os.share)
endif
endif

ifndef INSTALL_PLAT_DIR
INSTALL_PLAT_DIR = $(INSTALL_LIMA_DIR)/$(INSTALL_PLAT)
endif

INSTALL_LIB_DIR = $(INSTALL_PLAT_DIR)/Lib

ifneq ($(COMPILE_CORE), 0)
INSTALL_CORE_DIR = $(INSTALL_PLAT_DIR)/Core
CORE_FULL_VERSION = $(shell cat common/VERSION)
CORE_MAJ_VERSION = $(shell echo $(CORE_FULL_VERSION) | cut -d. -f1)
INSTALL_CORE_VDIR = $(INSTALL_CORE_DIR)/v$(CORE_FULL_VERSION)
endif

ifneq ($(COMPILE_SIMULATOR), 0)
INSTALL_SIMU_DIR = $(INSTALL_PLAT_DIR)/Simulator
SIMU_FULL_VERSION = $(shell cat camera/simulator/VERSION)
SIMU_MAJ_VERSION = $(shell echo $(SIMU_FULL_VERSION) | cut -d. -f1)
INSTALL_SIMU_VDIR = $(INSTALL_SIMU_DIR)/v$(SIMU_FULL_VERSION)
endif

ifneq ($(COMPILE_ESPIA), 0)
INSTALL_ESPIA_DIR = $(INSTALL_PLAT_DIR)/Espia
ESPIA_FULL_VERSION = $(shell cat camera/common/espia/VERSION)
ESPIA_MAJ_VERSION = $(shell echo $(ESPIA_FULL_VERSION) | cut -d. -f1)
INSTALL_ESPIA_VDIR = $(INSTALL_ESPIA_DIR)/v$(ESPIA_FULL_VERSION)
endif

ifneq ($(COMPILE_FRELON), 0)
INSTALL_FRELON_DIR = $(INSTALL_PLAT_DIR)/Frelon
FRELON_FULL_VERSION = $(shell cat camera/frelon/VERSION)
FRELON_MAJ_VERSION = $(shell echo $(FRELON_FULL_VERSION) | cut -d. -f1)
INSTALL_FRELON_VDIR = $(INSTALL_FRELON_DIR)/v$(FRELON_FULL_VERSION)
endif

ifneq ($(COMPILE_MAXIPIX), 0)
INSTALL_MAXIPIX_DIR = $(INSTALL_PLAT_DIR)/Maxipix
MAXIPIX_FULL_VERSION = $(shell cat camera/maxipix/VERSION)
MAXIPIX_MAJ_VERSION = $(shell echo $(MAXIPIX_FULL_VERSION) | cut -d. -f1)
INSTALL_MAXIPIX_VDIR = $(INSTALL_MAXIPIX_DIR)/v$(MAXIPIX_FULL_VERSION)
endif

ifneq ($(COMPILE_PILATUS), 0)
INSTALL_PILATUS_DIR = $(INSTALL_PLAT_DIR)/Pilatus
PILATUS_FULL_VERSION = $(shell cat camera/pilatus/VERSION)
PILATUS_MAJ_VERSION = $(shell echo $(PILATUS_FULL_VERSION) | cut -d. -f1)
INSTALL_PILATUS_VDIR = $(INSTALL_PILATUS_DIR)/v$(PILATUS_FULL_VERSION)
endif

ifneq ($(COMPILE_BASLER), 0)
INSTALL_BASLER_DIR = $(INSTALL_PLAT_DIR)/Basler
BASLER_FULL_VERSION = $(shell cat camera/basler/VERSION)
BASLER_MAJ_VERSION = $(shell echo $(BASLER_FULL_VERSION) | cut -d. -f1)
INSTALL_BASLER_VDIR = $(INSTALL_BASLER_DIR)/v$(BASLER_FULL_VERSION)
endif

.PHONY: install

install:
	mkdir -p $(INSTALL_LIMA_DIR)
	cp python/__init__.py $(INSTALL_LIMA_DIR)
	mkdir -p $(INSTALL_LIB_DIR)
ifneq ($(COMPILE_CORE), 0)
	mkdir -p $(INSTALL_CORE_VDIR)
	cp common/python/__init__.py $(INSTALL_CORE_DIR)
	cp third-party/Processlib/build/libprocesslib.so $(INSTALL_LIB_DIR)
	cp third-party/Processlib/sip/processlib.so $(INSTALL_CORE_VDIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimacore.so*
	cp build/liblimacore.so.$(CORE_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimacore.so.$(CORE_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/core/limacore.so $(INSTALL_CORE_VDIR)
	cp common/python/Debug.py $(INSTALL_CORE_VDIR)
endif
ifneq ($(COMPILE_SIMULATOR), 0)
	mkdir -p $(INSTALL_SIMU_VDIR)
	cp camera/simulator/python/__init__.py $(INSTALL_SIMU_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimasimulator.so*
	cp build/liblimasimulator.so.$(SIMU_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimasimulator.so.$(SIMU_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/simulator/limasimulator.so $(INSTALL_SIMU_VDIR)
endif
ifneq ($(COMPILE_ESPIA), 0)
	mkdir -p $(INSTALL_ESPIA_VDIR)
	cp camera/common/espia/python/__init__.py $(INSTALL_ESPIA_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimaespia.so*
	cp build/liblimaespia.so.$(ESPIA_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimaespia.so.$(ESPIA_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/espia/limaespia.so $(INSTALL_ESPIA_VDIR)
endif
ifneq ($(COMPILE_FRELON), 0)
	mkdir -p $(INSTALL_FRELON_VDIR)
	cp camera/frelon/python/__init__.py $(INSTALL_FRELON_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimafrelon.so*
	cp build/liblimafrelon.so.$(FRELON_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimafrelon.so.$(FRELON_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/frelon/limafrelon.so $(INSTALL_FRELON_VDIR)
	cp camera/frelon/python/FrelonAcq.py $(INSTALL_FRELON_VDIR)
endif
ifneq ($(COMPILE_MAXIPIX), 0)
	mkdir -p $(INSTALL_MAXIPIX_VDIR)
	cp camera/maxipix/python/__init__.py $(INSTALL_MAXIPIX_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimamaxipix.so*
	cp build/liblimamaxipix.so.$(MAXIPIX_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimamaxipix.so.$(MAXIPIX_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/maxipix/limamaxipix.so $(INSTALL_MAXIPIX_VDIR)
	cp camera/maxipix/python/Mpx*.py $(INSTALL_MAXIPIX_VDIR)
endif
ifneq ($(COMPILE_PILATUS), 0)
	mkdir -p $(INSTALL_PILATUS_VDIR)
	cp camera/pilatus/python/__init__.py $(INSTALL_PILATUS_DIR)
	cp camera/pilatus/python/*.py $(INSTALL_PILATUS_VDIR)
endif
ifneq ($(COMPILE_BASLER), 0)
	mkdir -p $(INSTALL_BASLER_VDIR)
	cp camera/basler/python/__init__.py $(INSTALL_BASLER_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimabasler.so*
	cp build/liblimabasler.so.$(BASLER_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimabasler.so.$(BASLER_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/basler/limabasler.so $(INSTALL_BASLER_VDIR)
endif
