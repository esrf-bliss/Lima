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
GET_OS_SHARE = $(shell ls /csadmin/common/scripts/get_os.share 2> /dev/null)
ifeq ($(GET_OS_SHARE),)
INSTALL_PLAT=
else
INSTALL_PLAT=$(shell $(GET_OS_SHARE))
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

ifneq ($(COMPILE_PROSILICA), 0)
INSTALL_PROSILICA_DIR = $(INSTALL_PLAT_DIR)/Prosilica
PROSILICA_FULL_VERSION = $(shell cat camera/prosilica/VERSION)
PROSILICA_MAJ_VERSION = $(shell echo $(PROSILICA_FULL_VERSION) | cut -d. -f1)
INSTALL_PROSILICA_VDIR = $(INSTALL_PROSILICA_DIR)/v$(PROSILICA_FULL_VERSION)
endif

ifneq ($(COMPILE_ROPERSCIENTIFIC), 0)
INSTALL_ROPERSCIENTIFIC_DIR = $(INSTALL_PLAT_DIR)/RoperScientific
ROPERSCIENTIFIC_FULL_VERSION = $(shell cat camera/roperscientific/VERSION)
ROPERSCIENTIFIC_MAJ_VERSION = $(shell echo $(ROPERSCIENTIFIC_FULL_VERSION) | cut -d. -f1)
INSTALL_ROPERSCIENTIFIC_VDIR = $(INSTALL_ROPERSCIENTIFIC_DIR)/v$(ROPERSCIENTIFIC_FULL_VERSION)
endif

ifneq ($(COMPILE_ADSC), 0)
INSTALL_ADSC_DIR = $(INSTALL_PLAT_DIR)/Adsc
ADSC_FULL_VERSION = $(shell cat camera/adsc/VERSION)
ADSC_MAJ_VERSION = $(shell echo $(ADSC_FULL_VERSION) | cut -d. -f1)
INSTALL_ADSC_VDIR = $(INSTALL_ADSC_DIR)/v$(ADSC_FULL_VERSION)
endif


ifneq ($(COMPILE_MYTHEN), 0)
INSTALL_MYTHEN_DIR = $(INSTALL_PLAT_DIR)/Mythen
MYTHEN_FULL_VERSION = $(shell cat camera/mythen/VERSION)
MYTHEN_MAJ_VERSION = $(shell echo $(MYTHEN_FULL_VERSION) | cut -d. -f1)
INSTALL_MYTHEN_VDIR = $(INSTALL_MYTHEN_DIR)/v$(MYTHEN_FULL_VERSION)
endif

ifneq ($(COMPILE_UEYE), 0)
INSTALL_UEYE_DIR = $(INSTALL_PLAT_DIR)/Ueye
UEYE_FULL_VERSION = $(shell cat camera/ueye/VERSION)
UEYE_MAJ_VERSION = $(shell echo $(UEYE_FULL_VERSION) | cut -d. -f1)
INSTALL_UEYE_VDIR = $(INSTALL_UEYE_DIR)/v$(UEYE_FULL_VERSION)
endif

.PHONY: install

PROCESSOR_TYPE := $(shell uname -i)

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
ifneq ($(COMPILE_PROSILICA), 0)
	mkdir -p $(INSTALL_PROSILICA_VDIR)
	cp camera/prosilica/python/__init__.py $(INSTALL_PROSILICA_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimaprosilica.so*
	rm -rf $(INSTALL_LIB_DIR)/libPvAPI.so
	cp build/liblimaprosilica.so.$(PROSILICA_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimaprosilica.so.$(PROSILICA_MAJ_VERSION) $(INSTALL_LIB_DIR)
ifeq ($(PROCESSOR_TYPE),i386)
ifeq ($(INSTALL_PLAT),redhate4) 
	cp camera/prosilica/sdk/bin/x86/old_libPvApi.so $(INSTALL_LIB_DIR)/libPvAPI.so
else
	cp camera/prosilica/sdk/bin/x86/libPvAPI.so $(INSTALL_LIB_DIR)
endif
else
ifeq ($(INSTALL_PLAT),redhate5) 
	cp camera/prosilica/sdk/bin/x64/old_libPvApi.so $(INSTALL_LIB_DIR)/libPvAPI.so
else
	cp camera/prosilica/sdk/bin/x64/libPvAPI.so $(INSTALL_LIB_DIR)
endif
endif
	cp sip/prosilica/limaprosilica.so $(INSTALL_PROSILICA_VDIR)
endif

ifneq ($(COMPILE_ROPERSCIENTIFIC), 0)
	mkdir -p $(INSTALL_ROPERSCIENTIFIC_VDIR)
	cp camera/roperscientific/python/__init__.py $(INSTALL_ROPERSCIENTIFIC_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimaroperscientific.so*
	cp build/liblimaroperscientific.so.$(ROPERSCIENTIFIC_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimaroperscientific.so.$(ROPERSCIENTIFIC_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/roperscientific/limaroperscientific.so $(INSTALL_ROPERSCIENTIFIC_VDIR)
endif

ifneq ($(COMPILE_ADSC), 0)
	mkdir -p $(INSTALL_ADSC_VDIR)
	cp camera/adsc/python/__init__.py $(INSTALL_ADSC_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimaadsc.so*
	cp build/liblimaadsc.so.$(ADSC_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimaadsc.so.$(ADSC_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/adsc/limaadsc.so $(INSTALL_ADSC_VDIR)
	cp camera/adsc/sdk/adsc_sources/lib/linux/libauxlib.so $(INSTALL_LIB_DIR)/libauxlib.so
	cp camera/adsc/sdk/adsc_sources/lib/linux/libdetcon_th.so $(INSTALL_LIB_DIR)/libdetcon_th.so
endif

ifneq ($(COMPILE_MYTHEN), 0)
	mkdir -p $(INSTALL_MYTHEN_VDIR)
	cp camera/mythen/python/__init__.py $(INSTALL_MYTHEN_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimamythen.so*
	cp build/liblimamythen.so.$(MYTHEN_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimamythen.so.$(MYTHEN_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/mythen/limamythen.so $(INSTALL_MYTHEN_VDIR)
endif

ifneq ($(COMPILE_UEYE), 0)
	mkdir -p $(INSTALL_UEYE_VDIR)
	cp camera/ueye/python/__init__.py $(INSTALL_UEYE_DIR)
	rm -rf $(INSTALL_LIB_DIR)/liblimaueye.so*
	cp build/liblimaueye.so.$(UEYE_FULL_VERSION) $(INSTALL_LIB_DIR)
	cp -d build/liblimaueye.so.$(UEYE_MAJ_VERSION) $(INSTALL_LIB_DIR)
	cp sip/ueye/limaueye.so $(INSTALL_UEYE_VDIR)
endif